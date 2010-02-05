/* clipper - a program to do time-domain clipping of filterbank files */

#include "dedisperse.h"

char inpfile[80], outfile[80];
main (int argc, char *argv[]) 
{
  float *timeser;
  int i,j,headersize,*ishift,maxshift,offset;
  long int iread,nsamp;
  unsigned char *rawdata;
  double thisdm,dmmax;

  if (argc<3 || help_required(argv[1])) {
    //blanker_help();
    exit(0);
  }
  print_version(argv[0],argv[1]);
  if (!file_exists(argv[1]))
    error_message("input file does not exist!");

  //  if (!file_exists(argv[2]))
  //  error_message("dmlist does not exist");

  strcpy(inpfile,argv[1]);
  input=open_file(inpfile,"r");
  strcpy(outfile,"stdout");
  output=stdout;
  dmmax=atof(argv[2]);
  i=2;
  while (i<argc) {
    //    if (strings_equal(argv[i],"-c"))       clip_level=atof(argv[++i]);
    //    if (strings_equal(argv[i],"-s"))       nsblk=atof(argv[++i]);
    i++;
  }

  if ((headersize=read_header(input))) {
    if (nifs>1) error_message("can only handle nifs=1");
    if (nbits!=8) error_message("can only do nbits=8");

    ishift=dmshift(fch1,foff,nchans,1,dmmax,fch1,tsamp,frequency_table);
    maxshift=ishift[nchans-1];

    nsamp=nsamples(argv[1],headersize,nbits,nifs,nchans);
    rawdata=(unsigned char *) 
      malloc((nsamp+maxshift)*nchans*sizeof(unsigned char));
    timeser=(float *) malloc(nsamp*sizeof(float));


    iread=fread(rawdata,1,nsamp*nchans,input);
    for (i=nsamp*nchans;i<(nsamp+maxshift)*nchans;i++) rawdata[i]=0.0;


    for (thisdm=0.0;thisdm<dmmax;thisdm+=1.0) {

      for (i=0;i<nsamp;i++) timeser[i]=0.0;
      ishift=dmshift(fch1,foff,nchans,1,thisdm,fch1,tsamp,frequency_table);
      for (i=0;i<nchans;i++) ishift[i]*=nchans;


      offset=0;
      for (i=0;i<nsamp;i++) {
	for (j=0;j<nchans;j++) {
	  timeser[i]+=rawdata[offset+ishift[j]+j];
	}
	offset+=nchans;
      }

      sprintf(outfile,"test%.1lf.tim",thisdm);
      puts(outfile);
      output=open_file(outfile,"w");
      /* broadcast the header parameters to the output stream */
      send_string("HEADER_START");
      if (!strings_equal(source_name,"")) {
	send_string("source_name");
	send_string(source_name);
      }
      send_int("telescope_id",telescope_id); 
      send_int("machine_id",machine_id);
      send_coords(src_raj,src_dej,az_start,za_start);
      send_int("data_type",2);
      send_double("refdm",thisdm);
      send_double("fch1",fch1);
      send_int("barycentric",barycentric);
      send_int("nchans",1);
      send_int("nbits",32);  
      send_double ("tstart",tstart); 
      send_double("tsamp",tsamp);
      send_int("nifs",nifs);
      send_string("HEADER_END");
      fwrite(timeser,4,nsamp,output);
      fclose(output);

    }
  }
}
