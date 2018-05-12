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

void TcpConnection::send(const StringPiece& message)
{
    if(state_==kConnected)
    {
        if(loop_->isInLoopThread())
        {
            
        }
    }
}

