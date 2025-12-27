#include <sys/uio.h>
#include <unistd.h>

#include "Buffer.h"

// 使用readv的好处是，仅需要一次系统调用，就能将fd中的数据全部读取上来，减少系统调用的次数
ssize_t Buffer::readfd(int fd, int *saveErrno) // 从fd上读取数据
{
    // 临时的缓冲区，如果从fd上读上来的数据长度超过了buffer中的writealbe，则将剩余的数据读到extrabuffer中，
    // 这样做的好处是，高效
    char extrabuffer[65536] = {0};

    const size_t writeable = writeableBytes();
    struct iovec vec[2];
    vec[0].iov_base = begin() + _writerIndex_;
    vec[0].iov_len = writeable;

    vec[1].iov_base = extrabuffer;
    vec[1].iov_len = sizeof(extrabuffer);

    const int iovcnt = (writeable < sizeof(extrabuffer)) ? 2 : 1;
    const ssize_t n = readv(fd, vec, iovcnt);

    if (n < 0)
    {
        *saveErrno = errno;
    }
    else if (n <= writeable)
    {
        _writerIndex_ += n;
    }
    else
    {
        _writerIndex_ = _buffer_.size();
        append(extrabuffer, n - writeable);
    }

    return n;
}

ssize_t Buffer::writefd(int fd, int *saveErrno) // 往fd上写入数据
{
    ssize_t n = write(fd, peek(), readableBytes());
    if (n < 0)
    {
        *saveErrno = errno;
    }
    return n;
}