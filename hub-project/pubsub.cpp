#include"pubsub.h"
#include"codec.h"
#include<boost/bind.hpp>
#include<string>

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

PubSubClient::PubSubClient(EventLoop* loop,
                           const InetAddress& hubAddr,
                           const std::string& name)
    :client_(loop,hubAddr,name.c_str())
{
    client_.setConnectionCallback(boost::bind(&PubSubClient::onConnection,this,_1));
    client_.setMessageCallback(boost::bind(&PubSubClient::onMessage,this,_1,_2,_3));
}

void PubSubClient::start()
{
    client_.connect();
}

void PubSubClient::stop()
{
    client_.disconnect();
}

bool PubSubClient::connected() const
{
    return conn_&&conn_->connected();
}

bool PubSubClient::subscribe(const std::string& rootName,const SubscribeCallback& cb)
{
    std::string message = "sub"+rootName+"\r\n";
    subscribeCallback_ = cb;
    return send(message);
}

void PubSubClient::unsubscribe(const std::string& roomName)
{
    std::string message = "unsub"+roomName+"\r\n";
    send(message);
}

bool PubSubClient::publish(const std::string& roomName,const std::string& content)
{
    std::string message = "pub"+roomName+"\r\n"+content+"\r\n";
    return send(message);
}

bool PubSubClient::send(const std::string& message)
{
    bool succeed = false;
    if(conn_&&conn_->connected())
    {
        conn_->send(message);
        succeed = true;
    }
    return  succeed;
}

void PubSubClient::onConnection(const TcpConnectionPtr& conn)
{
    if(conn->connected())
    {
        conn_ = conn;
    }
    else
    {
        conn_.reset();
    }
    if(connectionCallback_)
    {
        connectionCallback_(this);
    }
}

void PubSubClient::onMessage(const TcpConnectionPtr& conn,
                             Buffer* buf,
                             Timestamp receiveTime)
{
    ParseResult result = kSuccess;
    while(result==kSuccess)
    {
        std::string cmd, roomName,content;
        result = parseMessage(buf,cmd,roomName,content);
        if(result==kSuccess)
        {
            if(cmd=="pub"&&subscribeCallback_)
            {
                subscribeCallback_(roomName,content,receiveTime);
                    
            }
        }
        else if(result==kError)
        {
            conn->shutdown();
        }
    }
}
