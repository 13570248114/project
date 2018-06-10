#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include<thread>
#include"CountDownLatch.h"


class EventLoop;

class EventLoopThread: noncopyable
{
public:
    EventLoopThread()=default;
    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void runInThread();
    EventLoop* loop_ = nullptr;
    bool started_=false;
    std::thread thread_;
    CountDownLatch latch_{1};
};
#endif // !EVENTLOOPTHREAD_H

