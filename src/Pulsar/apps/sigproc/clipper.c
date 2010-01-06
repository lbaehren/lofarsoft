/* clipper - a program to do time-domain clipping of filterbank files */

#include "dedisperse.h"

char inpfile[80], outfile[80];
main (int argc, char *argv[]) 
{
  float clip_level, *blk, sum, mean, blksum, blksum2, blkmean, blkmean2;
  unsigned char *cblk;
  unsigned short *sblk;
  float blksigma, dmean, dsigma;
  int blksize, nread, nsblk, i, headersize, c, s, offset, obits;
  long int isamp=0, seed=0, nclip=0;

  if (argc<2 || help_required(argv[1])) {
    //blanker_help();
    exit(0);
  }
  print_version(argv[0],argv[1]);
  if (!file_exists(argv[1]))
    error_message("input file does not exist!");

  strcpy(inpfile,argv[1]);
  input=open_file(inpfile,"r");
  strcpy(outfile,"stdout");
  output=stdout;
  nsblk=1024;
  clip_level=6.0;
  obits=32;

  i=2;
  while (i<argc) {
    if (strings_equal(argv[i],"-c"))       clip_level=atof(argv[++i]);
    if (strings_equal(argv[i],"-s"))       nsblk=atof(argv[++i]);
    if (strings_equal(argv[i],"-b"))       obits=atoi(argv[++i]);
    i++;
  }

  if ((headersize=read_header(input))) {

    if (nifs>1) error_message("blanker can only handle nifs=1");

    send_string("HEADER_START");
    if (!strings_equal(source_name,"")) {
      send_string("source_name");
      send_string(source_name);
    }
    send_int("telescope_id",telescope_id); 
    send_int("machine_id",machine_id);
    send_coords(src_raj,src_dej,az_start,za_start);
    send_int("data_type",data_type);
    send_double("fch1",fch1);
    send_double("foff",foff);
    send_int("barycentric",barycentric);
    send_int("nchans",nchans);
    send_int("nbits",obits);  
    send_double ("tstart",tstart); 
    send_double("tsamp",tsamp);
    send_int("nifs",nifs);
    send_string("HEADER_END");

    
    blksize=nchans*nsblk;
    blk = (float *) malloc(blksize*sizeof(float));
    cblk = (unsigned char *) malloc(blksize*sizeof(unsigned char));
    sblk = (unsigned short *) malloc(blksize*sizeof(unsigned short));
    
    while (nread=read_block(input,nbits,blk,blksize)) {

      /* calculate mean and sigma of the whole block */
      blksum=blksum2=0.0;
      for (i=0; i<nread; i++) {
	blksum+=blk[i];
	blksum2+=blk[i]*blk[i];
      }
      blkmean=blksum/(float)nread;
      blkmean2=blksum2/(float)nread;
      blksigma=sqrt(blkmean2-blkmean*blkmean);
      /* expected mean and sigma of a dedispersed sample in this block */
      dmean=blkmean*(float)nchans;
      dsigma=blksigma*sqrt((float)nchans);

      /* now work through each sample in the block */
      nsblk=nread/nchans;
      offset=0;
      for (s=0; s<nsblk; s++) {
	isamp++;
	sum=0.0;
	for (c=0; c<nchans; c++) sum+=blk[offset+c];
	if (abs(sum-dmean)>clip_level*dsigma) {
	  nclip++;
	  for (c=0; c<nchans; c++) 
	    blk[offset+c]=gasdev(&seed)*blksigma+blkmean;
	}
	offset+=nchans;
      }

      /* write out the clipped data (32 bit!) */
      if (obits == 32) 
	fwrite(blk,sizeof(float),nread,output);
      } else if (obits == 8) {
	for (i=0;i<nread;i++) cblk[i]=blk[i];
	fwrite(cblk,sizeof(unsigned char),nread,output);
      } else if (obits == 16) {
	for (i=0;i<nread;i++) sblk[i]=blk[i];
	fwrite(sblk,sizeof(unsigned short),nread,output);
      } else {
	error_message("only 8, 16 or 32 bit output modes supported");
      }
    }
    fprintf(stderr,"%ld samples clipped\n",nclip);
  }
}
