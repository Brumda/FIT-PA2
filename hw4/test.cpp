#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <utility>

using namespace std;
#endif /* __PROGTEST__ */

// poupravený array ze cvičení
template<typename T>
struct Array {
    Array() = default;

    Array(const Array&src) {
        for (size_t i = 0; i < src.size(); i++)
            this->push_back(src[i]);
    };

    Array &operator=(Array src) {
        swap(src);
        return *this;
    }

    ~Array() noexcept {
        delete[] _data;
        _data = nullptr;
    }

    Array(Array &&src) noexcept {
        swap(src);
    }

    void swap(Array &other) {
        std::swap(_size, other._size);
        std::swap(_cap, other._cap);
        std::swap(_data, other._data);
    }

    void push_back(const T &value) {
        if (_cap <= _size) grow(_cap * 2 + 100);
        _data[_size++] = std::move(value);
    }

    void insert(const T &val, const size_t &pos) {
        if (pos == _size) {
            push_back(val);
            return;
        }
        if (_cap <= _size) grow(_cap * 2 + 100);

        for (auto i = _size - 1; i >= pos; --i) {
            _data[i + 1] = std::move(_data[i]);
            if (i == 0) break;
        }
        _data[pos] = std::move(val);
        ++_size;
    }


    size_t size() const { return _size; }

    const T &operator[](size_t i) const {
        if (i >= _size) throw std::out_of_range("");
        return _data[i];
    }

    T &operator[](size_t i) {
        if (i >= _size) throw std::out_of_range("");
        return _data[i];
    }


private:
    void grow(size_t new_cap) {
        auto new_data = new T[new_cap];

        for (size_t i = 0; i < _size; i++)
            new_data[i] = std::move(_data[i]);

        delete[] _data;
        _data = new_data;
        _cap = new_cap;
    }

    size_t _size = 0, _cap = 0;
    T *_data = nullptr;
};

// proseminářem inspirovaný string
class CStr {
public:
    CStr() = default;

    explicit CStr(const char *str) : m_Len(strlen(str)), m_Capacity(m_Len + 1) {
        m_Str = new char[m_Capacity];
        memcpy(m_Str, str, m_Capacity);
    }

    ~CStr() noexcept {
        delete[] m_Str;
        m_Str = nullptr;
    }

    CStr(const CStr &src) : m_Len(src.m_Len), m_Capacity(src.m_Len + 1) {
        m_Str = new char[m_Capacity];
        memcpy(m_Str, src.m_Str, m_Capacity);
    }

    CStr(CStr &&src) noexcept {
        swap(src);
    }

    void swap(CStr &other) {
        std::swap(m_Len, other.m_Len);
        std::swap(m_Capacity, other.m_Capacity);
        std::swap(m_Str, other.m_Str);
    }

    CStr &operator=(CStr src) {
        swap(src);
        return *this;
    }


    bool operator==(const CStr &rhs) const {
        return strcmp(m_Str, rhs.m_Str) == 0;
    }

    bool operator!=(const CStr &rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const CStr &rhs) const {
        return strcmp(m_Str, rhs.m_Str) < 0;
    }

    friend ostream &operator<<(ostream &os, const CStr &str) {
        os << str.m_Str;
        return os;
    }

private:
    char *m_Str = nullptr;
    size_t m_Len = 0;
    size_t m_Capacity = 0;

};


class CMail {
public:
    CStr from, to, body;

    CMail() = default;

    CMail(const char *f,
          const char *t,
          const char *b) : from(f), to(t), body(b) {}

    bool operator==(const CMail &rhs) const {
        return from == rhs.from &&
               to == rhs.to &&
               body == rhs.body;
    }

    bool operator!=(const CMail &rhs) const {
        return !(rhs == *this);
    }

    friend ostream &operator<<(ostream &os, const CMail &mail) {
        os << "From: " << mail.from << ", To: " << mail.to << ", Body: " << mail.body;
        return os;
    }
};

struct person {
    person() = default;

    explicit person(CStr add) : email_add(std::move(add)) {}

    CStr email_add;
    Array<CMail> inbox, outbox;
};

class CMailIterator {
public:

    CMailIterator() = default;

    CMailIterator(const Array<CMail> &arr, const size_t &in) : array(arr), index(in) {}

    explicit operator bool() const { return index < array.size(); }

    bool operator!() const { return index >= array.size(); }

