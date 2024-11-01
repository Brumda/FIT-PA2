#ifndef __PROGTEST__

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>

using namespace std;

class CTimeTester;

#endif /* __PROGTEST__ */

class CTime {
private:
    int m_Hour;
    int m_Minute;
    int m_Second;

    static int to_sec(const CTime &t) { return t.m_Hour * 3600 + t.m_Minute * 60 + t.m_Second; }

    static CTime to_date(int s) {
        while (s < 0) { s += 24 * 60 * 60; }
        s %= 24 * 60 * 60;
        int sec = (s % 3600) % 60;
        s -= sec;
        int min = (s % 3600) / 60;
        s -= min * 60;
        int hour = (s / 3600) % 24;

        return {hour, min, sec};

    }

public:
    // constructor, destructor
    CTime() = default;

    ~CTime() = default;

    CTime(int mHour, int mMinute, int mSecond) : m_Hour(mHour), m_Minute(mMinute), m_Second(mSecond) {
        if (mHour >= 24 || mMinute >= 60 || mSecond >= 60 || mHour < 0 || mMinute < 0 || mSecond < 0)
            throw invalid_argument("");
    }

    CTime(int mHour, int mMinute) : m_Hour(mHour), m_Minute(mMinute), m_Second(0) {
        if (mHour >= 24 || mMinute >= 60 || mHour < 0 || mMinute < 0)
            throw invalid_argument("");
    }

    CTime(const CTime &time) {
        m_Hour = time.m_Hour;
        m_Minute = time.m_Minute;
        m_Second = time.m_Second;
    }

    // arithmetic operators
    CTime &operator+=(int seconds) {
        int sec = to_sec(*this) + seconds;
        *this = to_date(sec);
        return *this;
    }

    CTime operator+(int seconds) const {
        CTime t(*this);
        t += seconds;
        return t;
    }

    CTime &operator-=(int seconds) {
        int sec = to_sec(*this) - seconds;
//        sec - seconds < 0 ? sec = 24 * 3600 - seconds : sec -= seconds;
        *this = to_date(sec);
        return *this;
    }

    CTime operator-(int seconds) const {
        auto t(*this);
        t -= seconds;
        return t;
    }

    int operator-(const CTime &b)const  {
        int s1 = to_sec(*this);
        int s2 = to_sec(b);
        int res = abs(s1 - s2);
        return res < 24 * 60 * 60 - res ? res : 24 * 60 * 60 - res;
    }

    friend CTime operator+(int seconds, const CTime &b) {
        CTime t(b);
        t += seconds;
        return t;
    }

    friend CTime operator-(int seconds, const CTime &b) {
        CTime t(b);
        t -= seconds;
        return t;
    }

    CTime &operator++() {
        *this += 1;
        return *this;
    }

    CTime operator++(int) {
        auto t(*this);
        *this += 1;
        return t;
    }

    CTime &operator--() {
        *this -= 1;
        return *this;
    }

    CTime operator--(int) {
        auto t(*this);
        *this -= 1;
        return t;
    }

    // comparison operators
    bool operator<(const CTime &rhs) const {
        return tie(m_Hour, m_Minute, m_Second)
               < tie(rhs.m_Hour, rhs.m_Minute, rhs.m_Second);
    }

    bool operator>(const CTime &rhs) const { return rhs < *this; }

    bool operator<=(const CTime &rhs) const { return !(rhs < *this); }

    bool operator>=(const CTime &rhs) const { return !(*this < rhs); }

    bool operator==(const CTime &rhs) const {
        return tie(m_Hour, m_Minute, m_Second)
               == tie(rhs.m_Hour, rhs.m_Minute, rhs.m_Second);
    }

    bool operator!=(const CTime &rhs) const { return !(rhs == *this); }

    // output operator
    friend ostream &operator<<(ostream &os, const CTime &cTime) {
        os << (cTime.m_Hour < 10 ? " " : "") << cTime.m_Hour << ':' << (cTime.m_Minute < 10 ? "0" : "")
           << cTime.m_Minute
           << ":" << (cTime.m_Second < 10 ? "0" : "") << cTime.m_Second;
        return os;
    }

    friend class ::CTimeTester;
};

#ifndef __PROGTEST__

struct CTimeTester {
    static bool test(const CTime &time, int hour, int minute, int second) {
        return time.m_Hour == hour
               && time.m_Minute == minute
               && time.m_Second == second;
    }
};

#include <cassert>
#include <sstream>

int main() {
    CTime a{12, 30};
    assert(CTimeTester::test(a, 12, 30, 0));

    CTime b{13, 30};
    assert(CTimeTester::test(b, 13, 30, 0));

    assert(b - a == 3600);
    assert(CTimeTester::test(a - 60, 12, 29, 0));
    assert(CTimeTester::test(a + 60, 12, 31, 0));

    assert(a < b);
    assert(a <= b);
    assert(a != b);
    assert(!(a > b));
    assert(!(a >= b));
    assert(!(a == b));

    while (++a != b);
    assert(a == b);

    std::ostringstream output;
    assert(static_cast<bool>( output << a ));
    assert(output.str() == "13:30:00");

    assert(a++ == b++);
    assert(a == b);

    assert(--a == --b);
    assert(a == b);

    assert(a-- == b--);
    assert(a == b);
    cout << "yay" << endl;
    CTime c(0, 0);
    assert(--c == CTime(23, 59, 59));
    try {
        CTime e(24, 32, 1);
        CTime g(23, 60, 1);
        CTime h(23, 32, 60);
        CTime z(-23, 32, 59);
        CTime u(23, -32, 59);
        CTime i(23, 32, -59);
        assert("No exception thrown!" == nullptr);
    }
    catch (...) {}
    CTime d(23, 59, 59);
    cout << d << endl;
    assert(++d == CTime(0, 0));
    cout << d << endl;
    CTime cool(20, 5);
    cool -= 3000000;
    cout << "mega yay" << endl;
    return 0;
}

#endif /* __PROGTEST__ */
