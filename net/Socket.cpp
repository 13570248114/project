#include"Socket.h"
#include"SocketsOps.h"
#include<string.h>
#include"InetAddress.h"
#include<sys/socket.h>
#include<unistd.h>

void Socket::listen()
{
    sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    int connfd = sockets::accept(sockfd_,&addr);
    if(connfd>=0)
    {
        peeraddr->setSockAddrInet(addr);
    }
    return connfd;
}

void Socket::bindAddress(const InetAddress& addr)
{
    int ret = ::bind(sockfd_,addr.getSockAddr(),static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    (void)ret;
}

Socket::~Socket()
{
  ::close(sockfd_);
}

void Socket::setKeepAlive(bool on)
{
      int optval = on ? 1 : 0;  
    ::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&optval,static_cast<socklen_t>(sizeof(optval)));
}

void Socket::shutdownWrite()
{
  sockets::shutdownWrite(sockfd_);
}
