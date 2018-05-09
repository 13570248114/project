#include<boost/function.hpp>
#include<boost/noncopyable.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/weak_ptr.hpp>

class EventLoop;

class Channel:public boost::noncopyable{
public:
    typedef boost::function<void()> EventCallback;
    Channel(EventLoop* loop, int fd);
    ~Channel();

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int  fd_;

};
