#include"codec.h"

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

#include <boost/bind.hpp>
#include<string>

#include <map>
#include <set>
#include <stdio.h>

using namespace muduo;
using namespace muduo::net;


namespace pubsub
{
    typedef std::set<std::string> ConnectionSubscription;
    class RoomName
    {
    public:
        RoomName(const std::string& name)
            :roomName_(name)
        {

        }

        void add(const TcpConnectionPtr& conn)
        {
            audiences_.insert(conn);
            if(lastPubTime_.valid())
            {
                conn->send(makeMessage());
            }
        }

        void remove(const TcpConnectionPtr& conn)
        {
            audiences_.erase(conn);
        }

        void publish(const std::string& content,Timestamp time)
        {
            content_ = content;
            lastPubTime_ = time;
            std::string message = makeMessage();
            for(std::set<TcpConnectionPtr>::iterator it = audiences_.begin();it!=audiences_.end();++it)
            {
                (*it)->send(message);
            }
        } 

    private:
        std::string makeMessage()
        {
            return "pub"+roomName_+"\r\n"+content_+"\r\n";
        }
        std::string roomName_;
        std::string content_;
        Timestamp lastPubTime_;
        std::set<TcpConnectionPtr> audiences_;
    };

    class PubSubServer : boost::noncopyable
    {
    public:
        PubSubServer(muduo::net::EventLoop* loop,
               const muduo::net::InetAddress& listenAddr)
            :loop_(loop),
            server_(loop,listenAddr,"PubSubServer")
        {
            server_.setConnectionCallback(boost::bind(&PubSubServer::onConnection,this,_1));
            server_.setMessageCallback(boost::bind(&PubSubServer::onMessage,this,_1,_2,_3));
        }

         void start()
         {
              server_.start();
         }
    
    private:
        void onConnection(const TcpConnectionPtr& conn)
        {
            if (conn->connected())
            {
                conn->setContext(ConnectionSubscription());
            }
            else
            {
                const ConnectionSubscription& connSub = boost::any_cast<const ConnectionSubscription&>(conn->getContext());
                for(ConnectionSubscription::const_iterator it = connSub.begin();it!=connSub.end();)
                {
                    doUnsubscribe(conn, *it++);
                }
            }
        }

        void onMessage(const TcpConnectionPtr& conn,
                 Buffer* buf,
                 Timestamp receiveTime)
        {
            ParseResult result = kSuccess;
            std::string cmd,roomName,content;
            result = parseMessage(buf,cmd,roomName,content);
            if(result==kSuccess)
            {
                if(cmd=="pub")
                    doPublish(conn->name().c_str(),roomName,content,receiveTime);
                else if(cmd=="sub")
                    doSubscribe(conn,roomName);
                else if(cmd=="unsub")
                    doUnsubscribe(conn,roomName);
                else
                {
                    conn->shutdown();
                    result=kError;
                }
            }
            else if(result==kError)
                conn->shutdown();
        }

        void doSubscribe(const TcpConnectionPtr& conn,const std::string& roomName)
        {
            ConnectionSubscription* connSub = boost::any_cast<ConnectionSubscription>(conn->getMutableContext());
            connSub->insert(roomName);
            getRoomName(roomName).add(conn);
        }

        void doUnsubscribe(const TcpConnectionPtr& conn,const std::string& roomName)
        {
            getRoomName(roomName).remove(conn);
            ConnectionSubscription* connSub = boost::any_cast<ConnectionSubscription>(conn->getMutableContext());
            connSub->erase(roomName);
        }

        RoomName& getRoomName(const std::string name)
        {
            std::map<std::string,RoomName>::iterator it = roomNames_.find(name);
            if(it==roomNames_.end())
            {
                it = roomNames_.insert(make_pair(name,RoomName(name))).first;
            }
            return it->second;
        }

         void doPublish(const std::string& source,
                 const std::string& roomName,
                 const std::string& content,
                 Timestamp time)
         {
            getRoomName(roomName).publish(content,time);
         }
        EventLoop* loop_;
        TcpServer server_;
        std::map<std::string,RoomName> roomNames_;
    };

}


int main(int argc,char* argv[])
{
    if(argc>1)
    {
        uint16_t port = static_cast<uint16_t>(atoi(argv[1]));
        EventLoop loop;
        pubsub::PubSubServer server(&loop,InetAddress(port));
        server.start();
        loop.loop();
    }
}
