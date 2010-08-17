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

#include "ascii_header.h"
#include "sock.h"
#include "daemon.h"
#include "multilog.h"
#include "bpsr_def.h"
#include "bpsr_udpdb.h"
#include "bpsr_udp.h"

#include "arch.h"
#include "Statistics.h"
#include "RealTime.h"
#include "StopWatch.h"

#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

/* DFB channels - only 8 bit */
unsigned int ** pol0_dfbs;
unsigned int ** pol1_dfbs;

/* Scaled DFB channels 18 bits */
unsigned int ** pol0_scaleds;
unsigned int ** pol1_scaleds;

/* Square law detected 24 */
unsigned int ** pol0_slds;
unsigned int ** pol1_slds;

/* Accumulated values 32 bit */
unsigned int ** pol0_accs;
unsigned int ** pol1_accs;

/* Bit selected values 8 bit */
char ** pol0_bits;
char ** pol1_bits;

/* packet bits */
char ** packed_signals;

/* UDP DATA ARRAY */
char ** udp_datas;
int current_array = 0;

void create_memory_arrays(int num_arrays);


/* Generation & encoding functions */
void generate_dfb_vals(int noise);
void scale_dfb_vals(unsigned int scale0, unsigned int scale1);
void sql_vals(void);
void accumulate_vals(unsigned int acc_len);
void bit_select(int bit_value);
void pack_pols();
void build_udp_arrays(uint64_t seq_no);

/* Debug / display functions */
void print_pol(unsigned int * pol);
void print_pol_chars(char * pol);
void displayBits(unsigned int value);
void displayCharBits(char value);
void convert_to_bits(unsigned int value, char * binary_string);
void print_bram_line(FILE * fp, unsigned int i, unsigned int value, char * binary);

int sendPacket(int sockfd, struct sockaddr_in addr, char *data, int size);
//struct in_addr *atoaddr(char *address);
void signal_handler(int signalValue);
void usage();
void quit();
char two_bit_convert(int value);
char four_bit_convert(int value);
void tcpip_control(void);
unsigned reverse (unsigned value, unsigned N);
void reverse_array(char *array, int array_size);
void create_udpout_socket(int *fd, struct sockaddr_in * dagram_socket,
                       char *client, int port, int broadcast);

/* UDP socket */
struct sockaddr_in dagram_socket;

unsigned int scale0 = 4096;
unsigned int scale1 = 4096;
int bit_value = 0;
int regenerate_signal = 0;
int reselect_bits = 0;
int reset_sequence = 0;
int num_arrays = 1;

