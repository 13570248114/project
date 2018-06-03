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
     void start();

     void setConnectionCallback(const ConnectionCallback& cb)
     { connectionCallback_ = cb; }

     void setMessageCallback(const MessageCallback& cb)
     { messageCallback_ = cb; }

     void setWriteCompleteCallback(const WriteCompleteCallback& cb)
     { writeCompleteCallback_ = cb; }

private:

     void newConnection(int sockfd, const InetAddress& peerAddr);

     void removeConnection(const TcpConnectionPtr& conn);

     void removeConnectionInLoop(const TcpConnectionPtr& conn);

     typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

     boost::scoped_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
     boost::shared_ptr<EventLoopThreadPool> threadPool_;

     ConnectionCallback connectionCallback_;
     MessageCallback messageCallback_;
     WriteCompleteCallback writeCompleteCallback_;
     ThreadInitCallback threadInitCallback_;

     std::atomic_int start_;
     EventLoop* loop_;  // the acceptor loop
     const std::string ipPort_;
     const std::string name_;

     int nextConnId_;
     ConnectionMap connections_;
};
