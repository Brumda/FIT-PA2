#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <iterator>
#include <functional>

using namespace std;

class CDate {
public:
    //---------------------------------------------------------------------------------------------
    CDate(int y, int m, int d) : m_Year(y), m_Month(m), m_Day(d) {
    }

    //---------------------------------------------------------------------------------------------
    int compare(const CDate &x) const {
        if (m_Year != x.m_Year)
            return m_Year - x.m_Year;
        if (m_Month != x.m_Month)
            return m_Month - x.m_Month;
        return m_Day - x.m_Day;
    }

    //---------------------------------------------------------------------------------------------
    int year() const {
        return m_Year;
    }

    //---------------------------------------------------------------------------------------------
    int month() const {
        return m_Month;
    }

    //---------------------------------------------------------------------------------------------
    int day() const {
        return m_Day;
    }

    //---------------------------------------------------------------------------------------------
    friend ostream &operator<<(ostream &os,
                               const CDate &x) {
        char oldFill = os.fill();
        return os << setfill('0') << setw(4) << x.m_Year << "-"
                  << setw(2) << static_cast<int> ( x.m_Month ) << "-"
                  << setw(2) << static_cast<int> ( x.m_Day )
                  << setfill(oldFill);
    }
    //---------------------------------------------------------------------------------------------
private:
    int16_t m_Year;
    int8_t m_Month;
    int8_t m_Day;
};

#endif /* __PROGTEST__ */

