#ifndef __DADA_UDP_H
#define __DADA_UDP_H

/*
 * UDP library functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "multilog.h"

#define KERNEL_BUFFER_SIZE_MAX     67108864
#define KERNEL_BUFFER_SIZE_DEFAULT 131071
#define STATS_INIT = {0, 0, 0, 0}

/* statistics on received and dropped */
typedef struct {
  uint64_t received;
  uint64_t dropped;
  uint64_t received_per_sec;
  uint64_t dropped_per_sec;
} stats_t;

stats_t * init_stats_t();

void reset_stats_t(stats_t * s);

size_t dada_sock_recv(int fd, char* buffer, size_t size, int flags);

size_t dada_sock_send(int fd, struct sockaddr_in addr, char *data, size_t size);

struct in_addr *atoaddr (char *address);

int dada_udp_sock_in(multilog_t* log, const char* iface, int port, int verbose);

int dada_udp_sock_out(int *fd, struct sockaddr_in * dagram, char *client, 
                      int port, int bcast, const char * bcast_addr);

int dada_udp_sock_set_buffer_size (multilog_t* log, int fd, int verbose, int pref_size);

#endif /* __DADA_UDP_H */
