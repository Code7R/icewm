/*
 * IceWM
 *
 * Copyright (C) 2004,2005 Marko Macek
 */
#include "config.h"

#include "mstring.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <new>
#include <cstdint>
#include <cstdarg>

#include "base.h"
#include "ascii.h"

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define dsoffset (offsetof(decltype(mstring::spod), cBytes) + 1)
#define posSmallCounter 0
#else
#define dsoffset 0
#define posSmallCounter 0 (sizeof(size_type)-1)
#endif

mstring::mstring(const char *str, size_type len) {
    set_len(0);
    spod.ext.pData = nullptr;
    if (len) {
        extendTo(len);
        memcpy(data(), str, len);
    }
    term(len);
}

mstring mstring::operator+(const mstring_view &rv) const {
    return mstring(*this, rv);
}

mstring::mstring(mstring &&other) {
    set_len(0);
    *this = std::move(other);
}

mstring& mstring::operator=(mstring_view rv) {
    // cannot optimize?
    if (input_from_here(rv))
        return *this = mstring(rv);
    auto l = rv.length();
    // if needing to move back to inplace, just reset
    if (l <= MSTRING_INPLACE_MAXLEN)
        clear();
    extendTo(l);
    memcpy(data(), rv.data(), l);
    term(l);
    return *this;
}

mstring& mstring::operator=(mstring &&rv) {
    if (this == &rv)
        return *this;
    // cannot optimize?
    if (input_from_here(rv))
        return *this = mstring(rv);
    clear();
    swap(*this, rv);
    return *this;
}

mstring& mstring::operator +=(mstring &&rv) {
    if (isEmpty())
        *this = std::move(rv);
    else if (input_from_here(rv))
        *this = mstring(mstring_view(*this)) + rv;
    else
        *this += mstring_view(rv);

    return *this;
}

mstring& mstring::operator +=(const mstring_view &rv) {

    if (input_from_here(rv))
        return (*this = mstring(mstring_view(*this)) + rv);

    auto len = rv.length();
    auto curLen = length();
    if (len) {
        extendBy(len);
        memcpy(data() + curLen, rv.data(), len);
    }
    term(curLen + len);
    return *this;
}

mstring mstring::operator +(mstring &&rv) const {
    mstring ret(*this);
    ret += std::move(rv);
    return ret;
}

inline void mstring::term(size_type len) {
    data()[len] = 0x0;
}

void mstring::set_len(size_type len) {
#ifdef SSO_NOUTYPUN
    spod.count = len;
#else
    if (len > MSTRING_INPLACE_MAXLEN) {
        spod.count = len;
        MSTRING_INPLACE_FLAG = false;
    } else {
        spod.cBytes[posSmallCounter] = uint8_t(len);
        MSTRING_INPLACE_FLAG = true;
    }
#endif
}

void mstring::clear() {
    if (!isLocal())
        free(data());
    set_len(0);
    term(0);
}

void swap(mstring &a, mstring &b) {
    std::swap(a.spod, b.spod);
}

inline void mstring::extendBy(size_type amount) {
    return extendTo(length() + amount);
}

// user must fill in the extended data (=amount) AND terminator later
inline void mstring::extendTo(size_type new_len) {
#ifdef SSO_NOUTYPUN
    if (isLocal()) {
        if (new_len <= MSTRING_INPLACE_MAXLEN) {
            set_len(new_len);
        } else {
            // local before, to become external now
            auto p = (char*) malloc(new_len + 1);
            if(!p)
                abort();

            memcpy(p, data(), length() + 1);
            memcpy(spod.cBytes, &p, sizeof(p));
            set_len(new_len);
        }
    } else {
        auto p = data();
        p = (char*) realloc((void*) p, new_len + 1);
        // XXX: do something more useful if failed?
        if (!p)
            abort();
        memcpy(spod.cBytes, &p, sizeof(p));
        set_len(new_len);
    }
#else
    if (isLocal()) {
        if (new_len <= MSTRING_INPLACE_MAXLEN) {
            set_len(new_len);
        } else {
            // local before, to become external now
            auto p = (char*) malloc(new_len + 1);
            if(!p)
                abort();
            memcpy(p, data(), length() + 1);
            spod.ext.pData = p;
            set_len(new_len);
        }
    } else {
        spod.ext.pData = (char*) realloc((void*) spod.ext.pData, new_len + 1);
        // XXX: do something more useful if failed?
        if (!spod.ext.pData)
            abort();
        set_len(new_len);
    }
#endif
}

