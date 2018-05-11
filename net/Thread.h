#ifndef THREAD_H
#define THREAD_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <muduo/base/CountDownLatch.h>
#include <pthread.h>
#include <atomic>
#include<string>

class Thread : boost::noncopyable
{
public:
    typedef boost::function<void ()> ThreadFunc;
    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    ~Thread();
    void start();
    int join(); // return pthread_join()
    bool started() const { return started_; }
    pid_t tid() const { return tid_; }
    const std::string& name() const { return name_; }
    static int numCreated() { return numCreated_.load(); }
private:
    void setDefaultName();
    bool       started_;
    bool       joined_;
    pthread_t  pthreadId_;
    pid_t      tid_;
    ThreadFunc func_;
    std::string     name_;
    muduo::CountDownLatch latch_;
    static std::atomic_int numCreated_;
};
#endif // !THREAD_H
