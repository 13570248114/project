#include<boost/noncopyable.hpp>

class Socket:public boost::noncopyable{
public:
    explicit Socket(int sockfd):sockfd_(sockfd){}
    ~Socket();
    int fd() const {return sockfd_;}
private:
    int sockfd_;
};
