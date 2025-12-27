#pragma once
// noncopyable作为基类来使用，使其派生类无法使用拷贝构造和赋值=运算符重载，达到我们想要的效果
// 同时外部无法实例化noncopyable类对象
class noncopyable
{
public:
    noncopyable(const noncopyable &) = delete;
    noncopyable &operator=(const noncopyable &) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};