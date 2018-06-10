#include"codec.h"

using namespace muduo;
using namespace muduo::net;
using namespace pubsub;

ParseResult pubsub::parseMessage(muduo::net::Buffer* buf,
                             std::string& cmd,
                             std::string& roomName,
                             std::string& content)
{
    ParseResult result = kError;
    const char* crlf = buf->findCRLF();
    if(crlf)
    {
        const char* space = std::find(buf->peek(), crlf, ' ');
        if(space!=crlf)
        {
            cmd.assign(buf->peek(),space);
            roomName.assign(space+1,crlf);
            if(cmd=="pub")
            {
                const char* start = crlf+2;
                crlf = buf->findCRLF(start);
                if(crlf)
                {
                    content.assign(start,crlf);
                    buf->retrieveUntil(crlf+2);
                    result=kSuccess;
                }
                else
                {
                    result = kContinue;
                }
            }
            else
            {
                buf->retrieveUntil(crlf+2);
                result=kSuccess;
            }
        }
        else
        {
            result=kError;
        }
    }
    else
    {
        result=kContinue;
    }
    return result;
}
