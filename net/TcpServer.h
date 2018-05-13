#include<atomic>
#include<string>

#include<map>
#include<boost/noncopyable.hpp>
#include<boost/scoped_ptr.hpp>
#include<boost/shared_ptr.hpp>
#include"TcpConnection.h"

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer : boost::noncopyable
{
public:
     typedef boost::function<void(EventLoop*)> ThreadInitCallback;
     enum Option
     {
        kNoReusePort,
        kReusePort,
     };

     TcpServer(EventLoop* loop,const InetAddress& listenAddr, const std::string& nameArg,Option option = kNoReusePort);
     ~TcpServer();

     const std::string& ipPort() const { return ipPort_; }
     const std::string& name() const { return name_; }
     EventLoop* getLoop() const { return loop_; }

     void setThreadNum(int numThreads);
     void setThreadInitCallback(const ThreadInitCallback& cb)
     { threadInitCallback_ = cb; }

     std::atomic_int start_;
     EventLoop* loop_;  // the acceptor loop
     const std::string ipPort_;
     const std::string name_;
};
