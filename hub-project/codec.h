#ifndef CODEC_H
#define CODEC_H

#include<muduo/net/Buffer.h>
#include<string>

namespace pubsub
{
    enum ParseResult
    {
        kError,
        kSuccess,
        kContinue,
    };

    ParseResult parseMessage(muduo::net::Buffer* buf,
                             std::string& cmd,
                             std::string& roomName,
                             std::string& content);
}
#endif // !CODEC_H
