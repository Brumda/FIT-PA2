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

struct zaznam {
    string m_name;
    string m_surname;
    string m_number;
};

void result(const string &name, const vector<zaznam> &people, ostream &out) {
    int quantity = 0;
    for (auto &person: people) {
        if (name == person.m_name || name == person.m_surname) {
            quantity++;
            out << person.m_name << " " << person.m_surname << " " << person.m_number << endl;
        }
    }
    out << "-> " << quantity << endl;

}

bool is_digits(const string &str) { return all_of(str.begin(), str.end(), ::isdigit); }

vector<string> split(const string &str) {
    vector<string> v;
    string word = "";
    bool start = false;

    for (auto &s: str + ' ') {
        if (isspace(s)) {
            if (start) {
                v.push_back(word);
                word = "";
                start = false;
            }

        } else {
            start = true;
            word += s;
        }
    }
    return v;
}

bool report(const string &fileName, ostream &out) {
    ifstream file(fileName);
    vector<zaznam> people;
    string line;
    vector<string> buffer;
    bool dotazy = false;
    if (file.fail()) { return false; }

    while (getline(file, line)) {
        if (!dotazy && line.empty()) { dotazy = true; }

        buffer = split(line);

        if (!dotazy) {
            if (buffer.size() != 3) { return false; }
            zaznam guy;
            guy.m_name = buffer[0];
            guy.m_surname = buffer[1];
            guy.m_number = buffer[2];

            if (guy.m_number.length() != 9 || guy.m_number.rfind('0', 0) == 0 ||
                !is_digits(guy.m_number)) { return false; }
            people.push_back(guy);
        } else if (!buffer.empty())
            result(buffer[0], people, out);


    }
    file.close();
    return true;
}

#ifndef __PROGTEST__

int main() {
    ostringstream oss;
    oss.str("");
    assert (report("tests/test0_in.txt", oss) == true);
    assert (oss.str() ==
            "John Christescu 258452362\n"
            "John Harmson 861647702\n"
            "-> 2\n"
            "-> 0\n"
            "Josh Dakhov 264112084\n"
            "Dakhov Speechley 865216101\n"
            "-> 2\n"
            "John Harmson 861647702\n"
            "-> 1\n");
    oss.str("");
    assert (report("tests/test1_in.txt", oss) == false);
    cout << "yep" << endl;
    return 0;
}

#endif /* __PROGTEST__ */
