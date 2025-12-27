#include <stdio.h>
#include <semaphore.h>

#include "Thread.h"
#include "CurrentThread.h"

std::atomic_int Thread::_numCreated_(0); // 记录创建线程的数量

Thread::Thread(ThreadFunc cb, const std::string &tname)
    : _start_(false),
      _join_(false),
      _tid_(0),
      _tname_(tname),
      _func_(std::move(cb))
{
    setDefaultName();
}

Thread::~Thread()
{
    if (_start_ && !_join_)
    {
        _thread_->detach();
    }
}

void Thread::start()
{
    _start_ = true;
    sem_t sem;                // 这里使用信号量的原因是以防万一，因为主线程创建一个新线程后，有可能去访问新线程的pid，但是这个时候，新线程的pid可能还没缓存好
    sem_init(&sem, false, 0); // false代表该信号量不跨进程（仅限于线程间通信）

    // 开启线程
    // 从线程执行下面的代码
    _thread_ = std::shared_ptr<std::thread>(new std::thread([&]()
                                                            {
        _tid_ = CurrentThread::tid();
        sem_post(&sem);//从线程创建成功 并且已经获取了其从线程的线程id，这个时候主线程可以继续往下运行了，所以从线程可以释放信号量，使信号量+1
        _func_(); }));

    // 主线程执行这一行代码
    sem_wait(&sem);
}

// 如果join被调用了，一般来说，是mainloop调用了subloop的join
void Thread::join()
{
    _join_ = true;
    _thread_->join();
}

void Thread::setDefaultName() // 设置线程默认名字
{
    int num = ++_numCreated_;
    if (_tname_.empty())
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer) - 1, "Thread%d", num);
        _tname_ = buffer;
    }
}
