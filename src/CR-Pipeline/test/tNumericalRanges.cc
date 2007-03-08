
#include <iostream>
#include <limits.h> 
#include <float.h>

int main(void) {
  
  /*  a byte doesn't have to contain 8 bits (but usually does) */
  printf("1 byte contains %i bits", CHAR_BIT);
  std::cout << std::endl;
  
  /*  char may be signed or unsigned, and all three will always be 1 byte  */
  printf("unsigned char   %u byte,  from %12i to %12i", 
         sizeof(unsigned char), 0, UCHAR_MAX);
  std::cout << std::endl;
  printf("signed char     %u byte,  from %12i to %12i", 
         sizeof(signed char), SCHAR_MIN, SCHAR_MAX);
  std::cout << std::endl;
  printf("char            %u byte,  from %12i to %12i", 
         sizeof(char), CHAR_MIN, CHAR_MAX);
  std::cout << std::endl;
  
  /*  short, int and long are signed  */
  printf("unsigned short  %u bytes, from %12hu to %12hu", 
         sizeof(unsigned short), 0, USHRT_MAX);
  std::cout << std::endl;
  printf("signed short    %u bytes, from %12hi to %12hi", 
         sizeof(signed short), SHRT_MIN, SHRT_MAX);
  std::cout << std::endl;
  printf("unsigned int    %u bytes, from %12u to %12u", 
         sizeof(unsigned int), (unsigned int) 0, (unsigned int) UINT_MAX);
  std::cout << std::endl;
  printf("signed int      %u bytes, from %12i to %12i", 
         sizeof(signed int), INT_MIN, INT_MAX);
  std::cout << std::endl;
  
  printf("unsigned long   %u bytes, from %12lu to %12lu", 
         sizeof(unsigned long), (unsigned long) 0, ULONG_MAX);
  std::cout << std::endl;
  printf("signed long     %u bytes, from %12li to %12li", 
         sizeof(signed long), LONG_MIN, LONG_MAX);
  std::cout << std::endl;
  
  /*   floating point types can store 0, may also be able to store 
       strange numbers like +Infinity and  */
  printf("float           %u bytes, positive range from %12e to %12e", 
         sizeof(float), FLT_MIN, FLT_MAX);
  printf("                         epsilon = %e", FLT_EPSILON);
  std::cout << std::endl;
  printf("double          %u bytes, positive range from %12e to %12e", 
         sizeof(double), DBL_MIN, DBL_MAX);
  printf("                         epsilon = %e", DBL_EPSILON);
  std::cout << std::endl;
  
  /*   L in format string is not universally supported  */
  printf("long double     %u bytes, positive range from %12Le to %12Le", 
         sizeof(long double), LDBL_MIN, LDBL_MAX);
  printf("                         epsilon = %Le", LDBL_EPSILON);
  std::cout << std::endl;

  return 0;
}
