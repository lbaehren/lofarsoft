/**********************************************************************************/
/* This is to convert combined 16bit data into sigproc filterbank format..        */
/* i.e. it flips the frequency ordering as well writes a header...                */
/*                  - Sarala 7 Nov 2006                                           */
/*                                                                                */
/* This is modified such that it convert both the 16bit and 8bit combined data    */
/*                  - Sarala 7 Feb 2007                                           */
/*                                                                                */
/* Incorporated into sigproc-4.3 and renamed gmrt2fb.c (DRL - June 21, 2007)      */
/*                                                                                */
/**********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <time.h>
#include "sigproc.h"
#include "header.h"
#include <errno.h>

extern int errno;
FILE *output;

#define nch 256
#define maxsamp 80000

main(int argc, char** argv)
{
    int 	  c,arg=1,i,j,k,nstep;
    long long int fsizeusb,nsamp,nextract=0,nsampuse;
    char	  fnameusb[180], fheader[180], headerline[80], junk[80];
    char          rastring[80], decstring[80], outputfile[180];
    double        ras, des;
    int           rah, ram, ded, dem, hh, mm, ssm;
    double        ss;
    FILE	  *finfileusb,*flog,*headerfile;
    short int     *DATA1,*DATA4,temp;
    unsigned char *DATA2,*DATA3;
    char          timeBuffer[80];
    time_t        now ;


    if (argc <= 1)
    {
      printf("USAGE : gmrt2fb -f filename -r raj -d decj -m MJD -s timeSample -n nbits -w channelWidth -c first_channelFreq -t No of Channels \n");
      printf("\n");
      printf("    filename: Combined Data filename \n");
      printf("\n");
      exit(0);
    }

    if (file_exists(argv[1])) {
      strcpy(fnameusb,argv[1]);
      if((finfileusb = fopen(fnameusb,"rb"))== NULL) {
	error_message("error opening GMRT data file");
      }
      strcpy(fheader,"");
      strncat(fheader,fnameusb,strlen(fnameusb)-3);
      strcat(fheader,"hdr");
      if((headerfile = fopen(fheader,"rb"))== NULL) {
	error_message("error opening GMRT data file");
      }
      while (fgets(headerline, 80, headerfile) != NULL) {
	if (strstr(headerline,"Num Channels") != NULL) {
	  sscanf(headerline,"%s %s %s %d",junk,junk,junk,&nchans);
	}
	if (strstr(headerline,"Channel width") != NULL) {
	  sscanf(headerline,"%s %s %s %lf",junk,junk,junk,&foff);
	  foff*=-1.0;
	}
	if (strstr(headerline,"Frequency Ch.1") != NULL) {
	  sscanf(headerline,"%s %s %s %lf",junk,junk,junk,&fch1);
	}
	if (strstr(headerline,"Num bits/sample") != NULL) {
	  sscanf(headerline,"%s %s %s %d",junk,junk,junk,&nbits);
	}
	if (strstr(headerline,"MJD") != NULL) {
	  sscanf(headerline,"%s %s %lf",junk,junk,&tstart);
	}
	if (strstr(headerline,"UTC") != NULL) {
	  sscanf(headerline,"%s %s %d:%d:%lf",junk,junk,&hh,&mm,&ss);
	  tstart+=(double)hh/24.0+(double)mm/1440.0+ss/86400.0;
	}
	if (strstr(headerline,"Sampling Time") != NULL) {
	  sscanf(headerline,"%s %s %s %lf",junk,junk,junk,&tsamp);
	  tsamp*=1.0e-6;
	}
	if (strstr(headerline,"Coordinates") != NULL) {
	  sscanf(headerline,"%s %s %s %s",junk,junk,junk,decstring);
	  strncat(rastring,junk,strlen(junk)-1);
	  rah=atoi(strtok(rastring,":"));
	  ram=atoi(strtok(NULL,":"));
	  ras=atof(strtok(NULL,":"));
	  src_raj=ras+ram*100.0+rah*10000.0;
	  ded=atoi(strtok(decstring,":"));
	  dem=atoi(strtok(NULL,":"));
	  des=atof(strtok(NULL,":"));
	  src_dej=des+100.0*dem+10000.0*abs(ded);
	}
	if (strstr(headerline,"Source") != NULL) {
	  sscanf(headerline,"%s %s %s",junk,junk,source_name);
	}
      }
    }
	

    //      flog=fopen("convert_sig_gmrt.log","a");
      one_more_time:
      now = time(0);
      strcpy(timeBuffer, ctime(&now));
      printf("\n\nStarting on %s\n",timeBuffer);
      //      fprintf(flog,"----------------------------\n");
      //      fprintf(flog,"Starting on %s\n",timeBuffer);

     while((c = getopt(argc, argv, "f:r:d:m:s:n:w:c:t:")) != -1)
      {
        switch (c)
        {
	 case 'f':
         sscanf(optarg,"%s",fnameusb);
              if((finfileusb = fopen(fnameusb,"rb"))== NULL)
              {
                    printf("%s\n",fnameusb);
                    printf("Error opening file \n");
		    //  fprintf(flog,"Error opening file \n");
                  //  exit(0);
               }
               else
               {
	            printf("\n opened file: %s \n",fnameusb);
		    // fprintf(flog,"\n opened file: %s \n",fnameusb);
               }
        }
        switch (c)
        {
          case 'r':
             sscanf(optarg,"%lf",&src_raj);
             printf("testing %lf\n",src_raj);
        }
        switch (c)
        {
          case 'd':
             sscanf(optarg,"%lf",&src_dej);
        }
        switch (c)
        {
          case 'm':
             sscanf(optarg,"%lf",&tstart);
        }
        switch (c)
        {
          case 's':
             sscanf(optarg,"%lf",&tsamp);
        }
        switch (c)
        {
          case 'n':
             sscanf(optarg,"%d",&nbits);
        }
        switch (c)
        {
          case 'w':
             sscanf(optarg,"%lf",&foff);
        }
        switch (c)
        {
          case 'c':
             sscanf(optarg,"%lf",&fch1);
        }
        switch (c)
        {
          case 't':
             sscanf(optarg,"%d",&nchans);
        }

      }

     printf("Filling the header for sigproc file\n");
     data_type=1;
     nifs=1;
/*   the earlier code (convertSigM) was built with these values :
     machine_id=1;    // this is the id used for analysing sigproc data at NCRA
     telescope_id=3;  // this Arecibo id is used for analysing sigproc data at NCRA for the time being..
*/

