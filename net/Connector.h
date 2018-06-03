#ifndef CONNECTOR_H
#define CONNECTOR_H

#include"InetAddress.h"

#include<boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

class Channel;
class EventLoop;

class Connector : boost::noncopyable,
                  public boost::enable_shared_from_this<Connector>
{
public:
    typedef boost::function<void (int sockfd)> NewConnectionCallback;
    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb; }

    void start();
    void restart();
    void stop();

    const InetAddress& serverAddress() const { return serverAddr_; }
private:
    enum States { kDisconnected, kConnecting, kConnected };
    static const int kMaxRetryDelayMs = 30*1000;
    static const int kInitRetryDelayMs = 500;

    void setState(States s) { state_ = s; }
    void startInLoop();
    void stopInLoop();

    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

    EventLoop* loop_;
    InetAddress serverAddr_;
    bool connect_; // atomic
    States state_;  // FIXME: use atomic variable
    boost::scoped_ptr<Channel> channel_;
    NewConnectionCallback newConnectionCallback_;
    int retryDelayMs_;

};
#endif // !CONNECTOR_H
i