    const CMail &operator*() const { return array[index]; }

    CMailIterator &operator++() {
        ++index;
        return *this;
    }

private:
    Array<CMail> array;
    size_t index = 0;
};

// basically lower_bound from cppreference
size_t find(const Array<person> &arr, const CStr &target) {
    size_t count = arr.size(), step;
    size_t first = 0;
    while (count > 0) {
        size_t it = first;
        step = count / 2;
        it += step;
        if (arr[it].email_add < target) {
            first = ++it;
            count -= step + 1;
        } else
            count = step;
    }
    return first;
}

class CMailServer {
public:
    void sendMail(const CMail &m) {
        auto from_index = find(persons, m.from);
        if (from_index == persons.size() || persons[from_index].email_add != m.from)
            persons.insert(person(m.from), from_index);

        auto to_index = find(persons, m.to);
        if (to_index == persons.size() || persons[to_index].email_add != m.to)
            persons.insert(person(m.to), to_index);

        // literally made the same mistake as in PT 2, kill me xddd
        from_index = find(persons, m.from);
        to_index = find(persons, m.to);

        persons[from_index].outbox.push_back(m);
        persons[to_index].inbox.push_back(m);
    }

    CMailIterator outbox(const char *email) const {
        auto index = find(persons, CStr(email));
        return {persons[index].outbox, 0};
    }

    CMailIterator inbox(const char *email) const {
        auto index = find(persons, CStr(email));
        return {persons[index].inbox, 0};
    }

private:
    Array<person> persons;
};

#ifndef __PROGTEST__

bool matchOutput(const CMail &m,
                 const char *str) {

    ostringstream oss;
    oss << m;
    return oss.str() == str;
}

int main() {
    char from[100], to[100], body[1024];

    CMailServer s42069;
    s42069.sendMail(CMail("Poggies", "Poggies", "super mega cool mail"));

    CMailIterator i69 = s42069.inbox("Poggies");
    assert(i69 && *i69 == CMail("Poggies", "Poggies", "super mega cool mail"));

    CMailIterator i420 = s42069.outbox("Poggies");
    assert(i420 && *i420 == CMail("Poggies", "Poggies", "super mega cool mail"));

//    assert(s42069.persons.size() == 1);









    assert(CMail("john", "peter", "progtest deadline") == CMail("john", "peter", "progtest deadline"));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("john", "progtest deadline", "peter")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("peter", "john", "progtest deadline")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("peter", "progtest deadline", "john")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("progtest deadline", "john", "peter")));
    assert(!(CMail("john", "peter", "progtest deadline") == CMail("progtest deadline", "peter", "john")));
    CMailServer s0;
    s0.sendMail(CMail("john", "peter", "some important mail"));
    strncpy(from, "john", sizeof(from));
    strncpy(to, "thomas", sizeof(to));
    strncpy(body, "another important mail", sizeof(body));
    s0.sendMail(CMail(from, to, body));
    strncpy(from, "john", sizeof(from));
    strncpy(to, "alice", sizeof(to));
    strncpy(body, "deadline notice", sizeof(body));
    s0.sendMail(CMail(from, to, body));
    s0.sendMail(CMail("alice", "john", "deadline confirmation"));
    s0.sendMail(CMail("peter", "alice", "PR bullshit"));
    CMailIterator i0 = s0.inbox("alice");
    assert(i0 && *i0 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i0, "From: john, To: alice, Body: deadline notice"));
    assert(++i0 && *i0 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i0, "From: peter, To: alice, Body: PR bullshit"));
    assert(!++i0);

    CMailIterator i1 = s0.inbox("john");
    assert(i1 && *i1 == CMail("alice", "john", "deadline confirmation"));
    assert(matchOutput(*i1, "From: alice, To: john, Body: deadline confirmation"));
    assert(!++i1);

    CMailIterator i2 = s0.outbox("john");
    assert(i2 && *i2 == CMail("john", "peter", "some important mail"));
    assert(matchOutput(*i2, "From: john, To: peter, Body: some important mail"));
    assert(++i2 && *i2 == CMail("john", "thomas", "another important mail"));
    assert(matchOutput(*i2, "From: john, To: thomas, Body: another important mail"));
    assert(++i2 && *i2 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i2, "From: john, To: alice, Body: deadline notice"));
    assert(!++i2);

    CMailIterator i3 = s0.outbox("thomas");
    assert(!i3);

    CMailIterator i4 = s0.outbox("steve");
    assert(!i4);

    CMailIterator i5 = s0.outbox("thomas");
    s0.sendMail(CMail("thomas", "boss", "daily report"));
    assert(!i5);

    CMailIterator i6 = s0.outbox("thomas");
    assert(i6 && *i6 == CMail("thomas", "boss", "daily report"));
    assert(matchOutput(*i6, "From: thomas, To: boss, Body: daily report"));
    assert(!++i6);

    CMailIterator i7 = s0.inbox("alice");
    s0.sendMail(CMail("thomas", "alice", "meeting details"));
    assert(i7 && *i7 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i7, "From: john, To: alice, Body: deadline notice"));
    assert(++i7 && *i7 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i7, "From: peter, To: alice, Body: PR bullshit"));
    assert(!++i7);

    CMailIterator i8 = s0.inbox("alice");
    assert(i8 && *i8 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i8, "From: john, To: alice, Body: deadline notice"));
    assert(++i8 && *i8 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i8, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i8 && *i8 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i8, "From: thomas, To: alice, Body: meeting details"));
    assert(!++i8);

    CMailServer s1(s0);
    s0.sendMail(CMail("joe", "alice", "delivery details"));
    s1.sendMail(CMail("sam", "alice", "order confirmation"));
    CMailIterator i9 = s0.inbox("alice");
    assert(i9 && *i9 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i9, "From: john, To: alice, Body: deadline notice"));
    assert(++i9 && *i9 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i9, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i9 && *i9 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i9, "From: thomas, To: alice, Body: meeting details"));
    assert(++i9 && *i9 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i9, "From: joe, To: alice, Body: delivery details"));
    assert(!++i9);

    CMailIterator i10 = s1.inbox("alice");
    assert(i10 && *i10 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i10, "From: john, To: alice, Body: deadline notice"));
    assert(++i10 && *i10 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i10, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i10 && *i10 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i10, "From: thomas, To: alice, Body: meeting details"));
    assert(++i10 && *i10 == CMail("sam", "alice", "order confirmation"));
    assert(matchOutput(*i10, "From: sam, To: alice, Body: order confirmation"));
    assert(!++i10);

    CMailServer s2;
    s2.sendMail(CMail("alice", "alice", "mailbox test"));
    CMailIterator i11 = s2.inbox("alice");
    assert(i11 && *i11 == CMail("alice", "alice", "mailbox test"));
    assert(matchOutput(*i11, "From: alice, To: alice, Body: mailbox test"));
    assert(!++i11);

    s2 = s0;
    s0.sendMail(CMail("steve", "alice", "newsletter"));
    s2.sendMail(CMail("paul", "alice", "invalid invoice"));
    CMailIterator i12 = s0.inbox("alice");
    assert(i12 && *i12 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i12, "From: john, To: alice, Body: deadline notice"));
    assert(++i12 && *i12 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i12, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i12 && *i12 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i12, "From: thomas, To: alice, Body: meeting details"));
    assert(++i12 && *i12 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i12, "From: joe, To: alice, Body: delivery details"));
    assert(++i12 && *i12 == CMail("steve", "alice", "newsletter"));
    assert(matchOutput(*i12, "From: steve, To: alice, Body: newsletter"));
    assert(!++i12);

    CMailIterator i13 = s2.inbox("alice");
    assert(i13 && *i13 == CMail("john", "alice", "deadline notice"));
    assert(matchOutput(*i13, "From: john, To: alice, Body: deadline notice"));
    assert(++i13 && *i13 == CMail("peter", "alice", "PR bullshit"));
    assert(matchOutput(*i13, "From: peter, To: alice, Body: PR bullshit"));
    assert(++i13 && *i13 == CMail("thomas", "alice", "meeting details"));
    assert(matchOutput(*i13, "From: thomas, To: alice, Body: meeting details"));
    assert(++i13 && *i13 == CMail("joe", "alice", "delivery details"));
    assert(matchOutput(*i13, "From: joe, To: alice, Body: delivery details"));
    assert(++i13 && *i13 == CMail("paul", "alice", "invalid invoice"));
    assert(matchOutput(*i13, "From: paul, To: alice, Body: invalid invoice"));
    assert(!++i13);

    cout << "yaaay" << endl;
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
