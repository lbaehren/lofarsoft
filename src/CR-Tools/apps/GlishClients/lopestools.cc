/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Andreas Horneffer ()                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <GlishClients/lopestools.h>
#include <Filters/MedianFilter.h>

//--------------------------------------------------------------------------
//
//name: filterroutines
//
//applies fft, weighs the frequencies with gainfactors,
//filters the spectrum by cutting out the peaks, inverse fft 
//(real to complex to real)
//
//input: array with data (timeline vectors): files     
//       number of antennas: numdata                             
//       length of timelines: tsize
//
//       choice of meanfilter (sel=1) or normfilter (sel=0)
//       strength of filter: k (k-mal Standardabweichung)
//       how often filter is applied: f
//
//ouput: array with fft applied: fftarray
//       array with filtered data, overwrites input array: fftarray
//       array with fft back, overwrites input array: files
//
//--------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
 
//applies fft

Matrix<Complex> fft (int numdata,
		     int tsize,
		     float *files)
{
  int n;       
  int fsize=(tsize+2)/2;                
  FFTServer<Float,Complex> fftserv(IPosition(1,tsize),FFTEnums::REALTOCOMPLEX);  
  Vector<Float> ary(tsize);
  Vector<Complex> fftary(fsize);
  Matrix<Complex> fftarray(numdata,fsize); 
 
  //applies fft
  for (n=0; n<numdata; n++)
    { 
      ary = Vector<Float>(IPosition(1,tsize),files+(n*tsize));
      fftserv.fft(fftary,ary);
      fftarray.row(n) = fftary;
    }

  printf("fft applied\n");
  return fftarray;

}

/////////////////////////////////////////////////////////////////////////////

float *calc_cal_vals(int numdata, int fsize, float *calfreq, float *calvalue,
		     int calsize)

{

  int n,f,t;
  float *erg;

  //get tempoary memory
  erg = (float *)malloc(numdata*fsize*sizeof(float));

  //linear interpolation
  for (n=0; n<numdata; n++)
    {
      f=0;
      for (t=0; t<calsize; t++)
	{
	  for (; f<(fsize*calfreq[t+1]); f++)
	    {
	      erg[n*fsize+f] = calvalue[n*calsize+t] + (calvalue[n*calsize+t+1]-calvalue[n*calsize+t])*
		((float)f/fsize-calfreq[n*calsize+t])/(calfreq[n*calsize+t+1]-calfreq[n*calsize+t]);
	    }
	}
    }

  printf("gainfactors interpolated\n");
  return erg;

}	

/////////////////////////////////////////////////////////////////////////////

//application of gainfactors, overwrites input fftarray

Matrix<Complex> fftgain(int numdata, int fsize, float *erg, Matrix<Complex> fftarray)

{

  int n,f;

  for (n=0; n<numdata; n++)
    {
      for (f=0; f<fsize; f++)
	{
	  fftarray(n,f) *= erg[n*fsize+f];
	}
    }

  printf("gainfactors applied\n");
  return fftarray;

}

/////////////////////////////////////////////////////////////////////////////

//choice of filter and its strength, computes mask

int *gen_mask(int numdata, int fsize, Matrix<Complex> fftarray, char f, char k, char sel)

{ int i,m,n,t,nsum;
  double j,x,var,stad;
  int *mask;                        

  mask = (int*)malloc(fsize*sizeof(int));

  //choice of k and f (strength of filter)
  //  printf("Which strength of filter?\n");
  //  printf("Your choice: ");
  //  k = getchar();
  //  printf("How many times shall filter be applied?\n");
  //  printf("Times: "); 
  //  f = getchar();  

  //initialize mask
  for (t=0; t<fsize; t++)
    {
      mask[t] = 1;
    }

  //looprunning over all antennas
  for (n=0; n<numdata; n++)
    {

      //Normfilter sel=0
      if (sel == 0)
	{
          //application of filter f-times
          for (i=0; i<f; i++)
	    {

              //Mittelwert und Standardabweichung der mittleren 80%
              j = 0; x=0; nsum =0;
              for (t = fsize/10; t<fsize-(fsize/10); t++)
                { 
		  j += fabs(fftarray(n,t));
		  x += fabs(fftarray(n,t))*fabs(fftarray(n,t));
		  nsum++;
                }
              j /= nsum;
              var = x/nsum - j*j;  //Varianz
              stad = sqrt(var);    //Standardabweichung   

              //Peaks über Mittelwert+kmalStandardabweichung auf 0 setzen
              for (t = 0; t<fsize; t++)
                {
                  if (fabs(fftarray(n,t)) > j+(k*stad))
	            {
	              mask[t] = 0;
		      fftarray(n,t) *= 0;
	            }
                }

	    }
	}

      //Meanfilter sel=1
      else if (sel == 1)
	{
          if (fsize<200)
	    {
	      printf("Files are too small!\n");
	      return mask;
	    }
	  //application of filter f-times
          for (i=0; i<f; i++)
	    {
              for (t = 50; t<fsize-50; t++)
		{
		  //laufenden Mittelwert und Standardabweichung berechnen
		  j = 0;x=0;
	          //benachbarte 100 Werte berücksichtigen
	          for (m=t-50; m<t+50; m++)
	            {
	               j += fabs(fftarray(n,m));
	               x += fabs(fftarray(n,m))*fabs(fftarray(n,m));
	            }
	          j /= 100;
                  var = x/100 - j*j;    //Varianz
                  stad = sqrt(var);     //Standardabweichung  
	          if (fabs(fftarray(n,t)) > j+(k*stad))
	            {
	               mask[t] = 0;
	            }	
		}
	      for (t = 50; t<fsize-50; t++)
		{
		  fftarray(n,t) *= mask[t];
		}
	    }
	  for (t=0; t<51; t++)
	    {
	      mask[t] = 0;
	    }
          for (t=fsize-50; t<fsize; t++)
	    {
	      mask[t] = 0;
	    }
	}

    }

  printf("filtermask computed\n");
  return mask;
  
}

