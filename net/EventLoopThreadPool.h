#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include<vector>
#include<boost/function.hpp>
#include<boost/noncopyable.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : boost::noncopyable
{
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg);
    ~EventLoopThreadPool();
    void setThreadNum(int numThreads) { numThreads_ = numThreads; }
    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    EventLoop* getNextLoop();
    EventLoop* getLoopForHash(size_t hashCode);

    std::vector<EventLoop*> getAllLoops();
    bool started() const
     { return started_; }

    const std::string& name() const
    { return name_; }

private:
    EventLoop* baseLoop_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    boost::ptr_vector<EventLoopThread> threads_;
    std::vector<EventLoop*> loops_;
};

#endif // !EVENTLOOPTHREADPOOL_H
