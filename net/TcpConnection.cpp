#include"TcpConnection.h"
#include<muduo/base/WeakCallback.h>
#include"Channel.h"
#include"EventLoop.h"
#include"Socket.h"
#include"SocketsOps.h"

#include<boost/bind.hpp>
#include<errno.h>

#include<string>

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
    printf("%s -> %s is ",conn->localAddress().toIpPort().c_str(),
           conn->peerAddress().toIpPort().c_str());
    if(conn->connected())
        printf("UP");
    else
        printf("DOWN");
}
void defaultMessageCallback(const TcpConnectionPtr&,
                                        Buffer* buf,
                                        muduo::Timestamp)
{
    buf->retrieveAll();
}

TcpConnection::TcpConnection(EventLoop* loop,const std::string& nameArg,int sockfd,const InetAddress& localAddr,const InetAddress& peerAddr)
    :loop_(loop),name_(nameArg),state_(kConnecting),reading_(true),socket_(new Socket(sockfd)),channel_(new Channel(loop,sockfd)),
     localAddr_(localAddr),peerAddr_(peerAddr),highWaterMark_(64*1024*1024)
{
    channel_->setReadCallback(boost::bind(&TcpConnection::handleRead,this,_1));
    channel_->setWriteCallback(boost::bind(&TcpConnection::handleWrite,this));
    channel_->setErrorCallback(boost::bind(&TcpConnection::handleError,this));
    printf( "TcpConnection::ctor %s at , fd = %d",name_.c_str(),sockfd);
    socket_->setKeepAlive(true);
}

const char* TcpConnection::stateToString() const
{
  switch (state_)
  {
    case kDisconnected:
      return "kDisconnected";
    case kConnecting:
      return "kConnecting";
    case kConnected:
      return "kConnected";
    case kDisconnecting:
      return "kDisconnecting";
    default:
      return "unknown state";
  }
}

TcpConnection::~TcpConnection()
{
    printf("TcpConnection::dtor[%s] , fd = %d, state= %s",name_.c_str(),channel_->fd(),stateToString());
}

void TcpConnection::send(const void* data, int len)
{
    send(muduo::StringPiece(static_cast<const char*>(data),len));
}

void TcpConnection::send(const muduo::StringPiece& message)
{
    if(state_==kConnected)
    {
        if(loop_->isInLoopThread())
        {
            sendInLoop(message);
        }
        else
        {
            loop_->runInLoop(boost::bind(&TcpConnection::sendInLoop,this,message.as_string()));
        }
    }
}

void TcpConnection::send(Buffer* buf)
{
    if(state_==kConnected)
    {
        if(loop_->isInLoopThread())
        {
            sendInLoop(buf->peek(),buf->readableBytes());
            buf->retrieveAll();
        }
        else
        {
            loop_->runInLoop(boost::bind(&TcpConnection::sendInLoop,this,buf->retrieveAllAsString()));
        }
    }
}

void TcpConnection::sendInLoop(const muduo::StringPiece& message)
{
  sendInLoop(message.data(), message.size());
}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
    loop_->assertInLoopThread();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if (state_ == kDisconnected)
    {
        printf("disconnected, give up writing");
        return;
    }
    //如果输出缓冲区空,直接写
    if(!channel_->isWriting()&&outputBuffer_.readableBytes()==0)
    {
        nwrote=sockets::write(channel_->fd(),data,len);
        if(nwrote>=0)
        {
            remaining = len-nwrote;
            if(remaining==0&&writeCompleteCallback_)
            {
                loop_->queueInLoop(boost::bind(writeCompleteCallback_,shared_from_this()));
            }
        }
        else
        {
            nwrote=0;
            /*错误处理*/
        }
    }

    assert(remaining <= len);
    if(!faultError&&remaining>0)
    {
        size_t oldLen = outputBuffer_.readableBytes();
        if(oldLen+remaining>=highWaterMark_&&oldLen<highWaterMark_&&highWaterMarkCallback_)
        {
            loop_->queueInLoop(boost::bind(highWaterMarkCallback_,shared_from_this(),oldLen+remaining));
        }
        outputBuffer_.append(static_cast<const char*>(data)+nwrote,remaining);
        if(!channel_->isWriting())
        {
            channel_->enableWriting();       
        }

    }
}

void TcpConnection::shutdown()
{
    if (state_ == kConnected)
    {
        setState(kDisconnecting);
        loop_->runInLoop(boost::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop()
{
    loop_->assertInLoopThread();
    if (!channel_->isWriting())
    {
        socket_->shutdownWrite();
    }
}

void TcpConnection::forceClose()
{
    if(state_==kConnected||state_==kDisconnecting)
    {
        setState(kDisconnecting);
        loop_->queueInLoop(boost::bind(&TcpConnection::forceCloseInLoop,shared_from_this()));
    }
}

void TcpConnection::forceCloseInLoop()
{
    loop_->assertInLoopThread();
    if (state_ == kConnected || state_ == kDisconnecting)
    {
         handleClose();
    }
}

void TcpConnection::handleClose()
{
    loop_->assertInLoopThread();
    assert(state_ == kConnected || state_ == kDisconnecting);
    setState(kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);

    closeCallback_(guardThis);
}
