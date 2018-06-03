#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <chrono>

namespace net
{
using std::chrono::system_clock;
//using namespace std::literals::chrono_literals;

typedef std::chrono::nanoseconds Nanosecond;
typedef std::chrono::microseconds  Microsecond;
typedef std::chrono::milliseconds  Millisecond;
typedef std::chrono::seconds       Second;
typedef std::chrono::minutes       Minute;
typedef std::chrono::hours         Hour;
typedef std::chrono::time_point<system_clock,Nanosecond> Timestamp;

namespace clock
{
inline Timestamp now()
{ return system_clock::now(); }

inline Timestamp nowAfter(Nanosecond interval)
{ return now()+interval; }

inline Timestamp nowBefore(Nanosecond interval)
{ return now()-interval; }

}

}

#endif // !TIMESTAMP_H

