#include"Thread.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/weak_ptr.hpp>

#include "CurrentThread.h"

namespace CurrentThread
{
  __thread int t_cachedTid = 0;
  __thread char t_tidString[32];
  __thread int t_tidStringLength = 6;
  __thread const char* t_threadName = "unknown";
  const bool sameType = boost::is_same<int, pid_t>::value;
  BOOST_STATIC_ASSERT(sameType);
}

namespace detail
{
    pid_t gettid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    void afterFork()
    {
        CurrentThread::t_cachedTid = 0;
        CurrentThread::t_threadName = "main";
        CurrentThread::tid();
    }

    class ThreadNameInitializer
    {
    public:
        ThreadNameInitializer()
        {
            CurrentThread::t_threadName = "main";
            CurrentThread::tid();
            pthread_atfork(NULL, NULL, &afterFork);
        }
    };

    ThreadNameInitializer init;

    struct ThreadData
    {
        typedef Thread::ThreadFunc ThreadFunc;
        ThreadFunc func_;
        std::string name_;
        pid_t* tid_;
        muduo::CountDownLatch* latch_;

        ThreadData(const ThreadFunc& func,
             const std::string& name,
             pid_t* tid,
             muduo::CountDownLatch* latch)
        :func_(func),
         name_(name),
         tid_(tid),
         latch_(latch)
        {  }

        void runInThread()
        {
            *tid_=CurrentThread::tid();
            tid_=NULL;
            latch_->countDown();
            latch_=NULL;

            CurrentThread::t_threadName = name_.empty() ? "muduoThread" : name_.c_str();
            try
            {
                func_();
                CurrentThread::t_threadName = "finished";
            }
            catch(...)
            {
                CurrentThread::t_threadName = "crashed";
                fprintf(stderr,"unknown exception caught in Thread %s\n",name_.c_str());
                throw ;
            }
        }
    };

    void* startThread(void* obj)
    {
        ThreadData* data = static_cast<ThreadData*>(obj);
        data->runInThread();
        delete data;
        return NULL;
    }
}

void CurrentThread::cacheTid()
{
    if(t_cachedTid==0)
    {
        t_cachedTid=detail::gettid();
        t_tidStringLength=snprintf(t_tidString,sizeof(t_tidString),"%5d ",t_cachedTid);
    }
}

std::atomic<int> numCreated_(0);

Thread::Thread(const ThreadFunc& func,const std::string& n)
    :started_(false),
     joined_(false),
     pthreadId_(0),
     tid_(0),
     func_(func),
     name_(n),
     latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_&&!joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    int num = ++numCreated_;
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    detail::ThreadData* data = new detail::ThreadData(func_,name_,&tid_,&latch_);
    if(pthread_create(&pthreadId_,NULL,&detail::startThread,data))
    {
        started_=false;
        delete data;
        printf("Failed in pthread_create");
    }
    else
    {
        latch_.wait();
        assert(tid_>0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_,NULL);
}
