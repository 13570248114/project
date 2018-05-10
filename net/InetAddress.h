#ifndef INETADDRESS_H
#define INETADDRESS_H

#include<netinet/in.h>
#include"SocketsOps.h"
#include<string>

class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);
    sa_family_t family() const {return addr_.sin_family;}
    const struct sockaddr* getSockAddr() const {return sockets::sockaddr_cast(&addr_);}
    void setSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }

    std::string toIpPort() const;
private:
    struct sockaddr_in addr_;
};
#endif // !INETADDRESS_H
