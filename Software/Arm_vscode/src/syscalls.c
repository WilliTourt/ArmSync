/*
 * Minimal newlib syscall stubs for bare-metal targets.
 *
 * This project uses newlib-nano. If you provide only `_write()` for printf,
 * the linker may still pull in re-entrant wrappers that reference other
 * syscalls (e.g. `_close`, `_read`, `_lseek`, `_fstat`, `_isatty`).
 */

#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>

int _close(int file);
int _lseek(int file, int ptr, int dir);
int _read(int file, char * ptr, int len);
int _fstat(int file, struct stat * st);
int _isatty(int file);
int _open(const char * name, int flags, int mode);
int _kill(int pid, int sig);
int _getpid(void);
void _exit(int status);

int _close(int file)
{
    (void) file;
    errno = EBADF;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    (void) file;
    (void) ptr;
    (void) dir;
    errno = ESPIPE;
    return -1;
}

int _read(int file, char * ptr, int len)
{
    (void) file;
    (void) ptr;
    (void) len;
    errno = EBADF;
    return -1;
}

int _fstat(int file, struct stat * st)
{
    (void) file;

    if (NULL == st)
    {
        errno = EINVAL;
        return -1;
    }

    st->st_mode  = S_IFCHR;
    st->st_nlink = 1;
    return 0;
}

int _isatty(int file)
{
    (void) file;
    return 1;
}

int _open(const char * name, int flags, int mode)
{
    (void) name;
    (void) flags;
    (void) mode;
    errno = ENOENT;
    return -1;
}

int _kill(int pid, int sig)
{
    (void) pid;
    (void) sig;
    errno = EINVAL;
    return -1;
}

int _getpid(void)
{
    return 1;
}

void _exit(int status)
{
    (void) status;
    while (1)
    {
        ;
    }
}
