#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include<string>
#include"Callbacks.h"
#include"Buffer.h"
#include"InetAddress.h"

#include <boost/any.hpp>
#include<boost/enable_shared_from_this.hpp>
#include<boost/noncopyable.hpp>
#include<boost/scoped_ptr.hpp>
#include<boost/shared_ptr.hpp>
#include<muduo/base/StringPiece.h>

// struct tcp_info is in <netinet/tcp.h>
struct tcp_info;

class Channel;
class EventLoop;
class Socket;

class TcpConnection:public boost::enable_shared_from_this<TcpConnection>
{
public:
  /// Constructs a TcpConnection with a connected sockfd
  ///
  /// User should not create this object.
    TcpConnection(EventLoop* loop,const std::string& name,
                  int sockfd,const InetAddress& localAddr,
                  const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }
    bool connected() const { return state_ == kConnected; }

    void send(const void* message, int len);
    void send(const muduo::StringPiece& message);
    void send(Buffer* buf);

    void shutdown(); 

    void forceClose();
    void forceCloseWithDelay(double seconds);
    void setTcpNoDelay(bool on);

    void connectDestroyed();  // should be called only once
    void connectEstablished();   // should be called only once

    void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; }

     void setConnectionCallback(const ConnectionCallback& cb)
     { connectionCallback_ = cb; }

     void setWriteCompleteCallback(const WriteCompleteCallback& cb)
     { writeCompleteCallback_ = cb; } 

     void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
     { highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark; }

     void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }


private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void handleRead(muduo::Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    
    // void sendInLoop(string&& message);
    void sendInLoop(const muduo::StringPiece& message);
    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();

    void forceCloseInLoop();
    void setState(StateE s) { state_ = s; }
    const char* stateToString() const;
    void stopReadInLoop();

    EventLoop* loop_;
    const std::string name_;
    StateE state_;  // FIXME: use atomic variable
    bool reading_;
    
    boost::scoped_ptr<Socket> socket_;
    boost::scoped_ptr<Channel> channel_;
    const InetAddress localAddr_;
    const InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;
    size_t highWaterMark_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
    boost::any context_;
};

typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

#endif // !TCPCONNECTION_H
