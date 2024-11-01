#ifndef __PROGTEST__

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <typeinfo>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <algorithm>
#include <type_traits>

using namespace std;

class CRect {
public:
    CRect(double x, double y, double w, double h) : m_X(x), m_Y(y), m_W(w), m_H(h) {
    }

    friend ostream &operator<<(ostream &os, const CRect &x) {
        return os << '(' << x.m_X << ',' << x.m_Y << ',' << x.m_W << ',' << x.m_H << ')';
    }

    double m_X;
    double m_Y;
    double m_W;
    double m_H;
};

#endif /* __PROGTEST__ */
/**
 * Writing this code was a journey on almost a spiritual level. I don't mean reading the questions about '|' on Discord
 * for a week before I even had the time to read the assignment. I mean the fact that I rewrote the whole "simple"
 * version about three times and the printing function alone about four times. Then the same with the extended version.
 * And once more just now. The toll it had on my will to live was higher than the ego of an average fuck-boy
 * in an American high school drama show.
 * This version isn't improved only by adding comments. The first working version was an abomination and hate crime
 * against anyone who knows even the slightest about programming. While writing the simple version of this blessed PT,
 * I made some dreadful design choices. When I started to write the extended version it was too late for me. I kept
 * digging myself deeper and deeper into the C++ hell. I almost ended up quitting about four times per hour (not just
 * the PT, but life itself). There were two voices in my head (there is usually much more than that). One was @Jace,
 * who kept telling me that the 2 points aren't worth it. The second one was @/dev/null, who kept telling me that it
 * is important. That it isn't about the points but the learning journey. I wasn't sure who represented the Angel
 * and who was the Devil, but in the end, I kept going. And I'm glad I did. Not only did I get myself +2.2 points,
 * but I also got a deeper understanding of class inheritance (or so I thought) and my limitations. Then it happened.
 * I passed all the asserts, and a wave of euphoria washed over me. The feeling was so intense, that I forgot to reread
 * my code and uploaded it to PT immediately. When it worked on the first attempt, I couldn't believe my own eyes.
 * I abandoned this code, to never be looked at by another living soul ever again. Unfortunately, -5 points from the
 * code review hurt my feelings too much, so I came back to make it not just functional but elegant as well (FYI,
 * did not work). The things I found here cannot be explained by words. (if you want to keep the last shreds of your
 * sanity intact, don't examine the first version too much). Not only did I do some truly horrific things like passing
 * ints and bools by const references instead of values, using copy constructor completely wrong etc., but my code
 * shouldn't have even worked in the first place. I was in shock once again and considered using some other assignment.
 * Unfortunately for all of us, the other ones were as bad, some even worse, and some didn't even work. So I sat down,
 * got 3 grams of caffeine in my bloodstream, got my last two brain cells semi-operational and began to fight the
 * heresies I made earlier. I passed out several times, and lord Vagner himself came down to me in a hallucination
 * and gave me advice on how to proceed. I finally represent to you my final version of this crime against humanity.
 * I hope your eyes won't burst out in streams of blood while reading it.
 */
/// Version 3 because I deleted const alongside the reference for ints
#define DBG_LINE string(80, '-')

/**
 * Abstract parent class for GUI elements
 */
class UIElement {
protected:
    int id;                                 ///< id of the element
    CRect relPos{1, 1, 1, 1}; ///< relative position of the element
    CRect absPos{0, 0, 0, 0}; ///< absolute position of the element
    bool containsOthers = false;            ///< bool if the element can contain other elements

    static CRect getAbsolute(const CRect &elementPos, const CRect &containerPos) {
        return CRect{containerPos.m_X + elementPos.m_X * containerPos.m_W,
                     containerPos.m_Y + elementPos.m_Y * containerPos.m_H,
                     elementPos.m_W * containerPos.m_W,
                     elementPos.m_H * containerPos.m_H};
    }

public:
    UIElement(const int id_, const CRect &relPos_) : id(id_), relPos(relPos_) {}

    virtual ~UIElement() noexcept = default;

    /**
     * Abstract method for printing the elements
     * @param os output stream
     * @param prefix string used to determine the indent and prefix
     */
    virtual void print(ostream &os, const string &prefix) const = 0;

    /**
     * @return deep copy of a unique pointer to the element
     */
    virtual unique_ptr<UIElement> clone() const = 0;

    /**
     * Sets absolute position of the element
     * @param newPosition new position of the container used to set/calculate the absolute position
     */
    virtual void setPosition(const CRect &newPosition) = 0;

    /**
     * searches for the element with id
     * @param id_
     * @return pointer to the element if id found, null pointer otherwise
     */
    virtual UIElement *search(int id_) = 0;

    bool ContainsOthers() const {
        return containsOthers;
    }

    /**
     * @param os output stream
     * @param element which element to print
     * @return filled output stream
     */
    friend ostream &operator<<(ostream &os, const UIElement &element) {
        element.print(os, "");
#ifndef __PROGTEST__
        element.print(cout, "");
        cout << DBG_LINE << endl;
#endif
        return os;
    }

    /**
     * Calculates absolute position from relative position of the element and absolute position of the container
     * @param elementPos relative position of element
     * @param containerPos absolute position of container
     * @return absolute position of the element
     */

};

class CWindow : public UIElement {
private:
    /**
     * Swaps all the variables of windows -> should call swap of super class!!
     * @param other window to swap from
     */
    void swap(CWindow &other) {
        std::swap(id, other.id);
        std::swap(relPos, other.relPos);
        std::swap(absPos, other.absPos);
        std::swap(containsOthers, other.containsOthers);
        std::swap(title, other.title);
        std::swap(elements, other.elements);
    }

protected:
    vector<unique_ptr<UIElement>> elements; ///< vector of unique pointers to the stored elements
    string title;                           ///< window title
public:
    /**
     * Window doesn't really have a relative position, but having more classes
     * to handle relative and absolute position separately seems worse than this (I may be wrong about this)
     */
    CWindow(const int id_, string title_, const CRect &absPos_) : UIElement(id_, {1, 1, 1, 1}),
                                                                  title(std::move(title_)) { absPos = absPos_; }

    ~CWindow() noexcept override = default;

    /**
     * Copy constructor makes deep copies of all the stored elements
     * @param src
     */
    CWindow(const CWindow &src) : UIElement(src), title(src.title) {
        for (const auto &el: src.elements) {
            elements.push_back(el->clone());
        }
    }

    CWindow &operator=(CWindow src) {
        swap(src);
        return *this;
    }

    /**
     * @inherit
     */
    unique_ptr<UIElement> clone() const override {
        return make_unique<CWindow>(*this);
    }

    /**
     * Clones element to the window and calculates its absolute position
     * @param element
     * @return self
     */
    CWindow &add(const UIElement &element) {
        auto el = element.clone();
        el->setPosition(absPos);
        elements.push_back(std::move(el));
        return *this;
    }

    /**
     * Changes position of window and recalculates absolute position of all the elements inside
     * @param newPosition
     */
    void setPosition(const CRect &newPosition) override {
        absPos = newPosition;
        for (auto &el: elements)
            el->setPosition(absPos);

    }

    /**
     * Searches for id in all stored elements
     * @param id_
     * @return first element with given id if found, nullptr otherwise
     */
    UIElement *search(const int id_) override {
        if (id_ == id) return this;
        for (const auto &el: elements) {
            auto found = el->search(id_);
            if (found) return found;
        }
        return nullptr;
    }

    /**
    * Prints self - no prefix needed since window is always first
    * Iterates through stored elements, gives them prefix based on ASCII tree requirements
    * @param os output stream
    * @param prefix string used to determine the indent and prefix
    */
    void print(ostream &os, const string &prefix) const override {
        os << "[" << id << "]" << " Window " << "\"" << title << "\" " << absPos << endl;
        for (const auto &el: elements) {
            if (el->ContainsOthers()) {
                if (el == elements.back())
                    el->print(os, "   ");
                else
                    el->print(os, "|  ");

            } else
                el->print(os, "+- ");

        }
    }
};

class CPanel : public UIElement {
private:
    /**
     * Swaps all the variables of panels -> should call swap of super class!!
     * @param other panel to swap from
     */
    void swap(CPanel &other) {
        std::swap(id, other.id);
        std::swap(relPos, other.relPos);
        std::swap(absPos, other.absPos);
        std::swap(containsOthers, other.containsOthers);
        std::swap(elements, other.elements);
    }

protected:
    vector<unique_ptr<UIElement>> elements; ///< vector of unique pointers to the stored elements
public:
    CPanel(const int id, const CRect &relPos_) : UIElement(id, relPos_) {
        containsOthers = true;
    }

    ~CPanel() noexcept override = default;

    /**
     * Copy constructor makes deep copies of all the stored elements
     * @param src
     */
    CPanel(const CPanel &src) : UIElement(src) {
        for (const auto &el: src.elements)
            elements.push_back(el->clone());

    }

    CPanel &operator=(CPanel src) {
        swap(src);
        return *this;
    }

    /**
     * Clones elements to the panel and calculates its absolute position
     * @param element
     * @return self
     */
    CPanel &add(const UIElement &element) {
        auto el = element.clone();
        el->setPosition(absPos);
        elements.push_back(std::move(el));
        return *this;
    }

    /**
     * Changes position of panel and recalculates absolute position of all the elements inside
     * @param newPosition
     */
    void setPosition(const CRect &newPosition) override {
        absPos = getAbsolute(relPos, newPosition);
        for (auto &el: elements)
            el->setPosition(absPos);

    }

    /**
     * Searches for id in all stored elements
     * @param id_
     * @return first element with given id if found, nullptr otherwise
     */
    UIElement *search(const int id_) override {
        if (id_ == id) return this;
        for (const auto &el: elements) {
            auto found = el->search(id_);
            if (found) return found;
        }
        return nullptr;
    }

    unique_ptr<UIElement> clone() const override {
        return make_unique<CPanel>(*this);
    }

    /**
    * Prints the prefix without the last 3 characters since those should always be '+- ' and then itself.
    * Iterates through stored elements, gives them prefix based on ASCII tree requirements
    * @param os output stream
    * @param prefix string used to determine the indent and prefix
    */
    void print(ostream &os, const string &prefix) const override {
        if (prefix.size() >= 3) {
            os << prefix.substr(0, prefix.size() - 3);
            os << "+- ";
        }
        os << "[" << id << "]" << " Panel " << absPos << endl;
        for (const auto &el: elements) {
            if (el->ContainsOthers()) {
                if (el == elements.back())
                    el->print(os, prefix + "   ");
                else
                    el->print(os, prefix + "|  ");
            } else
                el->print(os, prefix + "+- ");
        }
    }
};

class CButton : public UIElement {
protected:
    string name; ///< button name
public:
    CButton(const int id_, const CRect &relPos_, string name_) : UIElement(id_, relPos_), name(std::move(name_)) {}

    unique_ptr<UIElement> clone() const override {
        return make_unique<CButton>(*this);
    }

    /**
     * @param id_
     * @return self if ids match, nullptr otherwise
     */
    UIElement *search(const int id_) override {
        return id_ == id ? this : nullptr;
    }

    /**
     * Calculates absolute position based on relative position and new given position of the container
     * @param newPosition
     */
    void setPosition(const CRect &newPosition) override {
        absPos = getAbsolute(relPos, newPosition);
    }

    /**
     * prints itself with the given prefix
     * @param os
     * @param prefix
     */
    void print(ostream &os, const string &prefix) const override {
        os << prefix << "[" << id << "]" << " Button " << "\"" << name << "\" " << absPos << endl;
    }
};

