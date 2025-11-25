#pragma once

#include <vector>
#include <string>
#include <algorithm>

// 整个缓冲区分为三个部分
//-------|-----------------|--------------
//   8   |                 |
//-------|-----------------|--------------
// prepend    reader             writer
// 其中prepend的作用是可以记录reader的大小，方便上层应用
class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitiaSize = 1024;

    explicit Buffer(size_t initialSize = kInitiaSize)
        : _buffer_(kCheapPrepend + initialSize),
          _readerIndex_(kCheapPrepend),
          _writerIndex_(kCheapPrepend)
    {
    }

    ~Buffer() = default;

    // 返回可读数据的大小
    size_t readableBytes() const { return _writerIndex_ - _readerIndex_; }

    // 返回可写数据的大小
    size_t writeableBytes() const { return _buffer_.size() - _writerIndex_; }

    // 返回prepend的大小
    size_t prependableBytes() const { return _readerIndex_; }

    // 返回缓冲区中可读数据的起始地址
    const char *peek() const { return begin() + _readerIndex_; }

    // 将buffer缓冲区里的数据向上交付    buffer -> onMessage回调函数
    void retrieve(size_t len)
    {
        if (len < readableBytes()) // 说明应用层只读取了一部分数据
            _readerIndex_ += len;
        else // 说明应用层读取了全部数据
            retrieveAll();
    }
    void retrieveAll() { _readerIndex_ = _writerIndex_ = kCheapPrepend; }

    // 将缓冲区中的数据读出来并以字符串的形式进行返回
    std::string retrieveAsString(size_t len)
    {
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }
    std::string retrieveAllAsString() { return retrieveAsString(readableBytes()); }

    // 检查writer缓冲区是否足够大，足以我们写入数据
    void ensureWriteableBytes(size_t len)
    {
        if (len > writeableBytes())
            makePlace(len); // 扩容
    }

    // 将需要进行输出给对端的数据copy到buffer缓冲区中
    void append(const char *data, size_t len)
    {
        ensureWriteableBytes(len);
        std::copy(data, data + len, beginWrite());
        _writerIndex_ += len;
    }

    // 返回可写入位置的地址
    char *beginWrite() { return begin() + _writerIndex_; }
    const char *beginWrite() const { return begin() + _writerIndex_; }

    ssize_t readfd(int fd, int *saveErrno);  // 从fd上读取数据
    ssize_t writefd(int fd, int *saveErrno); // 往fd上写入数据

private:
    // 返回vector底层首元素数据的地址
    char *begin() { return &*_buffer_.begin(); }
    const char *begin() const { return &*_buffer_.begin(); }

    // 扩容
    void makePlace(size_t len)
    {
        if (writeableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            _buffer_.resize(_writerIndex_ + len);
        }
        else
        {
            size_t readable = readableBytes(); // 这里要注意，要提前将readable保存起来
            std::copy(begin() + _readerIndex_, begin() + _writerIndex_, begin() + kCheapPrepend);
            _readerIndex_ = kCheapPrepend;
            _writerIndex_ = _readerIndex_ + readable; // 这里不能直接使用readableBytes
        }
    }

private:
    std::vector<char> _buffer_;
    size_t _readerIndex_;
    size_t _writerIndex_;
};
// static成员变量不能直接在类中初始化
// 但是static const成员变量可以在类中初始化