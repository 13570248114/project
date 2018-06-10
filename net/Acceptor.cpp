#include"Acceptor.h"
#include"SocketsOps.h"
#include"Socket.h"
#include"InetAddress.h"
#include<boost/bind.hpp>

Acceptor::Acceptor(EventLoop* loop,const InetAddress& listenAddr)
    :loop_(loop),
    acceptorSocket_(sockets::createNonblockingOrDie(listenAddr.family())),
    acceptorChannel_(loop,acceptorSocket_.fd()),
    listenning_(false)
{
    acceptorSocket_.bindAddress(listenAddr);
    acceptorChannel_.setReadCallback(boost::bind(&Acceptor::handleRead,this));
};

Acceptor::~Acceptor()
{
    acceptorChannel_.disableAll();
    acceptorChannel_.remove();
}

void Acceptor::listen()
{
    listenning_=true;
    acceptorSocket_.listen();
    acceptorChannel_.enableReading();
}

void Acceptor::handleRead()
{
    InetAddress peerAddr;
    int connfd = acceptorSocket_.accept(&peerAddr);
    if(connfd>=0)
    {
        if(newConnectionCallback_)
            newConnectionCallback_(connfd,peerAddr);
        else
            sockets::close(connfd);
    }
    else{
        printf("in Acceptor::handleRead");
        /*错误处理*/
    }
}
