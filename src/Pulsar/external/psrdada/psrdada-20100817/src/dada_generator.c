/* 
 *  Library functions for generating signals at various bit levels
 *  for udp generator codes
 */

#include <math.h>

#include "dada_generator.h"

/*
 * prints the binary representatino of a char
 */
char * char_to_binary_string(char value) {
  
  char * string = (char *) malloc(sizeof(char)*9);
  char c;
  char displayMask = 1 << 7;

  for (c=1; c<= 8; c++) {
    string[c-1] = value & displayMask ? '1' : '0';
    value <<= 1;
  }
  string[8] = '\0';
  return string;
}

/*
 * return the binary string of an unsigned int
 */
char * unsigned_to_binary_string(unsigned int value) {

  char * string = (char *) malloc(sizeof(char)*257);
  unsigned int c;
  unsigned int displayMask = 1 << 31;

  for (c=1; c<= 32; c++) {
    string[c-1] = value & displayMask ? '1' : '0';
    value <<= 1;
  }
  string[32] = '\0';
  return string;
}

/*
 * fill an char array with gaussian noise 
 */
void fill_gaussian_chars(char *data, int size, int nbits, int gain) {

  float max = powf(2, nbits);
  
  int val;
  int twos;
  int i;
  float y2=0;
  
  float centre = max * ((float) (gain-500) / 1000);
  float div = 4.0;
  
  unsigned int mask = 0x000000ff;

  for (i=0; i < size; i++) {
                                  
    val = 0;
    y2 = (float) rand_normal(centre, max/div);

    if ((y2 < -128) || (y2 > 128)) {
      y2 = 1;
    }

    val = (int) floor(y2);

    /* determine the twos complement equivalent */
    twos = ~val;
    twos = twos + 1;
    twos &= mask;

    val &= mask;

    data[i] = (char) val;

  }
}

/*
 * fill an unsigned int  array with gaussian noise 
 */
void fill_gaussian_unsigned(unsigned int *data, int size, int nbits, int gain) {

  double max = powf(2, nbits);
  double y = 0;

  int val;
  int twos;
  int i;

  double centre = max * ((double) (gain-500) / 1000);
  double div = 4.0;

  unsigned int mask = 0x000000ff;

  for (i=0; i < size; i++) {
    val = 0;
    y = rand_normal(centre, max/div);
    data[i] = (unsigned int) y;
  }
}


/*
 * Fill char array of length size with a cal signal _--_ 
 * gain value of 1 - 1000
 */

void fill_cal_chars(char *data, int size, int nbits, int gain) {

  float max = powf(2, nbits);
  float div = 4.0;
  int val;
  float y2=0;

  float centre = max * ((float) (gain-500) / 1000);
  float off = centre / 1.3;
  float on = centre * 1.3;

  unsigned int mask = 0x000000ff;

  int i;
  for (i=0; i < size; i++) {

    if ((i < (size*0.25)) || (i > (size*0.75) )) {
      y2 = (float) rand_normal(off, max/div);
    } else {
      y2 = (float) rand_normal(on, max/div);
    }

    if ((y2 < -128) || (y2 > 128)) {
      y2 = 1;
    }

    val = (int) floor(y2);

    val &= mask;
    data[i] = (char) val;
  }
}

/*
 * fill an unsigned int  array with gaussian noise 
 */ 
void fill_cal_unsigned(unsigned int *data, int size, int nbits, int gain) {
    
  double max = powf(2, nbits);
  double y = 0;

  int val; 
  int twos;
  int i;

  double centre = max * ((double) (gain-500) / 1000);
  double div = 4.0;
  double off = centre / 1.3;
  double on = centre * 1.3;
  
  unsigned int mask = 0x000000ff;

  for (i=0; i < size; i++) {

    if ((i < (size*0.25)) || (i > (size*0.75) )) {
      y = rand_normal(off, max/div);
    } else {
      y = rand_normal(on, max/div);
    }
    data[i] = (unsigned int) y;
  }
}




/*
 * return a gaussian random value with the specified mean and stddev
 */
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