class CInput : public UIElement {
protected:
    string value; ///< input value
public:
    CInput(const int id_, const CRect &relPos_, string value_) : UIElement(id_, relPos_), value(std::move(value_)) {}

    unique_ptr<UIElement> clone() const override {
        return make_unique<CInput>(*this);
    }

    /**
     * @param id_
     * @return self if ids match, nullptr otherwise
     */
    UIElement *search(const int id_) override {
        return id_ == id ? this : nullptr;
    }

    /**
     * Calculates absolute position based on relative position and new given position of the container
     * @param newPosition
     */
    void setPosition(const CRect &newPosition) override {
        absPos = getAbsolute(relPos, newPosition);
    }

    /**
     * prints itself with the given prefix
     * @param os
     * @param prefix
     */
    void print(ostream &os, const string &prefix) const override {
        os << prefix << "[" << id << "]" << " Input " << "\"" << value << "\" " << absPos << endl;
    }

    /**
     * Sets value of the input to the new value
     * @param newValue
     */
    void setValue(string newValue) {
        value = std::move(newValue);
    }

    string getValue() const {
        return value;
    }
};

class CLabel : public UIElement {
protected:
    string label; ///< label
public:
    CLabel(const int id_, const CRect &relPos_, string label_) : UIElement(id_, relPos_), label(std::move(label_)) {}

    unique_ptr<UIElement> clone() const override {
        return make_unique<CLabel>(*this);
    }

    /**
     * @param id_
     * @return self if ids match, nullptr otherwise
     */
    UIElement *search(const int id_) override {
        return id_ == id ? this : nullptr;
    }

    /**
     * Calculates absolute position based on relative position and new given position of the container
     * @param newPosition
     */
    void setPosition(const CRect &newPosition) override {
        absPos = getAbsolute(relPos, newPosition);
    }

    /**
     * prints itself with the given prefix
     * @param os
     * @param prefix
     */
    void print(ostream &os, const string &prefix) const override {
        os << prefix << "[" << id << "]" << " Label " << "\"" << label << "\" " << absPos << endl;
    }
};

class CComboBox : public UIElement {
protected:
    size_t selected = 0;    ///< index to the selected combo
    vector<string> options; ///< vector of combo options
public:
    CComboBox(const int id_, const CRect &relPos_) : UIElement(id_, relPos_) { containsOthers = true; }

    unique_ptr<UIElement> clone() const override {
        return make_unique<CComboBox>(*this);
    }

    /**
     * @param id_
     * @return self if ids match, nullptr otherwise
     */
    UIElement *search(const int id_) override {
        return id_ == id ? this : nullptr;
    }

    /**
     * Calculates absolute position based on relative position and new given position of the container
     * @param newPosition
     */
    void setPosition(const CRect &newPosition) override {
        absPos = getAbsolute(relPos, newPosition);
    }

    /**
    * Prints the prefix without the last 3 characters since those should always be '+- ' and then itself.
    * Iterates through the combo options, gives them prefix based on selection
    * @param os output stream
    * @param prefix string used to determine the indent and prefix
    */
    void print(ostream &os, const string &prefix) const override {
        if (!prefix.empty()) {
            os << prefix.substr(0, prefix.size() - 3);
            os << "+- ";
        }
        os << "[" << id << "]" << " ComboBox " << absPos << endl;
        for (const auto &el: options) {
            os << prefix;
            if (el == options[selected]) {
                os << "+->" << el << "<" << endl;
            } else {
                os << "+- " << el << endl;
            }
        }
    }

    /**
     * Adds new combo option
     * @param name
     * @return reference to self
     */
    CComboBox &add(string name) {
        options.push_back(std::move(name));
        return *this;
    }

    void setSelected(const size_t index) {
        selected = index;
    }

    size_t getSelected() const {
        return selected;
    }
};

#ifndef __PROGTEST__

template<typename T_>
string toString(const T_ &x) {
    ostringstream oss;
    oss << x;
    return oss.str();
}

