#ifndef PUBSUB_H
#define PUBSUB_H

#include<muduo/net/TcpClient.h>

namespace pubsub
{
    using muduo::Timestamp;
    
    class PubSubClient
    {
        public:
            typedef boost::function<void (PubSubClient*)> ConnectionCallback;
            typedef boost::function<void (const std::string& roomName,
                                          const std::string& content,
                                          Timestamp)> SubscribeCallback;

            PubSubClient(muduo::net::EventLoop* loop,const muduo::net::InetAddress& hubAddr,const std::string& name);
            void start();
            void stop();
            bool connected() const;

            void setConnectionCallback(const ConnectionCallback& cb)
            { connectionCallback_ = cb; }

            bool subscribe(const std::string& roomName,const SubscribeCallback& cb);
            void unsubscribe(const std::string& roomName);
            bool publish(const std::string& roomName,const std::string& content);

        private:
            void onConnection(const muduo::net::TcpConnectionPtr& conn);
            void onMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buf,muduo::Timestamp receiveTime);
            bool send(const std::string& message);

            muduo::net::TcpClient client_;
            muduo::net::TcpConnectionPtr conn_;
            ConnectionCallback connectionCallback_;
            SubscribeCallback subscribeCallback_;
    };
}
#endif // !PUBSUB_H

