#include <fnmatch.h>

#include "mstringex.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <regex.h>
#include <new>
#include "base.h"
#include "ascii.h"

enum STATE_FLAGS {
    STATE_NONE = 0,
    STATE_COMPILED = 1,
    STATE_ERROR = 2
};

mstring mstring::match(const precompiled_regex &rex) const {
    if (rex.stateFlags == STATE_ERROR) {
        warn("match regcomp: %s", rex.mCompError);
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
        execFlags(0), stateFlags(0) {

    auto compFlags = REG_EXTENDED;
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
    if (0 == comp) {
        stateFlags = STATE_COMPILED;
    } else {
        stateFlags = STATE_ERROR;
        auto sResult = new char[200];
        regerror(comp, &preg, sResult, 200);
        sResult[199] = 0x0;
        regfree(&preg);
        mCompError = sResult;
    }
}

precompiled_regex::~precompiled_regex() {
    switch(stateFlags)
    {
    case STATE_NONE: return;
    case STATE_COMPILED:
        regfree(&preg);
        return;
    case STATE_ERROR:
        delete [] mCompError;
        return;
    }
}

bool precompiled_regex::matchIn(const char *s) const {
    if (STATE_ERROR == stateFlags) {
        warn("match regcomp: %s", mCompError);
        return false;
    }
    regmatch_t pos;
    return 0 == regexec(&preg, s, 1, &pos, execFlags);
}
