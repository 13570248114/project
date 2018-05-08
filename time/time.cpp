#include<cstdio>
#include<sys/time.h>
#include<inttypes.h>
#include<string>
#include<iostream>
using namespace std;

class Timestamp{
public:
    Timestamp():microSecondsSinceEpoch_(0){}
    explicit Timestamp(int64_t sec):microSecondsSinceEpoch_(sec){}
    string toString() const;
    static Timestamp now();
    string toFormattedString() const;
    static const int kMicroSecondsPerSecond = 1000 * 1000;
    Timestamp addTime(Timestamp timestamp,double secondss);
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
private:
    int64_t microSecondsSinceEpoch_;
};

Timestamp Timestamp::now(){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    int64_t secondss = tv.tv_sec;
    return Timestamp(secondss*kMicroSecondsPerSecond+tv.tv_usec);
}

string Timestamp::toString() const{
    return to_string(microSecondsSinceEpoch_/kMicroSecondsPerSecond)+":"+to_string(microSecondsSinceEpoch_%kMicroSecondsPerSecond);
}

string Timestamp::toFormattedString() const{
    struct tm tm_time;
    time_t secondss = static_cast<time_t>(microSecondsSinceEpoch_/kMicroSecondsPerSecond);
    gmtime_r(&secondss,&tm_time);
    return to_string(tm_time.tm_year+1900)+":"+to_string(tm_time.tm_mon+1)+":"+to_string(tm_time.tm_mday)+":"+to_string(tm_time.tm_hour)+":"+to_string(tm_time.tm_min)+":"+to_string(tm_time.tm_sec);
}

Timestamp Timestamp::addTime(Timestamp timestamp, double seconds){
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

int main(){
    Timestamp ts=ts.now();
    cout<<ts.toString()<<endl;
    cout<<ts.toFormattedString()<<endl;
}