int main() {
    assert(sizeof(CButton) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
    assert(sizeof(CInput) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
    assert(sizeof(CLabel) - sizeof(string) < sizeof(CComboBox) - sizeof(vector<string>));
    assert(sizeof(CButton) - sizeof(string) <= sizeof(CPanel) - sizeof(vector<void *>));
    assert(sizeof(CInput) - sizeof(string) <= sizeof(CPanel) - sizeof(vector<void *>));
    assert(sizeof(CLabel) - sizeof(string) <= sizeof(CPanel) - sizeof(vector<void *>));
    CWindow a(0, "Sample window", CRect(10, 10, 600, 480));
    a.add(CButton(1, CRect(0.1, 0.8, 0.3, 0.1), "Ok")).add(CButton(2, CRect(0.6, 0.8, 0.3, 0.1), "Cancel"));
    a.add(CLabel(10, CRect(0.1, 0.1, 0.2, 0.1), "Username:"));
    a.add(CInput(11, CRect(0.4, 0.1, 0.5, 0.1), "chucknorris"));
    a.add(CPanel(12, CRect(0.1, 0.3, 0.8, 0.7)).add(
            CComboBox(20, CRect(0.1, 0.3, 0.8, 0.1)).add("Karate").add("Judo").add("Box").add("Progtest")));
    assert(toString(a) == "[0] Window \"Sample window\" (10,10,600,480)\n"
                          "+- [1] Button \"Ok\" (70,394,180,48)\n"
                          "+- [2] Button \"Cancel\" (370,394,180,48)\n"
                          "+- [10] Label \"Username:\" (70,58,120,48)\n"
                          "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
                          "+- [12] Panel (70,154,480,336)\n"
                          "   +- [20] ComboBox (118,254.8,384,33.6)\n"
                          "      +->Karate<\n"
                          "      +- Judo\n"
                          "      +- Box\n"
                          "      +- Progtest\n");
    CWindow b = a;
    assert(toString(b) == "[0] Window \"Sample window\" (10,10,600,480)\n"
                          "+- [1] Button \"Ok\" (70,394,180,48)\n"
                          "+- [2] Button \"Cancel\" (370,394,180,48)\n"
                          "+- [10] Label \"Username:\" (70,58,120,48)\n"
                          "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
                          "+- [12] Panel (70,154,480,336)\n"
                          "   +- [20] ComboBox (118,254.8,384,33.6)\n"
                          "      +->Karate<\n"
                          "      +- Judo\n"
                          "      +- Box\n"
                          "      +- Progtest\n");
    assert(toString(*b.search(20)) == "[20] ComboBox (118,254.8,384,33.6)\n"
                                      "+->Karate<\n"
                                      "+- Judo\n"
                                      "+- Box\n"
                                      "+- Progtest\n");
    assert(dynamic_cast<CComboBox &> ( *b.search(20)).getSelected() == 0);
    dynamic_cast<CComboBox &> ( *b.search(20)).setSelected(3);
    assert(dynamic_cast<CInput &> ( *b.search(11)).getValue() == "chucknorris");
    dynamic_cast<CInput &> ( *b.search(11)).setValue("chucknorris@fit.cvut.cz");
    CPanel &p = dynamic_cast<CPanel &> ( *b.search(12));
    p.add(CComboBox(21, CRect(0.1, 0.5, 0.8, 0.1)).add("PA2").add("OSY").add("Both"));
    assert(toString(b) == "[0] Window \"Sample window\" (10,10,600,480)\n"
                          "+- [1] Button \"Ok\" (70,394,180,48)\n"
                          "+- [2] Button \"Cancel\" (370,394,180,48)\n"
                          "+- [10] Label \"Username:\" (70,58,120,48)\n"
                          "+- [11] Input \"chucknorris@fit.cvut.cz\" (250,58,300,48)\n"
                          "+- [12] Panel (70,154,480,336)\n"
                          "   +- [20] ComboBox (118,254.8,384,33.6)\n"
                          "   |  +- Karate\n"
                          "   |  +- Judo\n"
                          "   |  +- Box\n"
                          "   |  +->Progtest<\n"
                          "   +- [21] ComboBox (118,322,384,33.6)\n"
                          "      +->PA2<\n"
                          "      +- OSY\n"
                          "      +- Both\n");
    assert(toString(a) == "[0] Window \"Sample window\" (10,10,600,480)\n"
                          "+- [1] Button \"Ok\" (70,394,180,48)\n"
                          "+- [2] Button \"Cancel\" (370,394,180,48)\n"
                          "+- [10] Label \"Username:\" (70,58,120,48)\n"
                          "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
                          "+- [12] Panel (70,154,480,336)\n"
                          "   +- [20] ComboBox (118,254.8,384,33.6)\n"
                          "      +->Karate<\n"
                          "      +- Judo\n"
                          "      +- Box\n"
                          "      +- Progtest\n");
    assert(toString(p) == "[12] Panel (70,154,480,336)\n"
                          "+- [20] ComboBox (118,254.8,384,33.6)\n"
                          "|  +- Karate\n"
                          "|  +- Judo\n"
                          "|  +- Box\n"
                          "|  +->Progtest<\n"
                          "+- [21] ComboBox (118,322,384,33.6)\n"
                          "   +->PA2<\n"
                          "   +- OSY\n"
                          "   +- Both\n");
    b.setPosition(CRect(20, 30, 640, 520));
    assert(toString(b) == "[0] Window \"Sample window\" (20,30,640,520)\n"
                          "+- [1] Button \"Ok\" (84,446,192,52)\n"
                          "+- [2] Button \"Cancel\" (404,446,192,52)\n"
                          "+- [10] Label \"Username:\" (84,82,128,52)\n"
                          "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
                          "+- [12] Panel (84,186,512,364)\n"
                          "   +- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
                          "   |  +- Karate\n"
                          "   |  +- Judo\n"
                          "   |  +- Box\n"
                          "   |  +->Progtest<\n"
                          "   +- [21] ComboBox (135.2,368,409.6,36.4)\n"
                          "      +->PA2<\n"
                          "      +- OSY\n"
                          "      +- Both\n");
    p.add(p);
    assert(toString(p) == "[12] Panel (84,186,512,364)\n"
                          "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
                          "|  +- Karate\n"
                          "|  +- Judo\n"
                          "|  +- Box\n"
                          "|  +->Progtest<\n"
                          "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
                          "|  +->PA2<\n"
                          "|  +- OSY\n"
                          "|  +- Both\n"
                          "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
                          "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
                          "   |  +- Karate\n"
                          "   |  +- Judo\n"
                          "   |  +- Box\n"
                          "   |  +->Progtest<\n"
                          "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
                          "      +->PA2<\n"
                          "      +- OSY\n"
                          "      +- Both\n");
    p.add(p);
    assert(toString(p) == "[12] Panel (84,186,512,364)\n"
                          "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
                          "|  +- Karate\n"
                          "|  +- Judo\n"
                          "|  +- Box\n"
                          "|  +->Progtest<\n"
                          "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
                          "|  +->PA2<\n"
                          "|  +- OSY\n"
                          "|  +- Both\n"
                          "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
                          "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
                          "|  |  +- Karate\n"
                          "|  |  +- Judo\n"
                          "|  |  +- Box\n"
                          "|  |  +->Progtest<\n"
                          "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
                          "|     +->PA2<\n"
                          "|     +- OSY\n"
                          "|     +- Both\n"
                          "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
                          "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
                          "   |  +- Karate\n"
                          "   |  +- Judo\n"
                          "   |  +- Box\n"
                          "   |  +->Progtest<\n"
                          "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
                          "   |  +->PA2<\n"
                          "   |  +- OSY\n"
                          "   |  +- Both\n"
                          "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
                          "      +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
                          "      |  +- Karate\n"
                          "      |  +- Judo\n"
                          "      |  +- Box\n"
                          "      |  +->Progtest<\n"
                          "      +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
                          "         +->PA2<\n"
                          "         +- OSY\n"
                          "         +- Both\n");
    p.add(p);
    assert(toString(p) == "[12] Panel (84,186,512,364)\n"
                          "+- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
                          "|  +- Karate\n"
                          "|  +- Judo\n"
                          "|  +- Box\n"
                          "|  +->Progtest<\n"
                          "+- [21] ComboBox (135.2,368,409.6,36.4)\n"
                          "|  +->PA2<\n"
                          "|  +- OSY\n"
                          "|  +- Both\n"
                          "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
                          "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
                          "|  |  +- Karate\n"
                          "|  |  +- Judo\n"
                          "|  |  +- Box\n"
                          "|  |  +->Progtest<\n"
                          "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
                          "|     +->PA2<\n"
                          "|     +- OSY\n"
                          "|     +- Both\n"
                          "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
                          "|  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
                          "|  |  +- Karate\n"
                          "|  |  +- Judo\n"
                          "|  |  +- Box\n"
                          "|  |  +->Progtest<\n"
                          "|  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
                          "|  |  +->PA2<\n"
                          "|  |  +- OSY\n"
                          "|  |  +- Both\n"
                          "|  +- [12] Panel (176.16,371.64,327.68,178.36)\n"
                          "|     +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
                          "|     |  +- Karate\n"
                          "|     |  +- Judo\n"
                          "|     |  +- Box\n"
                          "|     |  +->Progtest<\n"
                          "|     +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
                          "|        +->PA2<\n"
                          "|        +- OSY\n"
                          "|        +- Both\n"
                          "+- [12] Panel (135.2,295.2,409.6,254.8)\n"
                          "   +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
                          "   |  +- Karate\n"
                          "   |  +- Judo\n"
                          "   |  +- Box\n"
                          "   |  +->Progtest<\n"
                          "   +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
                          "   |  +->PA2<\n"
                          "   |  +- OSY\n"
                          "   |  +- Both\n"
                          "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
                          "   |  +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
                          "   |  |  +- Karate\n"
                          "   |  |  +- Judo\n"
                          "   |  |  +- Box\n"
                          "   |  |  +->Progtest<\n"
                          "   |  +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
                          "   |     +->PA2<\n"
                          "   |     +- OSY\n"
                          "   |     +- Both\n"
                          "   +- [12] Panel (176.16,371.64,327.68,178.36)\n"
                          "      +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
                          "      |  +- Karate\n"
                          "      |  +- Judo\n"
                          "      |  +- Box\n"
                          "      |  +->Progtest<\n"
                          "      +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
                          "      |  +->PA2<\n"
                          "      |  +- OSY\n"
                          "      |  +- Both\n"
                          "      +- [12] Panel (208.928,425.148,262.144,124.852)\n"
                          "         +- [20] ComboBox (235.142,462.604,209.715,12.4852)\n"
                          "         |  +- Karate\n"
                          "         |  +- Judo\n"
                          "         |  +- Box\n"
                          "         |  +->Progtest<\n"
                          "         +- [21] ComboBox (235.142,487.574,209.715,12.4852)\n"
                          "            +->PA2<\n"
                          "            +- OSY\n"
                          "            +- Both\n");
    assert(toString(b) == "[0] Window \"Sample window\" (20,30,640,520)\n"
                          "+- [1] Button \"Ok\" (84,446,192,52)\n"
                          "+- [2] Button \"Cancel\" (404,446,192,52)\n"
                          "+- [10] Label \"Username:\" (84,82,128,52)\n"
                          "+- [11] Input \"chucknorris@fit.cvut.cz\" (276,82,320,52)\n"
                          "+- [12] Panel (84,186,512,364)\n"
                          "   +- [20] ComboBox (135.2,295.2,409.6,36.4)\n"
                          "   |  +- Karate\n"
                          "   |  +- Judo\n"
                          "   |  +- Box\n"
                          "   |  +->Progtest<\n"
                          "   +- [21] ComboBox (135.2,368,409.6,36.4)\n"
                          "   |  +->PA2<\n"
                          "   |  +- OSY\n"
                          "   |  +- Both\n"
                          "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
                          "   |  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
                          "   |  |  +- Karate\n"
                          "   |  |  +- Judo\n"
                          "   |  |  +- Box\n"
                          "   |  |  +->Progtest<\n"
                          "   |  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
                          "   |     +->PA2<\n"
                          "   |     +- OSY\n"
                          "   |     +- Both\n"
                          "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
                          "   |  +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
                          "   |  |  +- Karate\n"
                          "   |  |  +- Judo\n"
                          "   |  |  +- Box\n"
                          "   |  |  +->Progtest<\n"
                          "   |  +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
                          "   |  |  +->PA2<\n"
                          "   |  |  +- OSY\n"
                          "   |  |  +- Both\n"
                          "   |  +- [12] Panel (176.16,371.64,327.68,178.36)\n"
                          "   |     +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
                          "   |     |  +- Karate\n"
                          "   |     |  +- Judo\n"
                          "   |     |  +- Box\n"
                          "   |     |  +->Progtest<\n"
                          "   |     +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
                          "   |        +->PA2<\n"
                          "   |        +- OSY\n"
                          "   |        +- Both\n"
                          "   +- [12] Panel (135.2,295.2,409.6,254.8)\n"
                          "      +- [20] ComboBox (176.16,371.64,327.68,25.48)\n"
                          "      |  +- Karate\n"
                          "      |  +- Judo\n"
                          "      |  +- Box\n"
                          "      |  +->Progtest<\n"
                          "      +- [21] ComboBox (176.16,422.6,327.68,25.48)\n"
                          "      |  +->PA2<\n"
                          "      |  +- OSY\n"
                          "      |  +- Both\n"
                          "      +- [12] Panel (176.16,371.64,327.68,178.36)\n"
                          "      |  +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
                          "      |  |  +- Karate\n"
                          "      |  |  +- Judo\n"
                          "      |  |  +- Box\n"
                          "      |  |  +->Progtest<\n"
                          "      |  +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
                          "      |     +->PA2<\n"
                          "      |     +- OSY\n"
                          "      |     +- Both\n"
                          "      +- [12] Panel (176.16,371.64,327.68,178.36)\n"
                          "         +- [20] ComboBox (208.928,425.148,262.144,17.836)\n"
                          "         |  +- Karate\n"
                          "         |  +- Judo\n"
                          "         |  +- Box\n"
                          "         |  +->Progtest<\n"
                          "         +- [21] ComboBox (208.928,460.82,262.144,17.836)\n"
                          "         |  +->PA2<\n"
                          "         |  +- OSY\n"
                          "         |  +- Both\n"
                          "         +- [12] Panel (208.928,425.148,262.144,124.852)\n"
                          "            +- [20] ComboBox (235.142,462.604,209.715,12.4852)\n"
                          "            |  +- Karate\n"
                          "            |  +- Judo\n"
                          "            |  +- Box\n"
                          "            |  +->Progtest<\n"
                          "            +- [21] ComboBox (235.142,487.574,209.715,12.4852)\n"
                          "               +->PA2<\n"
                          "               +- OSY\n"
                          "               +- Both\n");
    assert(toString(a) == "[0] Window \"Sample window\" (10,10,600,480)\n"
                          "+- [1] Button \"Ok\" (70,394,180,48)\n"
                          "+- [2] Button \"Cancel\" (370,394,180,48)\n"
                          "+- [10] Label \"Username:\" (70,58,120,48)\n"
                          "+- [11] Input \"chucknorris\" (250,58,300,48)\n"
                          "+- [12] Panel (70,154,480,336)\n"
                          "   +- [20] ComboBox (118,254.8,384,33.6)\n"
                          "      +->Karate<\n"
                          "      +- Judo\n"
                          "      +- Box\n"
                          "      +- Progtest\n");
    cout << "gamin" << endl;
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
