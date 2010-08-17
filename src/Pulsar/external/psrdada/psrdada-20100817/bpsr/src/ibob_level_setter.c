#include "sock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

void extract_counts(char * bram, long * vals);
long calculate_max(long * vals, long birdie_factor);
void swap(long *a, long *b);
void sort(long arr[], long beg, long end);
int find_bit_window(long pol1val, long pol2val, long *new_gain1, long* new_gain2);
void usage();


void usage() {
  fprintf(stdout,
    "ibob_level_setter [options] host port\n"
    " host        on which the ibob interface is accessible\n"
    " port        on which the ibob interace is accessible\n"
    "\n"
    " -n num      number of attempts to level set [default %d]\n"
    " -v          verbose\n"
    " -i val      intial scaling factor to apply [default %d]\n"
    " -h          print help text\n",3,131072);

}

int main (int argc, char** argv)
{

  char * hostname;
  int port;
  int verbose = 0;
  int init_scale = 131072;
  int n_attempts = 3;
  int arg = 0;
  
  //strcpy(hostname, "srv0.apsr.edu.au");
  //int port;

  while ((arg=getopt(argc,argv,"n:i:vh")) != -1) {
    switch (arg) {

    case 'v':
      verbose = 1;
      break;

    case 'i':
      init_scale = atoi(optarg);
      break;

    case 'n':
      n_attempts = atoi(optarg);
      break;

    case 'h':
      usage();
      return 0;

    default:
      usage();
      return 0;
    }
  }

  if ((argc - optind) != 2) {
    fprintf(stderr, "Error: host and port must be specified\n");
    usage();
    return EXIT_FAILURE;
  } else {
    hostname = argv[optind];
    port = atoi(argv[(optind+1)]);
  }

  FILE *fptr= fopen("/lfs/data0/bpsr/logs/levelsetter.log","a");
  if (!fptr) {
    system("echo 'could not open log file for appending' >> /lfs/data0/bpsr/logs/levelsetter.log");
  }

  char command[100];

  int fd;
  fd = sock_open(hostname, port);

  if (fd < 0)  {
    fprintf(stderr, "Error creating socket\n");
    fclose(fptr);
    return EXIT_FAILURE;
  } else {
    fprintf(stderr, "opened socket to %s on port %d [fd %d]\n",hostname, port, fd);
  }


  // Write the initial coefficients
  long pol1_coeff = init_scale;
  long pol2_coeff = init_scale;
  int rval = 0;

  char *pol1_bram = malloc(sizeof(char) * 163842);
  char *pol2_bram = malloc(sizeof(char) * 163842);

  long *pol1_vals = malloc(sizeof(long) * 512);
  long *pol2_vals = malloc(sizeof(long) * 512);

  long pol1_max_value;
  long pol2_max_value;

  int i=0;
  int bytes_read = 0;
  int bit_window;

  for (i=0; i<n_attempts; i++ ) {
  
    sprintf(command, "regwrite reg_coeff_pol1 %d\r\n",pol1_coeff);
    fprintf(stderr, "<- %s",command);
    if (rval = sock_write(fd, command, strlen(command)-1) < 0 ) {
      fprintf(stderr, "could not write command %s\n",command);
      sock_close(fd);
      fclose(fptr);
      return(EXIT_FAILURE);
    }

    sprintf(command, "regwrite reg_coeff_pol2 %d\r\n",pol1_coeff);
    fprintf(stderr, "<- %s",command);
    if (rval = sock_write(fd, command, strlen(command)-1) < 0 ) {
      fprintf(stderr, "could not write command %s\n",command);
      sock_close(fd);
      fclose(fptr);
      return(EXIT_FAILURE);
    }
    sleep(1);

    strcpy(command, "bramdump scope_output1/bram\r\n");
    fprintf(stderr, "<- %s",command);
    rval = sock_write(fd, command, strlen(command) -1);
    bytes_read = 0;
    rval = 1;
    while (rval > 0) {
      rval = sock_tm_read(fd, (void *) (pol1_bram + bytes_read), 4096, 0.5);
      bytes_read += rval;
    }
    pol1_bram[163841] = '\0';

    strcpy(command, "bramdump scope_output3/bram\r\n");
    fprintf(stderr, "<- %s",command);
    rval = sock_write(fd, command, strlen(command) -1);
    bytes_read = 0;
    rval = 1;
    while (rval > 0) {
      rval = sock_tm_read(fd, (void *) pol2_bram + bytes_read, 4096, 0.5);
      bytes_read += rval;
    }
    pol2_bram[163841] = '\0';

    extract_counts(pol1_bram, pol1_vals);
    extract_counts(pol2_bram, pol2_vals);

    //int j=0;
    //for(j=0; j<512;j++) {
    //  fprintf(stderr,"%d: %d, %d\n",j,pol1_vals[j], pol2_vals[j]);
    //}

    pol1_max_value = calculate_max(pol1_vals, 8);
    pol2_max_value = calculate_max(pol2_vals, 8);

    //fprintf(stderr, "max vals = %d,%d\n",pol1_max_value, pol2_max_value);
    
    bit_window = find_bit_window(pol1_max_value, pol2_max_value, &pol1_coeff, &pol2_coeff);

    fprintf(stderr, "bit window %d, new scale factors %d,%d\n", bit_window,pol1_coeff, pol2_coeff);
  }

  // Write the bit window choice to the ibob
  sprintf(command,"regwrite reg_output_bitselect %d\r\n",bit_window);
  fprintf(stderr, "<- %s",command);
  rval = sock_write(fd, command, strlen(command) -1);

  if (verbose) 
    fprintf(stderr, "closing sockets\n");
  sock_close (fd);

  if (fptr) {
    fclose(fptr);
  }

  return 0;
}

