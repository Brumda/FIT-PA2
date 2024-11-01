#ifndef __PROGTEST__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
#endif /* __PROGTEST__ */
//.!..nope.!..nope.!..nope.!..nope.!..nope.!..nope.!..nope.!..nope.!..nope.!..nope.!..nope.!..nope.!..nope
// all hail the 🍝 code



// forgive me
vector<int> fib = {1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946,
                   17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040};


bool check_file(const fstream &file) { return !(!file || file.fail()); }

bool num_to_fib(uint32_t num, char &c, int &pos, fstream &out) {
    uint32_t k = 0b00000000000000000000000000000000;
    bool start = false;
    num++;
    int rnd = 0;
    for (int i = fib.size() - 1; i >= 0; i--) {
        if (num % fib[i] == num) { continue; }
        if (!start) {
            rnd = i;
            k += (1 << (i + 1));
            start = true;
        }
        k += (1 << i);
        num = num - fib[i];
    }
    for (int i = 0; i < 32; i++) {
        if (i <= rnd + 1) {
            int bit2 = ((k >> i) & 1);
            c += (bit2 << pos++);
            if (pos % 8 == 0) {
                out.put(c);
                c = 0b00000000;
                pos = 0;
                if (out.fail()) { return false; }
            }
        } else break;
    }
    return true;

}

bool utf_check_decode(const int bytes, int &char_len, int c, uint32_t &num) {
//  the most hideous thing anyone has ever written
    int b = 0;
    if ((c & 0b11111000) >= 0b11111000) { return false; }
    if (bytes == 1) {
        if ((c & 128) == 0) {
            char_len = 1;
        } else if ((c & 0b11000000) == 128) { return false; }
        else if ((c & 0b11100000) == 0b11000000) {
            char_len = 2;
            b = 6 * (char_len - 1);
            c = (c ^ 0b11000000);
        } else if ((c & 0b11110000) == 0b11100000) {
            char_len = 3;
            b = 6 * (char_len - 1);
            c = (c ^ 0b11100000);
        } else if ((c & 0b11111000) == 0b11110000) {
            char_len = 4;
            b = 6 * (char_len - 1);
            c = (c ^ 0b11110000);
        }
    } else if ((c & 0b11000000) != 128) { return false; }
    else {
        c = (c ^ 0b10000000);
        b = (char_len - bytes) * 6;
    }
    num += (c << b);
    return true;
}

bool read_file_utf(fstream &infile, fstream &outfile) {
    char c;
    char chr = 0b00000000;
    int bytes = 0;
    int pos = 0;
    uint32_t num = 0;
    int n = 0;
    int char_len = 0;
    while (infile.get(c)) {
        n = 0;
        for (int i = 7; i >= 0; i--) {
            int bit = ((c >> i) & 1);
            n += (bit << i);
        }
        bytes++;
        if (!utf_check_decode(bytes, char_len, n, num)) { return false; }

        if (char_len == bytes) {
            if (!num_to_fib(num, chr, pos, outfile)) { return false; }
            n = 0;
            num = 0;
            bytes = 0;
            char_len = 0;
        }
    }
    if (chr != 0) {
        outfile.put(chr);
        if (outfile.fail()) { return false; }
    }
    if (char_len != bytes) { return false; }
    return true;
}

bool utf8ToFibonacci(const char *inFile, const char *outFile) {
    fstream f1{inFile, ios_base::binary | ios::in};
    fstream f2{outFile, ios_base::binary | ios::out};

    if (!check_file(f1) || !check_file(f2)) { return false; }

    return read_file_utf(f1, f2);
}

bool fib_check_decode(vector<bool> bits, uint32_t &num) {
    bits.erase(--bits.end(), bits.end());
    if (bits.size() > fib.size()) { return false; }
    for (size_t i = 0; i < bits.size(); i++)
        if (bits[i] == 1) { num += fib[i]; }
    num--;
    return true;

}

