#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <inttypes.h>

#define DEFAULT_MEM_SIZE 1024 

int quit_threads = 0;
int verbose = 0;
void * load_thread (void * arg);

typedef struct {
  int id;
  int size;
  int score;
} worker_thread_t;


void usage ()
{
  fprintf (stdout,   
     "load_test      test the memory/cache bandwidth for multitple threads\n\n"
     "Usage:   load_test [options]\n"
     " -m  size      size of memory array in KB [default %d]\n"
     " -t  num       launch num threads [default 1]\n"
     " -s  num       run test for num seconds [default 10]\n"
     " -v            verbose output\n"
     " -h            print usage\n", DEFAULT_MEM_SIZE);
}

int main (int argc, char** argv)
{
  int arg;

  int num_threads = 1;
  int num_seconds = 10;
  int mem_size = DEFAULT_MEM_SIZE;

  while ((arg = getopt(argc, argv, "m:t:s:v")) != -1) {

    switch (arg)  {

      case 'm':
        mem_size = atoi(optarg);
        break;

      case 't':
        num_threads = atoi(optarg);
        break;
 
      case 's':
        num_seconds = atoi(optarg);
        break;

      case 'v':
        verbose = 1;
        break;

      case 'h':
        usage();
        return 0;
        break;

      default:
        usage();
        return 1;
        break;
    }
  }

  int i = 0;

  if (verbose) 
    fprintf(stderr, "creating %d threads, running for %d seconds with %d byte memcpys\n", num_threads, num_seconds, mem_size);


  pthread_t * thread_ids = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);
  worker_thread_t * thread_vals = (worker_thread_t *) malloc(sizeof(worker_thread_t) * num_threads);

  for (i=0; i<num_threads; i++) {

    thread_ids[i] = 0;
    thread_vals[i].id = i;
    thread_vals[i].size = mem_size;
    thread_vals[i].score = 0;

    if (pthread_create (&(thread_ids[i]), 0, load_thread, (void *) &(thread_vals[i])) < 0) {
      perror ("Error creating new thread");
      return -1;
    } else {
      if (verbose)
        fprintf(stderr, "thread[%d] == %d\n", i, thread_ids[i]);
    } 
  }

  int count = 0;
  while (count < num_seconds) {
    if (verbose && (count % 10 == 0)) 
      fprintf(stderr, "main: %d seconds left\n", num_seconds-count);
    count++;
    sleep(1);
  }

  if (verbose) 
    fprintf(stderr, "joining %d threads\n", num_threads);

  quit_threads = 1;

  void * result = 0;
  int err = 0;
  int score = 0;

  for (i=0; i<num_threads; i++) {
    err = pthread_join (thread_ids[i], &result);
    if (err)
      perror("pthread_join");
    else {
      if (verbose)
        fprintf(stderr, "i=%d, score=%d\n", i, thread_vals[i].score);
      score += thread_vals[i].score;
    }
  }

  float avg_score = ((float) score  * (float) mem_size) / ((float) num_threads * (float) num_seconds);

  avg_score /= 388710;

  fprintf(stderr, "Avg = %5.4f, overall score: %d\n", avg_score, score);

  return 0;
}

void * load_thread (void * arg) {

  worker_thread_t * vals = (worker_thread_t *) arg;

  if (verbose)
    fprintf(stderr, "THREAD_START[%d] size=%d KB, score=%d\n", vals->id, vals->size, vals->score);


  int j = 0;
  int counter = 0;
  int reset = 1000;
  int size = vals->size * 1024;

  char * buf1 = malloc(sizeof(char) * size);
  char * buf2 = malloc(sizeof(char) * size);

  for (j=0; j<size; j++) {
    buf1[j] = 1;
    buf2[j] = 1; 
  }

  while (!quit_threads) {
    for (j=0; j<size; j++) {
      buf1[j] += buf2[j];
      if (buf1[j] == 127) 
        buf1[j] = 0;
    }

    counter++;
    if (counter == reset) {
       counter = 0;
    }
    vals->score++;
  }

  free(buf1);
  free(buf2);

  if (verbose)
    fprintf(stderr, "THREAD_END[%d] size=%d KB, score=%d\n", vals->id, vals->size, vals->score);

  pthread_exit((void*) &(vals->score));
  //return 0;

}


