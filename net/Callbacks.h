#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <muduo/base/Timestamp.h>

namespace net
{
    class Buffer;
    class TcpConnection;
    typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef boost::function<void()> TimerCallback;
    typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;
    typedef boost::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
    typedef boost::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;

    // the data has been read to (buf, len)
    typedef boost::function<void (const TcpConnectionPtr&,
                              Buffer*,
                              muduo::Timestamp)> MessageCallback;
    void defaultConnectionCallback(const TcpConnectionPtr& conn);
    void defaultMessageCallback(const TcpConnectionPtr& conn,
                            Buffer* buffer,
                            muduo::Timestamp receiveTime);
}
#endif // !CALLBACKS_H

