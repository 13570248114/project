#include"TcpServer.h"
#include"Acceptor.h"
#include"EventLoop.h"
#include"SocketsOps.h"

#include<boost/bind.hpp>
#include<stdio.h>

TcpServer::TcpServer(EventLoop* loop,
                     const InetAddress& listenAddr,
                     const string& nameArg,
                     Option option)
    :loop_(loop),
     ipPort_(listenAddr.toIpPort()),
     name_(nameArg),
     acceptor_(new Acceptor(loop,listenAddr)),
     connectionCallback_(defaultConnectionCallback),
     messageCallback_(defaultMessageCallback),
     nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(boost::bind(&TcpServer::newConnection,this,_1,_2));

}

TcpServer::~TcpServer()
{
    loop_->assertInLoopThread();
    for (ConnectionMap::iterator it(connections_.begin());
      it != connections_.end(); ++it)
    {
        TcpConnectionPtr conn(it->second);
        it->second.reset();
        conn->getLoop()->runInLoop(boost::bind(&TcpConnection::connectDestroyed,conn));
    }
}

void TcpServer::start()
{
    if(start_==false)
    {
        assert(!acceptor_->listenning());
        loop_->runInLoop(boost::bind(&Acceptor::listen,boost::get_pointer(acceptor_)));
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    loop_->assertInLoopThread();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    InetAddress localAddr(sockets::)
}