const char* mstring::data() const {
    auto inplace = isLocal();
#ifdef SSO_NOUTYPUN
    if(inplace)
        return spod.cBytes;
    char *ret;
    memcpy(&ret, spod.cBytes, sizeof(ret));
    return ret;
#else
    if (!inplace)
        return spod.ext.pData;
    return ((const char*) spod.cBytes) + dsoffset;
#endif
}
char* mstring::data() {
    auto inplace = isLocal();
#ifdef SSO_NOUTYPUN
    if(inplace)
        return spod.cBytes;
    char *ret;
    memcpy(&ret, spod.cBytes, sizeof(ret));
    return ret;
#else
    if (!inplace)
        return spod.ext.pData;
    return ((char*) spod.cBytes) + dsoffset;
#endif
}
// static_assert(offsetof(mstring::TRefData, pData) < sizeof(mstring::TRefData) - dsoffset);

mstring mstring::substring(size_type pos) const {
    auto l = length();
    return pos <= l ? mstring(data() + pos, l - pos) : null;
}

mstring mstring::substring(size_type pos, size_type len) const {
    return pos <= length() ?
            mstring(data() + pos, min(len, length() - pos)) : null;
}

bool mstring::split(unsigned char token, mstring *left, mstring *remain) const {
    PRECONDITION(token < 128);
    int splitAt = indexOf(char(token));
    if (splitAt < 0)
        return false;
    size_type i = size_t(splitAt);
    mstring l(substring(0, i));
    mstring r(substring(i + 1, length() - i - 1));
    *left = l;
    *remain = r;
    return true;
}

bool mstring::splitall(unsigned char token, mstring *left,
        mstring *remain) const {
    if (split(token, left, remain))
        return true;
    if (isEmpty())
        return false;
    *left = *this;
    *remain = null;
    return true;
}

int mstring::charAt(int pos) const {
    return size_t(pos) < length() ? data()[pos] : -1;
}

bool mstring::startsWith(mstring_view s) const {
    if (s.isEmpty())
        return true;
    if (s.length() > length())
        return false;
    return (0 == memcmp(data(), s.data(), s.length()));
}

bool mstring::endsWith(mstring_view s) const {
    if (s.isEmpty())
        return true;
    if (s.length() > length())
        return false;
    return (0 == memcmp(data() + length() - s.length(), s.data(), s.length()));
}

int mstring::find(mstring_view str) const {
    const char *found =
            (str.isEmpty() || isEmpty()) ?
                    nullptr :
                    static_cast<const char*>(memmem(data(), length(),
                            str.data(), str.length()));
    return found ? int(found - data()) : (str.isEmpty() - 1);
}

int mstring::indexOf(char ch) const {
    const char *str =
            isEmpty() ?
                    nullptr :
                    static_cast<const char*>(memchr(data(), ch, length()));
    return str ? int(str - data()) : -1;
}

int mstring::lastIndexOf(char ch) const {
    const char *str =
            isEmpty() ?
                    nullptr :
                    static_cast<const char*>(memrchr(data(), ch, length()));
    return str ? int(str - data()) : -1;
}

int mstring::count(char ch) const {
    int num = 0;
    for (const char *str = data(), *end = str + length(); str < end; ++str) {
        str = static_cast<const char*>(memchr(str, ch, end - str));
        if (str == nullptr)
            break;
        ++num;
    }
    return num;
}

bool mstring_view::operator==(mstring_view sv) const {
    auto lenA=sv.length();
    auto lenB = length();
    return lenA == lenB && 0 == memcmp(sv.data(), data(), lenA);
}

int mstring::collate(const mstring &s, bool ignoreCase) const {
    if (ignoreCase)
        return strcoll(this->lower(), s.lower());
    return strcoll(c_str(), s.c_str());
}

int mstring::compareTo(const mstring &s) const {
    auto len = min(s.length(), length());
    int cmp = len ? memcmp(data(), s.data(), len) : 0;
    return cmp ? cmp : int(length()) - int(s.length());
}

bool mstring::copyTo(char *dst, size_type len) const {
    if (len > 0) {
        auto copy = min(len - 1, length());
        if (copy)
            memcpy(dst, data(), copy);
        dst[copy] = 0;
    }
    return length() < len;
}

mstring mstring::replace(size_type pos, size_type len,
        const mstring &insert) const {
    return substring(0, size_t(pos)) + insert + substring(size_t(pos + len));
}

mstring mstring::remove(size_type pos, size_type len) const {
    mstring_view l(data(), size_t(pos));
    mstring_view r(data() + size_t(pos) + size_t(len), length() - pos - len);
    return mstring(l, r);
}

mstring mstring::insert(size_type pos, const mstring &str) const {
    mstring_view l(data(), pos);
    mstring_view r(data() + pos, length() - pos);
    return mstring(l, str, r);
}