int main(int argc, char *argv[])
{

  /* number of microseconds between packets */
  double sleep_time = 22;
 
  /* Size of UDP payload. Optimally 1472 (1500 MTU) or 8948 (9000 MTU) bytes */
  uint64_t size_of_frame = BPSR_UDP_PAYLOAD_BYTES;

  /* Accumulation length */
  int acc_len = BPSR_DEFAULT_ACC_LEN;

  /* be verbose? */ 
  int verbose = 0;

  /* udp port to send data to */
  int udp_port = BPSR_DEFAULT_UDPDB_PORT;

  /* total time to transmit for */ 
  uint64_t transmission_time = 5;   

  /* be a daemon? */
  int daemon = 0;

  /* DADA logger */
  multilog_t *log = 0;

  /* Hostname to send UDP packets to */
  char *udpdb_client;

  /* udp file descriptor */
  int udpfd;

  /* The generated signal arrays */
  char * pol0_signal;
  char * pol1_signal;

  uint64_t signal_size = 32000;

  /* CALFREQ / TSAMP */
  int signal_length = 64000;

  /* number of bytes */
  uint64_t udp_data_size = BPSR_UDP_PAYLOAD_BYTES;

  /* produce noise */
  int produce_noise = 0;

  /* broadcast udp packets ? */
  int broadcast = 0;

  int calfreq = 1000;

  double tsamp = 0.015625;
  double spectra_per_usec = 0;
  double data_rate= 0;

  /* start of transmission */
  time_t start_time;

  /* end of transmission */
  time_t end_time;

  /* enable gain control */
  int allow_gain_control = 0;

  opterr = 0;
  int c;
  while ((c = getopt(argc, argv, "a:bdjn:p:vy")) != EOF) {
    switch(c) {

      case 'a':
        acc_len = atoi(optarg);
        break;

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

      case 'n':
        transmission_time = atoi(optarg);
        break;

      case 'p':
        udp_port = atoi(optarg);
        break;

      case 'v':
        verbose = 1;
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

  log = multilog_open ("bpsr_udpgenerator", daemon);

  if (daemon) {
    be_a_daemon();
  } else {
    multilog_add(log, stderr);
  }

  create_memory_arrays(num_arrays);

  multilog_serve (log, BPSR_TEST_TRIWAVEUDP_LOG);

  udpdb_client = (char *) argv[optind];

  signal(SIGINT, signal_handler);

  /* create the socket for outgoing UDP data */
  create_udpout_socket(&udpfd, &dagram_socket, udpdb_client, udp_port, broadcast);

  if (verbose) 
    multilog(log,LOG_INFO,"udp socket setup\n");

  //char udp_data[BPSR_UDP_PAYLOAD_BYTES];
  uint64_t data_counter = 0;

  /* initialise data rate timing library */
  StopWatch wait_sw;
  RealTime_Initialise(1);
  StopWatch_Initialise(1);

  /* Determine the tsamp for given acc_len */
  spectra_per_usec= ((double) BPSR_IBOB_CLOCK) / (double) (BPSR_IBOB_NCHANNELS * acc_len);
  tsamp = 1 / spectra_per_usec;
  data_rate = spectra_per_usec * 1000000 * BPSR_UDP_DATASIZE_BYTES;

  if (verbose) {
    multilog(log,LOG_INFO,"Tsamp = %10.8f, data_rate = %f\n",tsamp, data_rate);
  }

  if (data_rate > 120000000) {
    multilog(log, LOG_ERR, "Data rate too hight for a 1Gbit connection: %f bytes per second\n",data_rate);
    close(udpfd);
    exit(1);
  }

  /* If we have a desired data rate, then we need to adjust our sleep time
   * accordingly */
  if (data_rate > 0) {
    double packets_per_second = ((double) data_rate) / ((double)udp_data_size);
    sleep_time = (1.0/packets_per_second)*1000000.0;
  }

  /* Generate signals */
  srand ( time(NULL) );
  
  generate_dfb_vals(produce_noise);
  scale_dfb_vals(scale0, scale1);
  sql_vals();
  accumulate_vals(acc_len);
  bit_select(bit_value);
  pack_pols();

  /* Always start the sequence no nice and high */
  uint64_t sequence_incr = 512 * (uint64_t) acc_len;
  uint64_t sequence_no = 100000000 * sequence_incr;

  build_udp_arrays(sequence_no);
  /* Encode the 2 pols into the udp_data char array */
  // encode_header(udp_data, sequence_no);
  // memcpy(udp_data+BPSR_UDP_COUNTER_BYTES, packed_signals[0], (size_t) BPSR_UDP_DATASIZE_BYTES);

  /* If we are running as a daemon, then we must wait for a "start"
   * message */
  uint64_t total_bytes_to_send = data_rate * transmission_time;

  uint64_t bytes_sent = 0;
  uint64_t total_bytes_sent = 0;

  uint64_t bytes_sent_thistime = 0;
  uint64_t prev_bytes_sent = 0;
  
  time_t current_time = time(0);
  time_t prev_time = time(0);

  multilog(log,LOG_INFO,"Total bytes to send = %"PRIu64"\n",total_bytes_to_send);
  multilog(log,LOG_INFO,"Packet size = %"PRIu64" bytes\n",BPSR_UDP_PAYLOAD_BYTES);
  multilog(log,LOG_INFO,"Data size = %"PRIu64" bytes\n",BPSR_UDP_DATASIZE_BYTES);
  multilog(log,LOG_INFO,"Wire Rate\t\tUseful Rate\tPacket\tSleep Time\n");

  /* Start the external control thread */
  pthread_t gain_thread;

  if (pthread_create (&gain_thread, 0, (void *) tcpip_control, 0) < 0) {
    perror ("nexus_connect: Error creating new thread");
    return -1;
  }
                                                                                                        
  pthread_detach(gain_thread);

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
      current_array = (int) (num_arrays * (rand() / (RAND_MAX + 1.0)));
    }

    /* If the control thread has asked us to reset the sequence number */
    if (reset_sequence) {
      sequence_no = 0;
      reset_sequence = 0;
    }

    /* put the sequence no in the first 8 bytes */
    encode_header(udp_datas[current_array], sequence_no);

    if (regenerate_signal) {
      //fprintf(stderr, "regenerating signal\n");
      generate_dfb_vals(produce_noise);
      scale_dfb_vals(scale0, scale1); // Can be updated by the gain thread 
      sql_vals();
      accumulate_vals(acc_len);
      bit_select(bit_value);
      pack_pols();
      build_udp_arrays(sequence_no);
      //fprintf(stderr, "done\n");
      regenerate_signal = 0;
    }

    if (reselect_bits) {
      //fprintf(stderr, "reselecting bits\n");
      bit_select(bit_value);
      pack_pols();
      build_udp_arrays(sequence_no);
      //fprintf(stderr, "done\n");
      reselect_bits = 0;
    }

    bytes_sent = sendPacket(udpfd, dagram_socket, udp_datas[current_array], BPSR_UDP_PAYLOAD_BYTES); 

    if (bytes_sent != BPSR_UDP_PAYLOAD_BYTES) 
      multilog(log,LOG_ERR,"Error. Attempted to send %d bytes, but only "
                           "%"PRIu64" bytes were sent\n",BPSR_UDP_PAYLOAD_BYTES,
                           bytes_sent);

    /* This is how much useful data we actaully sent */
    total_bytes_sent += (bytes_sent - BPSR_UDP_COUNTER_BYTES);

    data_counter++;
    prev_time = current_time;
    current_time = time(0);
    
    if (prev_time != current_time) {

      double complete_udp_packet = (double) bytes_sent;
      double useful_data_only = (double) (bytes_sent - BPSR_UDP_COUNTER_BYTES);
      double complete_packet = 28.0 + complete_udp_packet;

      double wire_ratio = complete_packet / complete_udp_packet;
      double useful_ratio = useful_data_only / complete_udp_packet;
        
      uint64_t bytes_per_second = total_bytes_sent - prev_bytes_sent;
      prev_bytes_sent = total_bytes_sent;
      double rate = ((double) bytes_per_second) / (1024*1024);

      double wire_rate = rate * wire_ratio;
      double useful_rate = rate * useful_ratio;
             
      multilog(log,LOG_INFO,"%"PRIu64": %5.2f MB/s  %5.2f MB/s  %"PRIu64"  %5.2f, %"PRIu64"\n",
                            (sequence_no/sequence_incr), wire_rate, useful_rate,data_counter,sleep_time,bytes_sent);
    }

    sequence_no += sequence_incr;

    StopWatch_Delay(&wait_sw, sleep_time);

  }

  uint64_t packets_sent = sequence_no / sequence_incr;;;;

  multilog(log, LOG_INFO, "Sent %"PRIu64" bytes\n",total_bytes_sent);
  multilog(log, LOG_INFO, "Sent %"PRIu64" packets\n",packets_sent);

  close(udpfd);
  return 0;
}


void signal_handler(int signalValue) {

  exit(EXIT_SUCCESS);

}

int sendPacket(int sockfd, struct sockaddr_in their_addr, char *udp_data, int size_of_frame) {

  int numbytes;

  if ((numbytes=sendto(sockfd, udp_data, size_of_frame, 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr))) < 0) {
    perror("sendto");
    exit(1);
  }
  
  return(numbytes);
}

void usage() {
  fprintf(stdout,
    "bpsr_udpgenerator [options] dest_host\n"
    "\t-a acc_len    accumulation length [default %d]\n"
    "\t-b            broadcast packets to 192.168.1.255\n"
    "\t-j            produce random noise [default off]\n"
    "\t-p n          udp port to send packets to [default %d]\n"
    "\t-n n          number of seconds to transmit [default 5]\n"
    "\t-v            verbose output\n"
    "\t-d            daemon mode. expects TCP/IP control\n"
    "\t-y            allow gain control\n"
    "\n\tdest_host     hostname of a machine running dada_udpdb\n\n"
    ,BPSR_DEFAULT_ACC_LEN, BPSR_DEFAULT_UDPDB_PORT);
}

/*
struct in_addr *atoaddr(char *address) {
  struct hostent *host;
  static struct in_addr saddr;

  // First try it as aaa.bbb.ccc.ddd. 
  saddr.s_addr = inet_addr(address);
  if ((int) saddr.s_addr != -1) {
    return &saddr;
  }
  host = gethostbyname(address);
  if (host != NULL) {
    return (struct in_addr *) *host->h_addr_list;
  }
  return NULL;
}
*/


/*
 * Debug & Display functions
 */
void print_pol(unsigned int * pol) {

  int i = 0;
  unsigned int val = 0;

  fprintf(stderr,"["); 

  for (i=0; i < BPSR_IBOB_NCHANNELS; i++) {

    fprintf(stderr,"%u,",pol[i]);

    //val = (unsigned int) pol[i];
    //val &= 0x000000ff;
    //fprintf(stderr,"%u.", val);

  }

  fprintf(stderr,"]\n");

}

void print_pol_chars(char * pol) {

  int i = 0;
  unsigned int val = 0;

  fprintf(stderr,"[");

  for (i=0; i < BPSR_IBOB_NCHANNELS; i++) {

    val = (unsigned int) pol[i];
    val &= 0x000000ff;
    fprintf(stderr,"%u.", val);

  }
 
  fprintf(stderr,"]\n");
  
}


void convert_to_bits(unsigned int value, char * binary_string) {

  unsigned int c, displayMask = 1 << 31;
  for (c=1; c<= 32; c++) {
    binary_string[(c-1)] = value & displayMask ? '1' : '0';
    value <<= 1;
  }
}


void displayBits(unsigned int value) {
  unsigned int c, displayMask = 1 << 31;

  printf("%7u = ", value);

  for (c=1; c<= 32; c++) {
    putchar(value & displayMask ? '1' : '0');
    value <<= 1;

    if (c% 8 == 0)
      putchar(' ');
  }    
  putchar('\n');
}


