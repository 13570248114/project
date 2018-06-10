
#include <sys/timerfd.h>
#include <strings.h>
#include <unistd.h>
#include <ratio> // std::nano::den

#include "Logger.h"
#include "EventLoop.h"
#include "TimerQueue.h"


int timerfdCreate()
{
    int fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (fd == -1)
        SYSFATAL("timer_create()");
    return fd;
}

void timerfdRead(int fd)
{
    uint64_t val;
    ssize_t n = read(fd, &val, sizeof(val));
    if (n != sizeof(val))
        ERROR("timerfdRead get %ld, not %lu", n, sizeof(val));
}

struct timespec durationFromNow(Timestamp when)
{
    struct timespec ret;
    Nanosecond ns = when - clock::now();
    if (ns < Millisecond(1)) 
        ns = Millisecond(1);

    ret.tv_sec = static_cast<time_t>(ns.count() / std::nano::den);
    ret.tv_nsec = ns.count() % std::nano::den;
    return ret;
}

void timerfdSet(int fd, Timestamp when)
{
    struct itimerspec oldtime, newtime;
    bzero(&oldtime, sizeof(itimerspec));
    bzero(&newtime, sizeof(itimerspec));
    newtime.it_value = durationFromNow(when);

    int ret = timerfd_settime(fd, 0, &newtime, &oldtime);
    if (ret == -1)
        SYSERR("timerfd_settime()");
}



TimerQueue::TimerQueue(EventLoop *loop)
        : loop_(loop),
          timerfd_(timerfdCreate()),
          timerChannel_(loop, timerfd_)
{
    loop_->assertInLoopThread();
    timerChannel_.setReadCallback([this](){handleRead();});
    timerChannel_.enableRead();
}

TimerQueue::~TimerQueue()
{
    for (auto& p: timers_)
        delete p.second;
    ::close(timerfd_);
}


Timer* TimerQueue::addTimer(TimerCallback cb, Timestamp when, Nanosecond interval)
{
    Timer* timer = new Timer(std::move(cb), when, interval);
    loop_->runInLoop([=](){
        auto ret = timers_.insert({when, timer});
        assert(ret.second);

        if (timers_.begin() == ret.first)
            timerfdSet(timerfd_, when);
    });
    return timer;
}

void TimerQueue::cancelTimer(Timer* timer)
{
    loop_->runInLoop([timer, this](){
        timer->cancel();
        timers_.erase({timer->when(), timer});
        delete timer;
    });
}

/*定时器超时时的处理函数
 * 1.首先,此类维护一个定时器组成的set
 * 2.其次,此类拥有一个timerfd,可读时说明有定时器超时,进入下面的处理函数
 * 3.每个timer由when和callback组成,用于告知在when时刻应该处理的函数
 * 4.Timestamp用于说明一个时刻,用std::chrono实现,当前时刻用system_clock::now()返回
 * 5.下面函数的处理逻辑:
 *      (1)获得超时的定时器vector
 *      (2)逐个处理
 *      (3)将set中的第一个定时器的时间设置为timerfd下次超时的时间
 *      */

void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    timerfdRead(timerfd_);

    Timestamp now(clock::now());
    for (auto& e: getExpired(now)) {
        Timer* timer = e.second;
        assert(timer->expired(now));

        if (!timer->canceled())
            timer->run();
        if (!timer->canceled() && timer->repeat()) {
            timer->restart();
            e.first = timer->when();
            timers_.insert(e);
        }
        else delete timer;
    }

    if (!timers_.empty())
        timerfdSet(timerfd_, timers_.begin()->first);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    Entry en(now + Nanosecond(1), nullptr);
    std::vector<Entry> entries;

    auto end = timers_.lower_bound(en);
    entries.assign(timers_.begin(), end);
    timers_.erase(timers_.begin(), end);

    return entries;
}