mstring mstring::searchAndReplaceAll(const mstring &s, const mstring &r) const {
    mstring modified(*this);
    const int step = int(1 + r.length() - s.length());
    for (int offset = 0; size_t(offset) + s.length() <= modified.length();) {
        int found = offset + modified.substring(size_t(offset)).find(s);
        if (found < offset)
            break;
        modified = modified.replace(found, int(s.length()), r);
        offset = max(0, offset + step);
    }
    return modified;
}

mstring mstring::lower() const {
    mstring ret;
    auto l = length();
    ret.extendTo(l);
    for (size_type i = 0; i < l; ++i) {
        ret.data()[i] = ASCII::toLower(data()[i]);
    }
    ret.term(l);
    return ret;
}

mstring mstring::upper() const {
    mstring ret;
    auto l = length();
    ret.extendBy(l);
    for (size_type i = 0; i < l; ++i) {
        ret.data()[i] = ASCII::toUpper(data()[i]);
    }
    ret.term(l);
    return ret;
}

mstring mstring::trim() const {
    size_type k = 0, n = length();
    while (k < n && ASCII::isWhiteSpace(data()[k])) {
        ++k;
    }
    while (k < n && ASCII::isWhiteSpace(data()[n - 1])) {
        --n;
    }
    return substring(k, n - k);
}

mstring_view::mstring_view(const char *s) :
        m_data(s), m_size(s ? strlen(s) : 0) {
}

inline bool mstring::input_from_here(mstring_view sv) {
    const char *svdata = sv.data(), *svend = svdata + sv.length(), *begin =
            data(), *end = begin + length();
    // inrange() suits here because of extra terminator char
    return inrange(svdata, begin, end) || inrange(svend, begin, end);
}

// this is extra copy-pasty in order to let the compiler optimize it better
mstring::mstring(mstring_view a, mstring_view b, mstring_view c, mstring_view d,
        mstring_view e) {
    set_len(0);
    auto len = a.length() + b.length() + c.length() + d.length() + e.length();
    extendBy(len);
    term(len);
    memcpy(data(), a.data(), a.length());
    auto pos = a.length();
    if (!b.isEmpty()) {
        memcpy(data() + pos, b.data(), b.length());
        pos += b.length();
    }
    if (!c.isEmpty()) {
        memcpy(data() + pos, c.data(), c.length());
        pos += c.length();
    }
    if (!d.isEmpty()) {
        memcpy(data() + pos, d.data(), d.length());
        pos += d.length();
    }
    if (!e.isEmpty()) {
        memcpy(data() + pos, e.data(), e.length());
        pos += e.length();
    }
    term(pos);
    //assert(pos == len);
}

mstring::mstring(mstring_view a, mstring_view b) :
        mstring(a, b, mstring_view(), mstring_view(), mstring_view()) {
}

mstring::mstring(mstring_view a, mstring_view b, mstring_view c) :
        mstring(a, b, c, mstring_view(), mstring_view()) {
}

bool mstring::equals(const char *&sz) const {
    return 0 == strcmp(sz, c_str());
}

mstring& mstring::appendFormat(const char *fmt, ...) {
    auto startLen = length();
    // it might fit into locally available space, can try that for free!
    auto haveLocalSpace =
            startLen < MSTRING_INPLACE_MAXLEN ?
                    (MSTRING_INPLACE_MAXLEN + 1 - startLen) : 0;
    va_list ap;
    va_start(ap, fmt);
    auto wantsLen = vsnprintf(haveLocalSpace ? data() + startLen : nullptr,
            haveLocalSpace, fmt, ap);
    va_end(ap);
    if (wantsLen < 0)
    {
        warn("bad format string or parameters: %s", fmt);
        data()[startLen] = 0x0;
        return *this;
    }
    if (haveLocalSpace) {
        if (size_type(wantsLen) < haveLocalSpace) {
            // it did all fit? Great!
            set_len(startLen + wantsLen);
            return *this;
        }
        // not lucky, undo the effects
        data()[startLen] = 0x0;
    }
    extendBy(wantsLen);
    va_start(ap, fmt);
    vsnprintf(data() + startLen, wantsLen + 1, fmt, ap);
    va_end(ap);
    return *this;
}

size_t mstring::getHashCode() const {
    // shortcut with little type punning on GCC but better not with others
#ifndef SSO_NOUTYPUN
    union {
        size_t ret;
        char bytes[sizeof(ret)];
    };
    ret = length();
    auto p = data();
    const auto end = p + ret;
    const auto fastEnd = end - (ret % sizeof(ret));
    for (; p < fastEnd; p += sizeof(ret))
        ret ^= *((size_t*) p);
    for (auto pr = bytes; p < end; ++p, ++pr)
        *pr ^= *p;
    return ret;
#else
    return strhash(c_str());
#endif
}


// vim: set sw=4 ts=4 et:
