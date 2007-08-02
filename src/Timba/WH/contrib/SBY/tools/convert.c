#include <fitsio.h>
#include <stdio.h>
#include <string.h>

#define DEBUG

/* $Id: convert.c 4160 2006-10-17 12:55:35Z sarod $ */
int main(int argc, char **argv) {
       fitsfile *outfptr;
			 FILE *infd;
			 int status;

			 long int naxis, nelements, maxrow;
			 long *naxes;
			 double *arr;
			 long int fpixel=1;
			 long int ii,jj;

			 char **ttype,**tform,**tunit;
			 double **cells;
			 long int firstrow,firstelem;

			 long int totaxs, *real_naxes;
			 int is_complex,has_cells;

   		if (argc==1)  {
				fprintf(stderr,"Usage: %s [infilename] [outfilename]\n",argv[0]);
				fprintf(stderr,"infilename: A text file describing the VellSet. default: 'bp.txt'\n");
				fprintf(stderr,"outfilename: Output FITS file that can be read from FITSReader. default: 'out.fits'\n");
				fprintf(stderr,"If you want to overwrite an existing file, prepend a '!' to the front of the FITS file name like '!out.fits'.\n");
				return 0;
		}

	  char *infile;
	  char *outfile;
		if(argc>1) {
			infile=strdup(argv[1]);
		}else {
			infile=strdup("bp.txt");
		}
		if(argc>2) {
			outfile=strdup(argv[2]);
		}else {
			outfile=strdup("out.fits");
		}

			 infd =fopen( infile , "r" ); 
			 if ( infd == NULL ) {
							   fprintf(stderr, "%s: %d: could not open file %s\n", __FILE__,__LINE__,infile);
								   exit(1);
									 }
			 if ( fscanf(infd, "%ld" , &naxis) == EOF ) {
			    fprintf(stderr, "%s: %d: could not read file %s\n", __FILE__,__LINE__,infile);  
				  exit(1);
			 }
			 printf("axis =%ld\n",naxis);
			 if ((naxes=(long int*)calloc((size_t)naxis,sizeof(long int)))==0) {
			   fprintf(stderr,"no free memory\n");
				 exit(1);
			 }
			 for (ii=0; ii<naxis; ii++) {
			   if ( fscanf(infd, "%ld" , &naxes[ii]) == EOF ) {
			    fprintf(stderr, "%s: %d: could not read file %s\n", __FILE__,__LINE__,infile);  
				  exit(1);
			  }
			 }
			 maxrow=0;
			 for (ii=0; ii<naxis; ii++) {
					if (maxrow<naxes[ii]) maxrow=naxes[ii];
			 }

       /* cells of each axes (centers) */
	  	 if ((cells=(double**)calloc((size_t)naxis,sizeof(double*)))==0) {
				fprintf(stderr,"no free memory\n");
				exit(1);
			 }
			 for (ii=0; ii<naxis; ii++) {
	  	   if ((cells[ii]=(double*)calloc((size_t)(naxes[ii]+1),sizeof(double)))==0) {
				  fprintf(stderr,"no free memory\n");
				  exit(1);
			   }
		   	 cells[ii][0]=naxes[ii];

	  		 for (jj=0; jj<naxes[ii];jj++)  {
   			   if ( fscanf(infd, "%lf" , &cells[ii][jj+1]) == EOF ) {
	    		    fprintf(stderr, "%s: %d: could not read file %s\n", __FILE__,__LINE__,infile);  
		    		  exit(1);
			    }
				 }

			 }

       nelements=1;
			 totaxs=0;
			 for (ii=0; ii<naxis; ii++) {
				if (naxes[ii]) {
         nelements*=naxes[ii];
				 totaxs++;
				}
			 }	

			 if ((arr=(double*)calloc((size_t)nelements,sizeof(double)))==0) {
				fprintf(stderr,"no free memory\n");
				exit(1);
			 }

			 for (ii=0; ii<nelements;ii++)  {
   			   if ( fscanf(infd, "%lf" , &arr[ii]) == EOF ) {
	    		    fprintf(stderr, "%s: %d: could not read file %s\n", __FILE__,__LINE__,infile);  
		    		  exit(1);
			    }
			 }

			 fclose(infd);

#ifdef DEBUG
			 for (ii=0;ii<nelements;ii++) {
					printf("%ld: %lf ",ii,arr[ii]);
			 }
			 printf("\n");
#endif


			 /* now allocate memory for real axes array */
			 if ((real_naxes=(long int*)calloc((size_t)totaxs,sizeof(long int)))==0) {
					fprintf(stderr,"no free memory\n");
					exit(1);
			 }
			 jj=0;
			 for (ii=0; ii<naxis; ii++) {
				if (naxes[ii] && jj<totaxs) {
					real_naxes[jj++]=naxes[ii];
			  }
			 }



			 /* create a nonzero axes array */
			 status=0;
			 
			 fits_create_file(&outfptr,outfile,&status);

			 fits_create_img(outfptr,DOUBLE_IMG,totaxs,real_naxes,&status);

			 fits_write_img(outfptr,TDOUBLE,fpixel,nelements,arr,&status);

			 /* write keywords to header */
			 is_complex=0;
			 fits_update_key(outfptr, TINT, "CPLEX", &is_complex,"Complex data 1: yes 0: no", &status);

			 has_cells=1;
			 fits_update_key(outfptr, TINT, "CELLS", &has_cells,"Has Cells 1: yes 0: no", &status);



			 /* the table to store the cells: one column for one axis */
			 /* the number of rows will be the max axis length */
			 /* define the name, datatype, and physical units for all the columns */
			 if ((ttype=(char**)calloc((size_t)naxis,sizeof(char*)))==0) {
				fprintf(stderr,"no free memory\n");
				exit(1);
			 }
			 if ((tform=(char**)calloc((size_t)naxis,sizeof(char*)))==0) {
				fprintf(stderr,"no free memory\n");
				exit(1);
			 }
			 if ((tunit=(char**)calloc((size_t)naxis,sizeof(char*)))==0) {
				fprintf(stderr,"no free memory\n");
				exit(1);
			 }

			 for (ii=0; ii<naxis; ii++) {
  			 if ((ttype[ii]=(char*)calloc((size_t)6,sizeof(char)))==0) {
  				fprintf(stderr,"no free memory\n");
  				exit(1);
  			 }
  			 if ((tform[ii]=(char*)calloc((size_t)3,sizeof(char)))==0) {
  				fprintf(stderr,"no free memory\n");
  				exit(1);
  			 }
  			 if ((tunit[ii]=(char*)calloc((size_t)1,sizeof(char)))==0) {
  				fprintf(stderr,"no free memory\n");
  				exit(1);
  			 }
				 /* copy column names */
				 sprintf(ttype[ii],"Ax %ld",ii);
				 /* form of data */
				 sprintf(tform[ii],"1E");
				 /*tunit is not needed, so we just null terminate */
				 strcpy(tunit[ii],"\0");
			 }


#ifdef DEBUG
			 for (ii=0; ii<naxis; ii++) {
				printf("axis %ld\n",ii);
	  		 for (jj=0; jj<=naxes[ii];jj++) 
		   			printf(" %lf, ",cells[ii][jj]);
				 printf("\n");

			 }
#endif
      /* only store the absolute value of the first element */
		  for (ii=0; ii<naxis; ii++) {
	  		 for (jj=2; jj<=naxes[ii];jj++) 
		   			cells[ii][jj]-=cells[ii][1];
			 }
#ifdef DEBUG
			 for (ii=0; ii<naxis; ii++) {
				printf("axis %ld\n",ii);
	  		 for (jj=0; jj<=naxes[ii];jj++) 
		   			printf(" %lf, ",cells[ii][jj]);
				 printf("\n");

			 }
#endif


			 /* create table */
			 fits_create_tbl( outfptr, BINARY_TBL, maxrow, naxis, ttype, tform,
																	                tunit, "Cells_TBL", &status);


			 firstrow  = 1;  /* first row in table to write   */
			 firstelem = 1;  /* first element in row  (ignored in ASCII tables) */

			 for (ii=0; ii<naxis; ii++) {
			  fits_write_col(outfptr, TDOUBLE, ii+1, firstrow, firstelem, naxes[ii]+1, cells[ii],
													                    &status);
			 }


			 fits_close_file(outfptr,&status);

			 fits_report_error(stderr,status);
			 
			 for (ii=0; ii<naxis; ii++) {
							 free(ttype[ii]);
							 free(tform[ii]);
							 free(tunit[ii]);
							 free(cells[ii]);
			 }

			 free(real_naxes);
			 free(arr);
			 free(naxes);
			 free(ttype);
			 free(tform);
			 free(tunit);
			 free(cells);
				return 0;
}
