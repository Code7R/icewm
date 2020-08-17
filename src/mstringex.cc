#include <fnmatch.h>

#include "mstringex.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <new>
#include "base.h"
#include "ascii.h"

mstring mstring::match(const precompiled_regex &rex) const {
    if (rex.mCompError) {
        warn("match regcomp: %s", rex.mCompError.data());
        return null;
    }
    regmatch_t pos;
    int exec = regexec(&rex.preg, data(), 1, &pos, rex.execFlags);
    return exec ?
            null : mstring(data() + pos.rm_so, size_t(pos.rm_eo - pos.rm_so));
}

mstring mstring::match(const char *regex, const char *flags) const {
    return match(precompiled_regex(regex, flags));
}

precompiled_regex::precompiled_regex(const char *regex, const char *flags) :
        mCompError(nullptr) {

    auto compFlags = REG_EXTENDED;
    execFlags = 0;
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

    int comp = regcomp(&preg, regex, compFlags);
    if (comp) {
        mCompError = new char[200];
        mCompError[200] = 0x0;
        regerror(comp, &preg, mCompError.data(), 200);
    }
}

precompiled_regex::~precompiled_regex() {
    if (!mCompError)
        regfree(&preg);
}

bool precompiled_regex::inside(const char *s) {
    if (mCompError) {
        warn("match regcomp: %s", mCompError.data());
        return false;
    }
    regmatch_t pos;
    return 0 == regexec(&preg, s, 1, &pos, execFlags);
}