void extract_counts(char * bram, long * vals) {

  const char *sep = "\r\n";
  char *line;
  char value[11];
  int i=0;

  line = strtok(bram, sep);
  strncpy(value, line+69, 11);
  vals[i] = atol(value);

  for (i=1; i<512; i++) {
    line = strtok(NULL, sep);
    strncpy(value, line+69, 11); 
    vals[i] = atol(value);
  }

}

long calculate_max(long * vals, long birdie_factor) {

  sort(vals,0,511);
  long median  = vals[255];
  long birdie_cutoff = birdie_factor * median;
  long max_value = 0;

  int i=0;
  int n_sum=0;
  double sum = 0;
  double sum_mean = 0;

  for (i=0; i<512; i++) {
    if (vals[i] < birdie_cutoff) {
      sum += (double) vals[i]; 
      n_sum++;
      if (vals[i] > max_value)
        max_value = vals[i];
    }
  }
  
  long mean_value = (long) ( sum / (double) n_sum);
  //fprintf(stderr, "max = %d, mean = %d\n",max_value, mean_value);

  return max_value;
}

void swap(long *a, long *b)
{
  long t=*a; *a=*b; *b=t;
}

void sort(long arr[], long beg, long end)
{
  if (end > beg + 1)
  {
    long piv = arr[beg], l = beg + 1, r = end;
    while (l < r)
    {
      if (arr[l] <= piv)
        l++;
      else
        swap(&arr[l], &arr[--r]);
    }
    swap(&arr[--l], &arr[beg]);
    sort(arr, beg, l);
    sort(arr, r, end);
  }
}

int find_bit_window(long pol1val, long pol2val, long *gain1, long* gain2) {

  long bitsel_min[4] = {0, 256, 65535, 16777216 };
  long bitsel_mid[4] = {64, 8192, 2097152, 536870912};
  long bitsel_max[4] = {255, 65535, 16777215, 4294967295};

  long val = ((long) (((double) pol1val + (double) pol2val) / 2.0));

  //Find which bit selection window we are currently sitting in
  int i=0;
  int current_window = 0;
  int desired_window = 0;

  for (i=0; i<4; i++) {

    //If average (max) value is in the lower half
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

  long desired_val =  ((bitsel_max[desired_window]+1) / 2);

  //printf("desired val = %d\n",desired_val);
  
  double gain_factor1 = (double) desired_val / (double) pol1val;
  double gain_factor2 = (double) desired_val / (double) pol2val;

  gain_factor1 = sqrt(gain_factor1);
  gain_factor2 = sqrt(gain_factor2);

  gain_factor1 *= *gain1;
  gain_factor2 *= *gain2;

  *gain1 = (long) floor(gain_factor1);
  *gain2 = (long) floor(gain_factor2);

  if (*gain1 > 200000) {
    *gain1 = 200000;
  }
  if (*gain2 > 200000) {
    *gain2 = 200000;
  }
  return desired_window;
} 
