#include"EventLoop.h"
#include"Channel.h"
#include"Poller.h"
#include"CurrentThread.h"

void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}


EventLoop::EventLoop()
    :looping_(false),
     quit_(false),
     eventHandling_(false),
     callingPendingFunctors_(false),
     iteration_(0),
     threadId_(CurrentThread::tid()),
     poller_(Poller::newDefaultPoller(this))
{

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

    }
}

