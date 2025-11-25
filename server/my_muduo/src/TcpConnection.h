#pragma once
#include <string>
#include <atomic>
#include <memory>

#include "InetAddress.h"
#include "Callbacks.h"
#include "Timestamp.h"
#include "Buffer.h"
#include "noncopyable.h"
#include "EventLoop.h"

class Socket;
class Channel;
class Buffer;

// 用于封装一个TCP连接
class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
    // 记录连接的状态
    enum StateE
    {
        kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting
    };

public:
    TcpConnection(EventLoop *loop,
                  uint64_t id,
                  const std::string &name,
                  int sockfd,
                  const InetAddress &localAddr,
                  const InetAddress &peerAddr);

    ~TcpConnection();

    EventLoop *getLoop() const { return _loop_; }
    const std::string &name() const { return _name_; }
    const InetAddress &localAddress() const { return _localAddr_; }
    const InetAddress &peerAddress() const { return _peerAddr_; }

    bool connected() const { return _state_ == kConnected; }

    void send(const std::string &buf); // 发送数据
    void shutdown();

    void setConnectionCallback(const ConnectionCallback &cb) { _connectionCallback_ = cb; }
    void setCloseCallback(const CloseCallback &cb) { _closeCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback &cb) { _writeCompleteCallback_ = cb; }
    void setMessageCallback(const MessageCallback &cb) { _messageCallback_ = cb; }
    void setHightWaterMarkCallback(const HightWaterMarkCallback &cb, size_t highWaterMark)
    {
        _hightWaterMarkCallback_ = cb;
        _hightWaterMark_ = highWaterMark;
    }

    // 建立连接
    void connectEstablished();
    // 连接销毁
    void connectDestroyed();

    void enableInactiveRelease(int timeouts) // 开启非活跃连接超时销毁机制
    {
        _loop_->runInLoop(std::bind(&TcpConnection::enableInactiveReleaseInLoop, this, timeouts));
    }

private:
    void setState(StateE state) { _state_ = state; }

    // 下面四个函数对应channel的四个回调
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handlerClose();
    void handleError();
    void handleEvent(); // 如果触发任意事件，则刷新定时任务

    // 在这个TcpConnection所属的EventLoop中发送数据
    void sendInLoop(const void *message, size_t len);
    void shutdownInLoop(); // 关闭连接

    void enableInactiveReleaseInLoop(int timeouts); // 开启非活跃连接超时机制
    void cancelInactiveReleaseInLoop();             // 关闭非活跃连接超时机制

private:
    EventLoop *_loop_;
    uint64_t _id_;            // 一个连接的id
    const std::string _name_; // 连接的名字
    std::atomic_int _state_;  // 记录连接的状态
    bool _reading_;
    bool _enable_inactive_release_; // 标识是否启动非活跃销毁机制，默认为false

    std::unique_ptr<Socket> _socket_;
    std::unique_ptr<Channel> _channel_;

    const InetAddress _localAddr_; // 指的是服务端的地址
    const InetAddress _peerAddr_;  // 指的是客户端的地址

    // 下面五个回调是TcpServer设置给TcpConnection的
    ConnectionCallback _connectionCallback_; // 连接状态变更时调用的回调
    CloseCallback _closeCallback_;
    WriteCompleteCallback _writeCompleteCallback_;
    MessageCallback _messageCallback_;
    HightWaterMarkCallback _hightWaterMarkCallback_;
    size_t _hightWaterMark_;

    Buffer _inputBuffer_;  // 输入缓冲区，即服务器从上面读取客户端的数据
    Buffer _outputBuffer_; // 输出缓冲区，即服务器发送给客户端的数据
};