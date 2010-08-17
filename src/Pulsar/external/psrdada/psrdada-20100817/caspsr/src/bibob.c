/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "bibob.h"
#include "dada_def.h"
#include "sock.h"

#define TEMPLATE_TYPE long
#include "median_smooth_zap.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define _DEBUG 1

bibob_t* bibob_construct ()
{
  bibob_t* bibob = malloc (sizeof(bibob_t));

  bibob->hdr = malloc (sizeof(bibob_header_t));

  bibob->sock = caspsr_init_sock(CASPSR_BIBOB_UDP_PACKET);

  bibob->fd = -1;
  bibob->buffer_size = 0;
  bibob->buffer = 0;

  bibob->host = 0;
  bibob->pol1_bit = 0;
  bibob->pol2_bit = 0;

  /* 2^12, unity scale factor */
  bibob->pol1_coeff = 4096;
  bibob->pol2_coeff = 4096;

  bibob->session_id = 0;
  bibob->bram_max = 8.8;   // log2(2^8) * 1.1
  bibob->n_brams = 0;

  // set the number of receiving hsots to BIBOB_NBANDS (16)
  bibob->num_dests = BIBOB_NBANDS;
  bibob->dest_ips = (char **) malloc(sizeof (char*)*bibob->num_dests);
  bibob->dest_ports = (unsigned *) malloc(sizeof (unsigned *)*bibob->num_dests);
  unsigned i=0;
  for (i=0; i<bibob->num_dests; i++) {
    bibob->dest_ips[i] = NULL;
    bibob->dest_ports[i] = 0;
  }

  /* allocate memory for each of the bramdump arrays */
  bibob->bram_arrays  = malloc(sizeof(bibob_bramdump_t *) * BIBOB_BRAM_ARRAYS); 
  for (i=0; i<BIBOB_BRAM_ARRAYS; i++) {
    bibob->bram_arrays[i] = init_bram_array();
  }

  return bibob;
}

bibob_bramdump_t * init_bram_array () {

  bibob_bramdump_t * bram = malloc(sizeof(bibob_bramdump_t));
  bram->vals = malloc(sizeof(long int) * BIBOB_NBANDS);
  bram->avgs = malloc(sizeof(float) * BIBOB_NBANDS);
  bram->count = 0;

  return bram;
}

int free_bram_array(bibob_bramdump_t * bram) 
{

  if (!bram)
    return -1;

  if (bram->vals)
    free (bram->vals);

  if (bram->avgs)
    free (bram->avgs);
  
  free (bram);

  return 0;

}

/*! free all resources reserved for bibob communications */
int bibob_destroy (bibob_t* bibob)
{
  if (!bibob)
    return -1;

  if (bibob->fd != -1)
    sock_close (bibob->fd);

  if (bibob->buffer)
    free (bibob->buffer);

  if (bibob->host)
    free (bibob->host);

  unsigned i=0;
  for (i=0; i<bibob->num_dests; i++)
    free (bibob->dest_ips[i]);

  free (bibob->dest_ips);
  free (bibob->dest_ports);

  caspsr_free_sock(bibob->sock);

  for (i=0; i<BIBOB_BRAM_ARRAYS; i++) 
    free_bram_array(bibob->bram_arrays[i]);

  free (bibob);

  return 0;
}

/*! set the host and port number of the bibob */
int bibob_set_host (bibob_t* bibob, const char* host, int port)
{
  if (!bibob)
    return -1;

  if (bibob->host)
    free (bibob->host);

  bibob->host = strdup (host);
  bibob->port = port;

  return 0;
}

/*! set the bibob number (use default IP base and port) */
int bibob_set_number (bibob_t* bibob, unsigned number)
{
  if (!bibob)
    return -1;

  if (bibob->host)
    free (bibob->host);

  bibob->host = strdup (BIBOB_VLAN_BASE"XXXXXX");
  sprintf (bibob->host, BIBOB_VLAN_BASE"%u", number);

  bibob->port = BIBOB_PORT;

  return 0;
}

/*! set the host and port number of a receiving host */
int bibob_set_dest (bibob_t* bibob, unsigned dest, const char* host, int port)
{
  if (!bibob)
    return -1;

  if (dest >= bibob->num_dests) 
    return -1;

  if (bibob->dest_ips[dest])
    free (bibob->dest_ips[dest]);

  bibob->dest_ips[dest] = strdup (host);
  bibob->dest_ports[dest]= port;

  return 0;
}


/*! start reading from/writing to an bibob */
int bibob_open (bibob_t* bibob)
{
  if (!bibob)
    return -1;

  if (bibob->fd != -1)
  {
    fprintf (stderr, "bibob_open: already open\n");
    return -1;
  }

  if (bibob->host == 0)
  {
    fprintf (stderr, "bibob_open: host name not set\n");
    return -1;
  }

  if (bibob->port == 0)
  {
    fprintf (stderr, "bibob_open: port not set\n");
    return -1;
  }

  fprintf (stderr, "bibob_open: opening %s:%d\n",bibob->host, bibob->port);
  int r = dada_udp_sock_out( &(bibob->fd), &(bibob->dagram), bibob->host, bibob->port, 
                             0, "");
  if (r < 0) {
    fprintf (stderr, "bibob_open: dada_udp_sock_out failed\n");
  } else {
    fprintf (stderr, "bibob_open: socket is open, fd=%d\n",bibob->fd);
  }

  if ( bibob->fd < 0 )
  {
    fprintf (stderr, "bibob_open: could not open %s %d - %s\n",
             bibob->host, bibob->port, strerror(errno));
    return -1;
  }

  // Generate a session id
  srand ( time(NULL) );
  bibob->session_id = (long int) (167777215 * (rand() / (RAND_MAX + 1.0)));

  if (!bibob->buffer_size)
    bibob->buffer_size = 1000;

  bibob->buffer = realloc (bibob->buffer, bibob->buffer_size);
  bibob_bramdump_reset(bibob);

  return 0;
}

/*! stop reading from/writing to an bibob */
int bibob_close (bibob_t* bibob)
{
  if (!bibob)
    return -1;

  if (bibob->fd == -1)
    return -1;

  int fd = bibob->fd;
  bibob->fd = -1;

  return close (fd);
}

/*! return true if already open */
int bibob_is_open (bibob_t* bibob)
{
  return bibob && (bibob->fd != -1);
}

/*! reset packet counter on next UTC second, returned */
time_t bibob_arm (bibob_t* bibob)
{
  return 0;
}

/*! configure the bibob */
int bibob_configure (bibob_t* bibob)
{
  if (!bibob)
    return -1;

  if (bibob->fd == -1)
    return -1;

  bibob_send (bibob, "regwrite pasp/reg_sync_period 102400");
  bibob_ignore (bibob);

  bibob_send (bibob, "regwrite pasp/reg_coeff_pol1 4096");
  bibob_ignore (bibob);
   
  bibob_send (bibob, "regwrite pasp/reg_coeff_pol2 4096");
  bibob_ignore (bibob);

  bibob_send (bibob, "regwrite pasp/reg_fft_shift 0xffffffff");
  bibob_ignore (bibob);

  bibob_send (bibob, "regwrite pasp/reg_output_bitselect_pol1 1");
  bibob_ignore (bibob);

  bibob_send (bibob, "regwrite pasp/reg_output_bitselect_pol1 2");
  bibob_ignore (bibob);

  bibob_send (bibob, "write l xd0000000 xffffffff");
  bibob_ignore (bibob);

  bibob_send (bibob, "setb x40000000");
  bibob_ignore (bibob);

  /* Sets the MAC address of the iBoB's 1st 10GbE Interface. It must be 
     valid, but it can be arbitary */
  bibob_send (bibob, "writeb l 0 x00000060");
  bibob_ignore (bibob);
  bibob_send (bibob, "writeb l 4 xdd47e301");
  bibob_ignore (bibob);

  /* Gateway IP Adress for 10GbE UDP packets, arbitrary */
  bibob_send (bibob, "writeb l 8 x00000000");
  bibob_ignore (bibob);

  /* Source IP Adress for 10GbE UDP packets 10.0.0.1 [first iface] */
  bibob_send (bibob, "writeb l 12 x0a000001");
  bibob_ignore (bibob);

  /* Source UDP port for 10 GbE packets */
  bibob_send (bibob, "writeb b x16 x0f");
  bibob_ignore (bibob);
  bibob_send (bibob, "writeb b x17 xa0");
  bibob_ignore (bibob);

  /* Not sure what these lines do, copied from Terrys init_pasp.txt */
  bibob_send (bibob, "writeb l x3120 x00000060");
  bibob_ignore (bibob);
  bibob_send (bibob, "writeb l x3124 xdd4716ee");
  bibob_ignore (bibob);

  /* Not sure what these lines do either */
  bibob_send (bibob, "writeb b x15 xff");
  bibob_ignore (bibob);
  bibob_send (bibob, "write l xd0000000 x0");
  bibob_ignore (bibob);


  /* Sets the MAC address of the iBoB's 2nd 10GbE Interface. It must be 
     valid, but it can be arbitary */
  bibob_send (bibob, "setb x40004000");
  bibob_ignore (bibob);

  bibob_send (bibob, "writeb l 0 x00000060");
  bibob_ignore (bibob);
  bibob_send (bibob, "writeb l 4 xdd47e30a");
  bibob_ignore (bibob);

  /* Gateway IP Adress for 10GbE UDP packets, arbitrary */
  bibob_send (bibob, "writeb l 8 x00000000");
  bibob_ignore (bibob);

  /* Source IP Adress for 10GbE UDP packets 10.0.0.2 [first iface] */
  bibob_send (bibob, "writeb l 12 x0a000002");
  bibob_ignore (bibob);
  
  /* Source UDP port for 10 GbE packets */
  bibob_send (bibob, "writeb b x16 x0f");
  bibob_ignore (bibob);
  bibob_send (bibob, "writeb b x17 xa0");
  bibob_ignore (bibob);
    
  /* Not sure what these lines do, copied from Terrys init_pasp.txt */
  bibob_send (bibob, "writeb l x3120 x00000060");
  bibob_ignore (bibob);
  bibob_send (bibob, "writeb l x3124 xdd4716ee");
  bibob_ignore (bibob); 
  
  /* Not sure what these lines do either */
  bibob_send (bibob, "writeb b x15 xff");
  bibob_ignore (bibob); 
  bibob_send (bibob, "write l xd0000000 x0");
  bibob_ignore (bibob);

  /* Set the IPs and ports of the receiving hosts */
  unsigned i=0;
  char ip_str[16];
  struct in_addr *inet;

  const char *sep = ".";
  char *saveptr;
  char *value;
  int ips[4];

  char msg[1000];

  for (i=0; i<bibob->num_dests; i++) {

    // convert the hostname/ip into an IP address string
    inet = atoaddr(bibob->dest_ips[i]);
    inet_ntop(AF_INET, (const void *)inet, ip_str, 16);

    // extract the numbers from the IP address string
    value = strtok_r(ip_str, sep, &saveptr);
    sscanf(value, "%d",&(ips[0]));
    value = strtok_r(NULL, sep, &saveptr);
    sscanf(value, "%d",&(ips[1]));
    value = strtok_r(NULL, sep, &saveptr);
    sscanf(value, "%d",&(ips[2]));
    value = strtok_r(NULL, sep, &saveptr);
    sscanf(value, "%d",&(ips[3]));

    // form the IP Address config command
    sprintf(msg, "regwrite pasp/dist_gbe/ip_ctr/reg_ip%d 0x%02X%02X%02X%02X",
                  (i+1), ips[0], ips[1], ips[2], ips[3]);
    fprintf(stderr, "bibob_configure: %s\n", msg);
    bibob_send (bibob, msg);
    bibob_ignore (bibob);

    // form the Port config command
    sprintf(msg, "regwrite pasp/dist_gbe/ip_ctr/reg_port%d %d", (i+1),
                 bibob->dest_ports[i]);
    fprintf(stderr, "bibob_configure: %s\n", msg);
    bibob_send (bibob, msg);
    bibob_ignore (bibob);

    /*
    regwrite pasp/dist_gbe/ip_ctr/reg_ip1 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip2 0x0A000004
    regwrite pasp/dist_gbe/ip_ctr/reg_ip3 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip4 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip5 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip6 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip7 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip8 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip9 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip10 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip11 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip12 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip13 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip14 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip15 0x0
    regwrite pasp/dist_gbe/ip_ctr/reg_ip16 0x0

    regwrite pasp/dist_gbe/ip_ctr/reg_port1 10001
    regwrite pasp/dist_gbe/ip_ctr/reg_port2 10002
    regwrite pasp/dist_gbe/ip_ctr/reg_port3 10003
    regwrite pasp/dist_gbe/ip_ctr/reg_port4 10004
    regwrite pasp/dist_gbe/ip_ctr/reg_port5 10005
    regwrite pasp/dist_gbe/ip_ctr/reg_port6 10006
    regwrite pasp/dist_gbe/ip_ctr/reg_port7 10007
    regwrite pasp/dist_gbe/ip_ctr/reg_port8 10008
    regwrite pasp/dist_gbe/ip_ctr/reg_port9 10009
    regwrite pasp/dist_gbe/ip_ctr/reg_port10 10010
    regwrite pasp/dist_gbe/ip_ctr/reg_port12 10011
    regwrite pasp/dist_gbe/ip_ctr/reg_port12 10012
    regwrite pasp/dist_gbe/ip_ctr/reg_port13 10013
    regwrite pasp/dist_gbe/ip_ctr/reg_port14 10014
    regwrite pasp/dist_gbe/ip_ctr/reg_port15 10015
    regwrite pasp/dist_gbe/ip_ctr/reg_port16 10016
    */
  }

  return 0;
}


/*! write bytes to bibob */
ssize_t bibob_send (bibob_t* bibob, const char* message)
{
  if (!bibob)
    return -1;

  if (bibob->fd == -1)
    return -1;

  int length = strlen (message) + CASPSR_BIBOB_UDP_HEADER;

  /* add the custom 8 byte UDP header to the message */
  if (length+2 > bibob->buffer_size)
  {
    bibob->buffer_size = length * 2;
    bibob->buffer = realloc (bibob->buffer, bibob->buffer_size);
  }

  bibob->hdr->id = bibob->session_id;

  bibob_udp_header_encode(bibob->buffer, bibob->hdr); 

  snprintf ( bibob->buffer+CASPSR_BIBOB_UDP_HEADER, 
             bibob->buffer_size-CASPSR_BIBOB_UDP_HEADER, "%s\r", message);
  length++;

  // always sleep for 10 millseconds before a command to ensure we aren't
  // flooding the connection
  bibob_pause(10);

  return dada_sock_send(bibob->fd, bibob->dagram, bibob->buffer, length);

}

ssize_t bibob_ignore (bibob_t* bibob)
{
  return bibob_recv (bibob);
}

/*! read a response from the ibob, writing the data into ptr [of size bytes] */
ssize_t bibob_recv (bibob_t* bibob)
{
  if (!bibob)
    return -1;

  if (bibob->fd == -1)
    return -1;

  int done = 0;
  int seq = 0;

  struct timeval timeout;
  fd_set *rdsp = NULL;
  fd_set readset;

  size_t total_got = 0;

  while (!done)
  {
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    FD_ZERO (&readset);
    FD_SET (bibob->fd, &readset);
    rdsp = &readset;

    if ( select((bibob->fd+1),rdsp,NULL,NULL,&timeout) == 0 ) {
      fprintf (stderr, "UDP packet timeout: no packet received for 1 second\n");
      done = 1;
      bibob->sock->got = 0;

    } else {

      bibob->sock->got = dada_sock_recv (bibob->fd, bibob->sock->buffer,
                                         bibob->sock->size, 0); 

      // decode the custom udp header
      bibob_udp_header_decode(bibob->sock->buffer, bibob->hdr);

      if (bibob->hdr->type == BIBOB_LAST_PACKET)
        done = 1;

      // check that we have not dropped a packet
      if (bibob->hdr->seq != seq) {
        fprintf (stderr, "UDP packet sequence mismatch. received=%lu, "
                         "expected=%lu\n", bibob->hdr->seq, seq+1);
      }

      // if this response is bigger than the current buffer size, grow it
      if (total_got + bibob->sock->got > bibob->buffer_size) {
        bibob->buffer_size = total_got + bibob->sock->got;
        bibob->buffer = realloc (bibob->buffer, bibob->buffer_size);
      }

      // copy the response from the socket buffer
      memcpy (bibob->buffer + total_got, 
              bibob->sock->buffer + CASPSR_BIBOB_UDP_HEADER,
              bibob->sock->got - CASPSR_BIBOB_UDP_HEADER);

      total_got += (bibob->sock->got - CASPSR_BIBOB_UDP_HEADER);
      seq++;

    }

    if (bibob->sock->got < 0)
    {
      perror ("bibob_recv: ");
      return -1;
    }
  }

  bibob->buffer[total_got] = '\0';

  return total_got;
}

/* sleep for msec milliseconds */
void bibob_pause(int msec)
{
  struct timeval pause;
  pause.tv_sec=0;
  pause.tv_usec=(msec * 1000);
  select(0,NULL,NULL,NULL,&pause);
}

/* gets the bram for each polarisation and stores the 
 * results in the struct */
int bibob_bramdump(bibob_t* bibob) 
{

  fprintf(stderr, "bibob_bramdump()\n");

  char msg[100];
  unsigned bramdump_size = BIBOB_BRAM_CHANNELS * BIBOB_CHARS_PER_BRAM;

  if (bibob->buffer_size < bramdump_size+1)
  {
    bibob->buffer_size = bramdump_size+1;

#ifdef _DEBUG
    fprintf (stderr, "bibob_bramdump: %s realloc bibob->buffer to %d bytes\n",
                      bibob->host, bibob->buffer_size);
#endif

    bibob->buffer = realloc (bibob->buffer, bibob->buffer_size);
    if (!bibob->buffer)
    {
      fprintf (stderr, "bibob_bramdump: realloc failed\n");
      return -1;
    }
  }

  unsigned i = 0;
  unsigned b = 0;
  for (i=0; i<BIBOB_BRAM_ARRAYS; i++) {

    if (i<2)
      b = i;
    else 
      b= i+2;
      
    /* get the odd values */
    sprintf(msg, "bramdump pasp/scope_output%d/bram 0 %d", (b+1), BIBOB_BRAM_CHANNELS);
    fprintf(stdout, "%s\n",msg);
    bibob_send (bibob, msg);
    bibob_recv (bibob);
    bibob->buffer[bramdump_size] = '\0';
    extract_counts(bibob->buffer, bibob->bram_arrays[i], 0);

    /* get the even values */
    sprintf(msg, "bramdump pasp/scope_output%d/bram 0 %d", (b+3), BIBOB_BRAM_CHANNELS);
    fprintf(stdout, "%s\n",msg);
    bibob_send (bibob, msg);
    bibob_recv (bibob);
    bibob->buffer[bramdump_size] = '\0';
    extract_counts(bibob->buffer, bibob->bram_arrays[i], 1);

    bibob->bram_arrays[i]->count += (BIBOB_BRAM_CHANNELS / BIBOB_NBANDS);
    calc_averages(bibob->bram_arrays[i]);
    print_vals(bibob->bram_arrays[i]);
    
  }

  /* increment internal counter for number of dumps in bibob->pol?_bram 
   * Each dump contains BIBOB_BRAM_CHANNELS / BIBOB_NBANDS samples (128) */
  bibob->n_brams += (BIBOB_BRAM_CHANNELS / BIBOB_NBANDS);
  fprintf(stdout, "bibob_bramdump complete\n");

  return 0;

}

/* reset accumulated bram values */
void bibob_bramdump_reset(bibob_t * bibob)
{

  unsigned i = 0;
  unsigned j = 0;

  for (i=0; i<BIBOB_BRAM_ARRAYS; i++) {
    for (j=0; j<BIBOB_NBANDS; j++) {
      bibob->bram_arrays[i]->vals[j] = 0;
      bibob->bram_arrays[i]->avgs[j] = 0;
    }
    bibob->bram_arrays[i]->count = 0;
  }

  bibob->n_brams = 0;

}

/* writes the accumulated bramdumps to disk */
int bibob_bramdisk(bibob_t * bibob)
{

  int fd = 0;
  int flags = O_CREAT | O_WRONLY | O_TRUNC;
  int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

  char fname[128];
  char time_str[64];

  time_t now = time(0);
  strftime (time_str, 64, DADA_TIMESTR, localtime(&now));

  sprintf(fname, "%s_%s.bramdump.tmp", time_str, bibob->host);

  fd = open(fname, flags, perms);
  if (fd < 0)
  {
    fprintf (stderr, "Error opening bramdump file: %s\n", fname);
    return -1;
  }

  write(fd, &bibob->pol1_bit, sizeof(unsigned));
  write(fd, &bibob->pol2_bit, sizeof(unsigned));
  write(fd, &bibob->n_brams, sizeof(unsigned));
  unsigned i = 0;
  for (i=0; i<BIBOB_BRAM_ARRAYS; i++) {
    write(fd, bibob->bram_arrays[i]->avgs, sizeof(float) * BIBOB_NBANDS);
  }

  close(fd);

  /* reset the averages since we have modified the arrays */
  bibob_bramdump_reset(bibob);

  /* rename the tmp bramdump file to the correct name */
  char real_fname[128];

  sprintf(real_fname, "%s_%s.bramdump", time_str, bibob->host);

  /* Sometimes it takes a while for the file to appear, wait for it */
  flags =  R_OK | W_OK;
  int max_wait = 10;

  while (access(fname, flags) != 0)
  {
    fprintf(stderr, "bibob_bramdisk: %s waiting 500ms for bramdump files\n", 
                     bibob->host);
    bibob_pause(500);
    max_wait--;

    if (max_wait == 0)
    {
      fprintf(stderr, "bibob_bramdisk: file not on disk after 5 sec\n");
      break;
    }
  }

  if (max_wait) 
  {
  
    char command[256];
    int rval = rename(fname, real_fname);

    if (rval != 0)
    {
      fprintf(stderr, "%s: rename failed: %d\n", bibob->host, rval);
      unlink (fname);
      return -1;
    }

    return 0;

  }
  else
    return -1;

}

/* 
 * pgplot print the 8 bramdump information
 */
int bibob_bramplot(bibob_t * bibob, char * device) {
/*
  if (cpgopen(device) != 1) {
    fprintf(stderr, "bibob_bramplot: error opening plot device\n");
    return -1;
  }

  // determine the floating point min/max values 
  float ymin = -1;
  float ymax = 1;
  float xmin = 1;
  float xmax = BIBOB_NBANDS;

  unsigned i = 0;
  unsigned j = 0;
  float val = 0;
  float divisor = (float) bibob->n_brams;
  float * xvals = malloc(sizeof(float) * BIBOB_NBANDS);
  
  for (i=0; i<BIBOB_BRAM_ARRAYS; i++) {
    for (j=0; j<BIBOB_NBANDS; j++) {
      val = bibob->bram_arrays[i]->avgs[j];
      if (val > ymax) 
        ymax = val;
      if (val < ymin) 
        ymin = val;
      xvals[j] = j+1;
    } 
  }

  ymax *= 1.1;
  ymin *= 1.1;

  cpgenv(xmin, xmax, ymin, ymax, 0, 0);
  //cpgsvp(0.1,0.9,0.1,0.9);
  //cpgswin(0, BIBOB_NBANDS, ymin, ymax);
  cpglab("Bands [1-16]", "Voltages", "BiBoB Server");

  for (i=0; i < BIBOB_BRAM_ARRAYS; i++) {
    cpgsci(2+i);
    cpgline(BIBOB_NBANDS, xvals, bibob->bram_arrays[i]->avgs); 
  }

  cpgclos();
  */
}



void * bibob_level_setter(void * context) 
{

  bibob_t * bibob = context;

#ifdef _DEBUG
  fprintf (stderr, "bibob_level_setter: %s\n", bibob->host);
#endif

  char msg[100];

  /* number of iterations at setting the levels */
  int n_attempts = 2;

  int i=0;
  int rval = 0;

  long pol1_max_value;
  long pol2_max_value;

  sprintf(msg, "regwrite reg_coeff_pol1 %d",bibob->pol1_coeff);
  bibob_send (bibob, msg);
  bibob_ignore (bibob);

  sprintf(msg, "regwrite reg_coeff_pol2 %d",bibob->pol2_coeff);
  bibob_send (bibob, msg);
  bibob_ignore (bibob);

  /* 2 iterations to get close to the required value */
  for (i=0; i<n_attempts; i++ ) {

    bibob_pause(50);

    bibob_bramdump_reset(bibob);
 
    /* Extracts a bram dump for each polarisation into bibob->pol?_bram */
    bibob_bramdump(bibob);

    //median_smooth_zap(BIBOB_BRAM_CHANNELS, bibob->pol1_bram, 11);
    //median_smooth_zap(BIBOB_BRAM_CHANNELS, bibob->pol2_bram, 11);

    //pol1_max_value = calculate_max(bibob->pol1_bram, BIBOB_BRAM_CHANNELS);
    //pol2_max_value = calculate_max(bibob->pol2_bram, BIBOB_BRAM_CHANNELS);

    bibob->pol1_bit = find_bit_window(pol1_max_value, pol2_max_value, 
                                 &(bibob->pol1_coeff), &(bibob->pol2_coeff));

#ifdef _DEBUG
    fprintf(stderr, "%s: bit: %d, scales [%d,%d] maxvals = [%d,%d]\n",
                    bibob->host, bibob->pol1_bit,bibob->pol1_coeff,
                    bibob->pol2_coeff, pol1_max_value, pol2_max_value);
#endif

    /* set scaling coefficients for each polarisation */
    sprintf(msg, "regwrite reg_coeff_pol1 %d",bibob->pol1_coeff);
    bibob_send (bibob, msg);
    bibob_ignore (bibob);

    sprintf(msg, "regwrite reg_coeff_pol2 %d",bibob->pol2_coeff);
    bibob_send (bibob, msg);
    bibob_ignore (bibob);

  }

  unsigned ndumps = 32;

  for (i=0; i<2; i++) {

    bibob_pause(50);

    bibob_bramdump_reset(bibob);

    // intergrate 32 bram dumps to remove as much noise as possible 
    unsigned j=0;
    for (j=0; j<ndumps; j++) 
    {
      bibob_bramdump(bibob);
    }

    assert(bibob->n_brams == ndumps);

    // divide by ndumps
    for (j=0; j<BIBOB_BRAM_CHANNELS; j++)
    {
      //bibob->pol1_bram[j] = bibob->pol1_bram[j] / ndumps;
      //bibob->pol2_bram[j] = bibob->pol2_bram[j] / ndumps;
    }

    //median_smooth_zap(BIBOB_BRAM_CHANNELS, bibob->pol1_bram, 11);
    //median_smooth_zap(BIBOB_BRAM_CHANNELS, bibob->pol2_bram, 11);

    //pol1_max_value = calculate_max(bibob->pol1_bram, BIBOB_BRAM_CHANNELS);
    //pol2_max_value = calculate_max(bibob->pol2_bram, BIBOB_BRAM_CHANNELS);
    
    bibob->pol1_bit = find_bit_window(pol1_max_value, pol2_max_value,
                                 &(bibob->pol1_coeff), &(bibob->pol2_coeff));
                                                                                                         
#ifdef _DEBUG
    fprintf(stderr, "%s: bit: %d, scales [%d,%d] maxvals = [%d,%d]\n",
                    bibob->host, bibob->pol1_bit,bibob->pol1_coeff,
                    bibob->pol2_coeff, pol1_max_value, pol2_max_value);
#endif
    sprintf(msg, "regwrite reg_coeff_pol1 %d",bibob->pol1_coeff);
    bibob_send (bibob, msg);
    bibob_ignore (bibob);

    sprintf(msg, "regwrite reg_coeff_pol2 %d",bibob->pol2_coeff);
    bibob_send (bibob, msg);
    bibob_ignore (bibob);
  }
  
  sprintf(msg,"regwrite reg_output_bitselect %d", bibob->pol1_bit);
  bibob_send (bibob, msg);
  bibob_ignore (bibob);

  bibob_bramdump_reset(bibob);

  return ;
}


/*
 * Extracts values from a bibob bramdump in bram into the vals array.
 * Format is BIBOB_BRAM_CHANNELS (2048) lines consisting of 0x????????. 
 * BIBOB_NBANDS (16) lines correspond to the values on each band, with 
 * 128 time "slices".
 */
void extract_counts(char * bram, bibob_bramdump_t * bram_array, unsigned even) {

  const char *sep = "\r\n";
  char *saveptr;
  char *line;
  char value[9];
  long int val;
  int i=0;

  line = strtok_r(bram, sep, &saveptr);
  if (line == NULL)
  {
    fprintf(stderr, "extract_counts: unexpected end of input on line 0\n");
    return;
  }
  strncpy(value, line+2, 8);
  value[8] = '\0';
  sscanf(value, "%x", &val);
  bram_array->vals[0+even] += val;
  //vals[0] += val;

  for (i=1; i<BIBOB_BRAM_CHANNELS; i++) {
    line = strtok_r(NULL, sep, &saveptr);
    if (line == NULL)
    {
      fprintf(stderr, "extract_counts: unexpected end of input on line %d\n",i);
      return;
    }
    strncpy(value, line+2, 8); 
    value[8] = '\0';
    sscanf(value, "%x",&val);
    bram_array->vals[(((2*i)+even)%BIBOB_NBANDS)] += val;
    //vals[(i%BIBOB_NBANDS)] += val;
  }

}

void calc_averages(bibob_bramdump_t * bram_array) {

  unsigned i = 0;
  for (i=0; i<BIBOB_NBANDS; i++) {
    bram_array->avgs[i] = ((float) (int) bram_array->vals[i]) / ((float) bram_array->count);
  }

}

void print_vals(bibob_bramdump_t * bram_array) {

  unsigned i=0;
  for (i=0; i<BIBOB_NBANDS; i++) {
    fprintf(stdout, "[%05d, %5.2f] ", bram_array->vals[i], bram_array->avgs[i]);
  }
  fprintf(stdout, " %d\n", bram_array->count);

}

int find_bit_window(long pol1val, long pol2val, unsigned *gain1, unsigned * gain2) {

  long bitsel_min[4] = {0, 256, 65535, 16777216 };
  long bitsel_mid[4] = {64, 8192, 2097152, 536870912};
  long bitsel_max[4] = {255, 65535, 16777215, 4294967295};

  long val = ((long) (((double) pol1val + (double) pol2val) / 2.0));

  // Find which bit selection window we are currently sitting in
  int i=0;
  int current_window = 0;
  int desired_window = 0;

  for (i=0; i<4; i++) {

    // If average (max) value is in the lower half
    if ((val > bitsel_min[i]) && (val <= bitsel_mid[i])) {
      current_window = i;

      if (i == 0) {
        desired_window = 0;

      } else {
        desired_window = i-1;
      }
    }

    // If average (max)n value is in the upper half, simply raise to
    // the top of this window
    if ((val > bitsel_mid[i]) && (val <= bitsel_max[i])) {
      current_window = i;
      desired_window = i;
    }
  }

  if (desired_window == 3) {
    desired_window = 2;
  }

  if ((pol1val == 0) || (pol2val == 0)) {
    pol1val = 1;
    pol2val = 1;
  }

  // If current gains are too high (2^16) then shift down a window
  
  if ((*gain1 > 32000) || (*gain2 > 32000))
  {
    if (desired_window > 0) {
      desired_window--;
    }
  }

  // If current gains are too low (2^3) then shift down a window
  if ((*gain1 < 8) || (*gain2 < 8))
  {
    if (desired_window < 3) {
      desired_window++;
    }
  }

  long desired_val =  ((bitsel_max[desired_window]+1) / 4);

  double gain_factor1 = (double) desired_val / (double) pol1val;
  double gain_factor2 = (double) desired_val / (double) pol2val;

  gain_factor1 = sqrt(gain_factor1);
  gain_factor2 = sqrt(gain_factor2);

  gain_factor1 *= *gain1;
  gain_factor2 *= *gain2;

  *gain1 = (long) floor(gain_factor1);
  *gain2 = (long) floor(gain_factor2);

  if (*gain1 > 262143) {
    *gain1 = 262143;
  }
  if (*gain2 > 262143) {
    *gain2 = 262143;
  }
  return desired_window;
} 

long calculate_max(long * array, unsigned size) {

  unsigned i=0;
  long max = 0;
  for (i=0; i<size; i++)
  {
    max = (array[i] > max) ? array[i] : max;
  }
  return max;
}


/* encode the 8 byte header */
void bibob_udp_header_encode (char * buf, bibob_header_t* hdr) 
{

  buf[0] = 0xff;              // constant 255;
  buf[3] = hdr->id & 0xff;    //  Session ID 3 Bytes
  buf[2] = (hdr->id >> 8) & 0xff;
  buf[1] = (hdr->id >> 16) & 0xff;
  buf[4] = 0x00;              // Sequence no B0 (only used for recv's)
  buf[5] = 0x00;              // Sequence no B1 (only used for recv's)
  buf[6] = 0x02;              // Type 2
  buf[7] = 0x00;              // Unused

}

/* encode the 8 byte header */
void bibob_udp_header_decode (char * buf, bibob_header_t* hdr)
{

  unsigned long tmp;

  hdr->id = buf[1];
  hdr->id << 16;
  tmp = buf[2];
  hdr->id |= ((tmp << 8) & 0xff00);
  hdr->id |= buf[3] & 0xff;

  hdr->seq = buf[4];
  hdr->seq << 8;
  hdr->seq |= buf[5] & 0xff;

  hdr->type = buf[6];

}

/*! return true if BiBoB is alive */
int bibob_ping (bibob_t* bibob)
{
  if (bibob_send (bibob, " ") < 0)
    return -1;

  return bibob_ignore (bibob);
}



