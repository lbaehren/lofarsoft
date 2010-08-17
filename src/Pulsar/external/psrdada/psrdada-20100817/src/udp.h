#ifndef __UDP_H
#define __UDP_H

/*
 * Generic functions for udp sockets
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "multilog.h"

#define KERNEL_BUFFER_SIZE_MAX     67108864
#define KERNEL_BUFFER_SIZE_DEFAULT 131071
#define STATS_INIT = {0, 0, 0, 0}

typedef struct{
  uint64_t received;
  uint64_t dropped;
  uint64_t received_per_sec;
  uint64_t dropped_per_sec;
}stats_t;

int64_t sock_recv (int fd, char* buffer, uint64_t size, int flags)
{
  int64_t received = 0;
  received = recvfrom (fd, buffer, size, 0, NULL, NULL);
                                                                                                        
  if (received < 0) {
    perror ("sock_recv recvfrom");
    return -1;
  }
  if (received == 0) {
    fprintf (stderr, "sock_recv received zero bytes\n");
  }
                                                                                                        
  return received;
}

/*
 * Creates a UDP socket with the following parameters:
 *
 *   log        multilog to print messages to
 *   interface  interface to open socket on
 *   port       port to listen on
 *
 *   set the buffer size to 64MB
 *   prints output errors to log
 *   return a fd of the opened socket
 */
int dada_create_udp_socket(multilog_t* log, const char* interface, int port, int verbose) {

  const int std_buffer_size = KERNEL_BUFFER_SIZE_DEFAULT;
  const int pref_buffer_size = KERNEL_BUFFER_SIZE_MAX;

  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (fd < 0) {
    multilog(log, LOG_ERR, "Could not created UDP socket: %s\n",
             strerror(errno));
    return -1;
  }

  if (verbose) 
    multilog(log, LOG_INFO, "Created UDP socket\n");

  struct sockaddr_in udp_sock;
  bzero(&(udp_sock.sin_zero), 8);                     // clear the struct
  udp_sock.sin_family = AF_INET;                      // internet/IP
  udp_sock.sin_port = htons(port);                    // set the port number
  if (strcmp(interface,"any") == 0) {
    udp_sock.sin_addr.s_addr = htonl(INADDR_ANY);     // from any interface
  } else {
    udp_sock.sin_addr.s_addr = inet_addr(interface);  // from a specific IP address
  }

  if (bind(fd, (struct sockaddr *)&udp_sock, sizeof(udp_sock)) == -1) {
    multilog (log, LOG_ERR, "Error binding UDP socket: %s\n", strerror(errno));
    return -1;
  }

  if (verbose) 
    multilog(log, LOG_INFO, "UDP socket bound to %s:%d\n", interface, port);

  // try setting the buffer to the maximum, warn if we cant
  int len = 0;
  int value = pref_buffer_size;
  int retval = 0;
  len = sizeof(value);
  retval = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, len);
  if (retval != 0) {
    perror("setsockopt SO_RCVBUF");
    return -1;
  }

  // now check if it worked....
  len = sizeof(value);
  value = 0;
  retval = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, (socklen_t *) &len);
  if (retval != 0) {
    perror("getsockopt SO_RCVBUF");
    return -1;
  }

  // If we could not set the buffer to the desired size, warn...
  if (value/2 != pref_buffer_size) {
    multilog (log, LOG_WARNING, "Warning. Failed to set udp socket's "
              "buffer size to: %d, falling back to default size: %d\n",
              pref_buffer_size, std_buffer_size);

    len = sizeof(value);
    value = std_buffer_size;
    retval = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, len);
    if (retval != 0) {
      perror("setsockopt SO_RCVBUF");
      return -1;
    }

    // Now double check that the buffer size is at least correct here
    len = sizeof(value);
    value = 0;
    retval = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value,
                        (socklen_t *) &len);
    if (retval != 0) {
      perror("getsockopt SO_RCVBUF");
      return -1;
    }


    // If we could not set the buffer to the desired size, warn...
    if (value/2 != std_buffer_size) {
      multilog (log, LOG_WARNING, "Warning. Failed to set udp socket's "
                "buffer size to: %d\n", std_buffer_size);
    }

  } else {

    if (verbose)
      multilog(log, LOG_INFO, "UDP socket buffer size set to %d\n", pref_buffer_size);

  }

  return fd;
}

#endif /* __UDP_H */
