/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ibob.h"
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

//#define _DEBUG 1

ibob_t* ibob_construct ()
{
  ibob_t* ibob = malloc (sizeof(ibob_t));

  ibob->fd = -1;
  ibob->buffer_size = 0;
  ibob->buffer = 0;
  ibob->emulate_telnet = 1;

  ibob->host = 0;

  ibob->bit_window = 0;

  /* 2^12, unity scale factor */
  ibob->pol1_coeff = 4096;
  ibob->pol2_coeff = 4096;

  ibob->pol1_bram = 0;
  ibob->pol2_bram = 0;
  ibob->bram_max = 8.8;   // log2(2^8) * 1.1
  ibob->n_brams = 0;

  return ibob;
}

/*! free all resources reserved for ibob communications */
int ibob_destroy (ibob_t* ibob)
{
  if (!ibob)
    return -1;

  if (ibob->fd != -1)
    sock_close (ibob->fd);

  if (ibob->buffer)
    free (ibob->buffer);

  if (ibob->pol1_bram)
    free (ibob->pol1_bram);

  if (ibob->pol2_bram)
    free (ibob->pol2_bram);

  if (ibob->host)
    free (ibob->host);

  free (ibob);

  return 0;
}

/*! set the host and port number of the ibob */
int ibob_set_host (ibob_t* ibob, const char* host, int port)
{
  if (!ibob)
    return -1;

  if (ibob->host)
    free (ibob->host);

  ibob->host = strdup (host);
  ibob->port = port;

  return 0;
}

/*! set the ibob number (use default IP base and port) */
int ibob_set_number (ibob_t* ibob, unsigned number)
{
  if (!ibob)
    return -1;

  if (ibob->host)
    free (ibob->host);

  ibob->host = strdup (IBOB_VLAN_BASE"XXXXXX");
  sprintf (ibob->host, IBOB_VLAN_BASE"%u", number);

  ibob->port = IBOB_PORT;

  return 0;
}

int ibob_emulate_telnet (ibob_t* ibob)
{
  const char message1[] = { 255, 251, 37,
                            255, 253, 38,
                            255, 251, 38,
                            255, 253, 3,
                            255, 251, 24,
                            255, 251, 31,
                            255, 251, 32,
                            255, 251, 33,
                            255, 251, 34,
                            255, 251, 39,
                            255, 253, 5,
                            255, 251, 35,
                            0 };

  const char expected_response [] = { 255, 251, 1, 
                                      255, 251, 3, 
                                      0 };

  const char message2[] = { 255, 253, 1,
                            0 };

  int message_length = 0;

  if (!ibob)
    return -1;

  if (ibob->fd == -1)
    return -1;

  message_length = strlen (message1);

  if (sock_write (ibob->fd, message1, message_length) < message_length)
  {
    fprintf (stderr, "ibob_emulate_telnet: could not send message 1 - %s\n",
             strerror(errno));
    return -1;
  }

  message_length = strlen (expected_response);

  if (sock_read (ibob->fd, ibob->buffer, message_length) < message_length)
  {
    sock_close (ibob->fd);

    ibob->fd = sock_open (ibob->host, ibob->port);
    if ( ibob->fd < 0 )
    {
      fprintf (stderr, "ibob_emulate_telnet: could not reopen %s %d - %s\n",
               ibob->host, ibob->port, strerror(errno));
      return -1;
    }

    return ibob_emulate_telnet (ibob);
  }

  message_length = strlen (message2);

  if (sock_write (ibob->fd, message2, message_length) < message_length)
  {
    fprintf (stderr, "ibob_emulate_telnet: could not send message 2 - %s\n",
             strerror(errno));
    return -1;
  }

  // read the welcome message up to the iBoB prompt
  return ibob_recv (ibob, ibob->buffer, ibob->buffer_size);
}

static const char ibob_prompt[] = "IBOB % ";

/*! start reading from/writing to an ibob */
int ibob_open (ibob_t* ibob)
{
  if (!ibob)
    return -1;

  if (ibob->fd != -1)
  {
    fprintf (stderr, "ibob_open: already open\n");
    return -1;
  }

  if (ibob->host == 0)
  {
    fprintf (stderr, "ibob_open: host name not set\n");
    return -1;
  }

  if (ibob->port == 0)
  {
    fprintf (stderr, "ibob_open: port not set\n");
    return -1;
  }

  ibob->fd = sock_open (ibob->host, ibob->port);
  if ( ibob->fd < 0 )
  {
    fprintf (stderr, "ibob_open: could not open %s %d - %s\n",
             ibob->host, ibob->port, strerror(errno));
    return -1;
  }

  if (!ibob->buffer_size)
    ibob->buffer_size = 1024;

  ibob->buffer = realloc (ibob->buffer, ibob->buffer_size);

  ibob->pol1_bram = malloc(sizeof(long) * IBOB_BRAM_CHANNELS);
  ibob->pol2_bram = malloc(sizeof(long) * IBOB_BRAM_CHANNELS);

  ibob_bramdump_reset(ibob);

  if (ibob->emulate_telnet)
    return ibob_emulate_telnet (ibob);

  return 0;
}

/*! stop reading from/writing to an ibob */
int ibob_close (ibob_t* ibob)
{
  if (!ibob)
    return -1;

  if (ibob->fd == -1)
    return -1;

  int fd = ibob->fd;
  ibob->fd = -1;

  return sock_close (fd);
}

/*! return true if already open */
int ibob_is_open (ibob_t* ibob)
{
  return ibob && (ibob->fd != -1);
}

/*! return true if iBoB is alive */
int ibob_ping (ibob_t* ibob)
{
  if (ibob_send (ibob, " ") < 0)
    return -1;

  return ibob_ignore (ibob);
}

/*! reset packet counter on next UTC second, returned */
time_t ibob_arm (ibob_t* ibob)
{
  return 0;
}

int ibob_ignore (ibob_t* ibob)
{
  return ibob_recv (ibob, ibob->buffer, ibob->buffer_size);
}

/*! configure the ibob */
int ibob_configure (ibob_t* ibob, const char* mac_address)
{
  if (!ibob)
    return -1;

  if (ibob->fd == -1)
    return -1;

  int length = strlen (mac_address);
  if (length != 12)
  {
    fprintf (stderr,
             "ibob_configure: MAC address '%s' is not 12 characters\n",
             mac_address);
    return -1;
  }

  char msg[100];

  const char* mac_base = "x00000000";

  char* macone = strdup (mac_base);
  char* mactwo = strdup (mac_base);
  char* maclow = strdup (mac_address);

  int i = 0;
  for (i=0; i<length; i++)
    maclow[i] = tolower (mac_address[i]);

  strncpy(macone+5,maclow,4);
  strncpy(mactwo+1,maclow+4,8);

  free (maclow);

  ibob_send (ibob, "regwrite reg_ip 0x0a000004");
  ibob_ignore (ibob);

  ibob_send (ibob, "regwrite reg_10GbE_destport0 4001");
  ibob_ignore (ibob);

  ibob_send (ibob, "write l xd0000000 xffffffff");
  ibob_ignore (ibob);

  ibob_send (ibob, "setb x40000000");
  ibob_ignore (ibob);

  ibob_send (ibob, "writeb l 0 x00000060");
  ibob_ignore (ibob);

  ibob_send (ibob, "writeb l 4 xdd47e301");
  ibob_ignore (ibob);

  ibob_send (ibob, "writeb l 8 x00000000");
  ibob_ignore (ibob);

  ibob_send (ibob, "writeb l 12 x0a000001");
  ibob_ignore (ibob);

  ibob_send (ibob, "writeb b x16 x0f");
  ibob_ignore (ibob);

  ibob_send (ibob, "writeb b x17 xa0");
  ibob_ignore (ibob);

  sprintf(msg, "writeb l x3020 %s", macone);
  ibob_send (ibob, msg);
  ibob_ignore (ibob);
  free (macone);

  sprintf(msg, "writeb l x3024 %s", mactwo);
  ibob_send (ibob, msg);
  ibob_ignore (ibob);
  free (mactwo);

  ibob_send (ibob, "writeb b x15 xff");
  ibob_ignore (ibob);

  ibob_send (ibob, "write l xd0000000 x0");
  ibob_ignore (ibob);

  return 0;
}

/*! write bytes to ibob */
ssize_t ibob_send (ibob_t* ibob, const char* message)
{

  ssize_t wrote = ibob_send_async (ibob, message);

  if (wrote < 0)
    return -1;

  if (ibob_recv_echo (ibob, wrote) < 0)
    return -1;

  return wrote;
}

/*! write bytes to ibob */
ssize_t ibob_send_async (ibob_t* ibob, const char* message)
{
  if (!ibob)
    return -1;

  if (ibob->fd == -1)
    return -1;

  int length = strlen (message);

  if (length+2 > ibob->buffer_size)
  {
    ibob->buffer_size = length * 2;
    ibob->buffer = realloc (ibob->buffer, ibob->buffer_size);
  }

  snprintf (ibob->buffer, ibob->buffer_size, "%s\r", message);
  length++;

  // always sleep for 10 millseconds before a command to ensure we aren't
  // flooding the connection
  ibob_pause(10);

  return sock_write (ibob->fd, ibob->buffer, length);
}

/*! if emulating telnet, receive echoed characters */
int ibob_recv_echo (ibob_t* ibob, size_t length)
{
  if (!ibob->emulate_telnet)
    return length;

  /* read the echoed characters */
  ssize_t echo = ibob_recv (ibob, ibob->buffer, (length+1));
  if (echo < length)
    return -1;

  return echo;
}

/*! read bytes from ibob */
ssize_t ibob_recv (ibob_t* ibob, char* ptr, size_t bytes)
{
  if (!ibob)
    return -1;

  if (ibob->fd == -1)
    return -1;

  size_t total_got = 0;
  ssize_t got = 0;

  while (bytes)
  {
    // do not time-out while emulating telnet (end-of-data = prompt reception)
    if (ibob->emulate_telnet)
    {
      got = sock_read (ibob->fd, ptr+total_got, bytes);
      char* prompt = strstr (ptr, ibob_prompt);
      if (prompt)
      {
        *prompt = '\0';
        return strlen(ptr);
      }
    }
    else
    {
      got = sock_tm_read (ibob->fd, ptr+total_got, bytes, 0.1);
      if (got == 0)
      {
        ptr[total_got] = '\0';
        return total_got;
      }
    }

    if (got < 0)
    {
      perror ("ibob_recv: ");
      return -1;
    }

    total_got += got;
    bytes -= got;

  }


  return total_got;
}

/* sleep for msec milliseconds */
void ibob_pause(int msec)
{
  struct timeval pause;
  pause.tv_sec=0;
  pause.tv_usec=(msec * 1000);
  select(0,NULL,NULL,NULL,&pause);
}

/* gets the bram for each polarisation and stores the 
 * results in the struct */
int ibob_bramdump(ibob_t* ibob) 
{

  char msg[100];
  unsigned bramdump_size = IBOB_BRAM_CHANNELS * IBOB_CHARS_PER_BRAM;

  if (ibob->buffer_size < bramdump_size+1)
  {
    ibob->buffer_size = bramdump_size+1;

#ifdef _DEBUG
    fprintf (stderr, "ibob_bramdump: %s realloc ibob->buffer to %d bytes\n",
                      ibob->host, ibob->buffer_size);
#endif

    ibob->buffer = realloc (ibob->buffer, ibob->buffer_size);
    if (!ibob->buffer)
    {
      fprintf (stderr, "ibob_bramdump: realloc failed\n");
      return -1;
    }
  }

  sprintf(msg, "bramdump scope_output1/bram 0 %d", IBOB_BRAM_CHANNELS);
  ibob_send (ibob, msg);
  ibob_recv (ibob, ibob->buffer, bramdump_size);

  ibob->buffer[bramdump_size] = '\0';
  extract_counts(ibob->buffer, ibob->pol1_bram, IBOB_BRAM_CHANNELS);

  ibob_ignore (ibob);

  sprintf(msg, "bramdump scope_output3/bram 0 %d", IBOB_BRAM_CHANNELS);
  ibob_send (ibob, msg);
  ibob_recv (ibob, ibob->buffer, bramdump_size);
  ibob->buffer[bramdump_size] = '\0';
  extract_counts(ibob->buffer, ibob->pol2_bram, IBOB_BRAM_CHANNELS);
  ibob_ignore (ibob);

  /* increment internal counter for number of dumps in ibob->pol?_bram */
  ibob->n_brams++;

  return 0;

}

/* reset accumulated bram values */
void ibob_bramdump_reset(ibob_t * ibob)
{

  unsigned i=0;
  for (i=0; i<IBOB_BRAM_CHANNELS; i++) {
    ibob->pol1_bram[i] = 0;
    ibob->pol2_bram[i] = 0;
  }
  ibob->n_brams = 0;

}

/* writes the accumulated bramdumps to disk */
int ibob_bramdisk(ibob_t * ibob)
{

  int fd = 0;
  int flags = O_CREAT | O_WRONLY | O_TRUNC;
  int perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

  char fname[128];
  char time_str[64];

  time_t now = time(0);
  strftime (time_str, 64, DADA_TIMESTR, localtime(&now));

  sprintf(fname, "%s_%s.bramdump.tmp", time_str, ibob->host);

  fd = open(fname, flags, perms);
  if (fd < 0)
  {
    fprintf (stderr, "Error opening bramdump file: %s\n", fname);
    return -1;
  }

  /* floating point arrays for bramdump */
  float * pol1 = (float *) malloc(sizeof(float) * IBOB_BRAM_CHANNELS);
  float * pol2 = (float *) malloc(sizeof(float) * IBOB_BRAM_CHANNELS);
  if (!pol1 || !pol2)
  {
    fprintf(stderr, "ibob_bramdisk: could not malloc, oh dear me\n");
    return -1;
  }

  /* determine the average */
  unsigned i=0;
  for (i=0; i<IBOB_BRAM_CHANNELS; i++)
  {
    pol1[i] = ((float) ibob->pol1_bram[i]) / ((float) ibob->n_brams);
    pol2[i] = ((float) ibob->pol2_bram[i]) / ((float) ibob->n_brams);
  }

  write(fd, &ibob->bit_window, sizeof(unsigned));
  write(fd, pol1, sizeof(float) * IBOB_BRAM_CHANNELS);
  write(fd, pol2, sizeof(float) * IBOB_BRAM_CHANNELS);

  close(fd);

  free(pol1);
  free(pol2);

  /* reset the averages since we have modified the arrays */
  ibob_bramdump_reset(ibob);

  /* rename the tmp bramdump file to the correct name */
  char real_fname[128];

  sprintf(real_fname, "%s_%s.bramdump", time_str, ibob->host);

  /* Sometimes it takes a while for the file to appear, wait for it */
  flags =  R_OK | W_OK;
  int max_wait = 10;
                                                                                                               
  while (access(fname, flags) != 0)
  {
    fprintf(stderr, "ibob_bramdisk: %s waiting 500ms for bramdump files\n", 
                     ibob->host);
    ibob_pause(500);
    max_wait--;

    if (max_wait == 0)
    {
      fprintf(stderr, "ibob_bramdisk: file not on disk after 5 sec\n");
      break;
    }
  }

  if (max_wait) 
  {
  
    char command[256];
    int rval = rename(fname, real_fname);

    if (rval != 0)
    {
      fprintf(stderr, "%s: rename failed: %d\n", ibob->host, rval);
      unlink (fname);
      return -1;
    }

    return 0;

  }
  else
    return -1;


}



void * ibob_level_setter(void * context) 
{

  ibob_t * ibob = context;

#ifdef _DEBUG
  fprintf (stderr, "ibob_level_setter: %s\n", ibob->host);
#endif

  char msg[100];

  /* number of iterations at setting the levels */
  int n_attempts = 2;

  int i=0;
  int rval = 0;

  long pol1_max_value;
  long pol2_max_value;

  sprintf(msg, "regwrite reg_coeff_pol1 %d",ibob->pol1_coeff);
  ibob_send (ibob, msg);
  ibob_ignore (ibob);
                                                                                                                           
  sprintf(msg, "regwrite reg_coeff_pol2 %d",ibob->pol2_coeff);
  ibob_send (ibob, msg);
  ibob_ignore (ibob);

  /* 2 iterations to get close to the required value */
  for (i=0; i<n_attempts; i++ ) {

    ibob_pause(50);

    ibob_bramdump_reset(ibob);
 
    /* Extracts a bram dump for each polarisation into ibob->pol?_bram */
    ibob_bramdump(ibob);

    median_smooth_zap(IBOB_BRAM_CHANNELS, ibob->pol1_bram, 11);
    median_smooth_zap(IBOB_BRAM_CHANNELS, ibob->pol2_bram, 11);

    pol1_max_value = calculate_max(ibob->pol1_bram, IBOB_BRAM_CHANNELS);
    pol2_max_value = calculate_max(ibob->pol2_bram, IBOB_BRAM_CHANNELS);

    ibob->bit_window = find_bit_window(pol1_max_value, pol2_max_value, 
                                 &(ibob->pol1_coeff), &(ibob->pol2_coeff));

#ifdef _DEBUG
    fprintf(stderr, "%s: bit: %d, scales [%d,%d] maxvals = [%d,%d]\n",
                    ibob->host, ibob->bit_window,ibob->pol1_coeff,
                    ibob->pol2_coeff, pol1_max_value, pol2_max_value);
#endif

    /* set scaling coefficients for each polarisation */
    sprintf(msg, "regwrite reg_coeff_pol1 %d",ibob->pol1_coeff);
    ibob_send (ibob, msg);
    ibob_ignore (ibob);

    sprintf(msg, "regwrite reg_coeff_pol2 %d",ibob->pol2_coeff);
    ibob_send (ibob, msg);
    ibob_ignore (ibob);

  }

  unsigned ndumps = 32;

  for (i=0; i<2; i++) {

    ibob_pause(50);

    ibob_bramdump_reset(ibob);

    // intergrate 32 bram dumps to remove as much noise as possible 
    unsigned j=0;
    for (j=0; j<ndumps; j++) 
    {
      ibob_bramdump(ibob);
    }

    assert(ibob->n_brams == ndumps);

    // divide by ndumps
    for (j=0; j<IBOB_BRAM_CHANNELS; j++)
    {
      ibob->pol1_bram[j] = ibob->pol1_bram[j] / ndumps;
      ibob->pol2_bram[j] = ibob->pol2_bram[j] / ndumps;
    }

    median_smooth_zap(IBOB_BRAM_CHANNELS, ibob->pol1_bram, 11);
    median_smooth_zap(IBOB_BRAM_CHANNELS, ibob->pol2_bram, 11);

    pol1_max_value = calculate_max(ibob->pol1_bram, IBOB_BRAM_CHANNELS);
    pol2_max_value = calculate_max(ibob->pol2_bram, IBOB_BRAM_CHANNELS);
    
    ibob->bit_window = find_bit_window(pol1_max_value, pol2_max_value,
                                 &(ibob->pol1_coeff), &(ibob->pol2_coeff));
                                                                                                         
#ifdef _DEBUG
    fprintf(stderr, "%s: bit: %d, scales [%d,%d] maxvals = [%d,%d]\n",
                    ibob->host, ibob->bit_window,ibob->pol1_coeff,
                    ibob->pol2_coeff, pol1_max_value, pol2_max_value);
#endif
    sprintf(msg, "regwrite reg_coeff_pol1 %d",ibob->pol1_coeff);
    ibob_send (ibob, msg);
    ibob_ignore (ibob);

    sprintf(msg, "regwrite reg_coeff_pol2 %d",ibob->pol2_coeff);
    ibob_send (ibob, msg);
    ibob_ignore (ibob);
  }
  
  sprintf(msg,"regwrite reg_output_bitselect %d", ibob->bit_window);
  ibob_send (ibob, msg);
  ibob_ignore (ibob);

  ibob_bramdump_reset(ibob);

  return ;
}

void extract_counts(char * bram, long * vals, int n_chan) {

  const char *sep = "\r\n";
  char *saveptr;
  char *line;
  char value[9];
  long val;
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
  vals[0] += val;

  for (i=1; i<n_chan; i++) {
    line = strtok_r(NULL, sep, &saveptr);
    if (line == NULL)
    {
      fprintf(stderr, "extract_counts: unexpected end of input on line %d\n",i);
      return;
    }
    strncpy(value, line+2, 8); 
    value[8] = '\0';
    sscanf(value, "%x",&val);
    vals[i] += val;

  }

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
    if ((i<430) || (i>480))
      max = (array[i] > max) ? array[i] : max;
  }
  return max;
}


