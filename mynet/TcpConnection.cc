#include <cassert>
#include <unistd.h>

#include"Logger.h"
#include"EventLoop.h"
#include"TcpConnection.h"

enum ConnectionState
{
    kConnecting,
    kConnected,
    kDisconnecting,
    kDisconnected
};

void defaultThreadInitCallback(size_t index)
{
    TRACE("EventLoop thread #%lu started", index);
}

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
    INFO("connection %s -> %s %s",
         conn->peer().toIpPort().c_str(),
         conn->local().toIpPort().c_str(),
         conn->connected() ? "up" : "down");
}

void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer& buffer)
{
    TRACE("connection %s -> %s recv %lu bytes",
          conn->peer().toIpPort().c_str(),
          conn->local().toIpPort().c_str(),
          buffer.readableBytes());
    buffer.retrieveAll();
}

}

TcpConnection::TcpConnection(EventLoop *loop, int sockfd,
                             const InetAddress& local,
                             const InetAddress& peer)
    :loop_(loop),
     sockfd_(sockfd),
     local_(local),
     peer_(peer),
     channel_(loop,sockfd),
     state_(kConnecting),
     highWaterMark_(0)
{
    channel_.setReadCallback([this](){handleRead();});
    channel_.setWriteCallback([this](){handleWrite();});
    channel_.setCloseCallback([this](){handleClose();});
    channel_.setErrorCallback([this](){handleError();});

    TRACE("TcpConnection() %s fd=%d", name().c_str(), sockfd);
}

TcpConnection::~TcpConnection()
{
    assert(state_ == kDisconnected);
    ::close(sockfd_);

    TRACE("~TcpConnection() %s fd=%d", name().c_str(), sockfd_);
}

void TcpConnection::connectEstablished()
{
    assert(state_ == kConnecting);
    state_ = kConnected;
    channel_.tie(shared_from_this());
    channel_.enableRead();
}

bool TcpConnection::connected() const
{ return state_ == kConnected; }

bool TcpConnection::disconnected() const
{ return state_ == kDisconnected; }

void TcpConnection::send(std::string_view data)
{
    send(data.data(),data.length());
}

void TcpConnection::send(const char *data, size_t len)
{
    if (state_ != kConnected) {
        WARN("TcpConnection::send() not connected, give up send");
        return;
    }
    if(loop_->assertInLoopThread())
    {
        sendInLoop(data,len);
    }
    else
    {
        loop_->queueInLoop([ptr=shared_from_this(),message=std::string(data,data+len)](){ptr->sendInLoop(message)});
    }
}

void TcpConnection::sendInLoop(const char *data, size_t len)
{
    loop_->assertInLoopThread();
    // kDisconnecting is OK
    if (state_ == kDisconnected) {
        WARN("TcpConnection::sendInLoop() disconnected, give up send");
        return;
    }
    
    ssize_t n = 0;
    size_t remain = len;
    bool faultError = false;

    //如果套接字没在写,则直接发送
    if(!channel_.isWriting())
    {
        assert(outputBuffer_.readableBytes() == 0);
        n = write(sockfd_,data,len);
        if(n==-1)
        {
            if(errno!=EAGAIN)
            {
                SYSERR("TcpConnection::write()");
                if(errno==EPIPE||errno==ECONNRESET)
                    faultError = true;
            }
            n=0;
        }
        else
        {
            remain-=static_cast<size_t>(n);
            if(remain==0&&writeCompleteCallback_)
            {
                loop_->queueInLoop(std::bind(writeCompleteCallback_,shared_from_this());
            }
        }
    }
    
    //如果第一次直接调用wirte没写完,就把剩下的放入到输出缓冲区
    if(!faultError&&remain>0)
    {
        if(highWaterMarkCallback_)
        {
            size_t oldLen=outputBuffer_.readableBytes();
            size_t newLen=oldLen+remain;
            if(oldLen<highWaterMark_&&newLen>=highWaterMark_)
                loop_->queueInLoop(std::bind(highWaterMark_,shared_from_this(),newLen));
        }
        outputBuffer_.append(data+n,remain);
        channel_.enableWrite();
    }
}

void TcpConnection::sendInLoop(const std::string& message)
{
    sendInLoop(message.data(),message.size());
}

void TcpConnection::send(Buffer& buffer)
{
    if(state_!=kConnected)
    {
        WARN("TcpConnection::send() not connected, give up send");
        return;
    }
    if(loop_->isInLoopThread())
    {
        sendInLoop(buffer.peek(),buffer.readableBytes());
        buffer.retrieveAll();
    }
    else
    {
        loop_->queueInLoop()
    }
}
