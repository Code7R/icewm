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
#include <regex.h>
#include <new>
#include "base.h"
#include "ascii.h"

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define dsoffset (offsetof(decltype(mstring::spod), cBytes) + 1)
#else
#define dsoffset 0
#endif

mstring::mstring(const char *str, size_t len) {
    spod.fCount = 0;
    if(!len)
        return;
    extend(len);
    memcpy(data(), str, len);
    data()[len] = 0x0;
}


mstring mstring::operator+(const mstring_view &rv) const {
    mstring ret;
    auto curLen = length();
    ret.extend(curLen + rv.length());
    memcpy(ret.data(), data(), length());
    memcpy(ret.data() + curLen, rv.data(), rv.length());
    ret.data()[ret.length()] = 0x0;
    return ret;
}

mstring::mstring(mstring &&other) {
    spod.fCount = 0;
    *this = std::move(other);
}

mstring& mstring::operator=(mstring_view rv) {

    // cannot optimize
    if(input_from_here(rv))
        return *this = mstring(rv);

    // if needing to move back to inplace, just reset
    if (rv.length() < MSTRING_INPLACE_SIZE)
        clear();
    if (rv.length() > length())
        extend(rv.length() - length());
    memcpy(data(), rv.data(), rv.length() + 1);
    return *this;
}

mstring& mstring::operator=(mstring &&rv) {
    if (this == &rv)
        return *this;

    // cannot optimize
    if(input_from_here(rv))
        return *this = mstring(rv);

    clear();
    memcpy(&spod, &rv.spod, sizeof(spod));
    // invalidate!
    rv.spod.fCount = 0;
    return *this;
}

mstring& mstring::operator +=(mstring &&rv) {

    if (!length())
        *this = std::move(rv);
    else if(input_from_here(rv))
        return (*this = mstring(mstring_view(*this)) + rv);
    else
        *this += (const mstring&) rv;

    return *this;
}

mstring& mstring::operator +=(const mstring_view& rv) {

    if(input_from_here(rv))
        return (*this = mstring(mstring_view(*this)) + rv);

    auto len = rv.length();
    auto curLen = length();
    if (len) {
        extend(len);
        memcpy(data() + curLen, rv.data(), len);
    }
    data()[curLen + len] = 0x0;
    return *this;
}

mstring mstring::operator +(mstring &&rv) const {
    mstring ret(*this);
    ret += std::move(rv);
    return ret;
}

mstring::mstring(long n) {
    spod.fCount = 0;
    spod.cBytes[0] = snprintf(data(), MSTRING_INPLACE_SIZE, "%ld", n);
}

void mstring::clear() {
    if (!isLocal())
        free(data());
    spod.fCount = 0;
}

void swap(mstring &a, mstring &b) {
    std::swap(a.spod, b.spod);
}
// user must fill in the extended data (=amount) AND terminator later
mstring& mstring::extend(size_t amount) {
    auto cur_len = length();
    auto new_len = cur_len + amount;
    if (!isLocal()) {
        spod.ext.pData = (char*) realloc((void*) spod.ext.pData, new_len + 1);
        // XXX: do something more useful if failed?
        if (!spod.ext.pData)
            abort();
        spod.fCount = new_len;
    } else if ((new_len + 1) > MSTRING_INPLACE_SIZE) {
        // ATM local, to become external -> transition
        auto p = (char*) malloc(new_len + 1);
        memcpy(p, data(), cur_len + 1);
        spod.ext.pData = p;
        spod.fCount = new_len;
    } else {
        // otherwise still in SSO range!
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        spod.cBytes[0] = new_len;
#else
    spod.fCount = new_len;
#endif
    }

    return *this;
}

const char* mstring::data() const {
    return !isLocal() ? spod.ext.pData : spod.cBytes + dsoffset;
}
char* mstring::data() {
    return !isLocal() ? spod.ext.pData : spod.cBytes + dsoffset;
}
// static_assert(offsetof(mstring::TRefData, pData) < sizeof(mstring::TRefData) - dsoffset);

mstring mstring::substring(size_t pos) const {
    return pos <= length() ? mstring(data() + pos, length() - pos) : null;
}

mstring mstring::substring(size_t pos, size_t len) const {
    return pos <= length() ?
            mstring(data() + pos, min(len, length() - pos)) : null;
}

bool mstring::split(unsigned char token, mstring *left, mstring *remain) const {
    PRECONDITION(token < 128);
    int splitAt = indexOf(char(token));
    if (splitAt < 0)
        return false;
    size_t i = size_t(splitAt);
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
    return sv.length() == length() && 0 == memcmp(sv.data(), data(), sv.length());
}

int mstring::collate(const mstring& s, bool ignoreCase) const {
    if (ignoreCase)
        return strcoll(this->lower(), s.lower());
    return strcoll(c_str(), s.c_str());
}

int mstring::compareTo(const mstring &s) const {
    size_t len = min(s.length(), length());
    int cmp = len ? memcmp(data(), s.data(), len) : 0;
    return cmp ? cmp : int(length()) - int(s.length());
}

bool mstring::copyTo(char *dst, size_t len) const {
    if (len > 0) {
        size_t copy = min(len - 1, length());
        if (copy)
            memcpy(dst, data(), copy);
        dst[copy] = 0;
    }
    return length() < len;
}

mstring mstring::replace(int pos, int len, const mstring &insert) const {
    return substring(0, size_t(pos)) + insert + substring(size_t(pos + len));
}

mstring mstring::remove(int pos, int len) const {
    return substring(0, size_t(pos)) + substring(size_t(pos + len));
}

mstring mstring::insert(int pos, const mstring &str) const {
    return substring(0, size_t(pos)) + str + substring(size_t(pos));
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
    ret.extend(length());
    for (size_t i = 0; i < length(); ++i) {
        ret.data()[i] = ASCII::toLower(data()[i]);
    }
    ret.data()[length()] = 0x0;
    return ret;
}

mstring mstring::upper() const {
    mstring ret;
    ret.extend(length());
    for (size_t i = 0; i < length(); ++i) {
        ret.data()[i] = ASCII::toUpper(data()[i]);
    }
    ret.data()[length()] = 0x0;
    return ret;
}

mstring mstring::trim() const {
    size_t k = 0, n = length();
    while (k < n && ASCII::isWhiteSpace(data()[k])) {
        ++k;
    }
    while (k < n && ASCII::isWhiteSpace(data()[n - 1])) {
        --n;
    }
    return substring(k, n - k);
}

mstring mstring::match(const char *regex, const char *flags) const {
    int compFlags = REG_EXTENDED;
    int execFlags = 0;
    for (int i = 0; flags && flags[i]; ++i) {
        switch (flags[i]) {
        case 'i':
            compFlags |= REG_ICASE;
            break;
        case 'n':
            compFlags |= REG_NEWLINE;
            break;
        case 'B':
            execFlags |= REG_NOTBOL;
            break;
        case 'E':
            execFlags |= REG_NOTEOL;
            break;
        }
    }

    regex_t preg;
    // XXX: maybe the compiled result should be cached for reuse, giving the user some handle to store
    int comp = regcomp(&preg, regex, compFlags);
    if (comp) {
        if (testOnce(regex, __LINE__)) {
            char rbuf[123] = "";
            regerror(comp, &preg, rbuf, sizeof rbuf);
            warn("match regcomp: %s", rbuf);
        }
        return null;
    }

    regmatch_t pos;
    int exec = regexec(&preg, data(), 1, &pos, execFlags);

    regfree(&preg);

    if (exec)
        return null;

    return mstring(data() + pos.rm_so, size_t(pos.rm_eo - pos.rm_so));
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
mstring::mstring(mstring_view a, mstring_view b, mstring_view c,
        mstring_view d, mstring_view e) {
    spod.fCount = 0;
    auto len = a.length() + b.length() + c.length() + d.length() + e.length();
    extend(len);
    memcpy(data(), a.data(), a.length());
    if(b.length())
        memcpy(data() + a.length(), b.data(), b.length());
    if (c.length()) {
        memcpy(data() + a.length() + b.length(), c.data(), c.length());
    }
    if (d.length()) {
        memcpy(data() + a.length() + b.length() + c.length(), d.data(),
                d.length());
    }
    if (e.length()) {
        memcpy(data() + a.length() + b.length() + c.length() + d.length(),
                e.data(), e.length());
    }
    data()[len] = 0x0;
}

// vim: set sw=4 ts=4 et:
