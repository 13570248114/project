#ifndef SOCKETSOPS_H
#define SOCKETSOPS_H

#include<arpa/inet.h>

namespace sockets
{
    int createNonblockingOrDie(sa_family_t family);
    void bindOrDie(int sockfd,const struct sockaddr* addr);
    const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
    struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);
    void listenOrDie(int sockfd);
    int  accept(int sockfd, struct sockaddr_in* addr);
    void close(int sockfd);
    void toIpPort(char* buf, size_t size,
              const struct sockaddr* addr);
    void toIp(char* buf, size_t size,
                   const struct sockaddr* addr);
    ssize_t read(int sockfd, void *buf, size_t count);
    ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);
    ssize_t write(int sockfd, const void *buf, size_t count);
    void shutdownWrite(int sockfd);
    int connect(int sockfd,const struct sockaddr* addr);
    int getSocketError(int sockfd);

    struct sockaddr_in getLocalAddr(int sockfd);
    struct sockaddr_in getPeerAddr(int sockfd);
    bool isSelfConnect(int sockfd);
}
#endif // !SOCKETSOPS_H
