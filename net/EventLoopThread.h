#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include <muduo/base/Condition.h>
#include <muduo/base/Mutex.h>
#include"Thread.h"
#include<string>

class EventLoop;

class EventLoopThread : boost::noncopyable
{
public:
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                  const std::string& name = std::string());
    ~EventLoopThread();
    EventLoop* startLoop();
private:
    void threadFunc();

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    muduo::MutexLock mutex_;
    muduo::Condition cond_;
    ThreadInitCallback callback_;
};
#endif // !EVENTLOOPTHREAD_H

