
#include <time.h>

#include "Timestamp.h"

Timestamp::Timestamp()
    : _seconds_(0)
{
}

Timestamp::Timestamp(int64_t seconds)
    : _seconds_(seconds)
{
}

// 获取当前时间
Timestamp Timestamp::Now()
{
    return Timestamp(time(nullptr));
}

std::string Timestamp::To_String() const
{
    char buffer[32] = {0};
    struct tm *tm_time = localtime(&_seconds_);
    strftime(buffer, sizeof(buffer) - 1, "%Y-%m-%d %H:%M:%S", tm_time);
    return buffer;
}
