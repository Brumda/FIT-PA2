#ifndef __PROGTEST__

#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
#include <iterator>
#include <stdexcept>

using namespace std;
#endif /* __PROGTEST__ */

template<typename M_>
class CContest {
    struct pairCmp {
        bool operator()(const pair<string, string> &a, const pair<string, string> &b) const {
            string a1 = min(a.first, a.second);
            string a2 = max(a.first, a.second);
            string b1 = min(b.first, b.second);
            string b2 = max(b.first, b.second);
            return a1 != b1 ? a1 < b1 : a2 < b2;
        }
    };

    struct Node {
        string name;
        vector<Node *> wonOver;
        int inDegree = 0;
    };
    map<pair<string, string>, M_, pairCmp> matches;
public:
    CContest() = default;

    ~CContest() = default;

    CContest &addMatch(string contestant1, string contestant2, const M_ &result) {
        auto placed = matches.try_emplace({std::move(contestant1), std::move(contestant2)}, result).second;
        if (!placed) throw logic_error("Match already entered!");
        return *this;
    }

    unordered_map<string, Node> make_graph(function<int(M_)> comparator) const {
        unordered_map<string, Node> graph;
        for (const auto &match: matches) {
            const auto type = comparator(match.second);
            const auto first = match.first.first;
            const auto second = match.first.second;

            if (type == 0)
                return {};
            else if (type < 0) {
                graph[first].inDegree++;
                graph[first].name = first;
                graph[second].name = second;
                graph[second].wonOver.push_back(&graph[first]);
            } else if (type > 0) {
                graph[second].inDegree++;
                graph[second].name = second;
                graph[first].name = first;
                graph[first].wonOver.push_back(&graph[second]);
            }
        }
        return graph;
    }

    bool isOrdered(function<int(M_)> comparator) const {
        auto graph = make_graph(comparator);
        if (graph.empty()) return false;

        queue<Node> q;
        for (const auto &node: graph) { if (node.second.inDegree == 0) q.push(node.second); }
        if (q.size() > 1) return false;

        while (!q.empty()) {
            auto node = q.front();
            q.pop();
            for (auto &el: node.wonOver) {
                el->inDegree--;
                if (el->inDegree == 0) q.push(*el);
                if (q.size() > 1) return false;
            }
        }
        return all_of(graph.begin(), graph.end(), [](const auto &node) { return node.second.inDegree == 0; });
    }

    list<string> results(function<int(M_)> comparator) const {
        list<string> res;
        auto graph = make_graph(comparator);
        if (graph.empty()) throw logic_error("Can't make the list");

        queue<Node> q;
        for (const auto &node: graph) { if (node.second.inDegree == 0) q.push(node.second); }
        if (q.size() > 1) throw logic_error("Can't make the list");

        while (!q.empty()) {
            auto node = q.front();
            q.pop();
            res.push_back(node.name);
            for (auto &el: node.wonOver) {
                el->inDegree--;
                if (el->inDegree == 0) q.push(*el);
                if (q.size() > 1) throw logic_error("Can't make the list");
            }
        }

        if (!all_of(graph.begin(), graph.end(), [](const auto &node) { return node.second.inDegree == 0; }))
            throw logic_error("Can't make the list");
        return res;
    }

};

#ifndef __PROGTEST__

struct CMatch {
public:
    CMatch(int a,
           int b)
            : m_A(a),
              m_B(b) {
    }

    int m_A;
    int m_B;
};

class HigherScoreThreshold {
public:
    HigherScoreThreshold(int diffAtLeast)
            : m_DiffAtLeast(diffAtLeast) {
    }

    int operator()(const CMatch &x) const {
        return (x.m_A > x.m_B + m_DiffAtLeast) - (x.m_B > x.m_A + m_DiffAtLeast);
    }

private:
    int m_DiffAtLeast;
};

int HigherScore(const CMatch &x) {
    return (x.m_A > x.m_B) - (x.m_B > x.m_A);
}

int main() {
    CContest<CMatch> x;

    x.addMatch("C++", "Pascal", CMatch(10, 3))
            .addMatch("C++", "Java", CMatch(8, 1))
            .addMatch("Pascal", "Basic", CMatch(40, 0))
            .addMatch("Java", "PHP", CMatch(6, 2))
            .addMatch("Java", "Pascal", CMatch(7, 3))
            .addMatch("PHP", "Basic", CMatch(10, 0));


    assert(!x.isOrdered(HigherScore));
    try {
        list<string> res = x.results(HigherScore);
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e) {
    }
    catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    x.addMatch("PHP", "Pascal", CMatch(3, 6));

    assert(x.isOrdered(HigherScore));
    try {
        list<string> res = x.results(HigherScore);
        assert((res == list<string>{"C++", "Java", "Pascal", "PHP", "Basic"}));
    }
    catch (...) {
        assert("Unexpected exception!" == nullptr);
    }


    assert(!x.isOrdered(HigherScoreThreshold(3)));
    try {
        list<string> res = x.results(HigherScoreThreshold(3));
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e) {
    }
    catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    assert(x.isOrdered([](const CMatch &x) {
        return (x.m_A < x.m_B) - (x.m_B < x.m_A);
    }));
    try {
        list<string> res = x.results([](const CMatch &x) {
            return (x.m_A < x.m_B) - (x.m_B < x.m_A);
        });
        assert((res == list<string>{"Basic", "PHP", "Pascal", "Java", "C++"}));
    }
    catch (...) {
        assert("Unexpected exception!" == nullptr);
    }

    CContest<bool> y;

    y.addMatch("Python", "PHP", true)
            .addMatch("PHP", "Perl", true)
            .addMatch("Perl", "Bash", true)
            .addMatch("Bash", "JavaScript", true)
            .addMatch("JavaScript", "VBScript", true);

    assert(y.isOrdered([](bool v) {
        return v ? 10 : -10;
    }));
    try {
        list<string> res = y.results([](bool v) {
            return v ? 10 : -10;
        });
        assert((res == list<string>{"Python", "PHP", "Perl", "Bash", "JavaScript", "VBScript"}));
    }
    catch (...) {
        assert("Unexpected exception!" == nullptr);
    }

    y.addMatch("PHP", "JavaScript", false);
    assert(!y.isOrdered([](bool v) {
        return v ? 10 : -10;
    }));
    try {
        list<string> res = y.results([](bool v) {
            return v ? 10 : -10;
        });
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e) {
    }
    catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    try {
        y.addMatch("PHP", "JavaScript", false);
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e) {
    }
    catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }

    try {
        y.addMatch("JavaScript", "PHP", true);
        assert("Exception missing!" == nullptr);
    }
    catch (const logic_error &e) {
    }
    catch (...) {
        assert("Invalid exception thrown!" == nullptr);
    }
    cout << "gamin" << endl;
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
