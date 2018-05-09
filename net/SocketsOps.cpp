#include"SocketsOps.h"
#include<fcntl.h>

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


