#include"Acceptor.h"
#include"SocketsOps.h"
#include"Socket.h"

Acceptor::Acceptor(EventLoop* loop,const InetAddress& listenAddr)
    :loop_(loop),
    acceptorSocket_(sockets::createNonblockingOrDie(listenAddr.family())),
    acceptorChannel_(loop,acceptorSocket_.fd()),
    listenning_(false),
{

}
