#ifndef __PROGTEST__

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <array>
#include <cassert>

using namespace std;
#endif /* __PROGTEST__ */

class CTimeStamp {
public:
    CTimeStamp(int y, int mo, int d, int h, int mi, int s) : year(y), month(mo), day(d), hour(h), min(mi), sec(s) {}

    bool operator<=(const CTimeStamp &second) const {
        return tie(year, month, day, hour, min, sec) <=
               tie(second.year, second.month, second.day, second.hour,
                   second.min, second.sec);
    }

    bool operator>=(const CTimeStamp &second) const {
        return tie(year, month, day, hour, min, sec) >=
               tie(second.year, second.month, second.day, second.hour,
                   second.min, second.sec);
    }

private:
    int year, month, day, hour, min, sec;
};

class CContact {
public:
    int num1;
    int num2;

    CContact(CTimeStamp tim, int n1, int n2) : num1(n1), num2(n2), time(tim) {}

    bool error() const { return num1 == num2; }

    bool compare_time(const CTimeStamp &second, const string &str) const {
        if (str == "<=")
            return time <= second;
        if (str == ">=")
            return time >= second;
        return false;
    }

private:
    CTimeStamp time;
};

class CEFaceMask {
public:
    CEFaceMask() = default;

    CEFaceMask &addContact(CContact contact) {
        if (!contact.error())
            list.push_back(contact);
        return *this;
    }

    vector<int> listContacts(int num) const {
        vector<int> numbers;
        int new_contact;
        for (auto &&el: list) {
            if (el.num1 == num)
                new_contact = el.num2;
            else if (el.num2 == num)
                new_contact = el.num1;
            else continue;
            if (find(numbers.begin(), numbers.end(), new_contact) == numbers.end())
                numbers.push_back(new_contact);
        }
        return numbers;
    }

    vector<int> listContacts(int num, CTimeStamp st, CTimeStamp end) const {
        vector<int> numbers;
        int new_contact;
        for (auto &&el: list) {
            if (el.compare_time(st, ">=") && el.compare_time(end, "<=")) {
                if (el.num1 == num)
                    new_contact = el.num2;
                else if (el.num2 == num)
                    new_contact = el.num1;
                else continue;
                if (find(numbers.begin(), numbers.end(), new_contact) == numbers.end())
                    numbers.push_back(new_contact);
            }
        }
        return numbers;
    }

private:
    vector<CContact> list;
};

#ifndef __PROGTEST__

int main() {
    CEFaceMask test;

    test.addContact(CContact(CTimeStamp(2021, 1, 10, 12, 40, 10), 123456789, 999888777));
    test.addContact(CContact(CTimeStamp(2021, 1, 12, 12, 40, 10), 123456789, 111222333))
            .addContact(CContact(CTimeStamp(2021, 2, 5, 15, 30, 28), 999888777, 555000222));
    test.addContact(CContact(CTimeStamp(2021, 2, 21, 18, 0, 0), 123456789, 999888777));
    test.addContact(CContact(CTimeStamp(2021, 1, 5, 18, 0, 0), 123456789, 456456456));
    test.addContact(CContact(CTimeStamp(2021, 2, 1, 0, 0, 0), 123456789, 123456789));
    assert (test.listContacts(123456789) == (vector<int>{999888777, 111222333, 456456456}));
    assert (test.listContacts(999888777) == (vector<int>{123456789, 555000222}));
    assert (test.listContacts(191919191) == (vector<int>{}));
    assert (test.listContacts(123456789, CTimeStamp(2021, 1, 5, 18, 0, 0), CTimeStamp(2021, 2, 21, 18, 0, 0)) ==
            (vector<int>{999888777, 111222333, 456456456}));
    assert (test.listContacts(123456789, CTimeStamp(2021, 1, 5, 18, 0, 1), CTimeStamp(2021, 2, 21, 17, 59, 59)) ==
            (vector<int>{999888777, 111222333}));
    assert (test.listContacts(123456789, CTimeStamp(2021, 1, 10, 12, 41, 9), CTimeStamp(2021, 2, 21, 17, 59, 59)) ==
            (vector<int>{111222333}));
    cout << "=====================<yep>=====================" << endl;
    return 0;
}

#endif /* __PROGTEST__ */
