
#include <sys/types.h>
#include <sys/socket.h>

#ifdef MSG_NOSIGNAL
#define DADA_MSG_FLAGS MSG_NOSIGNAL | MSG_WAITALL
#else
#define DADA_MSG_FLAGS MSG_WAITALL
#endif

#include <pthread.h>

#ifdef PTHREAD_MUTEX_RECURSIVE
#define DADA_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE
#else
#define DADA_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif
