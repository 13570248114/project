#include"Acceptor.h"
#include<cstdio>
#include"EventLoop.h"
#include"InetAddress.h"
#include"Socket.h"

void newConnect(int sockfd,const InetAddress& peerAddr)
{
    printf("newConnection: accept a new connection from %s\n",peerAddr.toIpPort().c_str());    
    ::write(sockfd,"how are you?\n",13);
    sockets::close(sockfd);
}

int main(){
    printf("main():pid=%d\n",getpid());
    InetAddress listenAddr(9981);
    EventLoop loop;
    Acceptor acceptor(&loop,listenAddr);
    acceptor.setNewConnectionCallback(newConnect);
    acceptor.listen();
    loop.loop();
    return 0;
}
