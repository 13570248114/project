#include"EventLoop.h"
#include"Channel.h"
#include"Poller.h"
#include"CurrentThread.h"
#include"SocketsOps.h"
#include<sys/eventfd.h>
#include<boost/bind.hpp>

const int kPollTimeMs = 10000;

void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}

__thread EventLoop* t_loopInThisThread = 0;

int createEventfd()
{
    int evtfd =::eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
    if(evtfd<0)
    {
        printf("Failed in eventfd");
        abort();
    }
    return evtfd;
}


EventLoop::EventLoop()
    :looping_(false),
     quit_(false),
     eventHandling_(false),
     callingPendingFunctors_(false),
     iteration_(0),
     threadId_(CurrentThread::tid()),
     poller_(Poller::newDefaultPoller(this)),
     wakeupFd_(createEventfd()),
     wakeupChannel_(new Channel(this, wakeupFd_))
{
    if(t_loopInThisThread)
    {
        printf("Another EventLoop  exists in this thread");
    }
    else
    {
        t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(boost::bind(&EventLoop::handleRead,this));
    wakeupChannel_->enableReading();
}

void EventLoop::removeChannel(Channel* channel)
{
    if(eventHandling_)
    {
        /*一些检查,没懂*/
    }
    poller_->removeChannel(channel);
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;  // FIXME: what if someone calls quit() before loop() ?
    printf("start looping\n");

    while(!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_=poller_->poll(kPollTimeMs,&activeChannels_);
        ++iteration_;
        eventHandling_=true;
        for(ChannelList::iterator it=activeChannels_.begin();it!=activeChannels_.end();++it)
        {
            currentActiveChannel_=*it;
            currentActiveChannel_->handleEvent(pollReturnTime_);
        }
        currentActiveChannel_=NULL;
        eventHandling_=false;
        doPendingFunctors();
    }
    looping_=false;
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = sockets::write(wakeupFd_,&one,sizeof(one));
    (void)n;
}


void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = sockets::read(wakeupFd_,&one,sizeof(one));
    (void)n;
}

void EventLoop::queueInLoop(const Functor& cb)
{
    {
        muduo::MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }

    if(!isInLoopThread()||callingPendingFunctors_)
    {
        wakeup();
    }
}

size_t EventLoop::queueSize() const
{
    muduo::MutexLockGuard lock(mutex_);
    return pendingFunctors_.size();
}

void EventLoop::queueInLoop(Functor&& cb)
{
  {
      muduo::MutexLockGuard lock(mutex_);
      pendingFunctors_.push_back(std::move(cb));  // emplace_back
  }

  if (!isInLoopThread() || callingPendingFunctors_)
  {
      wakeup();
  }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_=true;

    {
        muduo::MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(size_t i=0;i<functors.size();i++)
    {
        functors[i]();
    }
    callingPendingFunctors_=false;
}

void EventLoop::runInLoop(const Functor& cb)
{
    if(isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}

void EventLoop::runInLoop(Functor&& cb)
{
    
  if (isInLoopThread())
  {
    cb();
  }
  else
  {
      queueInLoop(std::move(cb));
  }
}
