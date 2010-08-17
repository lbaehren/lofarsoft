#include "ascii_header.h"
#include "sock.h"
#include "daemon.h"
#include "multilog.h"
#include "apsr_def.h"
#include "apsr_udpdb.h"
#include "apsr_udp.h"

#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <sys/timeb.h> 
//#include <arpa/inet.h>
#include <math.h>
#include <pthread.h>

#include "arch.h"
#include "Statistics.h"
#include "RealTime.h"
#include "StopWatch.h"

#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

int sendPacket(int sockfd, struct sockaddr_in addr, char *data, int size);
void signal_handler(int signalValue);
void usage();
void quit();
void generate_signal(char *signal, int signal_length, int nbit, int ndim,
                      int npol, int produce_noise, int gain);
void generate_2bit(char *array, int array_size, int produce_noise, int gain);
void generate_4bit(char *array, int array_size, int produce_noise, int gain);
void generate_8bit(char *array, int array_size, int produce_noise, int gain);
void print_triwave(char *tri_data, int tri_datasize, int nbits);
void displayBits(unsigned int value);
void displayCharBits(char value);
void print_two_bit_char(char value);
void get_two_bit_char(char value, int * values);
int encode_tri_data(char * udp_data, int size_of_frame, char * tri_data,
                    int tri_datasize, int tri_counter, int npol, int print);
char two_bit_convert(int value);
char four_bit_convert(int value);
void gain_monitor(void);
void create_udpout_socket(int *fd, struct sockaddr_in * dagram_socket,
                       char *client, int port, int broadcast);
time_t wait_for_start(int daemon, multilog_t* log);
double rand_normal(double mean, double stddev);

/* UDP socket */
struct sockaddr_in dagram_socket;

int new_gain_value = 33;  // Standard gain of 33 % (for 33:33:16:16)
int current_gain = 33;

int main(int argc, char *argv[])
{

  /* custom header size of size UDPHEADERSIZE */
  header_struct header;

  /* number of microseconds between packets */
  double sleep_time = 22;
 
  /* Size of UDP payload. Optimally 1472 (1500 MTU) or 8948 (9000 MTU) bytes */
  uint64_t size_of_frame = UDPHEADERSIZE + DEFAULT_UDPDATASIZE;

  /* be verbose? */ 
  int verbose = 0;

  /* udp port to send data to */
  int udp_port = APSR_DEFAULT_UDPDB_PORT;

  /* total time to transmit for */ 
  uint64_t transmission_time = 5;   

  /* Desired data rate in Bytes per second */
  unsigned int data_rate = 64000000;       

  /* be a daemon? */
  int daemon = 0;

  /* NBANDS header value */
  int nband = 1;

  /* NBIT header value */
  int nbit = 8;

  /* NPOL header value */
  int npol = 1;

  /* NDIM header value */
  int ndim = 1;

  /* NCHANNEL header value */
  int nchannel = 1;

  /* DADA logger */
  multilog_t *log = 0;

  /* Hostname to send UDP packets to */
  char *udpdb_client;

  /* udp file descriptor */
  int udpfd;

  /* The generated signal array */
  char ** signal_arrays;

  uint64_t signal_size = 32000;

  /* CALFREQ / TSAMP */
  int signal_length = 64000;

  /* triangular wave data array */
  //char * tri_data;

  /* length of triangular wave in bytes */
  //uint64_t tri_datasize = 512;

  /* number of bits in the triangular wave */
  uint64_t udp_data_size = DEFAULT_UDPDATASIZE;

  /* produce noise */
  int produce_noise = 0;

  /* broadcast udp packets ? */
  int broadcast = 0;

  int calfreq = 1000;

  float tsamp = 0.015625;

  /* start of transmission */
  time_t start_time;

  /* end of transmission */
  time_t end_time;

  /* enable gain control */
  int allow_gain_control = 0;

  opterr = 0;
  int c;
  while ((c = getopt(argc, argv, "b:k:c:t:g:jds:p:n:r:avy")) != EOF) {
    switch(c) {

      case 'b':      
        nbit = atoi(optarg);
        if (!((nbit == 2) || (nbit == 4) || (nbit == 8))) {
          fprintf(stderr,"only 2, 4 or 8 nbit are supported\n");
          return  EXIT_FAILURE;
        }
        break;

      case 'k':
        npol = atoi(optarg);
        if (!((npol == 1) || (npol == 2))) {
          fprintf(stderr,"only 1 or 2 polarisations are supported\n");
          return  EXIT_FAILURE;
        }
        break;

      case 'j':
        produce_noise = 1;
        break;

      case 's':
        size_of_frame = atoi(optarg);
        udp_data_size = size_of_frame - UDPHEADERSIZE;
        if ((size_of_frame <= 0) || (size_of_frame >= UDPBUFFSIZE)) {
          fprintf(stderr,"packet size must be between > 0 and < %d bytes\n",
                          UDPBUFFSIZE);
          usage();
          return EXIT_FAILURE;
        }
        break;

      case 'n':
        transmission_time = atoi(optarg);
        break;

      case 'g':
        ndim = atoi(optarg);
        break;

      case 'd':
        daemon = 1;
        break;
        
      case 'v':
        verbose = 1;
        break;

      case 'p':
        udp_port = atoi(optarg);
        break;

      case 'c':
        calfreq = atoi(optarg);
        break;

      case 't':
        tsamp = atof(optarg);
        break;

      case 'r':
        data_rate = atoi(optarg);
        break;
 
      case 'a':
        broadcast = 1;
        break;

      case 'y':
        allow_gain_control = 1;
        break;

      default:
        usage();
        return EXIT_FAILURE;
        break;
    }
  }

  /* Check arguements */
  if ((argc - optind) != 1) {
    fprintf(stderr,"no udpdb_client was specified\n");
    usage();
    return EXIT_FAILURE;
  }

  log = multilog_open ("apsr_udpgenerator", daemon);

  if (daemon) {
    be_a_daemon();
    multilog_serve (log, APSR_UDPGENERATOR_LOG);
  } else {
    multilog_add(log, stderr);
  }

  udpdb_client = (char *) argv[optind];

  signal(SIGINT, signal_handler);

  /* create the socket for outgoing UDP data */
  create_udpout_socket(&udpfd, &dagram_socket, udpdb_client, udp_port, broadcast);

  if (verbose) multilog(log,LOG_INFO,"udp socket setup\n");

  char udp_data[(UDPHEADERSIZE+udp_data_size)];
  uint64_t data_counter = 0;

  /* initialise data rate timing library */
  StopWatch wait_sw;
  RealTime_Initialise(1);
  StopWatch_Initialise(1);

  /* setup udp header information */
  header.length = (int) UDPHEADERSIZE;
  header.source = (char) 3;     /* This is not used by udpdb */
  header.sequence = 0;
  header.bits = nbit;
  header.pollength = (int) (udp_data_size / npol);
  header.channels = nchannel;
  header.bands = nband;    
  header.bandID[0] = 1;         /* This is not used by udpdb */
  header.bandID[1] = 2;         /* This is not used by udpdb */
  header.bandID[2] = 3;         /* This is not used by udpdb */
  header.bandID[3] = 4;         /* This is not used by udpdb */

  if (verbose) multilog(log,LOG_INFO,"data structures setup\n");

  /* If we have a desired data rate, then we need to adjust our sleep time
   * accordingly */
  if (data_rate > 0) {
    double packets_per_second = ((double) data_rate) / ((double)udp_data_size);
    sleep_time = (1.0/packets_per_second)*1000000.0;
  }

  /* Determine size of the signal array
   *
   * Tsamp corresponds to each sample timeslice.
   * with ndim=2, npol=2 that means 4 "samples" every tsamp
   * signal_length = 1000000/(CALFREQ * Tsamp)
   *
   * Dont let npol affect this size as it will be account for when we 
   * encode the packets
   */

  fprintf(stderr,"calfreq = %d\n", calfreq);

  float time_slices = 1000000 / tsamp;	// 64,000,000
  float time_slices_ina_cal = time_slices / (float) calfreq;	// 64,000
  int bits_for_a_time_slice = ndim * npol * nbit; // 8 
  float bytes_for_a_time_slice = (float) bits_for_a_time_slice / 8.0; // 1
  float bytes_for_a_cal = time_slices_ina_cal * bytes_for_a_time_slice;
  fprintf(stderr,"bytes in a CAL = %f\n",bytes_for_a_cal);

  signal_size = (int) (((float) ndim) * ((float) npol) * (((float) nbit) /8.0) * (1.0 / (float) calfreq) * (1000000.0/tsamp));

  fprintf(stderr,"signal bytes = %d\n",signal_size);

  int i=0;
  int n_signals = 101;

  /* Thread to respond to external gain setting requests */
  pthread_t gain_thread;
  if (allow_gain_control) {  

    if (pthread_create (&gain_thread, 0, (void *) gain_monitor, 0) < 0) {
      perror ("nexus_connect: Error creating new thread");
      return -1;
    }
    pthread_detach(gain_thread);
  }

  signal_arrays = malloc(sizeof(char *) * n_signals);
  srand ( time(NULL) );

  for(i=0; i<n_signals; i++) {

    if (verbose) fprintf(stderr, "creating signal %d\n",i);

    /* Create and allocate the triangular data array */
    signal_arrays[i] = malloc(sizeof(char) * signal_size);

    if (allow_gain_control) 
      generate_signal(signal_arrays[i], signal_size, nbit, ndim, npol, produce_noise, i*10);
    else
      generate_signal(signal_arrays[i], signal_size, nbit, ndim, npol, produce_noise, 333);

  }

  int tri_counter = 0;
  int bufsize = 4096;
  char* buffer = (char*) malloc ( bufsize);
  assert (buffer != 0);

  /* If we are running as a daemon, then we must wait for a "start"
   * message, and then return the UTC time of the first packet. Hence
   * we would start on "second" time boundary based on NTP */

  start_time = wait_for_start(daemon, log);
  end_time = start_time+transmission_time;

  strftime (buffer, bufsize, DADA_TIMESTR, (struct tm*) localtime(&start_time));
  multilog(log,LOG_INFO,"udp data starting at %s\n",buffer);

  strftime (buffer, bufsize, DADA_TIMESTR, (struct tm*) localtime(&end_time));
  multilog(log,LOG_INFO,"udp data will end at %s\n",buffer);

  uint64_t bytes_to_send = size_of_frame - UDPHEADERSIZE;
  uint64_t total_bytes_to_send = data_rate * transmission_time;

  uint64_t bytes_sent = 0;
  uint64_t total_bytes_sent = 0;

  uint64_t bytes_sent_thistime = 0;
  uint64_t prev_bytes_sent = 0;
  
  time_t current_time = time(0);
  time_t prev_time = time(0);

  multilog(log,LOG_INFO,"Total bytes to send = %"PRIu64"\n",total_bytes_to_send);
  multilog(log,LOG_INFO,"Packet size = %"PRIu64" bytes\n",size_of_frame);
  multilog(log,LOG_INFO,"Data size = %"PRIu64" bytes\n",size_of_frame-UDPHEADERSIZE);
  multilog(log,LOG_INFO,"Wire Rate\t\tUseful Rate\tPacket\tSleep Time\n");

  int signal_to_use = 0;

  current_gain = new_gain_value;

  int actual_gain = 0;

  while (total_bytes_sent < total_bytes_to_send) {
         
    if (daemon)  {
      while (current_time < start_time) {
        current_time = time(0);
      }
      daemon=0;
    }

    StopWatch_Start(&wait_sw);

    encode_header(udp_data, &header);
    header.sequence++;
    
    if (allow_gain_control) {

      if ((current_gain != new_gain_value) && (new_gain_value >= 0) && (new_gain_value < n_signals)) {
        fprintf(stderr, "Changing gain from %d to %d\n",current_gain, new_gain_value);
        current_gain = new_gain_value;
      }

      // Jiggle it a little bit (6 nearest gains)
      actual_gain = (int) (6 * (rand() / (RAND_MAX + 1.0)));
      actual_gain += current_gain - 3;
      if (actual_gain > n_signals) 

        actual_gain = n_signals -1;
      if (actual_gain < 0)
        actual_gain = 0;
      
    } else {
      current_gain = (int) (n_signals * (rand() / (RAND_MAX + 1.0)));
      actual_gain = current_gain;
    }

    tri_counter = encode_tri_data(udp_data+UDPHEADERSIZE, udp_data_size,
                                signal_arrays[actual_gain], signal_size, tri_counter, npol, 0);

    /* Bytes to send in the next packet */
    bytes_to_send = size_of_frame - UDPHEADERSIZE;

    if ((total_bytes_to_send - total_bytes_sent) < bytes_to_send) 
      bytes_to_send = total_bytes_to_send - total_bytes_sent;
    
    bytes_sent = sendPacket(udpfd, dagram_socket, udp_data, 
                                bytes_to_send+UDPHEADERSIZE); 

    if (bytes_sent != bytes_to_send+UDPHEADERSIZE) 
      multilog(log,LOG_ERR,"Error. Attempted to send %"PRIu64" bytes, but only "
                           "%"PRIu64" bytes were sent\n",size_of_frame,
                           bytes_sent_thistime);

    /* This is how much useful data we actaully sent */
    total_bytes_sent += (bytes_sent - UDPHEADERSIZE);

    data_counter++;
    prev_time = current_time;
    current_time = time(0);
    
    if (prev_time != current_time) {

      double complete_udp_packet = (double) bytes_sent;
      double useful_data_only = (double) (bytes_sent - UDPHEADERSIZE);
      double complete_packet = 28.0 + complete_udp_packet;

      double wire_ratio = complete_packet / complete_udp_packet;
      double useful_ratio = useful_data_only / complete_udp_packet;
        
      uint64_t bytes_per_second = total_bytes_sent - prev_bytes_sent;
      prev_bytes_sent = total_bytes_sent;
      double rate = ((double) bytes_per_second) / (1024*1024);

      double wire_rate = rate * wire_ratio;
      double useful_rate = rate * useful_ratio;
             
      multilog(log,LOG_INFO,"%5.2f MB/s\t%5.2f MB/s\t%"PRIu64"\t%5.2f, %"PRIu64"\n",
                            wire_rate, useful_rate,data_counter,sleep_time,bytes_sent);
    }

    StopWatch_Delay(&wait_sw, sleep_time);

  }


  multilog(log, LOG_INFO, "Sent %"PRIu64" bytes\n",total_bytes_sent);
  multilog(log, LOG_INFO, "Sent %"PRIu64" packets\n",header.sequence);

  close(udpfd);
  return 0;
}


