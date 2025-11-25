#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <functional>
#include <atomic>

#include "Callbacks.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "noncopyable.h"
#include "InetAddress.h"
#include "Logger.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Buffer.h"

class EventLoop;

// 提供给外部用的TcpServer类
class TcpServer : noncopyable
{
    using ThreadInitCallback = std::function<void(EventLoop *)>;

public:
    enum Option
    {
        kNoReusePort, // 不重用端口
        kReusePort    // 重用端口
    };

    TcpServer(EventLoop *loop,
              const InetAddress &listenAddr,
              const std::string &nameArg,
              Option option = kNoReusePort);

    ~TcpServer();

    void setThreadInitCallback(const ThreadInitCallback &cb) { _threadInitCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback &cb) { _connectionCallback_ = cb; } // 当连接状态发生改变时所调用的回调
    // void setCloseCallback(const CloseCallback &cb) { _closeCallback_ = std::move(cb); }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { _writeCompleteCallback_ = cb; }
    void setMessageCallback(const MessageCallback &cb) { _messageCallback_ = cb; }

    void setThreadNum(int numThreads);
    void setEnableInactiveRelease(int timeouts)
    {
        _timeouts_ = timeouts;
        _enable_inactive_release_ = true;
    }

    void start();

private:
    void newConnection(int sockfd, const InetAddress &peerAddr); // 有新连接到来的时候所调用的回调
    void removeConnection(const TcpConnectionPtr &conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

private:
    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop *_loop_; // 外部传入的mainloop
    const std::string _ipPort_;
    const std::string _name_;

    std::unique_ptr<Acceptor> _acceptor_; // 用于获取新连接，运行在mainloop中

    std::shared_ptr<EventLoopThreadPool> _threadPool_;

    ConnectionCallback _connectionCallback_; // 连接状态变更时发生的回调
    // CloseCallback _closeCallback_;
    WriteCompleteCallback _writeCompleteCallback_; // 写操作完成后回调
    MessageCallback _messageCallback_;

    ThreadInitCallback _threadInitCallback_;

    std::atomic_int _started_; // 表示服务器启动状态，0表示未启动
    int _nextConnId_;
    ConnectionMap _connectionMap_; // 保存所有连接

    bool _enable_inactive_release_; // 是否开启非活跃连接超时销毁机制
    int _timeouts_;                 // 非活跃连接超时时间
};