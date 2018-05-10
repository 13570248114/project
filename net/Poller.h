#ifndef POLLER_H
#define POLLER_H

#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include <muduo/base/Timestamp.h>
#include "EventLoop.h"

class Channel;

class Poller : boost::noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;
    Poller(EventLoop* loop);
    virtual ~Poller();

    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    virtual muduo::Timestamp poll(int timeoutMs,ChannelList* activeChannels)=0;

    static Poller* newDefaultPoller(EventLoop* loop);

protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channels_;
private:
    EventLoop* ownerLoop_;
};

struct pollfd;

class PollPoller : public Poller
{
public:
    PollPoller(EventLoop* loop);
    virtual ~PollPoller();
    virtual void updateChannel(Channel* channel);
    virtual void removeChannel(Channel* channel);
    virtual muduo::Timestamp poll(int timeoutMs,ChannelList* activeChannels)=0;

private:
    void fillActiveChannels(int numEvents,
                          ChannelList* activeChannels) const;
    typedef  std::vector<struct pollfd> PollFdList;
    PollFdList pollfds_;
};
#endif // !POLLER_H

