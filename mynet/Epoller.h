#ifndef EPOLLER_H
#define EPOLLER_H

#include <vector>
#include "noncopyable.h"

class EventLoop;
class Channel;

class EPoller:noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    explicit EPoller(EventLoop* loop);
    ~EPoller();

    void poll(ChannelList& activeChannels);
    void updateChannel(Channel* channel);
private:
    void updateChannel(int op,Channel* channel);
    EventLoop* loop_;
    std::vector<struct epoll_event> events_;
    int epollfd_;
        
};
#endif // !EPOLLER_H


