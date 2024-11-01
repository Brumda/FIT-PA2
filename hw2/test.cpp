#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <memory>

using namespace std;
#endif /* __PROGTEST__ */

class CPersonalAgenda {
public:
    CPersonalAgenda() = default;

    ~CPersonalAgenda() = default;

    bool add(const string &name, const string &surname, const string &email, unsigned int salary) {
        auto person = Person(name, surname, email, salary);
        auto name_pos = lower_name(person);
        auto email_pos = lower_email(email);
        if (!names.empty()) {
            if (name_pos != names.end() && cmp_names(person, *name_pos)) return false;
            if (email_pos != emails.end() && email == email_pos->email) return false;
        }
        names.emplace(name_pos, person);
        emails.emplace(email_pos, person);
        salaries.emplace(lower_salary(salary), salary);
        return true;

    }

    bool del(const string &name,
             const string &surname) {
        Person dummy = Person(name, surname, "", 0);
        auto name_pos = lower_name(dummy);
        if (name_pos == names.end() || !cmp_names(*name_pos, dummy)) return false;

        emails.erase(lower_email(name_pos->email));
        salaries.erase(lower_salary(name_pos->salary));
        names.erase(name_pos);

        return true;

    }

    bool del(const string &email) {
        auto email_pos = lower_email(email);
        if (email_pos == emails.end() || email_pos->email != email) return false;

        names.erase(lower_name(*email_pos));
        salaries.erase(lower_salary(email_pos->salary));
        emails.erase(email_pos);

        return true;

    }

    bool changeName(const string &email, const string &newName, const string &newSurname) {

        auto email_pos = lower_email(email);
        if (email_pos == emails.end() || email_pos->email != email) return false;

        Person dummy = Person(newName, newSurname, email, email_pos->salary);

        auto nn_pos = lower_name(dummy);
        if (nn_pos != names.end() && cmp_names(*nn_pos, dummy)) return false;

        names.erase(lower_name(*email_pos));
        names.emplace(lower_name(dummy), dummy);

        email_pos->name = newName;
        email_pos->surname = newSurname;
        return true;
    }

    bool changeEmail(const string &name, const string &surname, const string &newEmail) {
        Person dummy = Person(name, surname, newEmail, 0);
        auto name_pos = lower_name(dummy);
        if (name_pos == names.end() || !cmp_names(*name_pos, dummy)) return false;

        dummy.salary = name_pos->salary;

        auto ne_pos = lower_email(newEmail);
        if (ne_pos != emails.end() && ne_pos->email == newEmail) return false;

        emails.erase(lower_email(name_pos->email));
        emails.emplace(lower_email(newEmail), dummy);
        name_pos->email = newEmail;

        return true;
    }

    bool setSalary(const string &name, const string &surname, unsigned int salary) {
        Person dummy = Person(name, surname, "", 0);
        auto name_pos = lower_name(dummy);
        if (name_pos == names.end() || !cmp_names(*name_pos, dummy)) return false;

        salaries.erase(lower_salary(name_pos->salary));
        salaries.emplace(lower_salary(salary), salary);

        name_pos->salary = salary;
        lower_email(name_pos->email)->salary = salary;
        return true;
    }

    bool setSalary(const string &email,
                   unsigned int salary) {
        auto email_pos = lower_email(email);
        if (email_pos == emails.end() || email_pos->email != email) return false;

        salaries.erase(lower_salary(email_pos->salary));
        salaries.emplace(lower_salary(salary), salary);

        email_pos->salary = salary;
        lower_name(*email_pos)->salary = salary;
        return true;
    }

    unsigned int getSalary(const string &name, const string &surname) const {
        auto name_pos = lower_bound(names.begin(), names.end(), pair(name, surname),
                                    [](auto const &a, auto const &b) {
                                        return (a.surname == b.second ? a.name < b.first : a.surname < b.second);
                                    });
        if (name_pos == names.end() || name_pos->name != name || name_pos->surname != surname) return 0;
        return name_pos->salary;
    }

    unsigned int getSalary(const string &email) const {
        Person dummy = Person("", "surname", email, 0);
        auto email_pos = lower_bound(emails.begin(), emails.end(), dummy,
                                     [](auto const &a, auto const &b) { return (a.email < b.email); });

        if (email_pos == emails.end() || email_pos->email != email) return 0;
        return email_pos->salary;
    }

    bool getRank(const string &name, const string &surname, int &rankMin, int &rankMax) const {
        auto name_pos = lower_bound(names.begin(), names.end(), pair(name, surname),
                                    [](auto const &a, auto const &b) {
                                        return (a.surname == b.second ? a.name < b.first : a.surname < b.second);
                                    });
        if (name_pos == names.end() || name_pos->name != name || name_pos->surname != surname) return false;
        auto l_pos = lower_bound(salaries.begin(), salaries.end(), name_pos->salary);
        auto u_pos = upper_bound(salaries.begin(), salaries.end(), name_pos->salary);
        rankMin = distance(salaries.begin(), l_pos);
        rankMax = distance(salaries.begin(), u_pos) - 1;
        return true;
    }

    bool getRank(const string &email, int &rankMin, int &rankMax) const {
        auto email_pos = lower_bound(emails.begin(), emails.end(), email,
                                     [](auto const &a,
                                        auto const &b) { return (a.email < b); });

        if (email_pos == emails.end() || email_pos->email != email) return false;
        auto l_pos = lower_bound(salaries.begin(), salaries.end(), email_pos->salary);
        auto u_pos = upper_bound(salaries.begin(), salaries.end(), email_pos->salary);
        rankMin = distance(salaries.begin(), l_pos);
        rankMax = distance(salaries.begin(), u_pos) - 1;
        return true;
    }

    bool getFirst(string &outName, string &outSurname) const {
        if (names.empty()) return false;
        outName = names[0].name;
        outSurname = names[0].surname;
        return true;
    }

    bool getNext(const string &name, const string &surname, string &outName, string &outSurname) const {
        auto name_pos = lower_bound(names.begin(), names.end(), pair(name, surname),
                                    [](auto const &a, auto const &b) {
                                        return (a.surname == b.second ? a.name < b.first : a.surname < b.second);
                                    });
        if (name_pos >= names.end() - 1 || name_pos->name != name || name_pos->surname != surname) return false;
        outName = (++name_pos)->name;
        outSurname = name_pos->surname;

        return true;
    }

private:
    struct Person {
        Person(string name, string surname, string email,
               const unsigned int &salary) : name(std::move(name)), surname(std::move(surname)),
                                             email(std::move(email)), salary(salary) {}

        string name;
        string surname;
        string email;
        unsigned int salary;

    };

    vector<Person> names;
    vector<Person> emails;
    vector<unsigned int> salaries;

    vector<Person>::iterator lower_name(Person &person) {
        return lower_bound(names.begin(), names.end(), person,
                           [](auto const &a, auto const &b) {
                               return (a.surname == b.surname ? a.name < b.name : a.surname < b.surname);
                           });
    }

    vector<Person>::iterator lower_email(const string &email) {
        return lower_bound(emails.begin(), emails.end(), email,
                           [](auto const &a,
                              auto const &b) { return (a.email < b); });
    }


    vector<unsigned int>::iterator lower_salary(const unsigned int &salary) {
        return lower_bound(salaries.begin(), salaries.end(), salary);
    }

    static bool cmp_names(const Person &a, const Person &b) {
        return a.surname == b.surname && a.name == b.name;
    }
};

#ifndef __PROGTEST__

