#ifndef SRC_YFILEIO_H_
#define SRC_YFILEIO_H_

#include "ypointer.h"

#include <stdlib.h>

class filereader {
    int nFd;
    int bFinClose;

    filereader(const filereader&) =delete;
public:
    filereader(int fd, bool closeAfter = true) : nFd(fd), bFinClose(closeAfter) {}
    filereader(filereader&& rv) : nFd(rv.nFd), bFinClose(rv.bFinClose) {
        rv.nFd = -1;
    }

    ~filereader();
    filereader(const char *filename);

    /** @brief Read and zero terminate into user's buffer. */
    int read_all(char *buf, size_t buflen);

    /** @brief read all of filedescriptor and return a zero-terminated string.
     * @param assumeRegular If set, try to get the file size first and do a
     * single-step reading of that size. Otherwise, go to open-end
     * reading immediately.
     * @param timeoutMS Value to wait (milliseconds) and abort reading
     * @nErrorCode Error code (errno) or zero if succeeded
     * */
    fcsmart read_all(bool assumeRegular = true,
            int timeoutMS = -1,
            int* nErrorCode = nullptr);

    /**
     * Alternative, fully controllable version.
     * Returns 0 if succeeded, errno value otherwise.
     * Can take an existing buffer as input (owning it), will return
     * a new (or same) buffer with result data. fetchedSize must contain a
     * correct (less or equal) size of the specified buffer.
     */
    size_t read_all(bool assumeRegular,
                int timeoutMS,
                char* &buf,
                int *fetchedSize,
                size_t &allocatedSize);

    static bool make_pipe(int fds[2]);
};

struct file_raii {
    int m_fd;
    file_raii(int fd) : m_fd(fd) {}
    ~file_raii();
};

#if DRAFT_TBD
/**
 * Helper which finshes the incomplete reading and puts the result into a
 * a cache. Caller can fetch the result later, using something as key
 * (e.g. the command string). Optinal notification when the process has finished.
 */
class background_reader
{
    background_reader& getInstance();
    /**
     * Fetch a previous result from a similar call, unless it's expired
     */
    mstring fromCache(const mstring& cmdLine, bool clear);
    void finishReading(filereader&& rdr, const mstring& cmdLine,
            const timeval& readingExpTime,
            timeval& cacheExpTime, std::function<void(bool)> whenDoneReading);
};
#endif

#endif /* SRC_YFILEIO_H_ */
