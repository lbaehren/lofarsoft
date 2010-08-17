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
#include <math.h>
#include <pthread.h>
#include <assert.h>

//#include "ascii_header.h"
//#include "sock.h"
#include "daemon.h"
#include "multilog.h"
#include "caspsr_def.h"
#include "caspsr_udp.h"

#include "arch.h"
#include "Statistics.h"
#include "RealTime.h"
#include "StopWatch.h"

#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

#define UDPGEN_DEFAULT_PERIOD 10000
#define CASPSR_DEFAULT_BW 25
#define CASPSR_UDPGEN_LOG  49200

/* Cross thread global variables */
unsigned int scale0 = 4096;
unsigned int scale1 = 4096;
int bit_value = 0;
int regenerate_signal = 0;
int reselect_bits = 0;
int reset_sequence = 0;
int num_arrays = 1;

/* 32 bit values before bit selection */
unsigned int ** pol0_raw;
unsigned int ** pol1_raw;

/* Bit selected values 8 bit */
char ** pol0_bits;
char ** pol1_bits;

/* packet bits */
char ** packed_signals;

/* the current signal to send */
int s = 0;

/* Generation & encoding functions */
void create_memory_arrays(int num, int length);
void generate_test_data(int num, int length, int noise, int gain); 
void free_memory_arrays(int num);
void bit_select(int num, int length, int bit_value);
void pack_pols();

/* Debug / display functions */
void print_bram_line(FILE * fp, unsigned int i, unsigned int value, char * binary);
void signal_handler(int signalValue);
void usage();
void quit();
void tcpip_control(void);
unsigned reverse (unsigned value, unsigned N);
void reverse_array(char *array, int array_size);

