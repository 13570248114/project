#include"Channel.h"
#include"Poller.h"

#include<assert.h>
#include<errno.h>
#include<poll.h>

PollPoller::PollPoller(EventLoop* loop)
  : Poller(loop)
{
}

PollPoller::~PollPoller()
{
}

void PollPoller::updateChannel(Channel* channel)
{
    if(channel->index()<0)
    {
        struct pollfd pfd;
        pfd.fd=channel->fd();  
        pfd.events=static_cast<short>(channel->events());
        pfd.revents=0;
        pollfds_.push_back(pfd);
        int idx=static_cast<int>(pollfds_.size()-1);
        channel->set_index(idx);
        channels_[pfd.fd]=channel;
    }
    else
    {
        int idx = channel->index();
        struct pollfd& pfd = pollfds_[idx];
        pfd.fd=channel->fd();
        pfd.events=static_cast<short>(channel->events());
        pfd.revents=0;
        if(channel->isNoneEvent()){
            pfd.fd=-channel->fd()-1;
        }
    }

}

Poller* Poller::newDefaultPoller(EventLoop* loop){
    return new PollPoller(loop);
}

void PollPoller::removeChannel(Channel* channel)
{
    assert(channel->isNoneEvent());
    int idx = channel->index();
    const struct pollfd& pfd = pollfds_[idx]; (void)pfd;
    assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());
    size_t n = channels_.erase(channel->fd());
    if(static_cast<size_t>(idx)==pollfds_.size()-1)
    {
        pollfds_.pop_back();
    }
    else
    {
        int channelAtEnd = pollfds_.back().fd;
        iter_swap(pollfds_.begin()+idx,pollfds_.end()-1);
        if(channelAtEnd<0)
        {
            channelAtEnd=-channelAtEnd-1;
        }
        channels_[channelAtEnd]->set_index(idx);
        pollfds_.pop_back();
    }
}

void PollPoller::fillActiveChannels(int numEvents,ChannelList* activeChannels) const
{
    for(PollFdList::const_iterator pfd = pollfds_.begin();pfd!=pollfds_.end()&&numEvents>0;++pfd)
    {
        if(pfd->revents>0)
        {
            --numEvents;
            ChannelMap::const_iterator ch =channels_.find(pfd->fd);
            assert(ch!=channels_.end());
        }
    }
}
