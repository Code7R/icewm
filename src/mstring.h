#ifndef MSTRING_H
#define MSTRING_H

#ifdef YARRAY_H
#error include yarray.h after mstring.h
#endif

#include "ref.h"
#include <stdlib.h>
#include <stdint.h>

class mstring;
class mstring_view {
    const char *m_data;
    size_t m_size;
public:
    mstring_view(const char *s);
    mstring_view(const char *s, size_t len);
    mstring_view(null_ref &) : m_data(nullptr), m_size(0) {}
    mstring_view() : mstring_view(null) {}
    mstring_view(const mstring& s);
    size_t length() const { return m_size; }
    const char* data() const { return m_data; }
    bool operator==(mstring_view rv) const;
    bool isEmpty() const { return length() == 0; }
};
/*
 * Mutable strings with a small string optimization.
 */
class mstring {
public:
    using size_type = size_t;
private:
    friend mstring operator+(const char* s, const mstring& m);
    friend void swap(mstring& a, mstring& b);
    friend class mstring_view;
    struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        union {
            size_type count;
            char cBytes[sizeof(size_type)];
        };
#endif
        struct TRefData {
            char *pData;
            // for now: just inflate, later: to implement set preservation
            size_type nReserved;
        };
        union {
            TRefData ext;
            char extraBytes[sizeof(TRefData)];
        };
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
        union {
                    size_type count;
                    char cBytes[sizeof(size_type)];
        };
#endif
    } spod;
    const char* data() const;
    char* data();
    bool isLocal() const {
        return 0 == (spod.count & 0x1);
    }
    void set_len(size_type len);
    void term(size_type len);
    void extendBy(size_type amount);
    void extendTo(size_type new_len);
    // detect parameter data coming from this string, to take the slower path
    bool input_from_here(mstring_view sv);

public:
    mstring(const char *s, size_type len);
    mstring(mstring_view sv) : mstring(sv.data(), sv.length()) {}
    mstring(const mstring& s) : mstring(s.data(), s.length()) {}
    mstring(const char *s) : mstring(mstring_view(s)) {}
    mstring(mstring&& other);
    explicit mstring(long);

    // fast in-place concatenation for often uses
    mstring(mstring_view a, mstring_view b, mstring_view c = mstring_view(),
            mstring_view d = mstring_view(), mstring_view e = mstring_view());

    mstring(null_ref &) { set_len(0); data()[0] = 0x0; }
    mstring() : mstring(null) {}

    ~mstring() { clear(); };

    size_type length() const {
        return (isLocal() ? (spod.count & 0xff) : spod.count) >> 1;
    }
    bool isEmpty() const {
        return 0 == length();
    }
    bool nonempty() const { return !isEmpty(); }

    mstring& operator=(mstring_view rv);
    mstring& operator=(const mstring& rv) { return *this = mstring_view(rv); }
    mstring& operator+=(const mstring_view& rv);
    mstring& operator+=(const mstring& rv) {return *this += mstring_view(rv); }
    mstring operator+(const mstring_view& rv) const;
    mstring operator+(const char* s) const { return mstring(*this, s); }
    mstring operator+(const mstring& s) const { return mstring(*this, s); }
    // moves might just steal the other buffer
    mstring& operator=(mstring&& rv);
    mstring& operator+=(mstring&& rv);
    mstring operator+(mstring&& rv) const;
    // plain types
    mstring& operator=(const char* sz) { return *this = mstring_view(sz); }
    mstring& operator+=(const char* s) { return *this += mstring_view(s); }

    bool operator==(const char * rv) const { return equals(rv); }
    bool operator==(mstring_view rv) const { return equals(rv); }
    bool operator!=(const char *rv) const { return !equals(rv); }
    bool operator!=(mstring_view rv) const { return !equals(rv); }
    bool operator==(const mstring &rv) const { return equals(rv); }
    bool operator!=(const mstring &rv) const { return !equals(rv); }
    bool operator==(null_ref &) const { return isEmpty(); }
    bool operator!=(null_ref &) const { return nonempty(); }

    mstring operator=(null_ref &) { clear(); return mstring(); }
    mstring substring(size_type pos) const;
    mstring substring(size_type pos, size_type len) const;
    mstring match(const char* regex, const char* flags = nullptr) const;

    int operator[](int pos) const { return charAt(pos); }
    int charAt(int pos) const;
    int indexOf(char ch) const;
    int lastIndexOf(char ch) const;
    int count(char ch) const;

    bool equals(const char *&sz) const { return equals(mstring_view(sz)); }
    bool equals(mstring_view sv) const { return sv == *this; }
    bool equals(const mstring &s) const { return mstring_view(s) == *this; };
    bool equals(const char *sz, size_type len) const {
        return equals(mstring_view(sz, len));
    }

    int collate(const mstring& s, bool ignoreCase = false) const;
    int compareTo(const mstring &s) const;
    bool operator<(const mstring& other) const { return compareTo(other) < 0; }
    bool copyTo(char *dst, size_type len) const;

    bool startsWith(mstring_view sv) const;
    bool endsWith(mstring_view sv) const;
    int find(mstring_view) const;

    bool split(unsigned char token, mstring *left, mstring *remain) const;
    bool splitall(unsigned char token, mstring *left, mstring *remain) const;
    mstring trim() const;
    mstring replace(size_type position, size_type len, const mstring &insert) const;
    mstring remove(size_type position, size_type len) const;
    mstring insert(size_type position, const mstring &s) const;
    mstring searchAndReplaceAll(const mstring& s, const mstring& r) const;
    mstring lower() const;
    mstring upper() const;

    operator const char *() { return c_str(); }
    const char* c_str() const { return data();}

    void clear();
};

inline bool operator==(const char* s, const mstring& c) {
    return c == s;
}
inline bool operator!=(const char* s, const mstring& c) {
    return !(c == s);
}
inline mstring operator+(const char* s, const mstring& m) {
    return (s && *s) ? (mstring(s) + m) : m;
}
inline mstring_view::mstring_view(const char *s, size_t len) :
        m_data(s), m_size(len) {
}
inline mstring_view::mstring_view(const mstring &s) :
        mstring_view(s.data(), s.length()) {
}
void swap(mstring &a, mstring &b);

#endif

// vim: set sw=4 ts=4 et:
