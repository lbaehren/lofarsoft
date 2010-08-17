#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>     
#include <sys/timeb.h>
#include <sys/time.h>       
#include <sys/socket.h>  
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <getopt.h>

typedef struct disktime {
  double         time;
  unsigned long  size;
} disktime;

typedef struct writesocks {
  unsigned   nsocks;
  int *      socks;
  char **    hosts;
} writesocks;


#define NBUF       3    /* Minimum 3 buffers or locking will fail */
#define DEFAULT_BUFSIZE      2
#define MAXSTR              200 

#define DEBUG(x)

void usage(void);
double tim(void);
void gettime(disktime **, long, int *, int *);
void kill_signal (int);

int setup_net(int port, int window_size, int *listensock, int quiet);
int waitconnection (int window_size, int port, int listensock, 
                    int *serversock, writesocks *socks, int quiet);
void throttle_rate (double firsttime, float datarate, 
                    unsigned long long totalsize, double *tbehind);
void *netwrite(void *arg);
void *netread(void *arg);

volatile int time_to_quit = 0;
volatile int sig_received = 0;

/* Globals needed by network threads */
int  nfullbuf;
int bwrote;        /* Amount of net data written between updates */
int bufsize;
int write_failure;

char *buf[NBUF];
ssize_t nbufsize[NBUF];
pthread_mutex_t bufmutex[NBUF];
pthread_mutex_t globalmutex = PTHREAD_MUTEX_INITIALIZER;

