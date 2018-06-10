#include"Connector.h"

#include"Channel.h"
#include"EventLoop.h"
#include"SocketsOps.h"

#include<boost/bind.hpp>
#include<errno.h>

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr)
    :loop_(loop),
     serverAddr_(serverAddr),
     connect_(false),
     state_(kDisconnected),
     retryDelayMs_(kMaxRetryDelayMs)
{

}

Connector::~Connector()
{
    assert(!channel_);
}

void Connector::start()
{
    connect_ = true;
    loop_->runInLoop(boost::bind(&Connector::startInLoop,this));
}

void Connector::startInLoop()
{
    loop_->assertInLoopThread();
    assert(state_==kDisconnected);
    if(connect_)
    {
        connect();
    }
    else
    {
        printf("do not connect");
    }
}

void Connector::stop()
{
    connect_ = false;
    loop_->queueInLoop(boost::bind(&Connector::stopInLoop,this));
}

void Connector::stopInLoop()
{
    loop_->assertInLoopThread();
    if (state_ == kConnecting)
    {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::connect()
{
    int sockfd = sockets::createNonblockingOrDie(serverAddr_.family());
    int ret = sockets::connect(sockfd,serverAddr_.getSockAddr());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch(savedErrno)
    {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
          connecting(sockfd);
          break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            printf("connect error in Connector::startInLoop %d",savedErrno);
            sockets::close(sockfd);
            break;

        default:
            printf("Unexpected error in Connector::startInLoop %d",savedErrno);
            sockets::close(sockfd);
      // connectErrorCallback_();
            break;
    }

}

void Connector::restart()
{
    loop_->assertInLoopThread();
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}

void Connector::connecting(int sockfd)
{
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_,sockfd));
    channel_->setWriteCallback(boost::bind(&Connector::handleWrite,this));
    channel_->setErrorCallback(boost::bind(&Connector::handleError,this));
    channel_->enableWriting();
}

int Connector::removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();
    loop_->queueInLoop(boost::bind(&Connector::resetChannel, this));
    return sockfd;
}

void Connector::resetChannel()
{
    channel_.reset();
}

void Connector::handleWrite()
{
    printf("Connector::handleWrite %d ", state_);
    if (state_ == kConnecting)
    {
        int sockfd=removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if(err)
        {
            printf("Connector::handleWrite - SO_ERROR = ");
            retry(sockfd);
        }
        else if(sockets::isSelfConnect(sockfd))
        {
            printf("Connector::handleWrite - Self connect");
            retry(sockfd);
        }
        else
        {
            setState(kConnected);
            if (connect_)
            {
                newConnectionCallback_(sockfd);
            }
            else
            {
                sockets::close(sockfd);
            }
        }
    }
}

void Connector::handleError()
{
    printf("Connector::handleError state=");
    if (state_ == kConnecting)
    {
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

void Connector::retry(int sockfd)
{
    sockets::close(sockfd);
    setState(kDisconnected);
    if (connect_)
    {
        
    }
}
