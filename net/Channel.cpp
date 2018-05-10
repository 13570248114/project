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
