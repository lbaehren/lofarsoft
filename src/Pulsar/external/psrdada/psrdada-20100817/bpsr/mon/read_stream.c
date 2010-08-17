/***************************************************************************/
/*                                                                         */
/* function read_stream                                                    */
/*                                                                         */
/* it reads from a stream                                                  */
/*                                                                         */
/* Ver 2.0	RB 04 Oct 2008
		no longer needs large stacks (malloc replaces MAXNUMPOINTS) 
*/
/*                                                                         */
/***************************************************************************/

#include "plot4mon.h"

void read_stream(int ndata,char inpfile[],float *readstream,long *totvaluesread)
{
int i,nread;
long totnread;
long blksz=1;   // to be adjusted later, if required
FILE *pn2file; 

/* it opens the input binary files, testing for the successful completion */
  if ((pn2file = fopen(inpfile, "rb")) == NULL ) 
  { 
     printf(" Error opening %s for reading  \n",inpfile); 
     exit(-1); 
  }
  printf(" Reading the data of the file %s... \n",inpfile);   

/* "i" runs on the location of the values in the array readstream */
  totnread=0;
  for (i=0;i<ndata;i++) 
  {      
     nread = fread(&readstream[blksz*i],1,blksz*sizeof(float),pn2file);
     totnread=totnread+(long)nread;
     if (feof(pn2file)) break;
  } 
  if (totnread > ndata*sizeof(float)) 
  {
     printf(" Data overflow error! \n");
     exit(-1);
  }
  fclose(pn2file);
  *totvaluesread=totnread/sizeof(float);  // tot number of read values
  printf(" Read %ld %d-bytes long values from %s \n",
          *totvaluesread,sizeof(float),inpfile);  
}
