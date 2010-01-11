/* scamp2fb - converts SCAMP search-mode data into "filterbank" data 
   modified - 05/04/04 (drl@jb.man.ac.uk) to flip the incoming data if
   it has been saved in the less-common lower sideband format i.e. lowest
   frequency channel first 
   modified - 02/07/04 (drl@jb.man.ac.uk) to cope with 24/48k block sizes
   modified - 29/04/05 (drl@jb.man.ac.uk) to clip samples before writing
   modified - 21/07/05 (drl@jb.man.ac.uk) to ignore sections of the band
*/
#include "filterbank.h"
int scamp_ignore[1024],scamp_chans;
int scamp_block_size; /* read in from header, can be either 48k or 24k */
int scamp_rawdata;    /* signifies whether data has been sc_td'd or not */
/* bit-reverse the order of (i.e. flip) a byte */
unsigned char flipchar(unsigned char orig) {
  unsigned char flipped,c;
  int i,j[8];
  flipped=0;
  j[0]=1;
  j[1]=2;
  j[2]=4;
  j[3]=8;
  j[4]=16;
  j[5]=32;
  j[6]=64;
  j[7]=128;
  for (i=0;i<8;i++) {
    if (c=(orig&j[i])) flipped+=128/c;
  }
  return (flipped);
}

/* utility to sum the 1-bit samples in a byte */
int sumchar(unsigned char c) /*includefile*/
{
  unsigned char i,j[8],sum;
  sum=0;
  j[0]=1;
  j[1]=2;
  j[2]=4;
  j[3]=8;
  j[4]=16;
  j[5]=32;
  j[6]=64;
  j[7]=128;
  for (i=0;i<8;i++) if (c&j[i]) sum++;
  return(sum);
}

void scamp2fb(FILE *input, FILE *output) /* includefile */
{
  FILE *fpou;
  unsigned char *gulp,*spew,*flip,sumof[256];
  float realtime;
  double junk;
  char string[80],schead[640];
  int i,j,k,doit,idump=0,nread,opened=0,s,f,
	ntotal=0,nclipped=0,ichans,sum,clipmax;

  //if (scamp_chans != nchans) exit(0);

  /* allocated space to store incoming and outgoing data blocks */
  gulp = (unsigned char *) malloc(scamp_block_size * sizeof(unsigned char));
  flip = (unsigned char *) malloc(scamp_block_size * sizeof(unsigned char));
  spew = (unsigned char *) malloc(8*scamp_block_size * sizeof(unsigned char));

  if (headerfile) {
    /* write output ASCII header file */
    fpou=open_file("head","w");
    fprintf(fpou,"Original SCAMP file: %s\n",inpfile);
    fprintf(fpou,"Sample time (us): %f\n",tsamp*1.0e6);
    fprintf(fpou,"Center freq (MHz): %f\n",fch1+(float)scamp_chans*foff/2.0);
    fprintf(fpou,"Channel band (kHz): %f\n",fabs(foff)*1000.0);
    fprintf(fpou,"Time stamp (MJD): %18.12f\n",tstart);
    fprintf(fpou,"RA (J2000): %f\n",src_raj);
    fprintf(fpou,"DEC (J2000):	%f\n",src_dej);
    fclose(fpou);
  }

  if (clip_threshold>0.0)  {
     /* 
       clip samples which deviate by more than clip_threshold 
       sigma times the mean. For these data, mean=nchan/2 and
       sigma = sqrt(nchan) 
     */
     clipmax=(float)(scamp_chans)/2.0+clip_threshold*sqrt((float)scamp_chans);
     for (i=0;i<256;i++) sumof[i]=sumchar(i); /* look-up table of byte sums */
  }

  while (!feof(input)) {

    /* read ina 48k block of data */
    nread=fread(gulp,1,scamp_block_size,input);

    /* do the band inversion here if necessary 
       DEDISPERSE requires that the first channel is the highest frequency */
    if (invert_band) {
      for (i=0; i<scamp_block_size; i++) flip[i]=flipchar(gulp[i]);
      for (i=0; i<scamp_block_size/scamp_chans; i++) {
	s=i*scamp_chans; f=(i+1)*scamp_chans; k=0;
	for (j=s;j<f;j++) {
	  gulp[j]=flip[f-1-k];
	  k++;
	}
      }
    }

    /* now do clipping if necessary */
    if (clip_threshold>0.0) {
      ichans=sum=j=0; 
      for (i=0; i<scamp_block_size; i++) {
	/* keep track of sum over each byte (8 channels!) */
	sum+=sumof[gulp[i]];
	ichans+=8;
	if (ichans==scamp_chans) {
		/* decide whether to clip this sample */
		if (sum>clipmax) {
			nclipped++;
			for (k=j; k<=i; k++) gulp[k]=0;
		}
		ntotal++;
	        j=i+1;
		sum=0;
		ichans=0;
	}
      }
    }

    if (scamp_rawdata) {
      /* read off the header from the data and proceed as normal */
      fread(schead,sizeof(schead),1,input);
    } else {
      /* extra (FORTRAN!) junk that gets written after each block */
      fread(&junk,8,1,input); 
    }

    /* decide whether to write out this block */
    realtime=tsamp*idump;
    if ((doit=process(realtime,start_time,final_time))==-1) break;
    if (doit) {
      if (idump%1024 == 0) {
	if (!opened) {
	  open_log("filterbank.monitor");
	  opened=1;
	}
	sprintf(string,"time:%.1fs",realtime);
	update_log(string);
      }
      /* output as single bit (default) or single byte data */
      switch (obits) {
      case 1:
	idump+=8*nread/scamp_chans;
	i=1;
	/* write out only segments not in scamp.ignore */
	for (j=1;j<=nread;j++) {
	  if (!scamp_ignore[i]) fwrite(&gulp[j-1],1,1,output);
	  i++;
	  if (i>scamp_chans/8) i=1;
	}
	/*fwrite(gulp,1,nread,output);*/
	break;
      case 8:
	k=0;
	for (i=0; i<nread; i++) {
	  for (j=0;j<8;j++) {
	    spew[k]=gulp[i]&1;
	    gulp[i]>>=1;
	    k++;
	    if (!(k%scamp_chans)) idump++;
	  }
	}
	fwrite(spew,1,8*scamp_block_size,output);
	break;
      }
    }
  }
  /* write out clipping statistics to ASCII file clip.stats */
  if (clip_threshold>0) {
    fpou=open_file("clip.stats","w");
    fprintf(fpou,"threshold %.1f sigma (sum = %d)\n",
	clip_threshold,clipmax);
    fprintf(fpou,"samples clipped = %d (total = %d)\n",
	nclipped,ntotal);
    fclose(fpou);
  }
  free(gulp);free(flip);free(spew);
}
