#include "Channel.h"
#include<poll.h>
#include"EventLoop.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd__)
    :loop_(loop),
    fd_(fd__),
    index_(-1)
{

}

void Channel::update()
{
    addedToLoop_=true;
    loop_->updateChannel(this);
}

Channel::~Channel()
{
    assert(!eventHandling_);
    assert(!addedToLoop_);
    if(loop_->isInLoopThread()){
      //  assert(!loop_->hasChannel(this));
    }
}

void Channel::remove()
{
    assert(isNoneEvent());
    addedToLoop_ = false;
    loop_->removeChannel(this);
}

void Channel::handleEvent(muduo::Timestamp receiveTime)
{
    boost::shared_ptr<void> guard;
    if(tied_)
    {
        guard=tie_.lock();
        if(guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
    
}

void Channel::handleEventWithGuard(muduo::Timestamp receiveTime)
{
    eventHandling_=true;
    if((revents_&POLLHUP)&&!(revents_&POLLIN))
    {
        printf("Channel::handle_event() POLLHUP");
        if(closeCallback_)
            closeCallback_();
    }

    if(revents_&POLLNVAL)
    {
        printf("Channel::handle_event() POLLNVAL");
    }

    if(revents_&(POLLERR|POLLNVAL))
    {
        if(errorCallback_)
            errorCallback_();
    }

    if(revents_&(POLLIN|POLLPRI|POLLRDHUP))
    {
        if(readCallback_)
            readCallback_(receiveTime);
    }

    if(revents_&POLLOUT)
    {
        if(writeCallback_)
            writeCallback_();
    }
    eventHandling_ = false;
}