///////////////////////////////////////////////////////////////////////////

//applies filter

Matrix<Complex> fftfilter(int numdata, int fsize, Matrix<Complex> fftarray, int *mask)

{

  int t,n,nsum;

  nsum = 0;
  for (t = 0; t<fsize; t++) if(mask[t]==0) nsum++;
//  cout << "Percentage filtered: " << nsum*100./fsize << "%" << endl; 

  for (n=0;n<numdata;n++)
    {
      for (t = 0; t<fsize; t++)
        {
          fftarray(n,t) *= mask[t];
        }
    } 
 
  printf("Data filtered!\n");
  return fftarray;

}

////////////////////////////////////////////////////////////////////////////

//fft back

float  *fftback(int numdata, int tsize, int fsize, Matrix<Complex> fftarray, float *files)

{

  int n,t; 
  Vector<Float> ary(tsize);
  Vector<Complex> fftary(fsize);                     
  FFTServer<Float,Complex> fftserv(IPosition(1,tsize),FFTEnums::REALTOCOMPLEX);  

  for (n=0; n<numdata; n++)
   { 
     fftserv.fft(ary,fftarray.row(n));
     for (t=0; t<tsize; t++)
       {
	 files[(n*tsize)+t] = ary(t);
       }
   }
 
  printf("fft back completed\n");
  return files;

}


// =============================================================================


float *do_fftfilter(int numdata, int tsize, float *files, int f, int k, int sel)

{
  int i,n,m,t,nsum,fsize=((tsize+2)/2);
  double j,x,var,stad;
  Vector<Float> ary(tsize);
  Vector<Complex> fftary((tsize+2)/2);
  Matrix<Complex> fftarray(numdata,((tsize+2)/2));
  int mask[fsize];                       
  FFTServer<Float,Complex> fftserv(IPosition(1,tsize),FFTEnums::REALTOCOMPLEX);  
  
  //mask for filter
  for (t=0; t<fsize; t++)
    {
      mask[t] = 1;
    }

  //apply fft
  for (n=0; n<numdata; n++)
    { 
      ary = Vector<Float>(IPosition(1,tsize),files+(n*tsize));
      fftserv.fft(fftary,ary);
      fftarray.row(n) = fftary;
    }

  //looprunning over all antennas
  for (n=0; n<numdata; n++)
    {

      //Normfilter sel=0
      if (sel == 0)
	{
          //application of filter f-times
          for (i=0; i<f; i++)
	    {
              //Mittelwert und Standardabweichung der mittleren 80%
              j = 0; x=0; nsum =0;
              for (t = fsize/10; t < fsize-(fsize/10); t++)
                { 
		  j += fabs(fftarray(n,t));
		  x += fabs(fftarray(n,t))*fabs(fftarray(n,t));
		  nsum++;
                }
              j /= nsum;
              var = x/nsum - j*j;  //Varianz
              stad = sqrt(var);             //Standardabweichung   

              //Peaks über Mittelwert+kmalStandardabweichung auf 0 setzen
              for (t = 0; t<fsize; t++)
                {
                  if (fabs(fftarray(n,t)) > j+(k*stad))
	            {
	              mask[t] = 0;
                      for (m=0; m<numdata; m++)
                        {
		          fftarray(m,t) *= 0;
                        }
	            }
                }
	    }
	}

      //Meanfilter sel=1
      else if (sel == 1)
	{
          if (fsize<200)
	    {
	      printf("Files are too small!\n");
	      return files;
	    }
	  //application of filter f-times
          for (i=0; i<f; i++)
	    {
              for (t = 50; t<fsize-50; t++)
		{
		  //laufenden Mittelwert und Standardabweichung berechnen
		  j = 0;x=0;
	          //benachbarte 100 Werte berücksichtigen
	          for (m=t-50; m<t+50; m++)
	            {
	               j += fabs(fftarray(n,m));
	               x += fabs(fftarray(n,m))*fabs(fftarray(n,m));
	            }
	          j /= 100;
                  var = x/100 - j*j;  //Varianz
                  stad = sqrt(var);     //Standardabweichung  
	          if (fabs(fftarray(n,t)) > j+(k*stad))
	            {
	               mask[t] = 0;
	            }	
		}
	      for (t = 50; t<fsize-50; t++)
		{
		  fftarray(n,t) *= mask[t];
		}
	    }
	  for (t=0; t<51; t++)
	    {
	      mask[t] = 0;
	    }
          for (t=fsize-50; t<fsize; t++)
	    {
	      mask[t] = 0;
	    }
	}

    }

  nsum =0;
  for (t = 0; t<fsize; t++) if(mask[t]==0) nsum++;
//  cout << "Percentage filtered: "<< nsum*100./fsize <<"%"<<endl; 

  for (n=0;n<numdata;n++)
    {
      for (t = 0; t<fsize; t++)
        {
          fftarray(n,t) *= mask[t];

        }
    } 
 
  //fft back
  for (n=0; n<numdata; n++)
   { 
     fftserv.fft(ary,fftarray.row(n));
     for (t=0; t<tsize; t++)
       {
	 files[(n*tsize)+t] = ary(t);
       }
   }
 
  return files;
}


//-----------------------------------------------------------------------------
//
//name: fftfilter
//      frequency-filter for all antennas
//applies fft and filters the spectrum by cutting out the peaks, inverse fft
//  (real to complex to real)
//
//return: array with filtered data
//input: array    = (2 dim) array with data
//       sel      = which kind of filter to use
//       strength = strength of filter: k (k-mal Standardabweichung)
//       f        = how often filter is applied
//-----------------------------------------------------------------------------

Bool fftfiler(GlishSysEvent &event, void *) {
  GlishSysEventSource *glishBus = event.glishSource();
  int numdata,tsize;
  float *files; 
  int strength=3,f=3,sel=0;
  Matrix<Float> array;
  Bool deleteIt;

  if (event.val().type() != GlishValue::RECORD) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  GlishRecord  inrec = event.val();
  if (! (inrec.exists("array")) ) {
//    cerr << "Need record with: array" << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
     };
    return True;
  };
  GlishArray gtmp;
  gtmp = inrec.get("array");
  gtmp.get(array);
  numdata = array.ncolumn();
  tsize = array.nrow();
//cout << "Data for " << numdata << " antennas with " << tsize << " samples"<<endl;
  if ( inrec.exists("strength") ) {
    gtmp = inrec.get("strength");
    gtmp.get(strength);
    if ((strength < 1) ||(strength > 1000)) {strength=3;};
  };
  if ( inrec.exists("sel") ) {
    gtmp = inrec.get("sel");
    gtmp.get(sel);
    if ((sel < 0) ||(sel > 1)) {sel=0;};
  };
  if ( inrec.exists("f") ) {
    gtmp = inrec.get("f");
    gtmp.get(f);
  };
  
  files = array.getStorage(deleteIt);
  //  do_fftfilter(numdata, tsize, files, f, strength, sel);
  Matrix<Complex> fftdata;
  fftdata = fft(numdata, tsize, files);
  //  float *gainvals;
  //  gainvals = calc_cal_vals(int numdata, int fsize, float *calfreq, float *calvalue, int calsize);
  //  fftdata = fftgain(int numdata, int fsize, float *erg, Matrix<Complex> fftarray);
  int *mask;
  mask = gen_mask(numdata, (tsize+2)/2 , fftdata, f, strength, sel);
  fftdata = fftfilter(numdata, (tsize+2)/2, fftdata, mask);
  fftback(numdata, tsize, (tsize+2)/2, fftdata, files);
  array.putStorage(files, deleteIt);

  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(array));
  };
  return True;
};


// =============================================================================


float *do_calc_meanpower(int nch, char **files, int bsize, char w)
{

  int k,i,j,n,numblocks;
  FILE **fds;
  struct stat statbuf;
  float *erg;
  Vector<Float> ary(bsize);
  Vector<Complex> fftarray(((bsize+2)/2));
  short *shpoint;
  float *flpoint;
  float *windowary,norm;
  FFTServer<Float,Complex> fftserv(IPosition(1,bsize),FFTEnums::REALTOCOMPLEX);  
  k = (bsize/2) + 1;
  erg = (float *)malloc(nch*sizeof(float)*k);  //return,memory for field with data
  windowary = (float *)malloc(sizeof(float)*bsize);
  fds = (FILE **)malloc(nch*sizeof(FILE *));
  
  norm = 0;
  for (i=0; i<bsize; i++)            //compute windowary, normfactor
  {
    if(w == 'W')  windowary[i] = 1 - ((i-(bsize/2))/(bsize/2))*((i-(bsize/2))/(bsize/2));
      //Welch window
    else if(w == 'N') windowary[i] = 1;
      //Norm window
    else if(w == 'B') windowary[i] = 1 - abs((i - (bsize/2))/(bsize/2));
      //Bartlett window
    else windowary[i] = 1;     
      //No window  
    norm += (windowary[i]*windowary[i]);
  };
  norm *= bsize;
  
  //get tempoary memory
  shpoint = (short *)malloc(bsize*2);              
  flpoint = (float *)malloc(bsize*sizeof(float));
  
  //looprunning over datasets 
  for (n=0; n<nch; n++)                   
  {
    if (stat(files[n],&statbuf)!=0)              //calc the number of blocks 
    { 
//      cerr << "Can't open File " << files[n] << endl;
      return erg; 
    };
    numblocks = (statbuf.st_size/2)/bsize; 
    fds[n] = fopen(files[n], "rb");              //Open the files
    if (fds[n] == NULL)
    {
//      cerr << "Can't open Files" << files[n] << endl;
      return erg;
    };
    for (i=0; i<k; i++)
    {
      erg[i+n*k] = 0.;
    }; 
    for (j=0; j<numblocks; j++)
    {
      fread(shpoint, 2, bsize, fds[n]);          //get data
      for (i=0; i<bsize; i++)
      {
        flpoint[i] = shpoint[i]*windowary[i];    //apply windowary, fft
      };
      ary = Vector<Float>(IPosition(1,bsize),flpoint);
      fftserv.fft(fftarray,ary);
     
      for (i=0; i<k; i++)              
      {
        erg[i+n*k] += square(2*abs( fftarray(k-i-1) )); 
      };
    };      
    for (i=0; i<k; i++)
    {
      erg[i+n*k] = erg[i+n*k]/numblocks/norm;
    };
    fclose(fds[n]);
  };
  free(shpoint);
  free(windowary);
  free(flpoint);
  free(fds); 
  return erg;
}
  
//-----------------------------------------------------------------------
//Name: calc_meanpower
//
//Type: Event Callback Function 
//Doc: calculate meanpower by averaging over all blocks 
//ret: Array with the data
//inp: nch    = number of (antenna) channels
//     files  = array of filesnames
//     bsize  = size of one block;
//     window = index for choice of windows
//-----------------------------------------------------------------------
Bool calc_meanpower(GlishSysEvent &event, void *) {
  GlishSysEventSource *glishBus = event.glishSource();
  Int tmpint,k;
  Vector<String> tmpstring;
  String windowstring;
  int i, nch, bsize;
  char **files, window='W';
  
  if (event.val().type() != GlishValue::RECORD) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  GlishRecord  inrec = event.val();
  if (! (inrec.exists("nch")&&inrec.exists("files")&&inrec.exists("bsize")) ) {
//    cerr << "Need record with: nch, files, bsize" << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
     };
    return True;
  };
  GlishArray gtmp;
  gtmp = inrec.get("nch");
  gtmp.get(tmpint,0);
  nch = tmpint;
  gtmp = inrec.get("bsize");
  gtmp.get(tmpint,0);
  bsize = tmpint;
  if ( inrec.exists("window") ) {
    gtmp = inrec.get("window");
    gtmp.get(windowstring,True);
    window = windowstring.c_str()[0];
  };
  files = (char **)calloc(nch,sizeof(char *));
  gtmp = inrec.get("files");
  gtmp.get(tmpstring,True);
  for (i=0;i<nch;i++) {
    files[i] = (char *)malloc(100);
    strncpy(files[i],tmpstring(i).c_str(),100);
  };
  
  float *ergp;
  ergp = do_calc_meanpower(nch, files, bsize, window);
  k = (bsize/2) + 1;
  GlishArray erg = Matrix<float>(IPosition(2,nch,k),ergp,SHARE);
  if (glishBus->replyPending()) {
    glishBus->reply(erg);
  };
  for (i=0;i<nch;i++) {
    free(files[i]);
  };
  free(files);free(ergp);
  return True;
}


// =============================================================================


float *do_calc_pixel(int nch, char **files, int *delays, int bsize, int nfreq){
  int h,i,j,maxdelay,numblocks;
  FILE **fds;
  struct stat statbuf;
  float *erg;
  Vector<Float> ary(bsize);
  Vector<Complex> fftarray(((bsize+2)/2));
  short *shpoint;
  float *flpoint, *window, nchfakt, normfactor;
  FFTServer<Float,Complex> fftserv(IPosition(1,bsize),FFTEnums::REALTOCOMPLEX);

  if (nch>16) {
//    cout << "NChannels > 16! Short to Double conversion may be wrong!" << endl;
  };

  erg = (float *)malloc(nfreq*sizeof(float));
  fds = (FILE **)malloc(nch*sizeof(FILE *));
  //make delays positive;
  j=0;
  for (i=0; i< nch; i++) {
    if (delays[i] < j) j=delays[i];
  };
  maxdelay=0;
  for (i=0; i< nch; i++) {
    delays[i] -= j;
    if (delays[i] > maxdelay) maxdelay=delays[i];
  };
  //calc the number of blocks
  numblocks=1000000;
  for (i=0; i< nch; i++) {
    if (stat(files[i],&statbuf)!=0) {
//      cerr << "cant open File " << files[i] << endl;
      return erg;
    };
    j = (statbuf.st_size/2 - maxdelay )/bsize;
    if (j<numblocks) numblocks=j;
  };
  if (numblocks <1) {
//    cerr << "File(s) are to small! (numblocks <1)" << endl;
    return erg;
  };
  //  cout << "Calculating pixel with " << numblocks << " blocks" <<endl;
  //Open the files, get tempmem,
  for (i=0; i< nch; i++) {
    fds[i] = fopen(files[i],"rb");
    if (fds[i] == NULL) {
//      cerr << "cant open File " << files[i] << endl;
      return erg;
    };
    fseek(fds[i],(delays[i]*2),SEEK_SET);
  };
  shpoint = (short *)malloc(bsize*2);
  flpoint = (float *)malloc(bsize*sizeof(float));
  window  = (float *)malloc(bsize*sizeof(float));

  normfactor =0;
  for (j=0;j<bsize;j++){
    window[j] = ((j-(bsize/2.))/bsize*2.);
    window[j] =  1-(window[j]* window[j]);
    normfactor += window[j]*window[j];
  };
  normfactor *= bsize;
  nchfakt = 1./(float)nch;

  //Calc the data
  for (j=0;j<nfreq;j++){
    erg[j] = 0.;
  };
  for (h=0;h<numblocks;h++){
    for (j=0;j<bsize;j++){
      flpoint[j] =0.;
    };
    for (i=0; i< nch; i++) {
      fread(shpoint, 2, bsize, fds[i]);
      for (j=0;j<bsize;j++){
	flpoint[j] += shpoint[j];
      };
    };
    for (j=0;j<bsize;j++){
      flpoint[j] *= window[j]*nchfakt;
    };
    ary = Vector<Float>(IPosition(1,bsize),flpoint);
    fftserv.fft(fftarray,ary);
    i=(bsize+2)/2;
    for(j=0;j<nfreq;j++){
      erg[j] += mean(square(amplitude(fftarray(Slice((j*i/nfreq),(i/nfreq))))));
    };

  };
  for (j=0;j<nfreq;j++){
    erg[j] /= (float)numblocks*normfactor;
  };

  for (i=0; i< nch; i++) {
    fclose(fds[i]);
  };
  free(shpoint);free(flpoint);free(fds);free(window);
  return erg;
}

//-----------------------------------------------------------------------
//Name: calc_pixel
//
//Type: Event Callback Function
//Doc: calculate
//ret: Array with the data
//inp: nch    = number of (antenna) channels
//     files  = array of filesnames
//     delays = array with the delays for this pixel;
//     bsize  = size of one block;
//     nfreq  = number of frequency channels
//-----------------------------------------------------------------------
Bool calc_pixel(GlishSysEvent &event, void *) {
  GlishSysEventSource *glishBus = event.glishSource();
  Int tmpint;
  Vector<String> tmpstring;
  int i, nch, bsize, nfreq, *delays;
  char **files;

  if (event.val().type() != GlishValue::RECORD) {
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
    };
    return True;
  };
  GlishRecord  pixdesc = event.val();
  if (! (pixdesc.exists("nch")&&pixdesc.exists("files")&&pixdesc.exists("delays")&&pixdesc.exists("bsize")&&pixdesc.exists("nfreq")) ) {
//    cerr << "Need record with: nch, files, delays, bsize, nfreq" << endl;
    if (glishBus->replyPending()) {
      glishBus->reply(GlishArray(False));
     };
    return True;
  };
  GlishArray gtmp;
  gtmp = pixdesc.get("nch");
  gtmp.get(tmpint,0);
  nch = tmpint;
  gtmp = pixdesc.get("bsize");
  gtmp.get(tmpint,0);
  bsize = tmpint;
  gtmp = pixdesc.get("nfreq");
  gtmp.get(tmpint,0);
  nfreq = tmpint;
  delays = (int *)calloc(nch,sizeof(int));
  files = (char **)calloc(nch,sizeof(char *));
  gtmp = pixdesc.get("delays");
  for (i=0;i<nch;i++) {
    gtmp.get(tmpint,i);
    delays[i] = tmpint;
  };
  gtmp = pixdesc.get("files");
  gtmp.get(tmpstring,True);
  for (i=0;i<nch;i++) {
    files[i] = (char *)malloc(100);
    strncpy(files[i],tmpstring(i).c_str(),100);
  };

  float *ergp;
  ergp = do_calc_pixel(nch, files, delays, bsize, nfreq);
  GlishArray erg = Vector<float>(IPosition(1,nfreq),ergp,SHARE);

  if (glishBus->replyPending()) {
    glishBus->reply(erg);
  } else {
    GlishRecord ergrec;
    ergrec.add("delays",pixdesc.get("delays"));
    ergrec.add("id",pixdesc.get("id"));
    ergrec.add("pixdata",erg);
    glishBus->postEvent("pixeldata",ergrec);
  };
  for (i=0;i<nch;i++) {
    free(files[i]);
  };
  free(delays);free(files);free(ergp);
  return True;
}

//-----------------------------------------------------------------------
//Name: avgstdfilter
//
//Type: Event Callback Function
//Doc: apply a meanvalue filter (including variance) to the data
//ret-data: GlishArray with the mean, GlishArray with the variance
//inp-data: filtrec =    Record with the data
//            filtrec.ary      = data to be filtered
//            filtrec.strength = strength of the filter (default =3)
//-----------------------------------------------------------------------

Bool doavgstd(Vector<Double> ary, Vector<Double> retmar, Vector<Double> retstd, int bs) {
//printf("<runavgstdc++>\n");

   // variables
   int size = ary.nelements();
   Vector<Double> mary(size+bs-1);
   Vector<Double> sary(size+bs-1);

   // array & blocksize
   if (bs > size || bs < 1) {
     return False;
   }

   // outary start values
   mary(0) = ary(0);
   mary(size+bs-2) = ary(size-1);
   sary(0) = 0;
   sary(size+bs-2) = 0;

   // first & last block
//printf("<first & last block>\n");
   for (int i = 1; i < bs; i ++) {
     // average first block
     mary(i)           = (mary(i-1)*i+ary(i))/(double)(i+1);
     // average last block
     mary(size+bs-2-i) = (mary(size+bs-1-i)*i+ary(size-1-i))/(double)(i+1);
     // stddev first & last block (-M_x²)
     //sary(i)           = -mary(i)*mary(i)*i;
     //sary(size+bs-2-i) = -mary(size+bs-2-i)*mary(size+bs-2-i)*i;
     for (int j = 0; j < (i+1); j ++) {  // (X_i²-M_x²)
       //sary(i)           += ary(j)*ary(j);
       //sary(size+bs-2-i) += ary(size-1-j)*ary(size-1-j);
       sary(i)           += (ary(j)-mary(i))*(ary(j)-mary(i));
       sary(size+bs-2-i) += (ary(size-1-j)-mary(size+bs-2-i))*(ary(size-1-j)-mary(size+bs-2-i));
     } // (X_i²-M_x²)/N!!!
     sary(i)           /= (double)(i+1); // (X_i²-M_x²)/(N-1)!!!: /= (double)i
     sary(size+bs-2-i) /= (double)(i+1); // (X_i²-M_x²)/(N-1)!!!: /= (double)i
   }

   // main (middle) blocks
//printf("<main blocks>\n");
   for (int i = bs; i < size; i ++) {
     mary(i) = mary(i-1)+(ary(i)-ary(i-bs))/(double)(bs);
     sary(i) = sary(i-1)+mary(i-1)*mary(i-1)-mary(i)*mary(i)+
               (ary(i)*ary(i)-ary(i-bs)*ary(i-bs))/(double)(bs);
   }

   // sigma = sqrt (varianz)
   sary=sqrt(abs(sary)); // sigma = sqrt( | (X_i²-M_x²)/N | )

   // printing of values
   //for (int i = 0; i < size+bs-1; i ++) {
   //  printf("%02d: %9f %9f\n", i, mary(i), sary(i));
   //}

   retmar = mary(Slice(bs/2,bs/2+size-1));
   retstd = sary(Slice(bs/2,bs/2+size-1));

//printf("<finished>\n");
   return True;
}

//-----------------------------------------------------------------------
//Name: avgstdfilter
//
//Type: Event Callback Function
//Doc: apply a meanvalue filter (including variance) to the data
//ret-data: GlishArray with the mean, GlishArray with the variance
//inp-data: filtrec =    Record with the data
//            filtrec.ary      = data to be filtered
//            filtrec.strength = strength of the filter (default =3)
//-----------------------------------------------------------------------

Bool avgstdfilter(GlishSysEvent &event, void *) {
   GlishSysEventSource *glishBus = event.glishSource();

   if (event.val().type() != GlishValue::RECORD) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     }
     return True;
   }
   GlishRecord filtrec = event.val();
   GlishRecord ret;
   // get data; mandatory!
   if (! filtrec.exists("ary") ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     }
     return True;
   }
   GlishArray gtmp = filtrec.get("ary");
   Vector<Double> ary;
   gtmp.get(ary);
   int size = ary.nelements();
   int bs=10;
   if ( filtrec.exists("bs") ) {
     gtmp = filtrec.get("bs");
     gtmp.get(bs);
   }

   Vector<Double> mary(size);
   Vector<Double> sary(size);
   if (!doavgstd(ary, mary, sary, bs)) {
     return False;
   }

   GlishRecord retval;
   retval.add("mary",mary);
   retval.add("sary",sary);
   if (glishBus->replyPending()) {
     glishBus->reply(retval);
   };
   return True;
}

//-----------------------------------------------------------------------
//Name: findPeaksInData
//
//Type: Event Callback Function
//Doc: apply a meanvalue filter (including variance) to the data
//ret-data: GlishArray with the mean, GlishArray with the variance
//inp-data: filtrec =    Record with the data
//            filtrec.ary      = data to be filtered
//            filtrec.strength = strength of the filter (default =3)
//-----------------------------------------------------------------------
/*
Bool findPeaksInData(GlishSysEvent &event, void *) {
   GlishSysEventSource *glishBus = event.glishSource();

   if (event.val().type() != GlishValue::RECORD) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     }
     return True;
   }
   GlishRecord filtrec = event.val();
   GlishRecord ret;
   // get data; mandatory!
   if (! filtrec.exists("ary") ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     }
     return True;
   }

    # x and y array should have the same number of elements
    if (len(xary) != len(yary)) {
      fail "x and y array length mismatch.";
    }

    ispeak := [];

    # Calculate running average. (Set blocksize equal to the number of data points to
    # get a flat average.)
    mean   := as_double(yary);
    var    := as_double(yary);
print '\\begin{c}'
    runningavgvar(yary,mean,var,bs);
print '\\end{c}'

    # determine peaks
    reto := [=];
    reto.x := array(0.,100);
    reto.y := array(0.,100);
    reto.s := array(0.,100);
    ispeak := (yary - mean)/var;
    _peaks := 0;
    _set   := 0;
    i := 0;
print '\\begin{ispeak}'
    while (i < len(yary)) {
      i +:= 1;
      if (ispeak[i] > sens) {
        # group peaks next to each other
        _set +:= 1;
        if (_set == 1) {
          _peaks +:= 1;
        }
      } else {
        # when a group is finished, give its data
        if (_set != 0) {
          x := as_double(i)-as_double(_set)/2.-.5;
          y := as_double(sum(yary[[(i-1):(i-_set)]]))/as_double(_set);
          t := as_double(xary[i-1]+xary[i-_set])/2.;
          reto.x[_peaks] := t;
          reto.y[_peaks] := y;
          reto.s[_peaks] := ispeak[x];
          _set := 0;
        }
      }
      if (_peaks >= 100) {
        print "# *** Maximum number of peaks exceeded ***";
        i := len(yary);
      }
    }
print '\\end{ispeak}'
    # write some statistics of the data set
    reto.count := _peaks;
    val obj := reto;
} 
*/

// ------------------------------------------------------------------ mediafilter

Bool meanfilter (GlishSysEvent &event, void *)
{
   GlishSysEventSource *glishBus = event.glishSource();
   int i (0);

   // ----------------------------------------------------------------
   // Check if the value sent with the event is a GlishRecord

   if (event.val().type() != GlishValue::RECORD) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };

   // ----------------------------------------------------------------
   // Extract the data from the record fields

   GlishRecord glishRec = event.val();
   GlishArray gtmp;
   Vector<Double> ary;

   // Field: ary -- contains input data and is mandatory
   if (glishRec.exists("ary")) {
     gtmp = glishRec.get("ary");
     gtmp.get(ary);
   } else {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     }
     return True;    
   }

   int size (ary.nelements());
   int strength (3);

   // Field: strength -- range over which the mean is computed
   if ( glishRec.exists("strength") ) {
     gtmp = glishRec.get("strength");
     gtmp.get(strength);
     if ((strength < 1) ||(strength > size)) {
       strength=1;
     }
   };

   // ----------------------------------------------------------------
   // Apply the mean filter to the data

   Matrix<Double> mary((size+strength),strength);
   for (i=0; i<strength; i++) {
     mary(Slice(i,size),Slice(i)) = ary;
   };

   for (i=0; i<size; i++) {
     ary(i) = mean(mary.row(i+strength/2));
   };

   // ----------------------------------------------------------------
   // Set up record to to return the results

   GlishArray retval (ary);
   //retval = partialMeans(mary,IPosition(1,1))(IPosition(1,(strength/2)),IPosition(1,(size+/2)));

   if (glishBus->replyPending()) {
     glishBus->reply(retval);
   }
   return True;
}

//-----------------------------------------------------------------------
//Name: medianfilter
//
//Type: Event Callback Function
//Doc: apply a median filter to the data
//ret-data: GlishArray with the data
//inp-data: filtrec =    Record with the data
//            filtrec.ary      = data to be filtered
//            filtrec.strength = strength of the filter (default =3)
//-----------------------------------------------------------------------

Bool medianfilter(GlishSysEvent &event, void *)
{
   GlishSysEventSource *glishBus = event.glishSource();
   int filterStrength (3);     // Filter strength
   int size (filterStrength);  // Filter size, nof samples in a block of data

   if (event.val().type() != GlishValue::RECORD) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishRecord filtrec = event.val();
   // get data; mandatory!
   if (! filtrec.exists("ary") ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishArray gtmp = filtrec.get("ary");
   Vector<Double> ary;
   gtmp.get(ary);
   size = ary.nelements();
   // get filter strength, i.e. number of samles over which to average
   if ( filtrec.exists("strength") ) {
     gtmp = filtrec.get("strength");
     gtmp.get(filterStrength);
     if ((filterStrength < 1) ||(filterStrength > size)) {
       filterStrength=1;
     }
   };

   // --- new code: use the MedianFilter class ---------------------------------

   MedianFilter<Double> medianFilter (filterStrength);
   Vector<Double> result = medianFilter.filter(ary);

   // --- return the computation result ----------------------------------------

   GlishArray retval (result);

   if (glishBus->replyPending()) {
     glishBus->reply(retval);
   };
   return True;
}

//#######################################################################
//-----------------------------------------------------------------------
//Name: runmin
//-----------------------------------------------------------------------
//Type: Event Callback Function
//Doc:  running minimum for array
//inp:  ary    = input array
//      bs     = blocksize
//ret:  outary = output GlishArray
//#######################################################################
//c++ function code
//-----------------------------------------------------------------------
Bool dorunmin(Vector<Double> ary, int bs) {
//printf("<runminc++>\n");

   // variables
   int arylen=ary.nelements();
   Vector<Double> outary(arylen);

   // array & blocksize
   if (bs >= arylen || bs < 1) {return False;}
//printf("bs=%d\n", bs);
//printf("arylen=%d\n", arylen);

   // block limits
   int left=int(floor((bs-1)/2.));
   int right=int(ceil((bs-1)/2.));
//printf("left=%d\n",left);
//printf("right=%d\n",right);

   // first block
//printf("<first block>\n");
   for (int i = 0; i < left; i ++) {
     outary(i)          = min(ary(Slice(0,i+right)));}
//printf("outary(0)=%.0f\n", outary(0));

   // main (middle) blocks
//printf("<main blocks>\n");
   for (int i = left; i < (arylen-right); i ++) {
     outary(i)           = min(ary(Slice(i-left,bs)));}

   // last block
//printf("<last block>\n");
   for (int i = 0; i < right; i ++) {
     outary(arylen-i-1) = min(ary(Slice(arylen-bs+1-i,bs-2+i)));}
//printf("outary(arylen-1)=%.0f\n", outary(arylen-1));

   // return array
   ary=outary;
//printf("<finished>\n");
//printf("<!!! OK !!!>\n");
   return True;

} // end dorunmin
//#######################################################################
//glish communication
//-----------------------------------------------------------------------
Bool runmin(GlishSysEvent &event, void *) {
   GlishSysEventSource *glishBus = event.glishSource();

   // check input for record
   if (event.val().type()!=GlishValue::RECORD) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     }
     return True;
   }
   GlishRecord input=event.val();

   // check input record for ary
   if (! input.exists("ary") ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     }
     return True;
   }

   // variables
   Vector<Double> ary;
   int bs=10;

   // save ary
   GlishArray gtmp=input.get("ary");
   gtmp.get(ary);

   // check & save bs
   if ( input.exists("bs") ) {
     gtmp=input.get("bs");
     gtmp.get(bs);
   }

   // run c++ function
   dorunmin(ary,bs);
   //if (!dorunmin(ary,bs)) {return False;}

   // return
   GlishArray outary;
   outary=ary;
   if (glishBus->replyPending()) {glishBus->reply(outary);}

   return True;
} // end runmin
//#######################################################################

