#include"SocketsOps.h"
#include<fcntl.h>
#include<sys/socket.h>
#include<unistd.h>
#include<cstdio>
#include<assert.h>
#include<string.h>
#include<errno.h>

void setNonBlockAndCloseOnExec(int sockfd)
{
    int flags = fcntl(sockfd,F_GETFL,0);
    flags|=O_NONBLOCK;
    flags |= FD_CLOEXEC;
    int ret = fcntl(sockfd,F_SETFL,flags);
    (void)ret;
}

int sockets::createNonblockingOrDie(sa_family_t family)
{
    int sockfd=socket(family,SOCK_STREAM,IPPROTO_TCP);
    setNonBlockAndCloseOnExec(sockfd);
    return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr* addr)
{
    int ret = bind(sockfd,addr,sizeof(struct sockaddr));
    (void)ret;
}

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr)
{
    return static_cast<const struct sockaddr*>((void*)addr);
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in* addr)
{
    return static_cast<struct sockaddr*>((void*)addr);
}

void sockets::listenOrDie(int sockfd)
{
    int ret=::listen(sockfd,SOMAXCONN);
    (void)ret;
}

int sockets::accept(int sockfd, struct sockaddr_in* addr)
{
    socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));
    int connfd = ::accept(sockfd,sockaddr_cast(addr),&addrlen);
    setNonBlockAndCloseOnExec(connfd);
    /*if(connfd<0)
     * 的一些错误处理!*/
    return connfd;
}

void sockets::close(int sockfd)
{
  if (::close(sockfd) < 0)
  {
    printf("sockets::close");
  }
}

void sockets::toIpPort(char* buf, size_t size,
                       const struct sockaddr* addr)
{
    toIp(buf,size,addr);
    size_t end = ::strlen(buf);
    const struct sockaddr_in* addr4=static_cast<const struct sockaddr_in*>((void*)addr);
    uint16_t port = be16toh(addr4->sin_port);
    snprintf(buf+end,size-end,":%u",port);
}

void sockets::toIp(char* buf, size_t size,
                   const struct sockaddr* addr)
{
    assert(size>=INET_ADDRSTRLEN);
    const struct sockaddr_in* addr4=static_cast<const struct sockaddr_in*>((void*)addr);
    ::inet_ntop(AF_INET,&addr4->sin_addr,buf,static_cast<socklen_t>(size));
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count)
{
    return ::write(sockfd,buf,count);
}

ssize_t sockets::read(int sockfd, void *buf, size_t count)
{
    return ::read(sockfd,buf,count);
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt)
{
    return ::readv(sockfd, iov, iovcnt);
}

void sockets::shutdownWrite(int sockfd)
{
    if(::shutdown(sockfd,SHUT_WR)<0)
    {
        printf("sockets::shutdownWrite");
    }
}

struct sockaddr_in sockets::getLocalAddr(int sockfd)
{
    struct sockaddr_in localAddr;
    bzero(&localAddr,sizeof(localAddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(localAddr));
    if(::getsockname(sockfd,sockaddr_cast(&localAddr),&addrlen)<0)
    {
        printf("sockets::getLocalAddr");
    }
    return localAddr;
}

int sockets::connect(int sockfd, const struct sockaddr* addr)
{
    return ::connect(sockfd,addr,static_cast<socklen_t>(sizeof(struct sockaddr_in)));
}

int sockets::getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);
    if(::getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&optval,&optlen)<0)
    {
        return errno;
    }
    else
    {
        return optval;
    }
}

struct sockaddr_in sockets::getPeerAddr(int sockfd)
{
    struct sockaddr_in peeraddr;
    bzero(&peeraddr, sizeof peeraddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);\
    if(::getpeername(sockfd,sockaddr_cast(&peeraddr),&addrlen)<0)
    {
        printf("sockets::getPeerAddr");   
    }
    return peeraddr;
}

bool sockets::isSelfConnect(int sockfd)
{
    struct sockaddr_in localaddr = getLocalAddr(sockfd);
    struct sockaddr_in peeraddr = getPeerAddr(sockfd);
    const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(&localaddr);
    const struct sockaddr_in* raddr4 = reinterpret_cast<struct sockaddr_in*>(&peeraddr);
    return laddr4->sin_port == raddr4->sin_port
        && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
}
