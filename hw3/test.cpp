#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <ctime>
#include <climits>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <memory>

using namespace std;
#endif /* __PROGTEST__ */

// uncomment if your code implements initializer lists
#define EXTENDED_SYNTAX

class CRange {
public:
    CRange(long long int lo, long long int hi) : lo(lo), hi(hi) {
        if (lo > hi) { throw logic_error(""); }
    }

    explicit CRange(long long int val) : lo(val), hi(val) {}

    CRange &operator=(const CRange &rhs) = default;

    bool operator==(const CRange &rhs) const {
        return tie(lo, hi) == tie(rhs.lo, rhs.hi);
    }

    bool operator!=(const CRange &rhs) const {
        return !(rhs == *this);
    }

    bool absorb_range(const CRange &pos) const {
//    ( range.lo > pos_h->hi ? range.lo == pos_h->hi + 1 : pos_h->lo <= range.hi || (pos_h->lo > range.hi && pos_h->lo == range.hi + 1))
        bool absorb_lower, absorb_higher;

        if (lo > pos.hi)
            absorb_lower = (lo == pos.hi + 1);
        else absorb_lower = (pos.lo <= hi);

        absorb_higher = (pos.lo > hi && pos.lo == hi + 1);

        return absorb_lower || absorb_higher;
    }

    friend ostream &operator<<(ostream &os, const CRange &range) {
        ios_base::fmtflags f(os.flags());
        os << dec;
        if (range.lo != range.hi)
            os << "<" << range.lo << ".." << range.hi << ">";
        else os << range.hi;
        os.flags(f);
        return os;
    }

    long long lo;
    long long hi;
};

class CRangeList {
public:
    // constructor
    CRangeList() = default;

    explicit CRangeList(const CRange &range) { rlist.push_back(range); }

    CRangeList(const initializer_list<CRange> &list) {
        for (const auto &range: list)
            *this += range;
    }

    CRangeList &operator+=(const CRange &range) {
        // find first range, where low of the new interval is smaller or equal to the found high
        auto pos_l = lower_bound(rlist.begin(), rlist.end(), range,
                                 [](auto const &p,
                                    auto const &r) { return r.lo > p.hi; });
        // check the previous interval (if there is any) for compatibility
        //                     range.lo > (pos_l-1)->hi
        if (!beg(pos_l) && range.lo <= (pos_l - 1)->hi + 1) --pos_l;
        // when new interval should be at the end or before the beginning
        if (pos_l == rlist.end() || (rlist.front().lo > range.hi && rlist.front().lo > range.hi + 1)) {
            rlist.emplace(pos_l, range);
            return *this;
        }
        // find all intervals that should be merged
        auto pos_h = pos_l;
        while (pos_h != rlist.end() && range.absorb_range(*pos_h)) { ++pos_h; }
        --pos_h;
//         weird way to say: take smaller low and bigger high
//        CRange n(pos_l->lo <= range.lo ? pos_l->lo : range.lo, range.hi >= pos_h->hi ? range.hi : pos_h->hi); that is just a reminder for me, how stupid am I
        CRange n(min(pos_l->lo, range.lo), max(range.hi, pos_h->hi));
        auto pos = rlist.erase(pos_l, pos_h + 1);
        rlist.emplace(pos, n);

        return *this;

    }

    CRangeList &operator+=(const CRangeList &list) {
        for (auto &&range: list.rlist)
            *this += range;
        return *this;
    }


    CRangeList &operator-=(const CRange &range) {
        // there is no list || there aren't any intervals to delete from
        if (rlist.empty() || range.hi < rlist.front().lo || range.lo > rlist.back().hi) return *this;
        // find first range, where low of the new interval is smaller or equal to the found high
        auto pos_l = lower_bound(rlist.begin(), rlist.end(), range,
                                 [](auto const &p,
                                    auto const &r) { return r.lo > p.hi; });

        auto pos_h = pos_l;
        //                                                  && range.lo <= pos_h->hi) redundant?
        while (pos_h != rlist.end() && (range.lo >= pos_h->lo || range.hi >= pos_h->lo)) { ++pos_h; }
        --pos_h;

        if (pos_h == pos_l) {
            if (pos_h->lo < range.lo) {
                auto tmp = pos_h->hi;
                pos_h->hi = range.lo - 1;
                if (range.hi < tmp)
                    *this += CRange(range.hi + 1, tmp);
            } else {
                if (range.hi < LLONG_MAX)
                    pos_h->lo = range.hi + 1;
                else pos_h->lo = range.hi;
                if (range.hi >= pos_h->hi)
                    rlist.erase(pos_h);
            }
            return *this;
        }

        if (range.lo <= pos_l->lo) {
            if (range.hi < pos_h->hi) {
                pos_h->lo = range.hi + 1;
                rlist.erase(pos_l, pos_h);
            } else {
                rlist.erase(pos_l, pos_h + 1);
            }
        } else {
            pos_l->hi = range.lo - 1;
            if (range.hi < pos_h->hi) {
                pos_h->lo = range.hi + 1;
                rlist.erase(pos_l + 1, pos_h);
            } else {
                rlist.erase(pos_l + 1, pos_h + 1);
            }
        }
        return *this;
    }

