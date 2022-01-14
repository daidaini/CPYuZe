#pragma once

#include <sys/time.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

using llong = long long;
using namespace std::chrono;
using std::cout;
using std::endl;

class TimerLog
{
public:
    TimerLog(const std::string tag)
    { // 对象构造时候保存开始时间
        m_begin_ = high_resolution_clock::now();
        m_tag_ = tag;
    }

    void Reset() { m_begin_ = high_resolution_clock::now(); }

    llong Elapsed()
    {
        return static_cast<llong>(
            duration_cast<std::chrono::microseconds>(high_resolution_clock::now() - m_begin_).count());
    }

    ~TimerLog()
    { // 对象析构时候计算当前时间与对象构造时候的时间差就是对象存活的时间
        auto time =
            duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin_).count();
        std::cout << "time { " << m_tag_ << " } " << static_cast<double>(time) << " us" << std::endl;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_begin_;
    std::string m_tag_;
};