int main (int argc, char * const argv[]) {
  int i, j, status, ofile, opt, tmp, nwrote;
  int ibuf, jbuf, nupdate, ntowrite;
  int serversock, clientsock, listensock;
  char msg[MAXSTR+50], *poff;
    
  int quiet = 0;          /* verbosity control */

  double firsttime, t1, t2;
  float ftmp, speed;
  double tbehind;
  unsigned long long totalsize;
  pthread_t netthread;

  int debug = 0;  
  float timeupdate = 50;        /* Time every 50 MiB by default */
  int portserver = 8000;        /* Port to listen on */
  int portclient = 8000;        /* Port to connect to */
  int window_size1 = -1;        /* 256 kilobytes */
  int window_size2 = -1;        /* 256 kilobytes */
  float datarate = 0;           /* Limit read/write to this data rate */
  char hostname[MAXSTR+1] = ""; /* Host name to send data to */
  struct hostent     *hostptr;
  int one_transfer = 0;
  
  bufsize   = DEFAULT_BUFSIZE * 1000*1000;
  ofile = -1;
  i = -1;
  write_failure = 0;

  writesocks socks;
  socks.nsocks = 0;
  socks.socks = 0;
  socks.hosts = 0;


  //nread = 0;

  srand(time(NULL));

  while ((opt=getopt(argc,argv,"b:w:p:z:t:H:Dqh1")) != -1) {

    switch (opt) {
    
      case 'b':
        status = sscanf(optarg, "%f", &ftmp);
        if (status!=1)
          fprintf(stderr, "Bad blocksize option %s\n", optarg);
        else
          bufsize = ftmp * 1000*1000;
      break;

      case 'w':
        status = sscanf(optarg, "%f", &ftmp);
        if (status!=1)
          fprintf(stderr, "Bad window option %s\n", optarg);
        else
          window_size1 = ftmp * 1024;
          window_size2 = ftmp * 1024;
        break;

      case 'p':
        status = sscanf(optarg, "%d", &tmp);
        if (status!=1)
          fprintf(stderr, "Bad port option %s\n", optarg);
        else {
          portserver = tmp;
          portclient = tmp;
        }
        break;

      case 'z':
        status = sscanf(optarg, "%f", &ftmp);
        if (status!=1)
          fprintf(stderr, "Bad rate option %s\n", optarg);
        else
          datarate = ftmp * 1000*1000/8;
        break;

      case 't':
        status = sscanf(optarg, "%d", &tmp);
        if (status!=1)
          fprintf(stderr, "Bad time update option %s\n", optarg);
        else
          timeupdate = tmp;
        break;

      // Hosts to send data to
      case 'H':
        if (strlen(optarg)>MAXSTR) {
          fprintf(stderr, "Hostname too long\n");
          return(1);
        }
        /* allow multiple destination hosts */
        const char *sep = " ";
        char *line;
        unsigned i=0;

        line = strtok(optarg, sep);
        while (line != NULL) {
          socks.nsocks++;
          socks.socks = realloc(socks.socks, (sizeof(int)*(i+1)));
          socks.hosts = realloc(socks.hosts, (sizeof(char*)*(i+1)));
          socks.hosts[i] = strdup(line);
          i++;
          line = strtok(NULL,sep);
        }
        break;

      case 'D':
        debug = 1;
        break;

      case 'q':
        quiet = 1;
        break;

      case '1':
        one_transfer = 1;
        break;
    
      case 'h':
        usage();
        return(0);
        break;
    }
  }

  if (strlen(hostname) == 0) {
    strcpy(hostname, "localhost");
  }

  /* default 1 sock if no args supplied */
  if (socks.nsocks == 0) {
    socks.nsocks = 1;
    socks.socks = malloc(sizeof(int));
    socks.hosts = malloc(sizeof(char *));
    socks.socks[0] = 0;
    strcpy(socks.hosts[0], "localhost");
  }

  if (!quiet) {
    printf("Listening on port %d\n", portserver);
    printf("Will connect to port %d\n", portclient);
  }

  nupdate = timeupdate/(bufsize/1e6);
    
  // Initialise buffers and syncronisation variables
  for (i=0; i<NBUF; i++) {
    buf[i] = malloc(bufsize);
    if (buf[i]==NULL) {
      sprintf(msg, "Trying to allocate %d MB", bufsize/1000/1000);
      perror(msg);
      return(1);
    }

    nbufsize[i] = 0;
    pthread_mutex_init(&bufmutex[i], NULL);
  }
  nfullbuf=0;
  
  /* Check we can lookup remote host name(s) before we start */
  for (i=0; i<socks.nsocks; i++) {
    hostptr = gethostbyname(socks.hosts[i]);
    if (hostptr==NULL) {
      fprintf(stderr,"Failed to look up hostname %s\n", socks.hosts[i]);
      exit(1);
    }
  }

  /* setup a socket for incoming data */
  status =  setup_net(portserver, window_size1, &listensock, quiet);
  if (status) return(1);

  while (!time_to_quit) {

    status =  waitconnection(window_size2, portclient, listensock, 
                             &serversock, &socks, quiet);

    if (status) break;

    /* Install a ^C catcher */
    signal (SIGINT, kill_signal);
  
    bwrote = 0;
    totalsize=0;
    firsttime = tim();
    t1 = firsttime;
    ibuf = 0;
    jbuf = NBUF-1;
    j = 0;
    
    /* Need to start net thread */

    // Lock the last mutex for the netread buffer
    DEBUG(printf("MAIN:   Locked %d for thread\n", jbuf));
    pthread_mutex_lock( &bufmutex[jbuf] );
    
    status = pthread_create( &netthread, NULL, netread, (void *)&serversock);
    if (status) {
      printf("Failed to start net read thread: %d\n", status);
      exit(1);    
    }
    
    /* Now wait on netread to lock the first buffer and unlock this one */
    DEBUG(printf("MAIN:   Wait on lock for buf %d\n", jbuf));
    pthread_mutex_lock( &bufmutex[jbuf] );
    DEBUG(printf("MAIN:   Got it (%d)\n", jbuf));
    
    while (1) {
 
      tbehind = 0;
      
      /* Loop over buffers */
      
      DEBUG(printf("MAIN:   Try and get lock on %d\n", ibuf));
      pthread_mutex_lock( &bufmutex[ibuf] );
      DEBUG(printf("MAIN:   Got it (%d)\n", ibuf));

      // Unlock the previous lock
      DEBUG(printf("MAIN:   Release lock %d\n", jbuf));
      pthread_mutex_unlock( &bufmutex[jbuf] );
      
      pthread_mutex_lock(&globalmutex);
      DEBUG(printf("MAIN:    Nfullbuf = %d\n", nfullbuf));
      if (nfullbuf==0) {
        pthread_mutex_unlock(&globalmutex);
        break;
      }
      pthread_mutex_unlock(&globalmutex);
      
      DEBUG(printf("MAIN: nbufsize = %d\n", nbufsize[ibuf]));
      
      ntowrite = nbufsize[ibuf];
      poff = buf[ibuf];
      
      while (ntowrite>0) {
        DEBUG(printf("MAIN:   Send from buffer %d\n", ibuf));
        
        for (i=0; i<socks.nsocks; i++) {
          nwrote = send(socks.socks[i], poff, ntowrite, 0);
        }

        if (nwrote==-1) {
          if (errno == EINTR) continue;
          perror("Error writing to network");
          pthread_mutex_lock(&globalmutex);
          write_failure = 1;
          pthread_mutex_unlock( &bufmutex[ibuf] );
          break;
          
        } else if (nwrote==0) {
          fprintf(stderr, "Warning: Did not write any bytes!\n");
          pthread_mutex_lock(&globalmutex);
          write_failure = 1;
          pthread_mutex_unlock(&globalmutex);
          pthread_mutex_unlock( &bufmutex[ibuf] );
          break;
          
        } else {
          ntowrite -= nwrote;
          poff += nwrote;
          totalsize += nwrote;
          bwrote += nwrote;
        }
      }
          
      pthread_mutex_lock(&globalmutex);
      nfullbuf--;
      pthread_mutex_unlock(&globalmutex);
      /* Fixed data rate if requested */
      if (datarate != 0.0) {
        throttle_rate (firsttime, datarate, totalsize, &tbehind);
      }
    
      
      if (!((i+1) % nupdate)) {
        t2 = tim();

        speed = bwrote/(t2-t1)/1e6*8; /* MB/s */

        if (!quiet) {
          printf("vsib_proxy: %8.1f %7.1f %7.2f Mbps ( %d %7.3f )\n", 
             (double)totalsize/1.0e6, t2-firsttime, speed, bwrote, t2-t1); 
        }
        t1 = t2;
        
        bwrote = 0;

      }
      
      DEBUG(printf("MAIN: ibuf++\n"));
      jbuf = ibuf;
      ibuf = (ibuf+1)%NBUF;
      
      pthread_mutex_lock(&globalmutex);
      DEBUG(printf("MAIN:    Nfullbuf = %d\n", nfullbuf));
      pthread_mutex_unlock(&globalmutex);


      if (time_to_quit) break;
      i++;
    } // Loop over disks

    status = close(serversock);
    if (status!=0) {
      perror("Error closing server socket");
    }

    status = close(listensock);
    if (status!=0) {
      perror("Error closing listening socket");
    }

    for (i=0; i<socks.nsocks; i++) {
      status = close(socks.socks[i]);
      if (status!=0) 
        perror("Error closing socket");
    }

    t2 = tim();
    speed = totalsize/(t2-firsttime)/1e6*8;

    if (quiet) 
      printf("Rate = %.2f Mbps/s (%.1f sec)\n", speed, t2-firsttime);
    else 
      printf("\n  Rate = %.2f Mbps/s (%.1f sec)\n\n", speed, t2-firsttime);
    
    DEBUG(printf("MAIN:   Release lock %d\n", ibuf));
    pthread_mutex_unlock(&bufmutex[ibuf]);

    if (one_transfer) 
      time_to_quit = 1;
    else 
      printf("Re-enabling signal\n");

    signal (SIGINT, SIG_DFL);
    if (time_to_quit) {
      raise (sig_received);
      signal (sig_received, SIG_DFL);
    }
  }

  return(0);
}
  
