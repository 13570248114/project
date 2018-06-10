#include <map>

#include "Logger.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "TcpServer.h"
using namespace std::placeholders;

class EchoServer
{
public:
    EchoServer(EventLoop* loop, const InetAddress& addr, size_t numThread = 1)
            : loop_(loop),
              server_(loop, addr),
              numThread_(numThread)
    {
        server_.setConnectionCallback(std::bind(
                &EchoServer::onConnection, this, _1));
        server_.setMessageCallback(std::bind(
                &EchoServer::onMessage, this, _1, _2));
        server_.setWriteCompleteCallback(std::bind(
                &EchoServer::onWriteComplete, this, _1));
    }

    ~EchoServer()
    {  }

    void start()
    {
        // set thread num here
        server_.setNumThread(numThread_);
        server_.start();
    }

    void onConnection(const TcpConnectionPtr& conn)
    {
        INFO("connection %s is [%s]",
             conn->name().c_str(),
             conn->connected() ? "up":"down");

        if (conn->connected()) {
            conn->setHighWaterMarkCallback(
                    std::bind(&EchoServer::onHighWaterMark, this, _1, _2),
                    1024);
        }
        else connections_.erase(conn);
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer& buffer)
    {
        TRACE("connection %s recv %lu bytes",
             conn->name().c_str(),
             buffer.readableBytes());

        // send will retrieve the buffer
        conn->send(buffer);
    }

    void onHighWaterMark(const TcpConnectionPtr& conn, size_t mark)
    {
        INFO("high water mark %lu bytes, stop read", mark);
        conn->stopRead();
    }

    void onWriteComplete(const TcpConnectionPtr& conn)
    {
        if (!conn->isReading()) {
            INFO("write complete, start read");
            conn->startRead();
        }
    }
    
private:
    EventLoop* loop_;
    TcpServer server_;
    const size_t numThread_;
    typedef std::map<TcpConnectionPtr, Timestamp> ConnectionList;
    ConnectionList connections_;
};

int main()
{
    setLogLevel(LOG_LEVEL_TRACE);
    EventLoop loop;
    InetAddress addr(9877);
    EchoServer server(&loop, addr);
    server.start();
    loop.loop();
}
