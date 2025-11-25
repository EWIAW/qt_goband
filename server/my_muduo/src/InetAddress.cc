
#include "InetAddress.h"

InetAddress::InetAddress(uint16_t port, std::string ip)
{
    bzero(&_addr_, sizeof(_addr_));
    _addr_.sin_family = AF_INET;
    _addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    _addr_.sin_port = htons(port);
}

InetAddress::InetAddress(const struct sockaddr_in &addr)
    : _addr_(addr)
{
}

std::string InetAddress::GetIp() const
{
    char buffer[32] = {0};
    inet_ntop(AF_INET, &_addr_.sin_addr, buffer, sizeof(buffer) - 1);
    return buffer;
}

uint16_t InetAddress::GetPort() const
{
    return ntohs(_addr_.sin_port);
}

std::string InetAddress::GetIpPort() const
{
    char ip[32] = {0};
    char port[32] = {0};
    inet_ntop(AF_INET, &_addr_.sin_addr, ip, sizeof(ip));
    snprintf(port, sizeof(port) - 1, "%d", ntohs(_addr_.sin_port));
    std::string ret;
    ret += ip;
    ret += " : ";
    ret += port;
    return ret;
}