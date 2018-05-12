#ifndef SOCKET_H
#define SOCKET_H

#include<boost/noncopyable.hpp>

class InetAddress;

class Socket:public boost::noncopyable{
public:
    explicit Socket(int sockfd):sockfd_(sockfd){}
    ~Socket();
    int fd() const {return sockfd_;}
    void bindAddress(const InetAddress& addr);
    void listen();
    int accept(InetAddress* peeraddr);
    void setKeepAlive(bool on);
private:
    int sockfd_;
};
#endif // !SOCKET_H