void displayCharBits(char value) {
  char c, displayMask = 1 << 7;
                                                                                
  //printf("    %c = ", value);
                                                                                
  for (c=1; c<= 8; c++) {
    putchar(value & displayMask ? '1' : '0');
    //if (c%2 == 0) printf(" ");
    value <<= 1;
                                                                                
  }
  //putchar('\n');
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
          convert_to_bits(pol0_accs[current_array][i], binary_string);
          print_bram_line(sockout, i, pol0_accs[current_array][i], binary_string);
        }
      }

      /* Print the values for polarisation 1 */
      if (strstr(buffer,"bramdump scope_output3/bram") != NULL) {
        for (i=0; i<1024; i+=2) {
          convert_to_bits(pol1_accs[current_array][i], binary_string);
          print_bram_line(sockout, i, pol1_accs[current_array][i], binary_string);
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
    fprintf(stderr, "Setting broadcast option\n");
    if (setsockopt(*fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof yes) < 0) {
      perror("Could not set socket option");
      exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Broadcast option set\n");
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
    addr = atoaddr (client);
    dagram_socket->sin_addr.s_addr = addr->s_addr;
  }

  bzero(&(dagram_socket->sin_zero), 8);       /* zero the rest of the struct */

}

/*
 * Generates 8 bit values for the DFB stage
 */
void generate_dfb_vals(int noise) {

  float max_value = 16384;
  int i,h;

  for (h=0; h < num_arrays; h++) {
  
    for (i=0; i < BPSR_IBOB_NCHANNELS; i++) {
    
      if (noise) {
        pol0_dfbs[h][i] =  1 + (int) (max_value * (rand() / (RAND_MAX + 1.0))); 
        pol1_dfbs[h][i] =  1 + (int) (max_value * (rand() / (RAND_MAX + 1.0))); 
      } else {
        float pos = ((float) i) / ((float) BPSR_IBOB_NCHANNELS);
        float y = 2 * (0.5 - fabs(pos - 0.5));
        pol0_dfbs[h][i] = (unsigned int) (max_value * (y+0.5/max_value));
        pol1_dfbs[h][i] = (max_value - pol0_dfbs[h][i]);
      }
    }
  }
}

/*
 * Generates pol?_scaled arrays from pol?_dfb arrays, multipling
 * by the scale factors for each polarisation
 */
void scale_dfb_vals(unsigned int scale0, unsigned int scale1) {

  int i=0;
  int h=0;
  for (h=0; h < num_arrays; h++) {
    for (i=0; i < BPSR_IBOB_NCHANNELS; i++) {
      pol0_scaleds[h][i] = (int) ( ((float) scale0 / 4096.0) * pol0_dfbs[h][i]);
      pol1_scaleds[h][i] = (int) ( ((float) scale1 / 4096.0) * pol1_dfbs[h][i]);
    }
  }
}

/*
 * Generates sld arrays. simply squares the scaled arrays.
 */
void sql_vals(void) {

  int i=0;
  int h=0;

  for (h=0; h < num_arrays; h++) {
    for (i=0; i < BPSR_IBOB_NCHANNELS; i++) {
      pol0_slds[h][i] = pol0_scaleds[h][i] * pol0_scaleds[h][i];
      pol1_slds[h][i] = pol1_scaleds[h][i] * pol1_scaleds[h][i];
    }
  }
}

/*
 * Accumulates the values by the specified acc_len
 * simply multiply the values by the acclen
 */
void accumulate_vals(unsigned int acc_len) {

  int i=0;
  int h=0;
                                                                                                         
  for (h=0; h < num_arrays; h++) {
    for (i=0; i < BPSR_IBOB_NCHANNELS; i++) {
      pol0_accs[h][i] = acc_len * pol0_slds[h][i];
      pol1_accs[h][i] = acc_len * pol1_slds[h][i];
    }
  }

}

/*
 * Selects the relevant 8 bit segment from the 32 bit
 * arrays. bit_value can be 0,1,2,3 which corresponds
 * to bits 0-7, 8-15, 16-23, 24-31 respectively
 */
void bit_select(int bit_value) {

  unsigned int shifted;
  char val;
  int i=0;
  int h=0;

  for (h=0; h < num_arrays; h++) {
    for (i=0; i < BPSR_IBOB_NCHANNELS; i++) {

      shifted = pol0_accs[h][i];
      shifted >>= (8*bit_value);
      shifted &= 0x000000ff;
      pol0_bits[h][i] = (char) shifted;;


      shifted = pol1_accs[h][i];
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
void pack_pols() {

  int i=0; 
  int h=0;
                                                                                                         
  for (h=0; h < num_arrays; h++) {
    for (i=0; i < (BPSR_IBOB_NCHANNELS/2); i++) {
      packed_signals[h][4*i]     = pol0_bits[h][2*i];
      packed_signals[h][(4*i)+1] = pol0_bits[h][(2*i)+1];
      packed_signals[h][(4*i)+2] = pol1_bits[h][2*i];
      packed_signals[h][(4*i)+3] = pol1_bits[h][(2*i)+1];
    }
  }

}

/* 
 * Pack the UDP arrays
 */
void build_udp_arrays(uint64_t seq_no) {

  int h=0;

  for (h=0; h < num_arrays; h++) {
    /* Encode the 2 pols into the udp_data char array */
    encode_header((udp_datas[h]), seq_no);
    memcpy((udp_datas[h])+BPSR_UDP_COUNTER_BYTES, packed_signals[h], (size_t) BPSR_UDP_DATASIZE_BYTES);
  }

}

/*
 * Prints a line to FP in the same format as the ibob's bramdump command 
 */
void print_bram_line(FILE *fp, unsigned int i, unsigned int value, char * binary) {

  fprintf(fp, "0x%04X / %05d -> 0x%08X / 0b%s / %010u\r\n",i,i,i,binary,value);

}

void create_memory_arrays(int num_arrays) {

  /* create memory arrays */
  pol0_dfbs = malloc(sizeof(unsigned int *) * num_arrays);
  pol1_dfbs = malloc(sizeof(unsigned int *) * num_arrays);
  pol0_scaleds = malloc(sizeof(unsigned int *) * num_arrays);
  pol1_scaleds = malloc(sizeof(unsigned int *) * num_arrays);
  pol0_slds = malloc(sizeof(unsigned int *) * num_arrays);
  pol1_slds = malloc(sizeof(unsigned int *) * num_arrays);
  pol0_accs = malloc(sizeof(unsigned int *) * num_arrays);
  pol1_accs = malloc(sizeof(unsigned int *) * num_arrays);
  pol0_bits = malloc(sizeof(char *) * num_arrays);
  pol1_bits = malloc(sizeof(char *) * num_arrays);
  packed_signals = malloc(sizeof(char *) * num_arrays);
  udp_datas = malloc(sizeof(char *) * num_arrays);

  int i=0;
  for (i=0; i<num_arrays; i++) {

    pol0_dfbs[i] = (unsigned int *) malloc(sizeof(unsigned int) * BPSR_IBOB_NCHANNELS);
    pol1_dfbs[i] = (unsigned int *) malloc(sizeof(unsigned int) * BPSR_IBOB_NCHANNELS);
    pol0_scaleds[i] = (unsigned int *) malloc(sizeof(unsigned int) * BPSR_IBOB_NCHANNELS);
    pol1_scaleds[i] = (unsigned int *) malloc(sizeof(unsigned int) * BPSR_IBOB_NCHANNELS);
    pol0_slds[i] = (unsigned int *) malloc(sizeof(unsigned int) * BPSR_IBOB_NCHANNELS);
    pol1_slds[i] = (unsigned int *) malloc(sizeof(unsigned int) * BPSR_IBOB_NCHANNELS);
    pol0_accs[i] = (unsigned int *) malloc(sizeof(unsigned int) * BPSR_IBOB_NCHANNELS);
    pol1_accs[i] = (unsigned int *) malloc(sizeof(unsigned int) * BPSR_IBOB_NCHANNELS);
    pol0_bits[i] = (char *) malloc(sizeof(char *) * BPSR_IBOB_NCHANNELS);
    pol1_bits[i] = (char *) malloc(sizeof(char *) * BPSR_IBOB_NCHANNELS);
    packed_signals[i] = (char *) malloc(sizeof(char *) * BPSR_IBOB_NCHANNELS * 2);
    udp_datas[i] = (char *) malloc(sizeof(char *) * BPSR_UDP_PAYLOAD_BYTES);

  }

}
  
