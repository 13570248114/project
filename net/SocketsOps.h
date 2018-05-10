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
}
#endif // !SOCKETSOPS_H