/*   new values of machine_id and telescope_id : expected to work with sigproc3.7 and 4.1 */
     machine_id=7;   // new machine ID for GMRT
     telescope_id=7; // new telescope ID for GMRT 

     az_start=0.0;
     za_start=0.0;
     printf("machine_id %d ; telescope_id %d  \n",machine_id,telescope_id);

/******************************************/

     /* Find the file size */
      fseeko(finfileusb,0,SEEK_END);
      fsizeusb = ftello(finfileusb);
      fseeko(finfileusb,0,SEEK_SET);

      printf("File size : %lld\n",fsizeusb);
      //      fprintf(flog,"File size : %lld\n",fsizeusb);
      
     /* Move the file pointer */
              
      printf("Location of file ptr: %lld\n",ftello(finfileusb));

      /* 
	 build a file name of the form:
	 source_MJD_secondssincemidnight_gmrt.fil
      */
      ssm = (int) 86400.0*(tstart-floor(tstart));
      sprintf(outputfile,"%s_%d_%d_gmrt.fil",source_name,(int)floor(tstart),ssm);
      /*
      strcpy(outputfile,"");
      strncat(outputfile,fnameusb,strlen(fnameusb)-4);
      strcat(outputfile,".fil");
      */
      output = fopen(outputfile,"w");
      printf("Outputfile: %s\n",outputfile);
      //      fprintf(flog,"Outputfile: %s\n",outputfile);

      nsamp = fsizeusb;

  if (nbits == 16) { // for 16bit data, read and write it as short int
    DATA1 = malloc (2*nch*maxsamp * sizeof (short int *));
    DATA4 = malloc (2*nch*maxsamp * sizeof (short int *));
    nsamp = nsamp/1024; /* converting nsamp in units of time samples */
    printf("Number of bytes: %lld, Number of time samples:%lld\n",1024*nsamp,nsamp);
    //    fprintf(flog,"Number of bytes: %lld, Number of time samples:%lld\n",1024*nsamp,nsamp);
  }
  else {// for 8bit data, read and write it as unsigned char
    printf("no of bits %d \n",nbits); 
    DATA2 = malloc (2*nch*maxsamp * sizeof (unsigned char *));
    DATA3 = malloc (2*nch*maxsamp * sizeof (unsigned char *));
    nsamp = nsamp/512; /* converting nsamp in units of time samples */
    printf("Number of bytes: %lld, Number of time samples:%lld\n",512*nsamp,nsamp);
    //    fprintf(flog,"Number of bytes: %lld, Number of time samples:%lld\n",512*nsamp,nsamp);
  }


/* write out the header for the *fil files for sigproc that is needed*/

    send_string("HEADER_START");
    send_string("source_name");
    send_string(source_name);
    send_int("machine_id",machine_id);
    send_int("telescope_id",telescope_id);
    send_int("data_type",data_type);
    send_coords(src_raj,src_dej,az_start,za_start);
    send_double("fch1",fch1);
    send_double("foff",foff);
    send_int("nchans",nchans);
    send_int("nbits",nbits);
    send_double("tstart",tstart);
    send_double("tsamp",tsamp);
    send_int("nifs",nifs);
    send_string("HEADER_END");

// End of the Sigproc header...
//    fclose(output);
//    exit(0);

    nstep=5;
    nsampuse = nsamp/nstep;

    while(nsampuse > maxsamp)
    {
        nstep=nstep+10;
        nsampuse = nsamp/nstep;
        printf("Nsampuse: %lld\n",nsampuse);
    }
          
    printf("samples per block:%lld\n",nsampuse);
    //    fprintf(flog,"samples per block:%lld\n",nsampuse);
          
    for(i=0;i<nstep;i++)
    {
      if (nbits == 16) fread(DATA4,sizeof(short),2*nch*nsampuse,finfileusb);
      else fread(DATA2,sizeof(unsigned char),2*nch*nsampuse,finfileusb);
      for(k=0;k<nsampuse;k++)
      {
        for(j=0;j<nchans;j++) {
          if (nbits == 16) DATA1[k*nchans+j]=DATA4[k*nchans+(nchans-1-j)];
          else DATA3[k*nchans+j]=DATA2[k*nchans+(nchans-1-j)];
        }
      }

      if (nbits == 16) fwrite(DATA1,sizeof(short int),2*nch*nsampuse,output);
      else fwrite(DATA3,sizeof(unsigned char),2*nch*nsampuse,output);
      printf("STEP:%d out of %d steps writing %lld time samples\n",i+1,nstep,(i+1)*nsampuse);
      //      fprintf(flog,"STEP:%d out of %d steps writing %lld time samples\n",i+1,nstep,(i+1)*nsampuse);
    }
    //    fprintf(flog,"Finished\n\n");

    fclose(output);
    fclose(finfileusb);
    //    fclose(flog);
}

