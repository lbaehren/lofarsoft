#ifndef __DADA_SOCK_H
#define __DADA_SOCK_H

#include <sys/types.h>
#include <unistd.h>
#include <limits.h>

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif

#ifdef __cplusplus
extern "C" {
#endif

int sock_getname (char* self, int length, int alias);

int sock_create (int* port);
int sock_accept (int fd);

int sock_open (const char* host, int port);
int sock_close (int fd);

int sock_read (int fd, void* buf, size_t size);
int sock_write (int fd, const void* buf, size_t size);

int sock_ready (int fd, int* to_read, int* to_write, float timeout);
int sock_tm_read (int fd, void* buf, size_t size, float timeout);
int sock_tm_write (int fd, void* buf, size_t size, float timeout);

int sock_block (int fd);
int sock_nonblock (int fd);

#ifdef __cplusplus
}
#endif

#endif /* SOCK_H */

