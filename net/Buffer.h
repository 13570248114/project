#ifndef BUFFER_H
#define BUFFER_H

#include<string>
#include <algorithm>
#include <vector>
#include <assert.h>
#include <string.h>

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

private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;

    static const char kCRLF[];
};
#endif // !BUFFER_H