double tim(void) {
  struct timeval tv;
  double t;

  gettimeofday(&tv, NULL);
  t = (double)tv.tv_sec + (double)tv.tv_usec/1000000.0;

  return t;
}

/*
 *  Create a listen socket for incoming data 
 */
int setup_net(int port, int window_size, int *listensock, int quiet) {

  int status;
  struct sockaddr_in server;    /* Socket address */
  /* Open a server connection for reading */

  /* Initialise server's address */
  memset((char *)&server,0,sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY); /* Anyone can connect */
  server.sin_port = htons((unsigned short)port); /* Which port number to use */

  /* Create a server to listen with */
  *listensock = socket(AF_INET,SOCK_STREAM,0); 
  if (*listensock==-1) {
    perror("Error creating socket");
    return(1);
  }

  /* Set the socket to reusable */
  int value = 1;
  status = setsockopt(*listensock, SOL_SOCKET, SO_REUSEADDR,
                      &value, sizeof(value));
  if (status!=0) {
    perror("Error setting socket reuse flag");
    close(*listensock);
    return(1);
  }


  /* Set the TCP window size */
  if (window_size>0) {
    status = setsockopt(*listensock, SOL_SOCKET, SO_SNDBUF,
                        (char *) &window_size, sizeof(window_size));
    if (status!=0) {
      perror("Error setting socket options");
      close(*listensock);
      return(1);
    } 

    status = setsockopt(*listensock, SOL_SOCKET, SO_RCVBUF,
                        (char *) &window_size, sizeof(window_size));
    if (status!=0) {
      perror("Error setting socket options");
      close(*listensock);
      return(1);
    } 
  }

  status = bind(*listensock, (struct sockaddr *)&server, sizeof(server));
  if (status!=0) {
    fprintf(stderr, "error binding to port %d\n", port);
    system("ps aux | grep vsib_proxy");
    perror("Error binding socket");
    close(*listensock);
    return(1);
  } 

  /* We are willing to receive conections, using the maximum
     back log of 1 */
  status = listen(*listensock,1);
  if (status!=0) {
    perror("Error listening on socket");
    close(*listensock);
    return(1);
  }

  return(0);
}

/*
 * create socket(s) for sending data 
 */
int waitconnection (int window_size, int port, int listensock, 
                    int *serversock, writesocks *socks, int quiet) {
  int status;
  unsigned long ip_addr;
  socklen_t client_len;
  struct sockaddr_in server, client;    /* Socket address */
  struct hostent *hostptr;

  if (!quiet) 
    printf("Waiting for connection\n");
  
  /* Accept connection from vsib_send */
  client_len = sizeof(client);
  *serversock = accept(listensock, (struct sockaddr *)&client, &client_len);
  if (*serversock==-1) {
    perror("Error connecting to client");
    close(listensock);
    return(1);
  }
  
  if (!quiet)    
    printf("Got a connection from %s\n",inet_ntoa(client.sin_addr));

  /* Now try and connect to vsib_recv(s) */
  int ssock = 0;
  unsigned i=0;
  for (i=0; i<socks->nsocks; i++) {

    hostptr = gethostbyname(socks->hosts[i]);

    memcpy(&ip_addr, (char *)hostptr->h_addr, sizeof(ip_addr));
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons((unsigned short)port); 
    server.sin_addr.s_addr = ip_addr;
 
    if (!quiet)  
      printf("Connecting to %s\n",inet_ntoa(server.sin_addr));

    socklen_t winlen;
    ssock = 0;
    socks->socks[i] = 0;

    /* Create the initial socket */
    ssock = socket(AF_INET,SOCK_STREAM,0);
    if (ssock==-1) {
      perror("Error creating socket");
      close(ssock);
      return(1);
    }

    /* ensure the socket can be reused */
    int value = 1;
    status = setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR,
                        &value, sizeof(value));
    if (status!=0) {
      perror("Error setting socket reuse flag");
      close(ssock);
      return(1);
    }

    /* set the window size for this socket */
    if (window_size > 0) {

      value = window_size;

      status = setsockopt(ssock, SOL_SOCKET, SO_SNDBUF,
                          &value, sizeof(value));
      if (status!=0) {
        perror("Error setting socket send buffer");
        close(ssock);
        return(1);
      }

      value = window_size;

      status = setsockopt(ssock, SOL_SOCKET, SO_RCVBUF,
                          &value, sizeof(value));

      if (status!=0) {
        perror("Error setting socket receive buffer");
        close(ssock);
        return(1);
      }

      /* Check what the window size actually was set to */
      value = 0;
      winlen = sizeof(value);
      status = getsockopt(ssock, SOL_SOCKET, SO_SNDBUF,
                        &value, (socklen_t * )&winlen);
      if (status!=0) {
        close(ssock);
        perror("Getting socket options");
        return(1);
      }
      if (!quiet)
        printf("Sending buffersize set to %f Kbytes\n", ((float)value) / 1024.0);

    }

    /* assign this socket */
    socks->socks[i] = ssock;

    /* allow 5 seconds to attempt to make the connection */
    int my_timeout = 5;
    int connected = 0;

    while (!connected && (my_timeout >= 0)) { 

      /* open the connection */
      status = connect(socks->socks[i], (struct sockaddr *) &server, sizeof(server));

      if (status != 0) {
        my_timeout--; 
        fprintf(stderr, "Could not connect to destination, %5d timeouts remaining\n", my_timeout);
        sleep(1);
      } else {
        connected = 1;
      }
    }

    if (!connected) {
      perror("Failed to connect to destination, no timeouts remaining");
      close(socks->socks[i]);
      return(1);
    }
  }

  return(0);
}

void throttle_rate (double firsttime, float datarate, 
                    unsigned long long totalsize, double *tbehind) {
  /* Sleep until time catches up with us, assuming we are writing faster than
     requested */
  int status;
  double t2, dt, twait, expected_time;

  t2 = tim();
  dt = t2-firsttime;

  expected_time = totalsize/(float)datarate;
  twait = (expected_time-dt);

  if (twait>0) {
    *tbehind = 0;
    status = usleep(twait*1e6);
    if ((status!=0) & (status!= EINTR)) {
      perror("Calling usleep\n");
      exit(1);
    }
  } else {
    twait *= -1;
    //if ((-twait>1) & (abs(twait-*tbehind)>0.1)) { 
    if ((abs(twait-*tbehind)>1)) { 
      /* More than a second difference */ 
      *tbehind = twait;
      fprintf(stderr, " Dropping behind %.1f seconds\n", twait);
    }
  }
  return;
}
  
void kill_signal (int sig) {
  /* We may get called twice */
  if (time_to_quit) {
    fprintf(stderr, "kill_signal called second time\n");
    return;
  }
  time_to_quit = 1;
  sig_received = sig;

  signal (sig, kill_signal); /* Re-install ourselves to disable double signals */
}  

void *netread (void *arg) {
  int sock;
  int ibuf = 0;
  int jbuf = NBUF-1;
  ssize_t nr;
  char *poff;
  size_t ntoread;

  sock = *(int*)arg;
  
  while (1) {

    DEBUG(printf("THREAD: Wait on %d\n", ibuf));
    pthread_mutex_lock( &bufmutex[ibuf] );
    DEBUG(printf("THREAD: Got it (%d)\n", ibuf));
    DEBUG(printf("THREAD: Release buf %d\n", jbuf));
    pthread_mutex_unlock( &bufmutex[jbuf] );

    if (time_to_quit || write_failure) {
      DEBUG(printf("THREAD: Time to quit\n"));
      pthread_exit(NULL);
    }

    nbufsize[ibuf] = 0;
    poff = buf[ibuf];
    ntoread = bufsize;

    while (nbufsize[ibuf]<ntoread) {
    
      nr = recv(sock,poff,ntoread-nbufsize[ibuf],0);
              
      if (nr==-1) {
        if (errno == EINTR) continue;
        perror("Error reading socket");
        DEBUG(printf("THREAD: Release buf %d\n", ibuf));
        pthread_mutex_unlock( &bufmutex[ibuf] );
        pthread_exit(NULL);

      } else if (nr==0) {
        DEBUG(printf("THREAD: Socket connection closed\n"));
        /* Assume for now this means the socket was closed */
        pthread_mutex_lock(&globalmutex);
        if (nbufsize[ibuf]>0) nfullbuf++;
        pthread_mutex_unlock(&globalmutex);
        DEBUG(printf("THREAD: Release buf %d\n", ibuf));
        pthread_mutex_unlock(&bufmutex[ibuf]);
        pthread_exit(NULL);

      } else {
        pthread_mutex_lock(&globalmutex);
        poff += nr;
        nbufsize[ibuf] += nr;
        pthread_mutex_unlock(&globalmutex);
      }
    }

    pthread_mutex_lock(&globalmutex);
    nfullbuf++;
    DEBUG(printf("THREAD: nfullbuf =  %d\n", nfullbuf));
    pthread_mutex_unlock(&globalmutex);

    jbuf = ibuf;
    ibuf = (ibuf+1)%NBUF;
  }
}

void usage() {

  printf("Usage: vsib_proxy [options]\n");
  printf("  -p <PORT>       Port number for transfer (server and clients)\n");
  printf("  -H <HOST>       Hosts to connect to\n");
  printf("  -t <TIME>       Update time (in MBytes) for network statistics\n");
  printf("  -w <SIZE>       Network window size (kB)\n");
  printf("  -t <TIME>       Number of blocks (-b) to average timing statistics\n");
  printf("  -b <SIZE>       Read/write block size\n");
  printf("  -z <RATE>       Fixed data rate (Mbps) to use (default as fast as possible)\n");
  printf("  -D              Debug messages\n");
  printf("  -q              quiet mode\n");
  printf("  -1              Receive 1 transfer, then exit\n");
  printf("  -h              This list\n");

}