int main() {
    string outName, outSurname;
    int lo, hi;

    CPersonalAgenda b1;
    assert (b1.add("John", "Smith", "john", 30000));
    assert (b1.add("John", "Miller", "johnm", 35000));
    assert (b1.add("Peter", "Smith", "peter", 23000));
    assert (b1.getFirst(outName, outSurname)
            && outName == "John"
            && outSurname == "Miller");
    assert (b1.getNext("John", "Miller", outName, outSurname)
            && outName == "John"
            && outSurname == "Smith");
    assert (b1.getNext("John", "Smith", outName, outSurname)
            && outName == "Peter"
            && outSurname == "Smith");
    assert (!b1.getNext("Peter", "Smith", outName, outSurname));
    assert (b1.setSalary("john", 32000));
    assert (b1.getSalary("john") == 32000);
    assert (b1.getSalary("John", "Smith") == 32000);
    assert (b1.getRank("John", "Smith", lo, hi)
            && lo == 1
            && hi == 1);
    assert (b1.getRank("john", lo, hi)
            && lo == 1
            && hi == 1);
    assert (b1.getRank("peter", lo, hi)
            && lo == 0
            && hi == 0);
    assert (b1.getRank("johnm", lo, hi)
            && lo == 2
            && hi == 2);
    assert (b1.setSalary("John", "Smith", 35000));
    assert (b1.getSalary("John", "Smith") == 35000);
    assert (b1.getSalary("john") == 35000);
    assert (b1.getRank("John", "Smith", lo, hi)
            && lo == 1
            && hi == 2);
    assert (b1.getRank("john", lo, hi)
            && lo == 1
            && hi == 2);
    assert (b1.getRank("peter", lo, hi)
            && lo == 0
            && hi == 0);
    assert (b1.getRank("johnm", lo, hi)
            && lo == 1
            && hi == 2);
    assert (b1.changeName("peter", "James", "Bond"));
    assert (b1.getSalary("peter") == 23000);
    assert (b1.getSalary("James", "Bond") == 23000);
    assert (b1.getSalary("Peter", "Smith") == 0);
    assert (b1.getFirst(outName, outSurname)
            && outName == "James"
            && outSurname == "Bond");
    assert (b1.getNext("James", "Bond", outName, outSurname)
            && outName == "John"
            && outSurname == "Miller");
    assert (b1.getNext("John", "Miller", outName, outSurname)
            && outName == "John"
            && outSurname == "Smith");
    assert (!b1.getNext("John", "Smith", outName, outSurname));
    assert (b1.changeEmail("James", "Bond", "james"));
    assert (b1.getSalary("James", "Bond") == 23000);
    assert (b1.getSalary("james") == 23000);
    assert (b1.getSalary("peter") == 0);
    assert (b1.del("james"));
    assert (b1.getRank("john", lo, hi)
            && lo == 0
            && hi == 1);
    assert (b1.del("John", "Miller"));
    assert (b1.getRank("john", lo, hi)
            && lo == 0
            && hi == 0);
    assert (b1.getFirst(outName, outSurname)
            && outName == "John"
            && outSurname == "Smith");
    assert (!b1.getNext("John", "Smith", outName, outSurname));
    assert (b1.del("john"));
    assert (!b1.getFirst(outName, outSurname));
    assert (b1.add("John", "Smith", "john", 31000));
    assert (b1.add("john", "Smith", "joHn", 31000));
    assert (b1.add("John", "smith", "jOhn", 31000));
    cout << "===============================<B1 done>===============================" << endl;
    CPersonalAgenda b2;
    assert (!b2.getFirst(outName, outSurname));
    assert (b2.add("James", "Bond", "james", 70000));
    assert (b2.add("James", "Smith", "james2", 30000));
    assert (b2.add("Peter", "Smith", "peter", 40000));
    assert (!b2.add("James", "Bond", "james3", 60000));
    assert (!b2.add("Peter", "Bond", "peter", 50000));
    assert (!b2.changeName("joe", "Joe", "Black"));
    assert (!b2.changeEmail("Joe", "Black", "joe"));
    assert (!b2.setSalary("Joe", "Black", 90000));
    assert (!b2.setSalary("joe", 90000));
    assert (b2.getSalary("Joe", "Black") == 0);
    assert (b2.getSalary("joe") == 0);
    assert (!b2.getRank("Joe", "Black", lo, hi));
    assert (!b2.getRank("joe", lo, hi));
    assert (!b2.changeName("joe", "Joe", "Black"));
    assert (!b2.changeEmail("Joe", "Black", "joe"));
    assert (!b2.del("Joe", "Black"));
    assert (!b2.del("joe"));
    assert (!b2.changeName("james2", "James", "Bond"));
    assert (!b2.changeEmail("Peter", "Smith", "james"));
    assert (!b2.changeName("peter", "Peter", "Smith"));
    assert (!b2.changeEmail("Peter", "Smith", "peter"));
    assert (b2.del("Peter", "Smith"));
    assert (!b2.changeEmail("Peter", "Smith", "peter2"));
    assert (!b2.setSalary("Peter", "Smith", 35000));
    assert (b2.getSalary("Peter", "Smith") == 0);
    assert (!b2.getRank("Peter", "Smith", lo, hi));
    assert (!b2.changeName("peter", "Peter", "Falcon"));
    assert (!b2.setSalary("peter", 37000));
    assert (b2.getSalary("peter") == 0);
    assert (!b2.getRank("peter", lo, hi));
    assert (!b2.del("Peter", "Smith"));
    assert (!b2.del("peter"));
    assert (b2.add("Peter", "Smith", "peter", 40000));
    assert (b2.getSalary("peter") == 40000);
    cout << "===============================<B2 done>===============================" << endl;

    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
