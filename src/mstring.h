#ifndef MSTRING_H
#define MSTRING_H

#ifdef YARRAY_H
#error include yarray.h after mstring.h
#endif

#include "ref.h"
#include <stdlib.h>

#define MSTRING_INPLACE_SIZE (sizeof(mstring::spod) - 1)
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
private:
    friend mstring operator+(const char* s, const mstring& m);
    friend void swap(mstring& a, mstring& b);
    friend class mstring_view;
    struct {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        union {
            size_t fCount;
            char cBytes[sizeof(size_t)];
        };
#endif
        struct TRefData {
            char *pData;
            size_t nUnused;
            //size_t nUnused2;
        };
        union {
            TRefData ext;
            char extraBytes[sizeof(TRefData)];
        };
#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
        union {
            size_t fCount;
            char cBytes[sizeof(size_t)];
        };
#endif
    } spod;
    const char* data() const;
    char* data();
    bool isLocal() const {
        return (unsigned(spod.cBytes[0]) < MSTRING_INPLACE_SIZE)
                || (spod.fCount < MSTRING_INPLACE_SIZE);
    }

    mstring& extend(size_t amount);
    // detect parameter data coming from this string, to take the slower path
    bool input_from_here(mstring_view sv);

public:
    mstring(const char *s, size_t len);
    mstring(mstring_view sv) : mstring(sv.data(), sv.length()) {}
    mstring(const mstring& s) : mstring(s.data(), s.length()) {}
    mstring(const char *s) : mstring(mstring_view(s)) {}
    mstring(mstring&& other);
    explicit mstring(long);

    // fast in-place concatenation for often uses
    mstring(mstring_view a, mstring_view b, mstring_view c = mstring_view(),
            mstring_view d = mstring_view(), mstring_view e = mstring_view());

    mstring(null_ref &) { spod.fCount = 0; data()[0] = 0x0; }
    mstring() { spod.fCount = 0; data()[0] = 0x0; }

    ~mstring() { clear(); };

    size_t length() const { return isLocal() ? spod.cBytes[0] : spod.fCount; }
    bool isEmpty() const { return 0 == length(); }
    bool nonempty() const { return !isEmpty(); }

    mstring& operator=(mstring_view rv);
    mstring& operator=(const mstring& rv) { return *this = mstring_view(rv); }
    mstring& operator+=(const mstring_view& rv);
    mstring& operator+=(const mstring& rv) {return *this += mstring_view(rv); }
    mstring operator+(const mstring_view& rv) const;
    mstring operator+(const char* s) const { return *this + mstring_view(s); }
    mstring operator+(const mstring& rv) const {
        return *this + mstring_view(rv);
    }
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
    mstring substring(size_t pos) const;
    mstring substring(size_t pos, size_t len) const;
    mstring match(const char* regex, const char* flags = nullptr) const;

    int operator[](int pos) const { return charAt(pos); }
    int charAt(int pos) const;
    int indexOf(char ch) const;
    int lastIndexOf(char ch) const;
    int count(char ch) const;

    bool equals(const char *&sz) const { return equals(mstring_view(sz)); }
    bool equals(mstring_view sv) const { return sv == *this; }
    bool equals(const mstring &s) const { return mstring_view(s) == *this; };
    bool equals(const char *sz, size_t len) const {
        return equals(mstring_view(sz, len));
    }

    int collate(const mstring& s, bool ignoreCase = false) const;
    int compareTo(const mstring &s) const;
    bool operator<(const mstring& other) const { return compareTo(other) < 0; }
    bool copyTo(char *dst, size_t len) const;

    bool startsWith(mstring_view sv) const;
    bool endsWith(mstring_view sv) const;
    int find(mstring_view) const;

    bool split(unsigned char token, mstring *left, mstring *remain) const;
    bool splitall(unsigned char token, mstring *left, mstring *remain) const;
    mstring trim() const;
    mstring replace(int position, int len, const mstring &insert) const;
    mstring remove(int position, int len) const;
    mstring insert(int position, const mstring &s) const;
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