void signal_handler(int signalValue) {
  exit(EXIT_SUCCESS);
}

int sendPacket(int sockfd, struct sockaddr_in their_addr, char *udp_data, 
               int size_of_frame) {

  int numbytes;

  if ((numbytes=sendto(sockfd, udp_data, size_of_frame, 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) < 0) {
    perror("sendto");
    exit(1);
  }
  
  return(numbytes);
}

void usage() {
  fprintf(stdout,
    "apsr_udpgenerator [options] dest_host\n"
    "\t-a            broadcast packets to 192.168.1.255\n"
    "\t-b n          number of bits per sample: 2,4 or 8 [default 8]\n"
    "\t-k n          number of polarisations: 1 or 2 [default 1]\n"
    "\t-j            produce random noise [default off]\n"
    "\t-s n          send frames of size n bytes [default %d]\n"
    "\t-p n          udp port to send packets to [default %d]\n"
    "\t-n n          number of seconds to transmit [default 5]\n"
    "\t-r n          data rate (Bytes/s) not including headers [default 64000000]\n"
    "\t-c n          cal freq [default 1000 Hz]\n"
    "\t-t n          Tsamp [default 0.015625 usec]\n"
    "\t-g n          ndim [default 1]\n"
    "\t-v            verbose output\n"
    "\t-d            daemon mode. expects TCP/IP control\n"
    "\t-y            allow gain control\n"
    "\n\tudpdb_client is the hostname of a machine running dada_udpdb\n\n"
    ,
    (UDPHEADERSIZE + DEFAULT_UDPDATASIZE), APSR_DEFAULT_UDPDB_PORT);
}

int encode_tri_data(char * udp_data, int size_of_frame, char * tri_data, 
                    int tri_datasize, int tri_counter, int npol, int print) {

  int i = 0;
  int j = tri_counter;

  /* If we have 2 polarisations, the first half of the packet is the first
   * polarsation, and the second half is the second polarisation */
  int data_length = (size_of_frame / npol);

  for (i=0; i < data_length; i++) {

    if (npol == 1) {

       if (j == tri_datasize)
         j = 0;
    
       udp_data[i] = tri_data[j];
       if (print) {
         fprintf(stderr,"[%d] <= [%d]\n",i,j);
       }

    /* For 2 polarisations, we pack pol1 and pol2 into the 2 halves of the
       packet */
    } else {

      if (j == tri_datasize) 
        j = 0;

      udp_data[i] = tri_data[j];

      j++;
      if (j == tri_datasize) 
        j = 0;

      udp_data[data_length + i] = tri_data[j];

    }

    j++;
  }

  return j;

}

/*
 * Encodes the signal into the udp packets. The encoding is dependant 
 * on npol and ndim.
 *
 * 1pol 1dim => [p1d1]
 * 2pol 1dim => [p1d1 p2d2]
 * 1pol 2dim => [p1s1 p1d2]
 * 2pol 2dim => [p1d1 p1d2 p2d1 p2d2]
 *
 * gain varies from 0 to 1000
 */

void generate_2bit(char *array, int array_size, int noise, int gain) {

  unsigned int val = 0;
  int rnd, i, j;

  int state0 = 0;
  int state1 = 0;
  int state2 = 0;
  int state3 = 0;

  int state4 = 0;
  int state5 = 0;
  int state6 = 0;
  int state7 = 0;

  int nsamples = array_size * 4; // 8/nbits
  char samples_array[nsamples];

  int low   = (int) ((1000-gain)/2);
  int _low  = (int) ((1000-gain)/2);
  int high  = (int) (gain/2);
  int _high = (int) (gain/2);

  int l = low;
  int _l = _low;
  int h = high;
  int _h = _high;

  int diff  = 100;
  int d = diff;

  for (i=0; i < nsamples; i++) {

    val = 0;
    rnd = 1 + (int) (1000.0 * (rand() / (RAND_MAX + 1.0)));

    /* If we a generating a CAL signal */
    if (!noise) {
 
      if ((i < (nsamples*0.25)) || (i > (nsamples*0.75))) {
        d = (diff/2);
      } else {
        d = -(diff/2);
      }

      l  =  low  + d;
      _l = _low  + d;
      h  =  high - d;
      _h = _high - d;

      _h = MAX(0,   _h);
      _l = MIN(500, _l);
       l = MIN(500,  l);
       h = MAX(0,    h);
    }

    if ((rnd >= 0)   && (rnd < l))
      val = 3;
    if ((rnd >= l) && (rnd < (l + _l)))
      val = 0;
    if ((rnd >= l + _l) && (rnd < l + _l + h)) 
      val = 2;
    if (rnd >= l + _l + h)                  
      val = 1;

    if (((i < (nsamples*0.25)) || (i > (nsamples*0.75))) || (noise)) {
      if (val == 0) state0++;
      if (val == 1) state1++;
      if (val == 2) state2++;
      if (val == 3) state3++;
    } else {
      if (val == 0) state4++;
      if (val == 1) state5++;
      if (val == 2) state6++;
      if (val == 3) state7++;
    }

    samples_array[i] = val;
  }

  if (!noise) {
    state0 += state4;
    state1 += state5;
    state2 += state6;
    state3 += state7;
  }

  /* Now pack the data from the samples into the char array */
  /* The second polarization gets packed into the second half of each packet */
  for (i=0; i<array_size; i++) {

    for (j=0; j < 4; j++) {

      /* Get the 2 bit value in char form */
      char two_bit_value = two_bit_convert(samples_array[((4*i)+j)]);

      if (j == 0) array[i]  = two_bit_value << 6;
      if (j == 1) array[i] |= two_bit_value << 4;
      if (j == 2) array[i] |= two_bit_value << 2;
      if (j == 3) array[i] |= two_bit_value;

    }
  }

}

void generate_4bit(char *array, int array_size, int noise, int gain) {

  unsigned int val = 0;
  int rnd, i, j;
  int nstates = 16;
 
  int state_starts[nstates]; 
  int states[nstates];
  int low_states[nstates];
  int high_states[nstates];

  for (i=0;i<nstates;i++) {
    states[i] = 0;
    low_states[i] = 0;
    high_states[i] = 0;
    state_starts[i] = 1000*i;
  }

  int convert[nstates];
  convert[0] = 0; 
  convert[1] = 1;
  convert[2] = 2;
  convert[3] = 3;
  convert[4] = 12;
  convert[5] = 13;
  convert[6] = 14;
  convert[7] = 15;
  convert[8] = 8;
  convert[9] = 9;
  convert[10] = 10;
  convert[11] = 11;
  convert[12] = 4;
  convert[13] = 5;
  convert[14] = 6;
  convert[15] = 7;

  int nsamples = array_size * 2;
  char samples_array[nsamples];
  float gain_multiplier = ((float) gain) / 1000;
  int gain_multi_int = (int) (gain_multiplier * 8);

  /* 0->8 if off */
  /* 9->15 is on */
  /* 0 -> 3 cal off */
  /* 4 -> 7 cal on */
  /* 8 -> 11 cal off */
  /* 12 -> 15 cal on */

  // If gain is 500 -> then 

  for (i=0; i < nsamples; i++) {

    float rnd = (float) rand() / (float) RAND_MAX;

    /* A number between 0 and 15 */
    val = (int) ((float) nstates * rnd);

    //fprintf(stderr, "rnd = %5.2f, val = %d\n",rnd, val);

    /* If we a generating a CAL signal */
    if (!noise) {

      /* Cal OFF */
      if ((i < (nsamples*0.25)) || (i > (nsamples*0.75) )) {

        val = (val > 7) ? (val - 8) : val;

        // We need to increase the number of high states in the off pulse
        if (gain > 500) 
          val += gain_multi_int;

        low_states[val]++;

      /* Cal ON */
      } else {

        val = (val < 8) ? (val + 8) : val;

        // We need to decrease the number of high states 
        if (gain < 500) 
          val -= (8-gain_multi_int);

        high_states[val]++;
      }

      /* change numeric value into the approriate value */
      states[val]++;
      val = convert[val];

    } else {

      if (gain > 500) 
        val += gain_multi_int;
      else
        val -= (8-gain_multi_int);

      val = (val < 0) ? 0 : val;
      val = (val > 15) ? 15 : val;

      val = convert[val];
      states[val]++;

    }
    samples_array[i] = val;

  }

  for (i=0; i<array_size; i++) {
    for (j=0; j < 2; j++) {
      /* Get the 4 bit value in char form */
      char four_bit_value = four_bit_convert(samples_array[((2*i)+j)]);
      if (j == 0) array[i]  = four_bit_value << 4;
      if (j == 1) array[i] |= four_bit_value;
    }
  }
}


/* 
 * Generates the signal 
 */
void generate_signal(char *array, int array_size, int nbits, int ndim, 
                     int npol, int noise, int gain) {

  if (nbits == 2)
    generate_2bit(array, array_size, noise, gain);
  else if (nbits == 4) 
    generate_4bit(array, array_size, noise, gain);
  else
    generate_8bit(array, array_size, noise, gain);

}

/*
 *  The goal is to produce a gaussian distribution, centered around
 *  0. The gain should change the width of the distribution, with 
 *  the nominal gain have 1 std deviation at 10
 *
 */

void generate_8bit(char *array, int array_size, int produce_noise, int gain) {

  // nominal stddev for 8bit data is 10
  double stddev = 10;

  // apply gain value to stddev
  stddev *= ((((float) gain) + 500) / 1000);

  double off = stddev;
  double on = stddev * 1.4;

  // generate an array of distribution for use with creating the array
  int random_count = 100000;
  int * off_array = (int *) malloc(sizeof(int) * random_count);
  int * on_array = (int *) malloc(sizeof(int) * random_count);

  int i, j;
  double n1, n2, x, y, r, d;

  for (i=0; i+1<random_count; i+=2) {

    // Choose a point x,y in the unit circle uniformaly at random
    do {
      // scale two random integers to doubles between -1 and 1
      x = 2.0*rand()/RAND_MAX - 1;
      y = 2.0*rand()/RAND_MAX - 1;
      r = x*x + y*y;
    } while (r == 0.0 || r > 1.0);

    {
      // Apply Box-Muller transform on x,y
      d = sqrt(-2.0*log(r)/r);
      n1 = x*d;
      n2 = y*d;

      off_array[i] = (int) floor(n1*off);
      off_array[i+1] = (int) floor(n2*off);

      if (!produce_noise) {
        on_array[i] = (int) (n1*on);
        on_array[i+1] = (int) (n2*on);
      }
    }
    assert(i+1 < random_count);
  }

  int val;
  int twos;
  unsigned int mask = 0x000000ff;
  for (i=0; i < array_size; i++) {
    
    j++;
    if (j >= random_count) 
      j=0;

    assert (j < random_count);

    if (produce_noise)
      val = off_array[j];
    else {
      if ( (i < (array_size*0.25)) || (i > (array_size*0.75)) )
        val = off_array[j];
      else
        val = on_array[j];
    } 

    twos = ~val;
    twos = twos + 1;
    twos &= mask;
    array[i] = (char) twos;
  }

  free(off_array);
  free(on_array);

}


/* Convert an integer value into its 2 bit representation */
char two_bit_convert(int val) {

  if (val == 0) 
    return 0x00;
  if (val == 1) 
    return 0x01;
  if (val == 2) 
    return 0x02;
  else 
    return 0x03;
}


void print_two_bit_char(char val) {

  int i=0;
  char mask = 1 << 7;
  int bits[8];

  for (i=0; i<8; i++) {
    bits[i] = val & mask ? 1 : 0; 
    val <<= 1;
  }

  for (i=0; i<4; i++) {

    int a = bits[i*2];
    int b = bits[(i*2)+1];

    if ((a == 0) && (b == 0)) printf("0");
    if ((a == 0) && (b == 1)) printf("1");
    if ((a == 1) && (b == 0)) printf("2");
    if ((a == 1) && (b == 1)) printf("3");

  }

}


void get_two_bit_char(char val, int *vals) {

  int i=0;
  char mask = 1 << 7;
  int bits[8];

  for (i=0; i<8; i++) {
    bits[i] = val & mask ? 1 : 0;
    val <<= 1;
  }

  for (i=0; i<4; i++) {

    int a = bits[i*2];
    int b = bits[(i*2)+1];

    if ((a == 0) && (b == 0)) vals[i] = 0;
    if ((a == 0) && (b == 1)) vals[i] = 1;
    if ((a == 1) && (b == 0)) vals[i] = 2;
    if ((a == 1) && (b == 1)) vals[i] = 3;
  }
}


char four_bit_convert(int val) {

  char mask = 15;
  char charval = val & mask;
  return charval;
  
}


void print_triwave(char *array, int array_size, int nbits) {

  int i = 0;
  int j = 0;
  unsigned int val = 0;
  int state0 = 0;
  int state1 = 0;
  int state2 = 0;
  int state3 = 0;
  int state4 = 0;
  int state5 = 0;
  int state6 = 0;
  int state7 = 0;

  int values[4] = {3,3,3,3};

  fprintf(stderr,"["); 
  for (i=0; i < array_size; i++) {

    if (nbits == 2) {
  
      values[0] = 3;
      values[1] = 3;
      values[2] = 3;
      values[3] = 3;

      get_two_bit_char(array[i], values);

      for (j=0;j<4; j++) {
        if (i<(array_size/2)) {
          if (values[j] == 0) state0++;
          if (values[j] == 1) state1++;
          if (values[j] == 2) state2++;
          if (values[j] == 3) state3++;
        } else {
          if (values[j] == 0) state4++;
          if (values[j] == 1) state5++;
          if (values[j] == 2) state6++;
          if (values[j] == 3) state7++;
        }
      }

    } else {

      val = (unsigned int) array[i];
      val &= 0x000000ff;
      fprintf(stderr,"%u.", val);

    }
  }
  fprintf(stderr,"]\n");
 
  float convert = ((float) array_size) * 4 / (2*100);

  if (nbits == 2) {
 
    fprintf(stderr, "LOW  : %5.3f%,  %5.3f%, %5.3f%, %5.3f%, %d\n", (float) state0 / convert, (float)state1 / convert, (float) state2 / convert, (float) state3 / (float)convert, array_size);
    fprintf(stderr, "HIGH : %5.3f%,  %5.3f%, %5.3f%, %5.3f%, %d\n", (float) state4 / convert, (float)state5 / convert, (float) state6 / convert, (float) state7 / (float)convert, array_size);
  }

}


void displayBits(unsigned int value) {
  unsigned int c, displayMask = 1 << 31;

  for (c=1; c<= 32; c++) {
    putchar(value & displayMask ? '1' : '0');
    value <<= 1;

    if (c% 8 == 0)
      putchar(' ');
  }    
  
}


void displayCharBits(char value) {
  char c, displayMask = 1 << 7;
                                                                                
  for (c=1; c<= 8; c++) {
    putchar(value & displayMask ? '1' : '0');
    value <<= 1;
                                                                                
  }
}

void gain_monitor(void) {

  FILE *sockin = 0;
  FILE *sockout = 0;

  /* Port for control from tcs simulator */
  int port = APSR_UDPGENERATOR_GAIN_PORT;

  int listen_fd;
  int fd;
  char *rgot;

  int bufsize = 4096;
  char* buffer = (char*) malloc ( bufsize);
  assert (buffer != 0);

  int quit = 0;

  while (!quit) {

    /* create a socket on which to listen */
    listen_fd = sock_create (&port);
    if (listen_fd < 0)  {
      fprintf(stderr,"Error creating socket: %s\n", strerror(errno));
      quit = 1;
      break;
    }

    /* wait for a connection */
    fd =  sock_accept (listen_fd);
    if (fd < 0)  {
      perror("Error accepting connection");
      quit = 1;
      break;
    }

    sockin = fdopen(fd,"r");
    if (!sockin) 
      perror("Error creating input stream");

    sockout = fdopen(fd,"w");
    if (!sockout) 
      perror("Error creating output stream");

    setbuf (sockin, 0);
    setbuf (sockout, 0);

    rgot = fgets (buffer, bufsize, sockin);

    if (rgot && !feof(sockin)) {
      buffer[strlen(buffer)-1] = '\0';

      int gain = 0;

      if (strstr(buffer,"APSRGAIN ") != NULL) {

        if ( sscanf( buffer, "APSRGAIN %d", &gain) != 1 ) 
          fprintf(stderr, "Could not extract gain from the command\n");
        else
          fprintf(stderr,"GAIN = %d\n",gain);

        if ((gain >= 0) && (gain <= 100)) {
          new_gain_value = gain;
          fprintf(stderr, "Setting Gain to %d\n",new_gain_value);
        } else {
          fprintf(stderr, "Invalid gain!!!\n");
        }

        fprintf(sockout, "ok\r\n");
      }

      if (strstr(buffer, "APSRGAIN") != NULL) {
        fprintf(sockout, "%d\r\n",current_gain);
      }
    }

    close(fd);
    close(listen_fd);

  }

}

/* Creates a UDP socket for outgoing UDP sockets */
void create_udpout_socket(int *fd, struct sockaddr_in * dagram_socket, 
                          char *client, int port, int broadcast) {

  /* Get the client hostname */
  struct hostent * client_hostname;
  client_hostname = gethostbyname(client);

  /* Setup the socket for UDP packets */
  *fd = socket(PF_INET, SOCK_DGRAM, 0);
  if (*fd < 0) {
    perror("failed to create UDP socket");
    exit(EXIT_FAILURE);
  }

  /* If broadcasting across the entire subnet */
  if (broadcast) {
   int yes = 1;
   if (setsockopt(*fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof yes) < 0) {
      perror("Could not set socket option");
      exit(EXIT_FAILURE);
    }
  }

  /* Setup the UDP socket parameters*/
  struct in_addr *addr;
  dagram_socket->sin_family = AF_INET;        /* host byte order */
  dagram_socket->sin_port = htons(port);  /* short, network byte order */

  /* If we are broadcasting */
  if (broadcast) {
    dagram_socket->sin_addr.s_addr = inet_addr ("192.168.1.255");

  /* Else packets direct to one host */
  } else {
    addr = atoaddr(client);
    dagram_socket->sin_addr.s_addr = addr->s_addr;
  }

  bzero(&(dagram_socket->sin_zero), 8);       /* zero the rest of the struct */

}


/* Create a socket to listen for a START command */
time_t wait_for_start(int daemon, multilog_t* log) {

  time_t utc_start_time;
  int bufsize = 64;
  char* buffer = (char*) malloc ( bufsize);
  char* utc_string = (char*) malloc ( bufsize);
  char* command = (char*) malloc ( bufsize);
  assert (buffer != 0);

  if (daemon) {

    FILE *sockin = 0;
    FILE *sockout = 0;

    /* Port for control from tcs simulator */
    int port = APSR_UDPGENERATOR_CTRL_PORT;

    /* create a socket on which to listen */
    int listen_fd = sock_create (&port);
    if (listen_fd < 0)  {
      multilog(log,LOG_ERR,"Error creating socket: %s\n", strerror(errno));
      return EXIT_FAILURE;
    }

    multilog(log,LOG_INFO,"waiting for command connection\n");

    /* wait for a connection */
    int tcpfd =  sock_accept (listen_fd);
    if (tcpfd < 0)  {
      multilog(log,LOG_ERR,"Error accepting connection: %s\n", strerror(errno));
      return EXIT_FAILURE;
    }

    sockin = fdopen(tcpfd,"r");
    sockout = fdopen(tcpfd,"w");
    setbuf (sockin, 0);
    setbuf (sockout, 0);

    char* rgot = NULL;
    rgot = fgets (buffer, bufsize, sockin);
    if (rgot && !feof(sockin)) {
      buffer[strlen(buffer)-1] = '\0';
    }

    /* check the command, should be of form "start YYYY-MM-DD-HH:MM:SS" */
    sscanf(buffer, "%s %s", command, utc_string);

    if (strstr(command ,"start") != NULL) {

      utc_start_time = str2utctime (utc_string);

      if (utc_start_time == (time_t)-1) {

        fprintf(sockout,  "Could not parse start time from '%s'\n", utc_string);
        fprintf(sockout,"fail\n");

      } else {

        time_t remainder = utc_start_time - time(0);
        fprintf(sockout,  "starting in %d seconds\n", (int) remainder);
        fprintf(sockout,"ok\n");

        sleep(remainder-1);
      }

    } else {
      fprintf(sockout,  "unrecognized command '%s'\n", buffer);
      fprintf(sockout,"fail\n");
    }

    close(tcpfd);
    close(listen_fd);
  } else {

    utc_start_time = time(0) + 1;
  }

  free(buffer);
  free(command);
  free(utc_string);

  return utc_start_time;

}

double rand_normal(double mean, double stddev) {

  static double n2 = 0.0;
  static int n2_cached = 0;
  
  if (!n2_cached) {
    // Choose a point x,y in the unit circle uniformaly at random
    double x, y, r;
    do {
      // scale two random integers to doubles between -1 and 1
      x = 2.0*rand()/RAND_MAX - 1;
      y = 2.0*rand()/RAND_MAX - 1;

      r = x*x + y*y;
    } while (r == 0.0 || r > 1.0);

    {
      // Apply Box-Muller transform on x,y
      double d = sqrt(-2.0*log(r)/r);
      double n1 = x*d;
      n2 = y*d;

      // Scale and translate to get desired mean and standard deviation
      double result = n1*stddev + mean;

      n2_cached = 1;
      return result;
    }
  } else {
     n2_cached = 0;
    return n2*stddev + mean;
  }
}