int main(int argc, char *argv[])
{

  /* number of microseconds between packets */
  double sleep_time = 22;
 
  /* be verbose */ 
  int verbose = 0;

  /* bandwidth to simulate sending */ 
  unsigned int bandwidth = CASPSR_DEFAULT_BW;

  /* udp port to send data to */
  int udp_port = CASPSR_DEFAULT_UDPDB_PORT;

  /* UDP socket struct */
  struct sockaddr_in dagram;

  /* total time to transmit for */ 
  uint64_t transmission_time = 5;   

  /* be a daemon? */
  int daemon = 0;

  /* DADA logger */
  multilog_t *log = 0;

  /* Hostname to send UDP packets to */
  char *dest_host;

  /* udp file descriptor */
  int udpfd;

  /* The generated signal arrays */
  char packet[UDP_PAYLOAD];

  uint64_t signal_size = 32000;

  /* Frequency of the signal pattern */
  int signal_freq = 1000; // Hz

  /* Length in bytes of the signal pattern*/
  int signal_length = 0;

  /* produce noise */
  int produce_noise = 0;

  /* broadcast udp packets ? */
  int broadcast = 0;

  int calfreq = 1000;

  /* data rate */
  double data_rate = 0;

  /* number of packets to send every second */
  uint64_t packets_ps = 0;

  /* start of transmission */
  time_t start_time;

  /* end of transmission */
  time_t end_time;

  /* enable gain control */
  int allow_gain_control = 0;

  /* Always start the sequence no nice and high */
  uint64_t seq_no = 0;
  //caspsr_header_t * header;

  opterr = 0;
  int c;
  while ((c = getopt(argc, argv, "bdjf:n:p:vw:y")) != EOF) {
    switch(c) {

      case 'b':
        broadcast = 1;
        break;

      case 'd':
        daemon = 1;
        break;
        
      case 'j':
        produce_noise = 1;
        num_arrays = 10;
        break;

      case 'f':
        signal_freq = atoi(optarg);
        break;

      case 'n':
        transmission_time = atoi(optarg);
        break;

      case 'p':
        udp_port = atoi(optarg);
        break;

      case 'v':
        verbose = 1;
        break;

      case 'w':
        bandwidth = atoi(optarg);
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
    fprintf(stderr,"no dest_host was specified\n");
    usage();
    return EXIT_FAILURE;
  }

  /* destination host */
  dest_host = (char *) argv[optind];

  /* initialise header struct */
  seq_no = 1000000;
  //header = init_caspsr_header();
  //header->seq_no = 1000000;

  signal(SIGINT, signal_handler);

  log = multilog_open ("caspsr_udpgen", daemon);

  if (daemon) {
    be_a_daemon();
  } else {
    multilog_add(log, stderr);
  }
  multilog_serve (log, CASPSR_UDPGEN_LOG);

  /* Determine data rate (25MHz * 2 pol * 2 dim) */
  multilog(log, LOG_INFO, "bandwidth = %d MHz\n", bandwidth);
  data_rate = bandwidth * 2 * 2 * 1000000;
  multilog(log, LOG_INFO, "data rate = %5.2f MB/s\n",data_rate/(1024*1024));

  /* length of a 'signal' */
  signal_length = floor(data_rate / (double) signal_freq);
  multilog(log, LOG_INFO, "signal_length  = %d\n",signal_length);

  /* create memory arrays based */
  create_memory_arrays(num_arrays, signal_length);

  multilog(log, LOG_INFO, "sending to %s:%d\n", dest_host, udp_port);

  /* create the socket for outgoing UDP data */
  dada_udp_sock_out(&udpfd, &dagram, dest_host, udp_port, broadcast, "192.168.1.255");

  uint64_t data_counter = 0;

  /* initialise data rate timing library */
  StopWatch wait_sw;
  RealTime_Initialise(1);
  StopWatch_Initialise(1);

  /*
  if (data_rate > 117000000) {
    multilog(log, LOG_ERR, "data rate [%f bps] too high 1GbE:\n",data_rate);
    close(udpfd);
    exit(1);
  }
  */

  /* If we have a desired data rate, then we need to adjust our sleep time
   * accordingly */
  if (data_rate > 0) {
    packets_ps = floor(((double) data_rate) / ((double) UDP_PAYLOAD));
    sleep_time = (1.0/packets_ps)*1000000.0;
  }
  multilog(log,LOG_INFO,"Packets/sec = %"PRIu64"\n",packets_ps);
  multilog(log,LOG_INFO,"sleep_time = %f\n",sleep_time);

  /* Seed the random number generator */
  srand ( time(NULL) );

  /* Simulate the iBoB Pipeline */ 
  generate_test_data(num_arrays, signal_length, produce_noise, 500);
  bit_select(num_arrays, signal_length, bit_value);
  pack_pols(num_arrays, signal_length);

  uint64_t total_bytes_to_send = data_rate * transmission_time;

  size_t bytes_sent = 0;
  uint64_t total_bytes_sent = 0;

  uint64_t bytes_sent_thistime = 0;
  uint64_t prev_bytes_sent = 0;
  
  time_t current_time = time(0);
  time_t prev_time = time(0);

  multilog(log,LOG_INFO,"Total bytes to send = %"PRIu64"\n",total_bytes_to_send);
  multilog(log,LOG_INFO,"UDP payload = %"PRIu64" bytes\n",UDP_PAYLOAD);
  multilog(log,LOG_INFO,"UDP data size = %"PRIu64" bytes\n",UDP_DATA);
  multilog(log,LOG_INFO,"Wire Rate\t\tUseful Rate\tPacket\tSleep Time\n");

  /* Start the external control thread */
  //pthread_t gain_thread;
  //if (pthread_create (&gain_thread, 0, (void *) tcpip_control, 0) < 0) {
  //  perror ("nexus_connect: Error creating new thread");
  //  return -1;
  //}
  //pthread_detach(gain_thread);
  
  unsigned int s_off = 0;

  while (total_bytes_sent < total_bytes_to_send) {
         
    if (daemon)  {
      while (current_time >= start_time) {
        current_time = time(0);
      }
      daemon=0;
    }

    StopWatch_Start(&wait_sw);

    /* If more than one option, choose the array to use */
    if (num_arrays > 1) {
      s = (int) (num_arrays * (rand() / (RAND_MAX + 1.0)));
    }

    /* If the control thread has asked us to reset the sequence number */
    if (reset_sequence) {
      seq_no = 0;
      reset_sequence = 0;
    }

    /* write the custom header into the packet */
    caspsr_encode_header(packet, seq_no, 0);

    /* write the data from the packed signal into the packet, update offset */
    s_off = caspsr_encode_data(packet+UDP_HEADER, packed_signals[s],
                             UDP_DATA, signal_length, s_off);

    if (regenerate_signal) {
      fprintf(stderr, "regenerating signal\n");
      generate_test_data(num_arrays, signal_length, produce_noise, 500);
      bit_select(num_arrays, signal_length, bit_value);
      pack_pols(num_arrays, signal_length);
      fprintf(stderr, "done\n");
      regenerate_signal = 0;
    }

    if (reselect_bits) {
      fprintf(stderr, "reselecting bits\n");
      bit_select(num_arrays, signal_length, bit_value);
      pack_pols(num_arrays, signal_length);
      fprintf(stderr, "done\n");
      reselect_bits = 0;
    }

    bytes_sent = dada_sock_send(udpfd, dagram, packet, (size_t) UDP_PAYLOAD); 

    if (bytes_sent != UDP_PAYLOAD) 
      multilog(log,LOG_ERR,"Error. Attempted to send %d bytes, but only "
                           "%"PRIu64" bytes were sent\n",UDP_PAYLOAD,
                           bytes_sent);

    /* This is how much useful data we actaully sent */
    total_bytes_sent += (bytes_sent - UDP_HEADER);

    data_counter++;
    prev_time = current_time;
    current_time = time(0);
    
    if (prev_time != current_time) {

      double complete_udp_packet = (double) bytes_sent;
      double useful_data_only = (double) (bytes_sent - UDP_HEADER);
      double complete_packet = 28.0 + complete_udp_packet;

      double wire_ratio = complete_packet / complete_udp_packet;
      double useful_ratio = useful_data_only / complete_udp_packet;
        
      uint64_t bytes_per_second = total_bytes_sent - prev_bytes_sent;
      prev_bytes_sent = total_bytes_sent;
      double rate = ((double) bytes_per_second) / (1024*1024);

      double wire_rate = rate * wire_ratio;
      double useful_rate = rate * useful_ratio;
             
      multilog(log,LOG_INFO,"%"PRIu64": %5.2f MB/s  %5.2f MB/s  %"PRIu64
                            "  %5.2f, %"PRIu64"\n", seq_no,
                            wire_rate, useful_rate, data_counter, sleep_time,
                            bytes_sent);
    }

    seq_no++;

    StopWatch_Delay(&wait_sw, sleep_time);

  }

  uint64_t packets_sent = seq_no;

  multilog(log, LOG_INFO, "Sent %"PRIu64" bytes\n",total_bytes_sent);
  multilog(log, LOG_INFO, "Sent %"PRIu64" packets\n",packets_sent);

  close(udpfd);

  return 0;
}


void signal_handler(int signalValue) {

  exit(EXIT_SUCCESS);

}

void usage() {
  fprintf(stdout,
    "caspsr_udpgen [options] dest_host\n"
    "\t-b            broadcast packets to 192.168.1.255\n"
    "\t-j            produce random noise [default off]\n"
    "\t-t bytes      period of the signal [default %d]\n"
    "\t-p n          udp port to send packets to [default %d]\n"
    "\t-n n          number of seconds to transmit [default 5]\n"
    "\t-v            verbose output\n"
    "\t-w freq       bandwidth in MHz to simulate [default %d]\n"
    "\t-d            daemon mode. expects TCP/IP control\n"
    "\t-y            allow gain control\n"
    "\n\tdest_host     hostname of a machine running dada_udpdb\n\n"
    ,UDPGEN_DEFAULT_PERIOD, CASPSR_DEFAULT_UDPDB_PORT, CASPSR_DEFAULT_BW);
}


/*
 *  Thread to provide TCPIP control 
 */
void tcpip_control(void) {

  FILE *sockin = 0;
  FILE *sockout = 0;

  /* Port for control from tcs simulator */
  int port = 57003;

  int listen_fd;
  int fd;
  char *rgot;

  int bufsize = 4096;
  char* buffer = (char*) malloc ( bufsize);
  assert (buffer != 0);
  char * binary_string = (char*) malloc(sizeof(char) * 33);
  binary_string[32] = '\0';

  int quit = 0;
  int trying_to_reset = 0;
  int i=0;
  int have_connection = 0;

  /* create a socket on which to listen */
  listen_fd = sock_create (&port);
  if (listen_fd < 0)  {
    fprintf(stderr,"Error creating socket: %s\n", strerror(errno));
    quit = 1;
  }
  //fprintf(stderr, "socket created\n");

  while (!quit) {  
  
    if (!have_connection) {

      /* wait for a connection */
      fd =  sock_accept (listen_fd);
      if (fd < 0)  {
        perror("Error accepting connection");
        quit = 1;
      }
      fprintf(stderr, "socket accepted a connection\n");

      sockin = fdopen(fd,"r");
      if (!sockin) 
        perror("Error creating input stream");

      sockout = fdopen(fd,"w");
      if (!sockout) 
        perror("Error creating output stream");

      //fprintf(stderr, "file descriptors opened\n");

      setbuf (sockin, 0);
      setbuf (sockout, 0);

      have_connection = 1;
    }


    rgot = fgets (buffer, bufsize, sockin);

    if (rgot && !feof(sockin)) {

      buffer[strlen(buffer)-1] = '\0';
      //fprintf(stderr, "buffer = %s\n",buffer);

      int gain = 0;

      /* Print the values for polarisation 0 */
      if (strstr(buffer,"bramdump scope_output1/bram") != NULL) {
        for (i=0; i<1024; i += 2) {
          //convert_to_bits(pol0_accs[current_array][i], binary_string);
          //print_bram_line(sockout, i, pol0_accs[current_array][i], binary_string);
        }
      }

      /* Print the values for polarisation 1 */
      if (strstr(buffer,"bramdump scope_output3/bram") != NULL) {
        for (i=0; i<1024; i+=2) {
          //convert_to_bits(pol1_accs[current_array][i], binary_string);
          //print_bram_line(sockout, i, pol1_accs[current_array][i], binary_string);
        }
      }

      /* Pol 0 gain set request */
      if (strstr(buffer,"regwrite reg_coeff_pol1 ") != NULL) {

        if ( sscanf( buffer, "regwrite reg_coeff_pol1 %d", &scale0) != 1 )
          fprintf(stderr, "Could not extract scale from the command\n");
        else {
          //fprintf(stderr,"scale0 = %d\n",scale0);
          regenerate_signal = 1;
        }
      }

      /* Pol 1 gain set request */
      if (strstr(buffer,"regwrite reg_coeff_pol2 ") != NULL) {
                                                                                                         
        if ( sscanf( buffer, "regwrite reg_coeff_pol2 %d", &scale1) != 1 )
          fprintf(stderr, "Could not extract scale from the command\n");
        else {
          //fprintf(stderr,"scale1 = %d\n",scale1);
          regenerate_signal = 1;
        }
      }

      /* Select the bits in question */
      if (strstr(buffer,"regwrite reg_output_bitselect ") != NULL) {

        if ( sscanf( buffer, "regwrite reg_output_bitselect %d", &bit_value) != 1 )
          fprintf(stderr, "Could not extract bit selection value\n");
        else {
          //fprintf(stderr, "bit value changed\n");
          reselect_bits = 1;
        }
      }

      if (strstr(buffer,"regwrite reg_arm 1") != NULL) {
        reset_sequence = 1;
        trying_to_reset = 0;
      }

      if (strstr(buffer,"regwrite reg_arm 0") != NULL) {
        trying_to_reset = 1;
      } else {
        trying_to_reset = 0;
      }
        
    } else {
      
      fprintf(stderr, "lost connection\n");
      have_connection = 0 ;

    }
  }

  close(fd);
  close(listen_fd);


}

/* 
 * Generate the 32 bit value for each polarisation
 */
void generate_test_data(int num, int length, int noise, int gain) {

  int i=0;
  for (i=0; i<num; i++) {

    if (noise) {
      fill_gaussian_unsigned(pol0_raw[i], length, 32, gain);
      fill_gaussian_unsigned(pol1_raw[i], length, 32, gain);
    } else {
      fill_cal_unsigned(pol0_raw[i], length, 32, gain);
      fill_cal_unsigned(pol1_raw[i], length, 32, gain);
    }
  }
}

/*
 * Selects the relevant 8 bit segment from the 32 bit
 * arrays. bit_value can be 0,1,2,3 which corresponds
 * to bits 0-7, 8-15, 16-23, 24-31 respectively
 */
void bit_select(int num, int length, int bit_value) {

  unsigned int shifted;
  char val;
  int i=0;
  int h=0;

  for (h=0; h < num; h++) {
    for (i=0; i < length; i++) {

      shifted = pol0_raw[h][i];
      shifted >>= (8*bit_value);
      shifted &= 0x000000ff;
      pol0_bits[h][i] = (char) shifted;;


      shifted = pol1_raw[h][i];
      shifted >>= (8*bit_value);
      shifted &= 0x000000ff;
      pol1_bits[h][i] = (char) shifted;;

    }
  }
}

/*
 * Packs the 2 polarisations into the char 
 * buffer ready for udp despatch
 */
void pack_pols(int num, int length) {

  int i=0; 
  int h=0;

  for (h=0; h < num; h++) {
    for (i=0; i < length/2; i++) {
      packed_signals[h][4*i]     = pol0_bits[h][2*i];
      packed_signals[h][(4*i)+1] = pol0_bits[h][(2*i)+1];
      packed_signals[h][(4*i)+2] = pol1_bits[h][2*i];
      packed_signals[h][(4*i)+3] = pol1_bits[h][(2*i)+1];
    }
  }

}


/*
 * Prints a line to FP in the same format as the ibob's bramdump command 
 */
void print_bram_line(FILE *fp, unsigned int i, unsigned int value, char * binary) {

  fprintf(fp, "0x%04X / %05d -> 0x%08X / 0b%s / %010u\r\n",i,i,i,binary,value);

}

/* 
 * Allocate the memory for the test data 
 */
void create_memory_arrays(int num, int length) {

  /* create memory arrays */
  pol0_raw = malloc(sizeof(unsigned int *) * num);
  pol1_raw = malloc(sizeof(unsigned int *) * num);
  pol0_bits = malloc(sizeof(char *) * num);
  pol1_bits = malloc(sizeof(char *) * num);
  packed_signals = malloc(sizeof(char *) * num);

  int i=0;
  for (i=0; i<num; i++) {

    pol0_raw[i] = (unsigned int *) malloc(sizeof(unsigned int) * length);
    pol1_raw[i] = (unsigned int *) malloc(sizeof(unsigned int) * length);
    pol0_bits[i] = (char *) malloc(sizeof(char *) * length);
    pol1_bits[i] = (char *) malloc(sizeof(char *) * length);
    packed_signals[i] = (char *) malloc(sizeof(char *) * length * 2);
  }

}

void free_memory_arrays(int num) {

  int i=0;

  for (i=0; i<num; i++) {
    free(pol0_raw[i]);
    free(pol1_raw[i]);
    free(pol0_bits[i]);
    free(pol1_bits[i]);
    free(packed_signals[i]);
  }

  free(pol0_raw);
  free(pol1_raw);
  free(pol0_bits);
  free(pol1_bits);
  free(packed_signals);
}