string transformString(string str, const bool &spaces) {
    if (spaces) {
        str.erase(unique(str.begin(), str.end(), [](char x, char y) { return x == ' ' && y == ' '; }), str.end());
        str.erase(0, str.find_first_not_of(' '));
        str.erase(str.find_last_not_of(' ') + 1);
    }
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

class CInvoice {
public:

    CInvoice(const CDate &date, string seller, string buyer, const unsigned int &amount, const double &vat) :
            seller_(std::move(seller)), buyer_(std::move(buyer)), date_(date), amount_(amount), vat_(vat) {}

//    ty fucking idiote ty sis to přejmenoval ...
    CDate date() const { return date_; }

    string seller() const { return seller_; } // I would delete this, but I'm scared progtest won't compile

    string buyer() const { return buyer_; }

    unsigned int amount() const { return amount_; }

    double vat() const { return vat_; }

    bool operator==(const CInvoice &rhs) const {
        return date_.compare(rhs.date()) == 0 &&
               transformString(seller_, true) == transformString(rhs.seller_, true) &&
               transformString(buyer_, true) == transformString(rhs.buyer_, true) &&
               amount_ == rhs.amount_ && vat_ == rhs.vat_;
    }

    friend ostream &operator<<(ostream &os, const CInvoice &invoice) {
        os << "order:" << invoice.order << "; date:" << invoice.date_
           << "; seller:" << invoice.seller_ << "; buyer:" << invoice.buyer_<< "; amount:" << invoice.amount_
           << "; vat:" << invoice.vat_;
        return os;
    }

    // public because I would need getter and setter
    int order = 0; // when did the invoice come
    string seller_, buyer_;
private:
    CDate date_;
    unsigned int amount_;
    double vat_;
};


class CSortOpt {
public:
    static const int BY_DATE = 0;
    static const int BY_BUYER = 1;
    static const int BY_SELLER = 2;
    static const int BY_AMOUNT = 3;
    static const int BY_VAT = 4;


    bool operator()(const CInvoice &lhs, const CInvoice &rhs) const {
        for (const auto &cmp: order) {
            switch (cmp.first) {
                case BY_DATE:
                    if (lhs.date().compare(rhs.date()) != 0) {
                        if (cmp.second) { return lhs.date().compare(rhs.date()) < 0; }
                        else { return lhs.date().compare(rhs.date()) > 0; }
                    }
                    break;
                case BY_BUYER:
                    if (transformString(lhs.buyer(), false) != transformString(rhs.buyer(), false)) {
                        if (cmp.second) {
                            return transformString(lhs.buyer(), false) < transformString(rhs.buyer(), false);
                        } else { return transformString(lhs.buyer(), false) > transformString(rhs.buyer(), false); }
                    }
                    break;
                case BY_SELLER:
                    if (transformString(lhs.seller(), false) != transformString(rhs.seller(), false)) {
                        if (cmp.second) {
                            return transformString(lhs.seller(), false) < transformString(rhs.seller(), false);
                        } else { return transformString(lhs.seller(), false) > transformString(rhs.seller(), false); }
                    }
                    break;
                case BY_AMOUNT:
                    if (lhs.amount() != rhs.amount()) {
                        if (cmp.second) { return lhs.amount() < rhs.amount(); }
                        else { return lhs.amount() > rhs.amount(); }
                    }
                    break;
                case BY_VAT:
                    if (lhs.vat() != rhs.vat()) {
                        if (cmp.second) { return (lhs.vat() < rhs.vat()); }
                        else { return (lhs.vat() > rhs.vat()); }
                    }
                    break;
            }
        }
        return lhs.order < rhs.order;
    }

    CSortOpt &addKey(int key, bool asc = true) {
        order.emplace_back(key, asc);
        return *this;
    }

private:
    vector<pair<int, bool>> order;
};

struct Company {
    explicit Company(string name) : name(std::move(name)) {}

    struct Invoice_cmp {
        bool operator()(const CInvoice &lhs, const CInvoice &rhs) const {
            if (lhs.date().compare(rhs.date()) != 0)
                return lhs.date().compare(rhs.date()) < 0;
            if (transformString(lhs.seller(), true) != transformString(rhs.seller(), true))
                return transformString(lhs.seller(), true) < transformString(rhs.seller(), true);
            if (transformString(lhs.buyer(), true) != transformString(rhs.buyer(), true))
                return transformString(lhs.buyer(), true) < transformString(rhs.buyer(), true);
            if (lhs.amount() != rhs.amount()) return lhs.amount() < rhs.amount();
            return lhs.vat() < rhs.vat();
        }
    };

    friend ostream &operator<<(ostream &os, const Company &company) {
        cout << "Issued:" << endl;
        for (const auto &el: company.issuedInvoices) {
            os << el << endl;
        }
        os << endl;
        cout << "Accepted:" << endl;
        for (const auto &el: company.acceptedInvoices) {
            os << el << endl;
        }
        os << endl;
        return os;
    }

    int numOfInvoices = 0; // number of invoices that went through the company
    string name;
    set<CInvoice, Invoice_cmp> issuedInvoices;
    set<CInvoice, Invoice_cmp> acceptedInvoices;
};


class CVATRegister {
public:
    CVATRegister() = default;

    void printCompany(const string &company) {
        auto it = companies.find(transformString(company, true));
        cout << it->second;
    }

    bool registerCompany(const string &name) {
        auto pos = companies.try_emplace(transformString(name, true), Company(name)).second;
        return pos;
    }

    static bool addInvoice(CInvoice &invoice, Company &company, const bool &accepted) {
        invoice.order = company.numOfInvoices++;
        bool check;
        if (accepted)
            check = company.acceptedInvoices.emplace(invoice).second;
        else
            check = company.issuedInvoices.emplace(invoice).second;
// probably could have just return check, but like this I keep track of the actual number of invoices inside the company
        if (!check) {
            --company.numOfInvoices;
            return false;
        }
        return true;
    }

    bool addIssued(const CInvoice &x) {
        if (transformString(x.buyer(), true) == transformString(x.seller(), true)) return false;
        auto itBuyer = companies.find(transformString(x.buyer(), true));
        if (itBuyer == companies.end()) return false;
        auto itSeller = companies.find(transformString(x.seller(), true));
        if (itSeller == companies.end()) return false;

        auto invoice(x);
        invoice.buyer_ = itBuyer->second.name;
        invoice.seller_ = itSeller->second.name;
        if (!addInvoice(invoice, itBuyer->second, false)) return false;
        addInvoice(invoice, itSeller->second, false);

        return true;
    }

    bool addAccepted(const CInvoice &x) {
        if (transformString(x.buyer(), true) == transformString(x.seller(), true)) return false;
        auto itBuyer = companies.find(transformString(x.buyer(), true));
        if (itBuyer == companies.end()) return false;
        auto itSeller = companies.find(transformString(x.seller(), true));
        if (itSeller == companies.end()) return false;

        auto invoice(x);
        invoice.buyer_ = itBuyer->second.name;
        invoice.seller_ = itSeller->second.name;
        if (!addInvoice(invoice, itBuyer->second, true)) return false;
        addInvoice(invoice, itSeller->second, true);

        return true;
    }

    bool delIssued(const CInvoice &x) {
        auto itBuyer = companies.find(transformString(x.buyer(), true));
        if (itBuyer == companies.end()) return false;

        auto invoiceB = itBuyer->second.issuedInvoices.find(x);
        if (invoiceB == itBuyer->second.issuedInvoices.end()) return false;

        // no need for checks, if the invoice wouldn't be there, we would've already return false
        auto itSeller = companies.find(transformString(x.seller(), true));

        auto invoiceS = itSeller->second.issuedInvoices.find(x);

        itBuyer->second.issuedInvoices.erase(invoiceB);
        itSeller->second.issuedInvoices.erase(invoiceS);
        return true;
    }

    bool delAccepted(const CInvoice &x) {
        auto itBuyer = companies.find(transformString(x.buyer(), true));
        if (itBuyer == companies.end()) return false;

        auto invoiceB = itBuyer->second.acceptedInvoices.find(x);
        if (invoiceB == itBuyer->second.acceptedInvoices.end()) return false;

        auto itSeller = companies.find(transformString(x.seller(), true));
        auto invoiceS = itSeller->second.acceptedInvoices.find(x);

        itBuyer->second.acceptedInvoices.erase(invoiceB);
        itSeller->second.acceptedInvoices.erase(invoiceS);
        return true;

    }

    list<CInvoice> unmatched(const string &name, const CSortOpt &sortBy) const {
        list<CInvoice> res{};
        set<CInvoice, CSortOpt> tmp(sortBy);
        auto company = companies.find(transformString(name, true));
        if (company == companies.end()) return res;

        auto invoicesA = company->second.acceptedInvoices;
        auto invoicesI = company->second.issuedInvoices;

        for (const auto &inv: invoicesI) {
            if (invoicesA.find(inv) == invoicesA.end())
                tmp.insert(inv);
        }
        for (const auto &inv: invoicesA) {
            if (invoicesI.find(inv) == invoicesI.end())
                tmp.insert(inv);

        }

        for (auto &x: tmp)
            res.emplace_back(x);

        return res;
    }

private:
    map<string, Company> companies;
};

#ifndef __PROGTEST__

bool equalLists(const list<CInvoice> &a, const list<CInvoice> &b) {
    return a == b;
}

int main() {
    CVATRegister r;
    assert (r.registerCompany("first Company"));
    assert (!r.registerCompany("  first  Company  "));
    assert (r.registerCompany("Second     Company"));
    assert (r.registerCompany("ThirdCompany, Ltd."));
    assert (r.registerCompany("Third Company, Ltd."));
    assert (!r.registerCompany("Third Company, Ltd."));
    assert (!r.registerCompany(" Third  Company,  Ltd.  "));
//    r.printMap();
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 100, 20)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 2), "FirSt Company", "Second Company ", 200, 30)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 100, 30)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 300, 30)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "Second Company ", "First Company", 300, 30)));
    assert (r.addIssued(CInvoice(CDate(2000, 1, 1), "Third  Company,  Ltd.", "  Second    COMPANY ", 400, 34)));
    assert (!r.addIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company ", 300, 30)));
    assert (!r.addIssued(CInvoice(CDate(2000, 1, 4), "First Company", "First   Company", 200, 30)));
    assert (!r.addIssued(CInvoice(CDate(2000, 1, 4), "Another Company", "First   Company", 200, 30)));
    assert (equalLists(r.unmatched("First Company", CSortOpt()
            .addKey(CSortOpt::BY_SELLER, true)
            .addKey(CSortOpt::BY_BUYER, false)
            .addKey(CSortOpt::BY_DATE, false)), list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                30.000000)
                               }));
    assert (equalLists(r.unmatched("First Company",
                                   CSortOpt().addKey(CSortOpt::BY_DATE, true).addKey(CSortOpt::BY_SELLER,
                                                                                     true).addKey(
                                           CSortOpt::BY_BUYER, true)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,
                                                30.000000)
                               }));
    assert (equalLists(r.unmatched("First Company",
                                   CSortOpt().addKey(CSortOpt::BY_VAT, true).addKey(CSortOpt::BY_AMOUNT,
                                                                                    true).addKey(
                                           CSortOpt::BY_DATE, true).addKey(CSortOpt::BY_SELLER, true).addKey(
                                           CSortOpt::BY_BUYER, true)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                30.000000)
                               }));
    assert (equalLists(r.unmatched("First Company", CSortOpt()),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                20.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                30.000000)
                               }));
    assert (equalLists(r.unmatched("second company", CSortOpt().addKey(CSortOpt::BY_DATE, false)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Third Company, Ltd.", "Second     Company", 400,
                                                34.000000)
                               }));
    assert (equalLists(r.unmatched("last company", CSortOpt().addKey(CSortOpt::BY_VAT, true)),
                       list<CInvoice>
                               {
                               }));
    assert (r.addAccepted(CInvoice(CDate(2000, 1, 2), "First Company", "Second Company ", 200, 30)));
    assert (r.addAccepted(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert (r.addAccepted(CInvoice(CDate(2000, 1, 1), "Second company ", "First Company", 300, 32)));
//    r.printCompany("First Company");
//    cout << s << endl;
    assert (equalLists(r.unmatched("First Company",
                                   CSortOpt().addKey(CSortOpt::BY_SELLER, true)
                                           .addKey(CSortOpt::BY_BUYER, true)
                                           .addKey(CSortOpt::BY_DATE, true)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                32.000000)
                               }));
    assert (!r.delIssued(CInvoice(CDate(2001, 1, 1), "First Company", "Second Company ", 200, 30)));
    assert (!r.delIssued(CInvoice(CDate(2000, 1, 1), "A First Company", "Second Company ", 200, 30)));
    assert (!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Hand", 200, 30)));
    assert (!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company", 1200, 30)));
    assert (!r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", "Second Company", 200, 130)));
    assert (r.delIssued(CInvoice(CDate(2000, 1, 2), "First Company", "Second Company", 200, 30)));
    assert (equalLists(r.unmatched("First Company",
                                   CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER,
                                                                                       true).addKey(
                                           CSortOpt::BY_DATE, true)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                32.000000)
                               }));

    assert (r.delAccepted(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert (equalLists(r.unmatched("First Company",
                                   CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER,
                                                                                       true).addKey(
                                           CSortOpt::BY_DATE, true)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Third Company, Ltd.", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                32.000000)
                               }));
    assert (r.delIssued(CInvoice(CDate(2000, 1, 1), "First Company", " Third  Company,  Ltd.   ", 200, 30)));
    assert (equalLists(r.unmatched("First Company",
                                   CSortOpt().addKey(CSortOpt::BY_SELLER, true).addKey(CSortOpt::BY_BUYER,
                                                                                       true).addKey(
                                           CSortOpt::BY_DATE, true)),
                       list<CInvoice>
                               {
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                20.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 100,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "first Company", "Second     Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 2), "first Company", "Second     Company", 200,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                30.000000),
                                       CInvoice(CDate(2000, 1, 1), "Second     Company", "first Company", 300,
                                                32.000000)
                               }));
    cout << "yaaaaaaaaaaaaaay" << endl;
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