bool num_to_utf(uint32_t &num, fstream &out) {
    char c = 0b00000000;
    int bytes;
    vector<bool> bits;
    for (int i = 0; i < 32; i++) {
        int bit = ((num >> i) & 1);
        bits.push_back(bit);
    }

    if (num <= 127) { bytes = 1; }
    else if (num <= 2047) {
        bytes = 2;
        bits.insert(bits.begin() + 6, 0);
        bits.insert(bits.begin() + 7, 1);
        bits.insert(bits.begin() + 13, 0);
        bits.insert(bits.begin() + 14, 1);
        bits.insert(bits.begin() + 15, 1);
    } else if (num <= 65535) {
        bytes = 3;
        bits.insert(bits.begin() + 6, 0);
        bits.insert(bits.begin() + 7, 1);
        bits.insert(bits.begin() + 14, 0);
        bits.insert(bits.begin() + 15, 1);
        bits.insert(bits.begin() + 20, 0);
        bits.insert(bits.begin() + 21, 1);
        bits.insert(bits.begin() + 22, 1);
        bits.insert(bits.begin() + 23, 1);
    } else if (num <= 1114111) {
        bytes = 4;
        bits.insert(bits.begin() + 6, 0);
        bits.insert(bits.begin() + 7, 1);
        bits.insert(bits.begin() + 14, 0);
        bits.insert(bits.begin() + 15, 1);
        bits.insert(bits.begin() + 22, 0);
        bits.insert(bits.begin() + 23, 1);
        bits.insert(bits.begin() + 27, 0);
        bits.insert(bits.begin() + 28, 1);
        bits.insert(bits.begin() + 29, 1);
        bits.insert(bits.begin() + 30, 1);
        bits.insert(bits.begin() + 31, 1);
    } else { return false; }

    bits.erase(bits.begin() + (bytes * 8), bits.end());

    for (int i = (bytes * 8) - 1; i >= 0; i--) {
        c += (bits[i] << (i % 8));
        if (i % 8 == 0) {
            out.put(c);
            c = 0b00000000;
            if (out.fail()) { return false; }
        }
    }


    return true;
}

bool read_file_fib(fstream &infile, fstream &outfile) {
    char c;
    uint32_t num = 0;
    vector<bool> bits;
    int found_one = 0;
    bool workin = false;
    while (infile.get(c)) {
        for (int i = 7; i >= 0; i--) {
            int bit = (((c << i) & 128) >> 7);
            bits.push_back(bit);
            if (bit) {
                found_one++;
                workin = true;
            } else { found_one = 0; }
            if (found_one == 2) {
                if (!fib_check_decode(bits, num)) { return false; }

                found_one = 0;
                if (!num_to_utf(num, outfile)) { return false; }
                num = 0;
                workin = false;
                bits.erase(bits.begin(), bits.end());
            }
        }
    }
    if (workin) { return false; }
    return true;
}

bool fibonacciToUtf8(const char *inFile,
                     const char *outFile) {
    fstream f1{inFile, ios_base::binary | ios::in};
    fstream f2{outFile, ios_base::binary | ios::out};
    if (!check_file(f1) || !check_file(f2)) { return false; }
    return read_file_fib(f1, f2);
}

#ifndef __PROGTEST__

bool identicalFiles(const char *file1, const char *file2) { return true; }

int main() {
    assert (utf8ToFibonacci("example/src_0.utf8", "output.fib")
            && identicalFiles("output.fib", "example/dst_0.fib"));
    assert (utf8ToFibonacci("example/src_1.utf8", "output.fib")
            && identicalFiles("output.fib", "example/dst_1.fib"));
    assert (utf8ToFibonacci("example/src_2.utf8", "output.fib")
            && identicalFiles("output.fib", "example/dst_2.fib"));
    assert (utf8ToFibonacci("example/src_3.utf8", "output.fib")
            && identicalFiles("output.fib", "example/dst_3.fib"));
    assert (utf8ToFibonacci("example/src_4.utf8", "output.fib")
            && identicalFiles("output.fib", "example/dst_4.fib"));
    assert (!utf8ToFibonacci("example/src_5.utf8", "output.fib"));
    assert (!utf8ToFibonacci("example/in_5029416.bin", "output.fib"));
    cout << "\n========================<utf8->fib done>========================\n" << endl;
    assert (fibonacciToUtf8("example/src_6.fib", "output.utf8")
            && identicalFiles("output.utf8", "example/dst_6.utf8"));
    assert (fibonacciToUtf8("example/src_7.fib", "output.utf8")
            && identicalFiles("output.utf8", "example/dst_7.utf8"));
    assert (fibonacciToUtf8("example/src_8.fib", "output.utf8")
            && identicalFiles("output.utf8", "example/dst_8.utf8"));
    assert (fibonacciToUtf8("example/src_9.fib", "output.utf8")
            && identicalFiles("output.utf8", "example/dst_9.utf8"));
    assert (fibonacciToUtf8("example/src_10.fib", "output.utf8")
            && identicalFiles("output.utf8", "example/dst_10.utf8"));
    assert (!fibonacciToUtf8("example/src_11.fib", "output.utf8"));
    cout << "\n========================<fib->utf8 done>========================\n" << endl;
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */
