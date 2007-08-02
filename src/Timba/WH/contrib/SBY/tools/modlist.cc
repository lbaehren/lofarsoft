#include <iostream>
#include <cmath>
#include <map>
#include <vector>
#include <iterator>
extern "C" {
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fitsio.h"
#include <wcs.h>
#include <wcsfix.h>
#include <wcshdr.h>
}

/* $Id: modlist.cc 3613 2006-05-30 10:46:29Z sarod $ */
using namespace std;
//#define DEBUG
class tpoint {
	friend ostream &operator<<(ostream &,const tpoint&);
  public:
		long int x;
		long int y;
		double val[4];
		tpoint();
		tpoint(const tpoint&);
		tpoint(long int nx, long int ny, double vl): x(nx),y(ny){val[0]=vl;
		val[1]=val[2]=val[3]=0;}
		tpoint &operator=(const tpoint&rhs);
		int operator==(const tpoint &rhs) const;
		int operator<(const tpoint &rhs) const;
};

tpoint::tpoint() {
x=y=0;
val[0]=0;
val[1]=0;
val[2]=0;
val[3]=0;
}
tpoint::tpoint(const tpoint &copyin) {
 x=copyin.x; y=copyin.y; val[0]=copyin.val[0];
val[1]=copyin.val[1];val[2]=copyin.val[2];val[3]=copyin.val[3];
}

ostream &operator<<(ostream &output, const tpoint &pp) {
		output<<pp.x<<","<<pp.y<<"="<<pp.val[0]<<endl;
		return output;
}

tpoint & tpoint::operator=(const tpoint &rhs) {
		this->x=rhs.x;
		this->y=rhs.y;
		this->val[0]=rhs.val[0];
		this->val[1]=rhs.val[1];
		this->val[2]=rhs.val[2];
		this->val[3]=rhs.val[3];
		return *this;
}

int tpoint::operator==(const tpoint &rhs) const {
		if(this->x !=rhs.x) return 0;
		if(this->y !=rhs.y) return 0;
		if(this->val[0]!=rhs.val[0]) return 0;
		if(this->val[1]!=rhs.val[1]) return 0;
		if(this->val[2]!=rhs.val[2]) return 0;
		if(this->val[3]!=rhs.val[3]) return 0;
		return 1;
}

int tpoint::operator<(const tpoint &rhs) const {
		if (this->x==rhs.x && this->y==rhs.y ) return 1;
		if (this->x==rhs.x && this->y<rhs.y ) return 1;
		if (this->x<rhs.x ) return 1;
		return 0;
}

// for the map less than comparison
struct compare_vec{
 bool operator()(const std::vector<long int> v1, const std::vector<long int> v2) const
 {
  return ((v1[0] < v2[0])
    ||( (v1[0] == v2[0]) && (v1[1] < v2[1])));
 }
};

typedef struct nlimits_ {
		int naxis;
		long int *d; //this is needed for coordinate calculation
		double tol;
		//std::list<tpoint> ll;
		std::map<const std::vector<long int>, tpoint, compare_vec> ll;
		int datatype;
} nlims; 

typedef struct drange_ {
	double lims[2];
	int datatype;
} drange;

extern int get_min_max(long totalrows, long offset, long firstrow, long nrows,
   int ncols, iteratorCol *cols, void *user_struct);
extern int zero_image_float(long totalrows, long offset, long firstrow, long nrows, int ncols, iteratorCol *cols, void *user_struct);


int main(int argc, char *argv[])
{

    fitsfile *fptr;
    iteratorCol cols[3];  /* structure used by the iterator function */
    int n_cols;
    long rows_per_loop, offset;
		nlims arr_dims;
		drange arr_limits;

    int status;
		int naxis;
		int bitpix;

		int datatype=0;
		double bscale,bzero;
		double cutoff=0.9;

    char *filename;     /* name of rate FITS file */

		if (argc==1)  {
				cout<<"Usage: "<<argv[0]<<" [filename] [cutoff]"<<endl;
				cout<<"filename: A FITS image, like the model file created in AIPS++ clean."<<endl;
				cout<<"cutoff: A value between 0 and 1, to indicate the amount of clean components extracted, sorted in flux level."<<endl;
				cout<<"Output: the program will print the clean components line by line."<<endl;
				cout<<"Each line will have RA,DEC (in degrees), I, Q, U, V."<<endl;

				return 0;
		}

		if(argc>1) {
			filename=strdup(argv[1]);
		}else {
			filename=strdup("iter_image.fit");
		}
		if(argc>2) {
			cutoff=strtod(argv[2],0);
			if (cutoff==0)cutoff=0.5;
		}else {
			cutoff=0.5;
		}
    status = 0; 

    fits_open_file(&fptr, filename, READWRITE, &status); /* open file */


		struct wcsprm *wcs;
		char *header;
		int ncard,nreject,nwcs;
		int stat[NWCSFIX];


		/* WCSLIB et al. */
		/* read FITS header */
		if ((status = fits_hdr2str(fptr, 1, NULL, 0, &header, &ncard, &status))) {
			fits_report_error(stderr, status);
			return 1;
	  }

#ifdef DEBUG
	  printf("header %s\n",header);
#endif
	  /* Parse the primary header of the FITS file. */
	  if ((status = wcspih(header, ncard, WCSHDR_all, 2, &nreject, &nwcs, &wcs))) {
				fprintf(stderr, "wcspih ERROR %d\n", status);
		}

		/* Fix non-standard WCS keyvalues. */
		if ((status = wcsfix(7, 0, wcs, stat))) {
			printf("wcsfix ERROR, status returns: (");
			for (int ii = 0; ii < NWCSFIX; ii++) {
			printf(ii ? ", %d" : "%d", stat[ii]);
			}
			printf(")\n\n");
	 	}

	  if ((status = wcsset(wcs))) {
		  fprintf(stderr, "wcsset ERROR %d:\n", status);
		  return 1;
		 }

     /* Print the struct. */
#ifdef DEBUG
		 if ((status = wcsprt(wcs))) return status;
#endif


		fits_get_img_dim(fptr, &naxis, &status);
		if ((arr_dims.d=(long int*)calloc((size_t)naxis,sizeof(long int)))==0) {
			fprintf(stderr,"no free memory\n");
			exit(1);
		}
		/* get axis dimensions */
		fits_get_img_size(fptr, naxis, arr_dims.d, &status);
		arr_dims.naxis=naxis;
		/* get data type */
		fits_get_img_type(fptr, &bitpix, &status);
		if(bitpix==BYTE_IMG) {
			datatype=TBYTE;
		}else if(bitpix==SHORT_IMG) {
			datatype=TSHORT;
		}else if(bitpix==LONG_IMG) {
			datatype=TLONG;
		}else if(bitpix==FLOAT_IMG) {
			datatype=TFLOAT;
		}else if(bitpix==DOUBLE_IMG) {
			datatype=TDOUBLE;
		}

		/* turn off scaling so that we copy the pixel values */
		bscale=1.0; bzero=0.0;
    fits_set_bscale(fptr,  bscale, bzero, &status);


		arr_dims.datatype=datatype;
		arr_limits.datatype=datatype;

    n_cols = 1;

    /* define input column structure members for the iterator function */
    fits_iter_set_file(&cols[0], fptr);
    fits_iter_set_iotype(&cols[0], InputOutputCol);
    fits_iter_set_datatype(&cols[0], 0);

    rows_per_loop = 0;  /* use default optimum number of rows */
    offset = 0;         /* process all the rows */

    /* apply the rate function to each row of the table */
#ifdef DEBUG
    printf("Calling iterator function...%d\n", status);
#endif

    fits_iterate_data(n_cols, cols, offset, rows_per_loop,
                      get_min_max, (void*)&arr_limits, &status);

#ifdef DEBUG
		  printf("Limits Min %lf, Max %lf\n",arr_limits.lims[0],arr_limits.lims[1]);
#endif
			arr_dims.tol=std::abs((1-cutoff)*(arr_limits.lims[1]-arr_limits.lims[0])+arr_limits.lims[0]);

		/* now find the right pixels */
    rows_per_loop = 0;  /* use default optimum number of rows */
    offset = 0;         /* process all the rows */
    /* apply the rate function to each row of the table */
#ifdef DEBUG
    printf("Calling iterator function...%d\n", status);
#endif

    fits_iterate_data(n_cols, cols, offset, rows_per_loop,
                      zero_image_float, (void*)&arr_dims, &status);

#ifdef DEBUG
		cout<<"Found "<<arr_dims.ll.size()<<" Elements"<<endl;
#endif

		int ncoord=arr_dims.ll.size();
		double *pixelc, *imgc, *worldc, *phic, *thetac;
	  int 	*statc;

		if ((pixelc=(double*)calloc((size_t)ncoord*4,sizeof(double)))==0) {
			fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
			return 1;
	  }
		if ((imgc=(double*)calloc((size_t)ncoord*4,sizeof(double)))==0) {
			fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
			return 1;
	  }
		if ((worldc=(double*)calloc((size_t)ncoord*4,sizeof(double)))==0) {
			fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
			return 1;
	  }
		if ((phic=(double*)calloc((size_t)ncoord,sizeof(double)))==0) {
			fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
			return 1;
		}
		if ((thetac=(double*)calloc((size_t)ncoord,sizeof(double)))==0) {
			fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
			return 1;
		}
		if ((statc=(int*)calloc((size_t)ncoord,sizeof(int)))==0) {
			fprintf(stderr,"%s: %d: no free memory\n",__FILE__,__LINE__);
			return 1;
		}

		//fill up the pixel coordinate array
		std::map<const std::vector<long int>,tpoint,compare_vec>::iterator lit=arr_dims.ll.begin();
		int kk=0;
		while(lit!=arr_dims.ll.end()) { //1-indexing
				tpoint &p=lit->second;
				pixelc[kk+0]=(double)p.x+1.0;
				pixelc[kk+1]=(double)p.y+1.0;
				pixelc[kk+2]=(double)1.0;
				pixelc[kk+3]=(double)1.0;
				kk+=4;
				lit++;
		}

		//get the coords
		if ((status = wcsp2s(wcs, ncoord, wcs->naxis, pixelc, imgc, phic, thetac,
			            worldc, statc))) {
				fprintf(stderr,"wcsp2s ERROR %2d\n", status);
				/* Handle Invalid pixel coordinates. */
				if (status == 8) status = 0;
	  }

#ifdef DEBUG
		for(kk=0;kk<ncoord*4; kk+=4) {
			printf("(%lf: %lf) : [%lf:%lf:%lf:%lf] : (%lf,%lf), [%lf:%lf:%lf:%lf] :: %d\n",pixelc[kk+0],pixelc[kk+1],
				imgc[kk+0],imgc[kk+1],imgc[kk+2],imgc[kk+3],phic[kk/4],thetac[kk/4],
				worldc[kk+0],worldc[kk+1],worldc[kk+2],worldc[kk+3],statc[kk/4]
				);

		}
#endif
		lit=arr_dims.ll.begin();
		kk=0;
		cout.precision(9);
		while(lit!=arr_dims.ll.end()) { 
			tpoint &p=lit->second;
      cout<<worldc[kk+0]<<" "<<worldc[kk+1]<<" "<<p.val[0]<<" "<<p.val[1]<<" "<<p.val[2]<<" "<<p.val[3]<<endl;
			lit++;
			kk+=4;
		}
	
    fits_close_file(fptr, &status);      /* all done */

    if (status)
        fits_report_error(stderr, status);  /* print out error messages */

		arr_dims.ll.clear();
		free(arr_dims.d);
		free(pixelc);
		free(imgc);
		free(worldc);
		free(phic);
		free(thetac);
		free(statc);
		wcsfree(wcs);
		free(wcs);
		free(header);
    return(status);
}

int zero_image_float(long totalrows, long offset, long firstrow, long nrows,
   int ncols, iteratorCol *cols, void *user_struct) {
    int ii;

    /* declare counts static to preserve the values between calls */
		/* so it traverses the whole array */
		static char *charp;
		static short int *sintp;
		static long int *lintp;
		static float *fptr;
		static double *dptr;

		static double tmpval;
		static long int pt,d1,d2,d3,d4;

    nlims *arr_dims=(nlims*)user_struct;
		int datatype=arr_dims->datatype;


    /*--------------------------------------------------------*/
    /*  Initialization procedures: execute on the first call  */
    /*--------------------------------------------------------*/
    if (firstrow == 1)
    {
       if (ncols != 1)
           return(-1);  /* number of columns incorrect */
#ifdef DEBUG
    for (ii=0;ii<arr_dims->naxis;ii++) {
			printf("%d %ld\n",ii,arr_dims->d[ii]);
		}
#endif
       /* assign the input pointers to the appropriate arrays and null ptrs*/
    switch (datatype) {
			case TBYTE:
       charp= (char *)  fits_iter_get_array(&cols[0]);
			 break;
		  case TSHORT:
       sintp= (short int*)  fits_iter_get_array(&cols[0]);
       break;
			case TLONG:
       lintp= (long int*)  fits_iter_get_array(&cols[0]);
       break;
			case TFLOAT:
       fptr= (float *)  fits_iter_get_array(&cols[0]);
			 break;
			case TDOUBLE:
       dptr= (double *)  fits_iter_get_array(&cols[0]);
			 
		}
    }

    //printf("limit (%ld,%ld)---(%ld,%ld)\n",xmin,ymin,xmax,ymax); 
    /*--------------------------------------------*/
    /*  Main loop: process all the rows of data */
    /*--------------------------------------------*/

    /*  NOTE: 1st element of array is the null pixel value!  */
    /*  Loop from 1 to nrows, not 0 to nrows - 1.  */

    switch (datatype) {
			case TBYTE:
         for (ii = 1; ii <= nrows; ii++) {
			     //printf("arr =%f\n",counts[ii]);
           //counts[ii] = 1.;
			     tmpval=(double)charp[ii];
			     if ((tmpval)&& arr_dims->tol<=fabs(tmpval)) {
			       //printf("arr =%lf\n",tmpval);
				     /* calculate 4D coords */
				     pt=firstrow+ii-1;
				     //printf("coord point=%ld ",pt);
				     d4=pt/(arr_dims->d[0]*arr_dims->d[1]*arr_dims->d[2]);
				     pt-=(arr_dims->d[0]*arr_dims->d[1]*arr_dims->d[2])*d4;
				     d3=pt/(arr_dims->d[0]*arr_dims->d[1]);
				     pt-=(arr_dims->d[0]*arr_dims->d[1])*d3;
				     d2=pt/(arr_dims->d[0]);
				     pt-=(arr_dims->d[0])*d2;
				     d1=pt;
	
						std::vector<long int> aa(2);
						aa[0]=d1;
						aa[1]=d2;
						if (arr_dims->ll.find(aa)==arr_dims->ll.end()) {
             tpoint p(d1,d2,tmpval);
		         arr_dims->ll[aa]=p;
					  } else {
							tpoint &p=arr_dims->ll[aa];
							//found
							cout<<p<<endl;
						}	

			    }

       }
			 break;

			case TSHORT:
         for (ii = 1; ii <= nrows; ii++) {
			     tmpval=(double)sintp[ii];
			     if ((tmpval)&& arr_dims->tol<=fabs(tmpval)) {
				     pt=firstrow+ii-1;
				     //printf("coord point=%ld ",pt);
				     d4=pt/(arr_dims->d[0]*arr_dims->d[1]*arr_dims->d[2]);
				     pt-=(arr_dims->d[0]*arr_dims->d[1]*arr_dims->d[2])*d4;
				     d3=pt/(arr_dims->d[0]*arr_dims->d[1]);
				     pt-=(arr_dims->d[0]*arr_dims->d[1])*d3;
				     d2=pt/(arr_dims->d[0]);
				     pt-=(arr_dims->d[0])*d2;
				     d1=pt;
				     //printf("coords =(%ld,%ld,%ld,%ld)\n",d1,d2,d3,d4);
						std::vector<long int> aa(2);
						aa[0]=d1;
						aa[1]=d2;
						if (arr_dims->ll.find(aa)==arr_dims->ll.end()) {
             tpoint p(d1,d2,tmpval);
		         arr_dims->ll[aa]=p;
					  } else {
							tpoint &p=arr_dims->ll[aa];
							//found
							cout<<p<<endl;
						}	

			
			    }

       }
			 break;

			case TLONG:
         for (ii = 1; ii <= nrows; ii++) {
			     tmpval=(double)lintp[ii];
			     if ((tmpval)&& arr_dims->tol<=fabs(tmpval)) {
				     pt=firstrow+ii-1;
				     //printf("coord point=%ld ",pt);
				     d4=pt/(arr_dims->d[0]*arr_dims->d[1]*arr_dims->d[2]);
				     pt-=(arr_dims->d[0]*arr_dims->d[1]*arr_dims->d[2])*d4;
				     d3=pt/(arr_dims->d[0]*arr_dims->d[1]);
				     pt-=(arr_dims->d[0]*arr_dims->d[1])*d3;
				     d2=pt/(arr_dims->d[0]);
				     pt-=(arr_dims->d[0])*d2;
				     d1=pt;
				     //printf("coords =(%ld,%ld,%ld,%ld)\n",d1,d2,d3,d4);
						std::vector<long int> aa(2);
						aa[0]=d1;
						aa[1]=d2;
						if (arr_dims->ll.find(aa)==arr_dims->ll.end()) {
             tpoint p(d1,d2,tmpval);
		         arr_dims->ll[aa]=p;
					  } else {
							tpoint &p=arr_dims->ll[aa];
							//found
							cout<<p<<endl;
						}	

			    }

       }
			 break;

			case TFLOAT:
         for (ii = 1; ii <= nrows; ii++) {
			     tmpval=(double)fptr[ii];
					 if (tmpval) {
				     pt=firstrow+ii-1;
				     //printf("coord point=%ld ",pt);
				     d4=pt/(arr_dims->d[0]*arr_dims->d[1]*arr_dims->d[2]);
				     pt-=(arr_dims->d[0]*arr_dims->d[1]*arr_dims->d[2])*d4;
				     d3=pt/(arr_dims->d[0]*arr_dims->d[1]);
				     pt-=(arr_dims->d[0]*arr_dims->d[1])*d3;
				     d2=pt/(arr_dims->d[0]);
				     pt-=(arr_dims->d[0])*d2;
				     d1=pt;
						std::vector<long int> aa(2);
						aa[0]=d1;
						aa[1]=d2;
						if ( (arr_dims->tol<=fabs(tmpval))
									&&(arr_dims->ll.find(aa)==arr_dims->ll.end())) {
             tpoint p(d1,d2,tmpval);
		         arr_dims->ll[aa]=p;
					  } else if (arr_dims->ll.find(aa)!=arr_dims->ll.end()) {
							tpoint &p=arr_dims->ll[aa];
							//found
							p.val[d3]=tmpval;
						}	
					 }

       }
			 break;

			case TDOUBLE:
         for (ii = 1; ii <= nrows; ii++) {
			     tmpval=(double)dptr[ii];
			     if ((tmpval)&& arr_dims->tol<=fabs(tmpval)) {
				     pt=firstrow+ii-1;
				     //printf("coord point=%ld ",pt);
				     d4=pt/(arr_dims->d[0]*arr_dims->d[1]*arr_dims->d[2]);
				     pt-=(arr_dims->d[0]*arr_dims->d[1]*arr_dims->d[2])*d4;
				     d3=pt/(arr_dims->d[0]*arr_dims->d[1]);
				     pt-=(arr_dims->d[0]*arr_dims->d[1])*d3;
				     d2=pt/(arr_dims->d[0]);
				     pt-=(arr_dims->d[0])*d2;
				     d1=pt;
				     //printf("coords =(%ld,%ld,%ld,%ld)\n",d1,d2,d3,d4);
							std::vector<long int> aa(2);
						aa[0]=d1;
						aa[1]=d2;
						if (arr_dims->ll.find(aa)==arr_dims->ll.end()) {
             tpoint p(d1,d2,tmpval);
		         arr_dims->ll[aa]=p;
					  } else {
							tpoint &p=arr_dims->ll[aa];
							//found
							cout<<p<<endl;
						}	
		
			    }

       }
			 break;


		}
    //printf("cols =%d, starting row=%ld, nrows = %ld\n", ncols, firstrow, nrows);
    //printf("limit (%ld,%ld)---(%ld,%ld)\n",xmin,ymin,xmax,ymax); 
		/* set the current limit */
    return(0);  /* return successful status */

}

/* function to read min-max values of fits file */
int get_min_max(long totalrows, long offset, long firstrow, long nrows,
   int ncols, iteratorCol *cols, void *user_struct) {

		static double min_val;
		static double max_val;
		static double tmpval;
		int ii;

    drange *xylims=(drange *)user_struct;
		static char *charp;
		static short int *sintp;
		static long int *lintp;
		static float *fptr;
		static double *dptr;
	  int		datatype=xylims->datatype;


    if (firstrow == 1)
    {
       if (ncols != 1)
           return(-1);  /* number of columns incorrect */
       /* assign the input pointers to the appropriate arrays and null ptrs*/
    switch (datatype) {
			case TBYTE:
       charp= (char *)  fits_iter_get_array(&cols[0]);
			 break;
		  case TSHORT:
       sintp= (short int*)  fits_iter_get_array(&cols[0]);
       break;
			case TLONG:
       lintp= (long int*)  fits_iter_get_array(&cols[0]);
       break;
			case TFLOAT:
       fptr= (float *)  fits_iter_get_array(&cols[0]);
			 break;
			case TDOUBLE:
       dptr= (double *)  fits_iter_get_array(&cols[0]);
			 
		}

		min_val=1e6;
		max_val=-1e6;
    }

   switch (datatype) {
			case TBYTE:
        for (ii = 1; ii <= nrows; ii++) {
			    tmpval=fabs((double)charp[ii]);
		      if (min_val>tmpval) min_val=tmpval;
		      if (max_val<tmpval) max_val=tmpval;
        }
				break;
		  case TSHORT:
        for (ii = 1; ii <= nrows; ii++) {
			    tmpval=fabs((double)sintp[ii]);
					//printf("%lf==%d\n",tmpval,sintp[ii]);
		      if (min_val>tmpval) min_val=tmpval;
		      if (max_val<tmpval) max_val=tmpval;
        }
				break;
			case TLONG:
        for (ii = 1; ii <= nrows; ii++) {
			    tmpval=fabs((double)lintp[ii]);
		      if (min_val>tmpval) min_val=tmpval;
		      if (max_val<tmpval) max_val=tmpval;
        }
				break;
			case TFLOAT:
        for (ii = 1; ii <= nrows; ii++) {
			    tmpval=fabs((double)fptr[ii]);
		      if (min_val>tmpval) min_val=tmpval;
		      if (max_val<tmpval) max_val=tmpval;
        }
				break;
			case TDOUBLE:
        for (ii = 1; ii <= nrows; ii++) {
			    tmpval=fabs((double)dptr[ii]);
		      if (min_val>tmpval) min_val=tmpval;
		      if (max_val<tmpval) max_val=tmpval;
        }
				break;
	 }

		xylims->lims[0]=min_val;
		xylims->lims[1]=max_val;
#ifdef DEBUG
		//printf("min_max: min=%lf max=%lf\n",min_val,max_val);
#endif
		return 0;
}