//-----------------------------------------------------------------------
//Name: arrayminmax
//
//Type: Event Callback Function
//Doc: do an element by element min/max
//ret-data: Glish record
//            erg.min = element by element minimum
//            erg.max = element by element maximum
//inp-data: ary =  input data, 2dim array, 1st dim: data, 2nd dim: datasets
//                 e.g.  erg.max[i] = max(ary[i,])
//-----------------------------------------------------------------------

Bool arrayminmax(GlishSysEvent &event, void *) {
   GlishSysEventSource *glishBus = event.glishSource();
   int i;

   if (event.val().type() != GlishValue::ARRAY) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishArray gtmp = event.val();
   Matrix<Double> ary;
   gtmp.get(ary);
   int size = ary.nrow();

   Vector<Double> minerg(size),maxerg(size);
   for (i=0; i<size; i++) {
     minerg(i) = min(ary.row(i));
     maxerg(i) = max(ary.row(i));
   };
   GlishRecord ergrec;
   ergrec.add("min",minerg);
   ergrec.add("max",maxerg);
     

   if (glishBus->replyPending()) {
     glishBus->reply(ergrec);
   };
   return True;   
}

//-----------------------------------------------------------------------
//Name: interpolate2d
//
//Type: Event Callback Function
//Doc: interpolate 2-dim data
//ret-data: 2-dim GlishArray with the interpolated data
//inp-data: intrec =    Record with the data
//            intrec.source      = 2-dim array [[x,y,value],n] of source data 
//            intrec.shape       = description of return array 
//                                 [(x/y)min, (x/y)max, no_points]
//-----------------------------------------------------------------------

Bool interpolate2d(GlishSysEvent &event, void *) {
   GlishSysEventSource *glishBus = event.glishSource();

   if (event.val().type() != GlishValue::RECORD) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishRecord filtrec = event.val();
   // get mandatory data!
   if (! filtrec.exists("source") ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   if (! filtrec.exists("shape") ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   Matrix<Double> source;
   Vector<Double> shape;
   GlishArray gtmp = filtrec.get("source");
   gtmp.get(source);
   gtmp = filtrec.get("shape");
   gtmp.get(shape);

   // check if the shape of the source array is O.K.
   if (source.ncolumn() != 3 ) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };     

   int i,j,k,kmin,numpoints,sourcesize;
   Double achsmin,achsmax,xpos,ypos,t,tmin;
   achsmin = shape(0);
   achsmax = shape(1);
   numpoints = (int)shape(2);
   sourcesize = source.nrow();

   Matrix<Double> erg(numpoints+1,numpoints+1);



   for (i=0; i<=numpoints ; i++) {
     xpos = achsmin+(achsmax-achsmin)/numpoints*i;
     for (j=0; j<=numpoints ; j++) {
       ypos = achsmin+(achsmax-achsmin)/numpoints*j;
       tmin = 1e36; kmin=0;
       for (k=0; k<sourcesize; k++) {
	 t = (source(k,0)-xpos)*(source(k,0)-xpos)+
	   (source(k,1)-ypos)*(source(k,1)-ypos);
	 if (t<tmin) {
	   tmin = t;
	   kmin = k;
	 };
	 erg(i,j) = source(kmin,2);
       };
     };
   };
   
   gtmp = erg;
   if (glishBus->replyPending()) {
     glishBus->reply(gtmp);
   };   

   return True;
}

//-----------------------------------------------------------------------
//Name: dofft
//
//Type: Event Callback Function
//Doc: fft the data
//ret-data: GlishArray with the fft'ed data
//inp-data: fftdata =  array with the data
//-----------------------------------------------------------------------

Bool dofft(GlishSysEvent &event, void *) {
   GlishSysEventSource *glishBus = event.glishSource();
   int i;

   if (event.val().type() != GlishValue::RECORD) {
     if (glishBus->replyPending()) {
       glishBus->reply(GlishArray(False));
     };
     return True;
   };
   GlishArray fftdata = event.val();
   // get data; mandatory!
   GlishArray gtmp ;
   Vector<Double> ary;
   gtmp.get(ary);
   int size = ary.nelements();
   int fs=3;
   Matrix<Double> mary((size+fs),fs);
   for (i=0; i<fs; i++) {
     mary(Slice(i,size),Slice(i)) = ary;
   };
   

   GlishArray retval;
   //retval = partialMeans(mary,IPosition(1,1))(IPosition(1,(fs/2)),IPosition(1,(size+fs/2)));
   for (i=0; i<size; i++) {
     ary(i) = mean(mary.row(i+fs/2));
   };
   retval = ary;


   if (glishBus->replyPending()) {
     glishBus->reply(retval);
   };
   return True;
}



int main(int argc, char *argv[])
{
   GlishSysEventSource glishStream(argc, argv);

   glishStream.addTarget(meanfilter, "meanfilter");
   glishStream.addTarget(avgstdfilter, "avgstdfilter");
   glishStream.addTarget(medianfilter, "medfilter");
   glishStream.addTarget(runmin,"runmin");
   glishStream.addTarget(interpolate2d, "interpolate2d");
   glishStream.addTarget(calc_pixel, "calc_pixel");
   glishStream.addTarget(calc_meanpower,"calc_meanpower");
   glishStream.addTarget(fftfiler,"fftfiler");
   glishStream.addTarget(dofft,"dofft");
   glishStream.addTarget(arrayminmax,"arrayminmax");

   glishStream.loop();
}
