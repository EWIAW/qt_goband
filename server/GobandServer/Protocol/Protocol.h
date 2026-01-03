#pragma once

// 协议号定义
namespace Protocol
{
    constexpr int REGISTER_REQUEST = 1;  // 注册请求
    constexpr int REGISTER_RESPONSE = 2; // 注册响应
    constexpr int LOGIN_REQUEST = 3;     // 登录请求
    constexpr int LOGIN_RESPONSE = 4;    // 登录响应
}