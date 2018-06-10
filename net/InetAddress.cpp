#include"InetAddress.h"
#include<string.h>

InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
    bzero(&addr_,sizeof(addr_));
    addr_.sin_family=AF_INET;
    in_addr_t ip=INADDR_ANY;
    addr_.sin_addr.s_addr=htobe32(ip);
    addr_.sin_port=htobe16(port);
}


std::string InetAddress::toIpPort() const
{
    char buf[64]="";
    sockets::toIpPort(buf,sizeof(buf),getSockAddr());
    return buf;
}
