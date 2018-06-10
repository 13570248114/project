#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include<mutex>

#include<condition_variable>
#include"noncopyable.h"

class CountDownLatch: noncopyable
{
public:
    explicit CountDownLatch(int count)
        :count_(count)
    {}

    void count()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        count_--;
        if(count_<=0)
            cond_.notify_all();
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while(count_>0)
            cond_.wait(lock);
    }
private:
    int count_;
    std::mutex mutex_;
    std::condition_variable cond_;
};
#endif // !COUNTDOWNLATCH_H

