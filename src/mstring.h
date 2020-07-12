#ifndef MSTRING_H
#define MSTRING_H

#ifdef YARRAY_H
#error include yarray.h after mstring.h
#endif

#include "ref.h"
#include <stdlib.h>

/*
 * Mutable strings with a reference counted string buffer.
 */
class mstring {
private:
    friend mstring operator+(const char* s, const mstring& m);

    struct TRefData
    {
        size_t fCount;
        char *pData;
        size_t just_padding; // to be used in case we need an offset for in-place trim later
        size_t just_padding2; // to be used in case we need an offset for in-place trim later
    };
    union
    {
        TRefData ext;
        char here[sizeof(TRefData)];
    };
    mstring(const char* str1, size_t len1, const char* str2, size_t len2);
    const char* data() const;
    char* data();
    bool isExt() const;
    mstring(const char *str, size_t len, bool nocopy);

public:
    mstring(const char *str);
    mstring(const char *str1, const char *str2);
    mstring(const char *str1, const char *str2, const char *str3);
    mstring(const char *str, size_t len);
    explicit mstring(long);

    mstring(null_ref &) { ext.fCount = 0; data()[0] = 0x0; }
    mstring() { ext.fCount = 0; data()[0] = 0x0; }

    mstring(const mstring &r) : mstring(r.data(), r.length()) {};
    mstring(mstring&& other);
    ~mstring();

    size_t length() const { return ext.fCount; }
//    size_t offset() const { return fOffset; }
    bool isEmpty() const { return 0 == ext.fCount; }
    bool nonempty() const { return !isEmpty(); }

    mstring& operator=(const mstring& rv);
    mstring& operator=(mstring&& rv);
    void operator+=(const mstring& rv);
    mstring operator+(const mstring& rv) const;

    bool operator==(const char *rv) const { return equals(rv); }
    bool operator!=(const char *rv) const { return !equals(rv); }
    bool operator==(const mstring &rv) const { return equals(rv); }
    bool operator!=(const mstring &rv) const { return !equals(rv); }
    bool operator==(null_ref &) const { return isEmpty(); }
    bool operator!=(null_ref &) const { return nonempty(); }

    mstring operator=(null_ref &) { return *this = mstring(); }
    mstring substring(size_t pos) const;
    mstring substring(size_t pos, size_t len) const;
    mstring match(const char* regex, const char* flags = nullptr) const;

    int operator[](int pos) const { return charAt(pos); }
    int charAt(int pos) const;
    int indexOf(char ch) const;
    int lastIndexOf(char ch) const;
    int count(char ch) const;

    bool equals(const char *s) const;
    bool equals(const char *s, size_t len) const;
    bool equals(const mstring &s) const;
    int collate(mstring s, bool ignoreCase = false);
    int compareTo(const mstring &s) const;
    bool copyTo(char *dst, size_t len) const;

    bool startsWith(const mstring &s) const;
    bool endsWith(const mstring &s) const;
    int find(const mstring &s) const;

    bool split(unsigned char token, mstring *left, mstring *remain) const;
    bool splitall(unsigned char token, mstring *left, mstring *remain) const;
    mstring trim() const;
    mstring replace(int position, int len, const mstring &insert) const;
    mstring remove(int position, int len) const;
    mstring insert(int position, const mstring &s) const;
    mstring append(const mstring &str) const { return *this + str; }
    mstring searchAndReplaceAll(const mstring& s, const mstring& r) const;
    mstring lower() const;
    mstring upper() const;

    operator const char *() { return c_str(); }
    const char* c_str() const { auto ret=data();
    return ret;};
};

inline bool operator==(const char* s, const mstring& c) { return c == s; }
inline bool operator!=(const char* s, const mstring& c) { return c != s; }

inline mstring operator+(const char* s, const mstring& m) {
    return mstring(s) + m;
}

#endif

// vim: set sw=4 ts=4 et:
