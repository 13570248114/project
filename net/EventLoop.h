#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include<vector>

#include<boost/any.hpp>
#include<boost/function.hpp>
#include<boost/noncopyable.hpp>
#include<boost/scoped_ptr.hpp>

#include<muduo/base/Mutex.h>
#include<muduo/base/Timestamp.h>
#include<muduo/base/Mutex.h>
#include"CurrentThread.h"


class Channel;
class Poller;
class TimerQueue;

class EventLoop:boost::noncopyable
{
public:
    typedef boost::function<void ()> Functor;
    EventLoop();
    //~EventLoop();
    void loop();
    //void quit();
    bool isInLoopThread() const {return threadId_==CurrentThread::tid();}
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    
    void assertInLoopThread()
    {
        if(!isInLoopThread())
            abortNotInLoopThread();
    }
    void runInLoop(const Functor& cb);
    void runInLoop(Functor&& cb);

    void queueInLoop(const Functor& cb);
    void queueInLoop(Functor&& cb);
    size_t queueSize() const;

    void wakeup();
    bool hasChannel(Channel* channel);


private:
    const pid_t threadId_;
    void abortNotInLoopThread(){};
    void handleRead();  // waked up
    void doPendingFunctors();
   // boost::scoped_ptr<Poller> poller_;
    Poller* poller_;
    bool looping_; /* atomic */
    bool quit_; /* atomic and shared between threads, okay on x86, I guess. */
    bool eventHandling_; /* atomic */
    bool callingPendingFunctors_; /* atomic */
    int64_t iteration_;
    muduo::Timestamp pollReturnTime_;

    typedef std::vector<Channel*> ChannelList;
    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    int wakeupFd_;
    boost::scoped_ptr<Channel> wakeupChannel_;

    mutable muduo::MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;

};
#endif // !EVENTLOOP_H
