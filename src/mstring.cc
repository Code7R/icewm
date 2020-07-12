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


#define dsoffset offsetof(mstring::TRefData, pData)
#define dsmax (sizeof(mstring::TRefData) - dsoffset)

/*
void MStringRef::alloc(size_t len) {
    fStr = new (malloc(sizeof(MStringData) + len + 1)) MStringData();
}

void MStringRef::create(const char* str, size_t len) {
    if (len) {
        alloc(len);
        if (str) {
            strncpy(fStr->fStr, str, len);
            fStr->fStr[len] = 0;
        } else {
            memset(fStr->fStr, 0, len + 1);
        }
    } else {
        fStr = nullptr;
    }
}

mstring::mstring(const MStringRef& str, size_t offset, size_t count):
    fRef(str),
    fOffset(offset),
    fCount(count)
{
    acquire();
}

mstring::mstring(const char* str1, size_t len1, const char* str2, size_t len2):
    fRef(len1 + len2), fOffset(0), fCount(len1 + len2)
{
    if (fRef) {
        if (len1)
            strncpy(fRef->fStr, str1, len1);
        if (len2)
            strncpy(fRef->fStr + len1, str2, len2);
        fRef[fCount] = 0;
        fRef.acquire();
    }
}
*/

mstring::mstring(const char* str):
    mstring(str, str ? strlen(str) : 0)
{
}

mstring::mstring(const char* str, size_t len) : mstring(str, len, false)
{
}

mstring::mstring(const char* str, size_t len, bool nocopy)
{
    ext.fCount = len;
    if(isExt())
        ext.pData = (char*) malloc(len+1);
    if(nocopy)
        return;
    memcpy(data(), str, len);
    data()[len] = 0x0;
}

mstring::mstring(const char *str1, const char *str2, const char *str3) {
    size_t len1 = str1 ? strlen(str1) : 0;
    size_t len2 = str2 ? strlen(str2) : 0;
    size_t len3 = str3 ? strlen(str3) : 0;
    ext.fCount = len1 + len2 + len3;
    if (isExt()) ext.pData = (char*) malloc(ext.fCount + 1);
    if(len1) memcpy(data(), str1, len1);
    if(len2) memcpy(data() + len1, str2, len2);
    if(len3) memcpy(data() + len1 + len2, str3, len3);
    data()[ext.fCount] = 0x0;
}


mstring::mstring(const char *str1, size_t len1, const char *str2, size_t len2)
{
    ext.fCount = len1 + len2;
    if (isExt())
        ext.pData = (char*) malloc(ext.fCount + 1);
    if (len1)
        memcpy(data(), str1, len1);
    if (len2)
        memcpy(data() + len1, str2, len2);
    data()[ext.fCount] = 0x0;
}

mstring::mstring(mstring&& r)
{
    (*this) = r;
}

mstring::~mstring() {
    if(isExt())
        free(data());
    ext.fCount=0;
}

mstring::mstring(const char *str1, const char *str2) :
        mstring(str1, str1?strlen(str1):0, str2, str2?strlen(str2):0) {
}


mstring mstring::operator+(const mstring& rv) const {
    return mstring(data(), length(), rv.data(), rv.length());
}

void mstring::operator+=(const mstring& rv) {
    auto nlen = rv.length() + length();
    if (isExt()) {
        auto pNew = (char*) realloc((void*) ext.pData, nlen + 1);
        // XXX: do something more useful if failed?
        if (pNew == ext.pData)
            abort();
        ext.pData = pNew;
    } else if (nlen + 1 >= dsmax) {
        // ATM local, to become external -> transition
        auto p = (char*) malloc(ext.fCount + 1);
        memcpy(p, data(), ext.fCount);
        ext.pData = p;
    }
    auto dst = data() + ext.fCount;
    memcpy(dst, rv.data(), rv.length());
    ext.fCount+=rv.length();
    dst[rv.length()] = 0x0;
}

mstring& mstring::operator=(const mstring &rv) {
    if(&rv == this)
        return *this;

    if(rv.isExt())
    {
        ext.fCount = rv.ext.fCount;
        ext.pData = (char*) malloc(ext.fCount + 1);
        memcpy(ext.pData, rv.ext.pData, rv.ext.fCount+1); // with terminator
    }
    else
        memcpy(here, rv.here, sizeof(here));
    return *this;
}


mstring& mstring::operator=(mstring&& rv) {
    if(this == &rv)
        return *this;

    if(isExt())
    {
        free(ext.pData);
    }
    ext.fCount = rv.ext.fCount;

    if(rv.isExt())
    {
        ext.pData = rv.ext.pData;
        rv.ext.pData = nullptr;
        rv.ext.fCount = 0;
    }
    else
    {
        memcpy(data(), rv.data(), rv.length());
        data()[ext.fCount] = 0x0;
    }

    return *this;
}

mstring mstring::substring(size_t pos) const {
    return pos <= length()
        ? mstring(data() + pos, ext.fCount - pos)
        : null;
}

mstring mstring::substring(size_t pos, size_t len) const {
    return pos <= length()
        ? mstring(data() + pos, min(len, ext.fCount - pos))
        : null;
}

bool mstring::split(unsigned char token, mstring *left, mstring *remain) const {
    PRECONDITION(token < 128);
    int splitAt = indexOf(char(token));
    if (splitAt >= 0) {
        size_t i = size_t(splitAt);
        mstring l(substring(0, i));
        mstring r(substring(i + 1, length() - i - 1));
        *left = l;
        *remain = r;
        return true;
    }
    return false;
}

bool mstring::splitall(unsigned char token, mstring *left, mstring *remain) const {
    if (split(token, left, remain))
        return true;

    if (nonempty()) {
        *left = *this;
        *remain = null;
        return true;
    }
    return false;
}

int mstring::charAt(int pos) const {
    return size_t(pos) < length() ? data()[pos] : -1;
}

bool mstring::startsWith(const mstring &s) const {
    return s.isEmpty() || (s.length() <= length()
        && 0 == memcmp(data(), s.data(), s.length()));
}

bool mstring::endsWith(const mstring &s) const {
    return s.isEmpty() || (s.length() <= length()
        && 0 == memcmp(data() + length() - s.length(), s.data(), s.length()));
}

int mstring::find(const mstring &str) const {
    const char* found = (str.isEmpty() || isEmpty()) ? nullptr :
        static_cast<const char*>(memmem(
                data(), length(), str.data(), str.length()));
    return found ? int(found - data()) : (str.isEmpty() - 1);
}

int mstring::indexOf(char ch) const {
    const char *str = isEmpty() ? nullptr :
        static_cast<const char *>(memchr(data(), ch, ext.fCount));
    return str ? int(str - data()) : -1;
}

int mstring::lastIndexOf(char ch) const {
    const char *str = isEmpty() ? nullptr :
        static_cast<const char *>(memrchr(data(), ch, ext.fCount));
    return str ? int(str - data()) : -1;
}

int mstring::count(char ch) const {
    int num = 0;
    for (const char* str = data(), *end = str + length(); str < end; ++str) {
        str = static_cast<const char *>(memchr(str, ch, end - str));
        if (str == nullptr)
            break;
        ++num;
    }
    return num;
}

bool mstring::equals(const char *str) const {
    return equals(str, str ? strlen(str) : 0);
}

bool mstring::equals(const char *str, size_t len) const {
    return len == length() && 0 == memcmp(str, data(), len);
}

bool mstring::equals(const mstring &str) const {
    return equals(str.data(), str.length());
}

int mstring::collate(mstring s, bool ignoreCase) {
    if (!ignoreCase)
        return strcoll(*this, s);
    else
        return strcoll(this->lower(), s.lower());
}

int mstring::compareTo(const mstring &s) const {
    size_t len = min(s.length(), length());
    int cmp = len ? memcmp(data(), s.data(), len) : 0;
    return cmp ? cmp : int(length()) - int(s.length());
}

bool mstring::copyTo(char *dst, size_t len) const {
    if (len > 0) {
        size_t copy = min(len - 1, ext.fCount);
        if (copy) memcpy(dst, data(), copy);
        dst[copy] = 0;
    }
    return ext.fCount < len;
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

mstring mstring::searchAndReplaceAll(const mstring& s, const mstring& r) const {
    mstring modified(*this);
    const int step = int(1 + r.length() - s.length());
    for (int offset = 0; size_t(offset) + s.length() <= modified.length(); ) {
        int found = offset + modified.substring(size_t(offset)).find(s);
        if (found < offset) {
            break;
        }
        modified = modified.replace(found, int(s.length()), r);
        offset = max(0, offset + step);
    }
    return modified;
}

mstring mstring::lower() const {
    mstring mstr(nullptr, ext.fCount, true);
    for (size_t i = 0; i < ext.fCount; ++i) {
        mstr.data()[i] = ASCII::toLower(data()[i]);
    }
    mstr.data()[ext.fCount] = 0x0;
    return mstr;
}

mstring mstring::upper() const {
    mstring mstr(nullptr, ext.fCount, true);
    for (size_t i = 0; i < ext.fCount; ++i) {
        mstr.data()[i] = ASCII::toUpper(data()[i]);
    }
    mstr.data()[ext.fCount] = 0x0;
    return mstr;
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

mstring mstring::match(const char* regex, const char* flags) const {
    int compFlags = REG_EXTENDED;
    int execFlags = 0;
    for (int i = 0; flags && flags[i]; ++i) {
        switch (flags[i]) {
            case 'i': compFlags |= REG_ICASE; break;
            case 'n': compFlags |= REG_NEWLINE; break;
            case 'B': execFlags |= REG_NOTBOL; break;
            case 'E': execFlags |= REG_NOTEOL; break;
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

const char* mstring::data() const {
    return isExt() ? ext.pData : here + dsoffset;
}
char* mstring::data() {
    return isExt() ? ext.pData : here + dsoffset;
}
bool mstring::isExt() const
{
    static_assert(offsetof(mstring::TRefData, pData) < sizeof(mstring::TRefData) - dsoffset);
    return ext.fCount >= dsmax;
}

mstring::mstring(long n) {
    ext.fCount = snprintf(data(), dsmax, "%ld", n);
}

// vim: set sw=4 ts=4 et:
