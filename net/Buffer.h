#ifndef BUFFER_H
#define BUFFER_H

#include<string>
#include <algorithm>
#include <vector>
#include <assert.h>
#include <string.h>
#include<muduo/base/StringPiece.h>

class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;
    explicit Buffer(size_t initialSize = kInitialSize)
        :buffer_(kCheapPrepend + initialSize),
        readerIndex_(kCheapPrepend),
        writerIndex_(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    size_t readableBytes() const
    { return writerIndex_-readerIndex_; }

    size_t writableBytes() const
    { return buffer_.size()-writerIndex_; }

    size_t prependableBytes() const
    { return readerIndex_; }

    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    char* begin()
    { return &*buffer_.begin(); }

    const char* begin() const
    { return &*buffer_.begin(); }

    const char* peek() const
    { return begin() + readerIndex_; }

    std::string retrieveAsString(size_t len)
    {
        assert(len<=readableBytes());
        std::string result(peek(),len);
        retrieve(len);
        return result;
    }

    void retrieve(size_t len)
    {
        assert(len<=readableBytes());
        if(len<readableBytes())
        {
            readerIndex_+=len;
        }
        else
        {
            retrieveAll();
        }
    }

    std::string retrieveAllAsString()
    {
       return retrieveAsString(readableBytes());
    }

    void append(const muduo::StringPiece& str)
    {
        append(str.data(), str.size());
    }

     void append(const void* /*restrict*/ data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

     void append(const char* /*restrict*/ data, size_t len)
     {
         ensureWritableBytes(len);
         std::copy(data, data+len, beginWrite());
         hasWritten(len);
     }

     void hasWritten(size_t len)
     {
         assert(len <= writableBytes());
         writerIndex_ += len;
     }

     char* beginWrite()
     { return begin() + writerIndex_; }

     const char* beginWrite() const
     { return begin() + writerIndex_; }

     void ensureWritableBytes(size_t len)
     {
        if(writableBytes()<len)
            makeSpace(len);
        assert(writableBytes() >= len);
     }



private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;

    static const char kCRLF[];
    void makeSpace(size_t len)
    {
        if(writableBytes()+prependableBytes()<len+kCheapPrepend)
        {
            buffer_.resize(writerIndex_+len);
        }
        else
        {
            assert(kCheapPrepend<readerIndex_);
            size_t readable=readableBytes();
            std::copy(begin()+readerIndex_,begin()+writerIndex_,begin()+kCheapPrepend);
            readerIndex_=kCheapPrepend;
            writerIndex_=readerIndex_+readable;
            assert(readable==readableBytes());
        }
    }
};
#endif // !BUFFER_H

