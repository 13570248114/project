#ifndef CHANNEL_H
#define CHANNEL_H

#include<boost/function.hpp>
#include<boost/noncopyable.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/weak_ptr.hpp>
#include<muduo/base/Timestamp.h>
#include<assert.h>

class EventLoop;

class Channel:public boost::noncopyable{
public:
    typedef boost::function<void()> EventCallback;
    typedef boost::function<void(muduo::Timestamp)> ReadEventCallback;
    Channel(EventLoop* loop, int fd);
    ~Channel();
    void setReadCallback(ReadEventCallback&& cb)
    {readCallback_=std::move(cb);}
    void disableAll(){events_=kNoneEvent;update();}
    void enableReading(){events_|=kReadEvent;update();}
    void update();
    EventLoop* ownerLoop() { return loop_; }
    int index() { return index_; }
    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_index(int idx) { index_ = idx; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    void remove();
    void set_revents(int revt) { revents_ = revt; } // used by pollers
    void handleEvent(muduo::Timestamp receiveTime);
    void tie(const boost::shared_ptr<void>&);

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int  fd_;
    ReadEventCallback readCallback_;
    int events_;
    int revents_;
    bool eventHandling_;
    bool addedToLoop_;
    int   index_; // used by Poller.

    void handleEventWithGuard(muduo::Timestamp receiveTime);
    boost::weak_ptr<void> tie_;
    bool tied_;

    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};
#endif // !CHANNEL_H
