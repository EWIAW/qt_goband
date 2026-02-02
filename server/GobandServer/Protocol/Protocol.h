#pragma once

// 协议号定义
enum Protocol
{
    REGISTER_REQUEST = 0,  // 注册请求
    REGISTER_RESPONSE = 1, // 注册响应
    LOGIN_REQUEST = 2,     // 登录请求
    LOGIN_RESPONSE = 3,     // 登录响应
    MATCH_REQUEST = 4,     // 匹配请求
    MATCH_RESPONSE = 5     // 匹配响应
};