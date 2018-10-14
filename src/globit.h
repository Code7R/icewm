#ifndef GLOBIT_H
#define GLOBIT_H

#include "ystring.h"
#include "mstring.h"
#include "yarray.h"

struct exeglobber
{
    exeglobber(mstring pattern);
    ~exeglobber();
    struct tMatch
    {
        mstring path;
        bool isDir;
    };
    YArray<tMatch> matches;
};

#endif /* GLOBIT_H */

// vim: set sw=4 ts=4 et:
