#include<boost/function.hpp>
#include<boost/noncopyable.hpp>

#include"Channel.h"
#include"Socket.h"

class EventLoop;
class InetAddress;

class Acceptor:public boost::noncopyable
{
public:
    typedef boost::function<void (int sockfd,const InetAddress&)> NewConnectionCallback;
    Acceptor(EventLoop* loop,const InetAddress& listenAddr);
    ~Acceptor();
    void setNewConnectionCallback(const NewConnectionCallback& cb){
        newConnectionCallback_=cb;
    }
    bool listenning() const{return listenning_;}
    void listen();
private:
    void handleRead();

    EventLoop* loop_;
    Socket acceptorSocket_;
    Channel acceptorChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
    int idleFd_;
};