    // -= range / range list
    CRangeList &operator-=(const CRangeList &list) {
        for (auto &&range: list.rlist)
            *this -= range;
        return *this;
    }

//     = range / range list
    CRangeList &operator=(const CRange &range) {
        *this = CRangeList(range);
        return *this;
    }

    // operator ==
    bool operator==(const CRangeList &rhs) const {
        if (rlist.size() != rhs.rlist.size()) return false;
        for (size_t i = 0; i < rlist.size(); ++i) {
            if (rlist[i] != rhs.rlist[i]) return false;
        }
        return true;
    }

    // operator !=
    bool operator!=(const CRangeList &rhs) const {
        return !(rhs == *this);
    }

    //includes
    bool includes(long long const &num) const {
        auto pos = lower_bound(rlist.begin(), rlist.end(), num,
                               [](auto const &a,
                                  auto const &b) { return b > a.hi; });
        return pos != rlist.end() && num >= pos->lo && num <= pos->hi;
    }

    bool includes(CRange const &range) const {
        auto pos = lower_bound(rlist.begin(), rlist.end(), range,
                               [](auto const &a,
                                  auto const &b) { return b.lo > a.hi; });
        return pos != rlist.end() && range.lo >= pos->lo && range.hi <= pos->hi;
    }

    // operator <<
    friend ostream &operator<<(ostream &os, const CRangeList &list) {
        ios_base::fmtflags f(os.flags());
        os << dec;
        os << "{";
        for (auto &&range: list.rlist) {
            os << range;
            if (&range != &list.rlist.back())
                os << ",";
        }
        os << "}";
        os.flags(f);
        return os;
    }

    // for each
    auto begin() const {
        return rlist.begin();
    }

    auto end() const {
        return rlist.end();
    }

private:
    bool beg(vector<CRange>::iterator const &it) const { return it == rlist.begin(); }

    vector<CRange> rlist;
};


CRangeList operator+(const CRange &a, const CRange &b) {
    CRangeList c = CRangeList(a);
    return c += b;
}

CRangeList operator+(const CRangeList &a, const CRange &b) {
    CRangeList c(a);
    return c += b;
}

CRangeList operator-(const CRange &a, const CRange &b) {
    CRangeList c = CRangeList(a);
    return c -= b;
}

CRangeList operator-(const CRangeList &a, const CRange &b) {
    CRangeList c(a);
    return c -= b;
}

#ifndef __PROGTEST__

string toString(const CRangeList &x) {
    ostringstream oss;
    oss << x;
    return oss.str();
}

int main() {
    CRangeList a, b, d;




//    cout << LLONG_MAX << endl;
//    cout << LLONG_MAX - 1 << endl;
    a = CRange(LLONG_MAX);
    cout << a << endl;
    a += CRange(LLONG_MIN, LLONG_MAX);
    cout << a << endl;
    a -= CRange(LLONG_MIN + 5, LLONG_MAX - 5) + CRange(LLONG_MAX - 2, LLONG_MAX - 1);
    cout << a << endl;
//    a = CRange(LLONG_MAX);
//    a += CRange(LLONG_MIN, LLONG_MAX - 1);
//    cout << a << endl;
//    a = CRange(LLONG_MAX);
//    a -= CRange(LLONG_MAX);
//    a += CRange(LLONG_MIN, LLONG_MAX - 2);
//    cout << a << endl;

    cout << "mega yaaaaay" << endl;
#ifdef EXTENDED_SYNTAX
    CRangeList x{{5,   20},
                 {150, 200},
                 {-9,  12},
                 {48,  93}};
    assert (toString(x) == "{<-9..20>,<48..93>,<150..200>}");
    ostringstream oss;
    oss << setfill('=') << hex << left;
    for (const auto &v: x + CRange(-100, -100))
        oss << v << endl;
    oss << setw(10) << 1024;
    assert (oss.str() == "-100\n<-9..20>\n<48..93>\n<150..200>\n400=======");
    cout << "extended yaaaaay" << endl;
#endif /* EXTENDED_SYNTAX */
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
