#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "main.h"

extern "C"
{
    __attribute__((weak)) int _isatty(int fd)
    {
        if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
            return 1;

        errno = EBADF;
        return 0;
    }

    __attribute__((weak)) int _close(int fd)
    {
        if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
            return 0;

        errno = EBADF;
        return -1;
    }

    __attribute__((weak)) int _lseek(int fd, int ptr, int dir)
    {
        (void)fd;
        (void)ptr;
        (void)dir;

        errno = EBADF;
        return -1;
    }

    __attribute__((weak)) int _fstat(int fd, struct stat *st)
    {
        if (fd >= STDIN_FILENO && fd <= STDERR_FILENO)
        {
            st->st_mode = S_IFCHR;
            return 0;
        }

        errno = EBADF;
        return 0;
    }

    __attribute__((weak)) int _read(int file, char *ptr, int len)
    {
        (void)file;
        int DataIdx;
        for (DataIdx = 0; DataIdx < len; DataIdx++)
        {
            *ptr++ = 0;
        }
        return len;
    }

    __attribute__((weak)) int _write(int file, char *ptr, int len)
    {
        (void)file;
        int DataIdx;
        for (DataIdx = 0; DataIdx < len; DataIdx++)
        {
            fputc(*ptr++, (FILE *)file);
        }
        return len;
    }
}