#ifndef PROTOCOL_H
#define PROTOCOL_H

//协议号定义
namespace Protocol
{
//注册登录相关
constexpr int REGISTER_REQUEST = 1;
constexpr int REGISTER_RESPONSE = 2;
constexpr int LOGIN_REQUEST = 3;
constexpr int LOGIN_RESPONSE = 4;
}

#endif // PROTOCOL_H
