/*!
  \file main.cpp

  \ingroup RM

  \brief RM-Synthesis test
 
  \author Sven Duscha
 
  \date 28.01.09.

  <h3>Synopsis</h3>
  RM-synthesis command line tool. Using algorithms provided in librm.a library. I/O of FITS format
  through casacore high-level functions.
*/
#include <sys/stat.h>
#include <sys/types.h>
#include <complex>
#include <vector>
#include <fstream>
#include <algorithm>            // for minimum element in vector
#include <limits>           // limit for S infinite
#include <iomanip>
#include <iostream>
#include <time.h>
#include "paramfile.h"
#include "rmNumUtils.h"
#include "rm.h"                 // RM Synthesis class
#include "rmCube.h"
#include "rmIO.h"
#include "WienerFilter.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "wavelet.h"
#define _debug  

using namespace std;    
using namespace RM;
using namespace casa;
/* definition of some constants for defining the different algorithms */
const int Meth_RMSynth=1;
const int Meth_Wiener=2;
const int Meth_Eigen=3;
const int Meth_Wavelet=4;
bool print ;
/*! Procedure fills the vector faradays with equidistant values of faradaydepths from the given munimum to 
   the also given maximum
   \param phi_min mininal value of the faradaydepth
   \param phi_max maximal value of the faradaydepth
   \param nFaraday number of values of the faradaydepths
   \param faras vector to write the faradays into   */
void fillFaras(double phi_min, double phi_max, int nFaraday, vector<double> &faras) 
{
  double delta = (phi_max-phi_min)/(nFaraday-1) ;
  for (int i=0; i<nFaraday; i++) 
  {
    faras[i]=phi_min+i*delta ;
  }
}

/*! procedure to create the output directories for the output of the polarized images.
    The procedure creates a directory given by the string path and then creates
    one subdirectory for the q = qout and one for the u= uout 
    \param path path to the root directory of the result */

void createOutDirs(string path,string q_name, string u_name) {
  mkdir(path.c_str(),0777) ;
  string q_path = path+"/"+q_name;
  string u_path = path+"/"+u_name;
  mkdir(u_path.c_str(),0777) ;
  mkdir(q_path.c_str(),0777) ;
}

vector<complex<double> > performWaveletSynthesis(vector<complex<double> > &arr, vector<double> &lam, double minScale, double maxScale, double stepScale, double phi_min, double phi_max, double deltaPhi) {
  WaveletSynthesis wv(minScale,maxScale,stepScale,phi_min,phi_max,deltaPhi); //minimum scale, maximum scale scale step left rigt step faraday depth
  wv.lamsq = lam;  // assigning of the lmada squared coordinates
  wv.wplus.useWavelet(1); // choice of wavelet mother function 
  wv.analys(arr, wv.wplus);  // calculating wavelet cofficients

  vector<double> fars = wavelet(minScale,maxScale,stepScale,phi_min,phi_max,deltaPhi).bs;  // creates a list of faradaydepths 
  vector<complex<double> > res = wv.synth(wv.wplus,fars);  //actual synthsis of polarised emission in faraday depth 
//  cout << fars.size() << " " << res.size() << endl ;
  return res ;
}
/*! procedure performs the signal reconstruction.
 */
vector<complex<double> > signalReconst(wienerfilter &Wiener, cvec &data, vector<double> &freqsC, vector<double> &freqsI, vector<double> &faras, double phi_min, double phi_max, uint nFara, int method, double nu_0, double alpha, double epsilon,double minWave, double maxWave, double stepWave ) {
  vector<complex<double> > P_farad ;
  /* get the gaps of the lambdasquared intervals for the rm-synthesis */
  vector<double> lcenter ;
  vector<double> linterv ;
  /* convert frequencies to lambda squared values,if this is neccesary */
  if ((method == Meth_RMSynth) || (method==Meth_Wavelet)) {
     freqToLambdaSq(freqsC,freqsI,lcenter,linterv) ;
  }

  if (method==Meth_RMSynth)  {    
    P_farad = performRMSynthesis(data.data,lcenter,linterv,faras, nu_0, alpha, epsilon) ;
  }
  else if (method==Meth_Wiener) {
    P_farad =Wiener.applyWF(data.data) ;
  }
  else if (method==Meth_Eigen) {
//     cout << Wiener.D.rows() << endl ;
    P_farad =Wiener.applyWF(data.data) ;
  }
  else if (method==Meth_Wavelet) {
    double deltaPhi=(phi_max-phi_min)/(nFara-1);
    P_farad = performWaveletSynthesis(data.data,lcenter, minWave, maxWave, stepWave, phi_min, phi_max, deltaPhi) ;
  }
  else {
    cout << "method is not defined " << endl ;
  }
  return P_farad;
}

/*! Method performs a "simulation" of applying the complete telescope to the 
   polarised emission, which is a function of the farraday depth.
   In the case of RM synthesis this means a application of a fourier transformation
   In the case of Wiener filter it mean the application of the Response matrix 
  \param Wiener a objec represents the full Wiener filter object, it has no meaning for rm synthesis
  \param QU signal of the polarised emission
  \param lambdC centers of the intervals of the squared lambda values
  \param faradays intervalbounders for the faraday depths
  \param mathod mehtod used for rm synthesis
  \param nu_0 real parameter value
  \param alpha exponent for the powerlaw of the polarised emission 
  \param epsikon real parameter determins the accaracy of a calculation

  */

vector<complex<double> > applyTeleskop(wienerfilter &Wiener, vector<complex<double> > &peak, vector<double> &lcenter, vector<double> &faras,  double nu_0, double alpha, double epsilon, int method ) {
  vector<complex<double> > test ;
  if (method==Meth_RMSynth) {
    test = performFarraday(peak, lcenter, faras,  nu_0, alpha, epsilon) ;
  }
  else {
    test = Wiener.applyR(peak) ;
  }
  return test ;
}
/*! routine performs the rm clean procedure. 
    For this it calls one of the implemented deconvolution 
    procedures, searches the maximum inside the "dirty map"
    approximates the dirty map around the maximum by a gauss function.
    This gaussfunction is maped to the frequencies of the telescope
    and the values of the frequncies are subtraced from the original signal.
    With this difference the Rm clean is started again. 
    \param data vector of the polarised signal from the antennas
    \param freqsC vector contains the interval centers of the frequencies of the observation
    \param freqaI vector contains the interval lenghts of the frequencies of the observation
    \param faraday vector of the intervals of the vector of the faradaydepths
    \param method int indicates the method of signalreconstruction used by the rm clean
*/

vector<complex<double> >  performRMClean(cvec &data, vec &freqsC, vec &freqsI, vector<double> &faras, int method, uint nFara, double phi_min, double phi_max,double nu_0, double alpha, double epsilon, int useClean, double rmFakt, uint maxIter, double cleanRatio, int addRes, double minWave, double maxWave, double stepWave) {
  vector<double> lcenter ;
  vector<double> linterv ;
//   char buffer [33];
  wienerfilter Wiener ; 
  cout << "Routine perforRMClean wurde aufrerufen: " << method  << endl ;
  complex<double> zero(0,0);
  uint lo=0 ;
  uint index=0 ;
  uint hi=0 ;
  double max=0 ;
  double maxVal = 1 ;
  double mePeak = 0 ;
  uint ind = 0 ;
  vector<complex<double> > P_farad ;
  vector<complex<double> > test ;
  vector<complex<double> > cleanImage(faras.size()); // init a vector for clean rm image
  if ((method == Meth_RMSynth) || (method == Meth_Wavelet))  {
     freqToLambdaSq(freqsC.data,freqsI.data,lcenter,linterv) ;
  }
  else {
    Wiener.prepare(freqsC.data, freqsI.data, faras, nu_0, alpha, epsilon, method) ;
    cout <<"test1: " << Wiener.R.rows() << endl ;
  }
//    for (uint i=0; i<data.size(); i++) cout << i << " " << data[i].real() << " " << data[i].imag() << endl  ;
 for(uint i=0; (i<maxIter) && (mePeak/maxVal<cleanRatio); i++) {
    P_farad = signalReconst(Wiener, data,freqsC.data,freqsI.data,faras,phi_min, phi_max,nFara,method, nu_0, alpha, epsilon, minWave, maxWave, stepWave) ;
//     for (uint j=0; j<P_farad.size(); j++) cout << j << " " << P_farad[j].real() << " " << P_farad[j].imag() << endl  ;
    cvec peak(P_farad.size()) ;
    double var = getVarForGauss(P_farad,faras,useClean,lo,index,hi,max) ;
    /* create the point peak for the current source => peak */
    createPeak(P_farad, max, var, faras, lo, index, hi, peak, useClean); 
    complex<double> fakt=calcFakt(peak,faras,lcenter,linterv,nu_0, alpha, epsilon, method, index) ;
    maxVal=abs(peak[index]); // get the peak value for convergence check 
    mePeak=meanPeak(P_farad) ; // get the mean value of all peaks of the the residual map in faraday depth
    ind = index ; // store the index of the peak maximum for output 
    test = applyTeleskop(Wiener, peak.data, lcenter, faras,  nu_0, alpha, epsilon, method) ;
    smoothPeak(peak,index,lcenter,linterv,faras,useClean) ; // smooth the peak for creating clean rm image
    if (test.size() != data.size()) {
      cerr << "Internernal error in the dimensions of RM synthesis software" << endl ;
      throw "Internernal error in the dimensions of RM synthesis software" ;
    }
    /* subtract image of the point source in Faraday depth from the residual data 
       and add up the peaks to the clean rm image */
    for(uint j=0; j<data.size() ; j++) {
      data.data[j] = data.data[j]-rmFakt*fakt*test[j] ;
    }
    for(uint j=0; j<cleanImage.size() ; j++) {
      cleanImage[j]=cleanImage[j]+rmFakt*peak[j] ;
    }
    /* create a filename for testoutput */
/*    cvec outVec(P_farad) ;
    sprintf(buffer,"%d",i);
    string Ind(buffer) ;
    string name1 = "testOuts/test"+Ind+".dat" ;
    string name2 = "testOuts/clean"+Ind+".dat" ;
    outVec.sepOut(name1) ;
    cleanImage.sepOut(name2); */
    cout <<i << "  peak index= "<< ind << "  peak value= " << maxVal << "  mean peak value= " << mePeak  << "  quotient= " << maxVal/mePeak << endl ;
 }
     /* add the current residual to the clean image if the is requested */
 if (addRes!=0) {
   P_farad = signalReconst(Wiener, data,freqsC.data,freqsI.data,faras,phi_min, phi_max,nFara,method, nu_0, alpha, epsilon, minWave, maxWave, stepWave) ;
   for (uint i=0; i<P_farad.size(); i++) {
      cleanImage[i]=cleanImage[i]+P_farad[i];
    }
 }
//     for (uint i=0; i<test.size(); i++) cout << i << " " << test[i].real() << " " << test[i].imag() << endl  ;
return cleanImage ;
}

/*! main procedure, which performs the rm-synthesis.
    It distinguishes between using "classical RM-Synthesis" or using 
    Wiener Filter. For the Wiener Filter there also exists the possibility of 
    estimating the signal powerspectrum from the data themself 
    Wich method is used is determined by the Parameter 
    method (1= Classical RM-Synthesis 2= Wiener Filter) */

void Convert(string nameQ, string nameU, string ausName, string out_names, double phi_min, double phi_max, 
             int nFaraday, double nu_0, double nu_min, double nu_max, double alpha, double epsilon,  
             int method,  int noNoise, int flatPrior, 
             double signalVar, double sigCorrLength, double noiseVar, 
	     double eps, uint smoothSteps, double powerEps) 
{

    string q_name("qout") ;
    string u_name("uout") ;
    string q_out_Path=ausName+"/"+q_name ;
    string u_out_Path=ausName+"/"+u_name ;
    wienerfilter Wiener;
    cout << "Kontrollausgabe: " "source q: " << nameQ << "  source u: " << nameU << "  Ausname: " 
         << ausName << "  Phi_min: " << phi_min << "  Phi_max: " << phi_max 
         << "  NFaraday: " << nFaraday << "  Nu_0: " << nu_0 << "  alpha: " 
         << alpha << "  epsilon: " <<  epsilon << "  Method: " << method  
         << "  noNoise: " << noNoise << "  flatPrior: " << flatPrior  
         << "  signalVar: " << signalVar  << "  Correlation length: " 
         << sigCorrLength << "  noise variance: " << noiseVar << endl ;
    /*declare objects for input and output */
    rmCube cubeIn(nameQ, nameU,1,nu_min,nu_max) ;  // skyimgage for input with pictures at each frequnecy    
    rmCube cubeOut(nFaraday,cubeIn.getXSize(),cubeIn.getYSize(),3,cubeIn.getPix()) ;  // skyimage for output with pictures at each faradaydepth
    vector<double> freqs = cubeIn.getFreqs() ;
    vector<double> lambdas ;
    /* convert frequencies to lambda squared values,if this is neccesary */
    if (cubeIn.rmType == 1) {
      lambdas = cubeIn.freqToLambdaSq(freqs) ;
    }
    else {
      lambdas = freqs ;
    }
    /* get the gaps of the lambdasquared intervals for the rm-synthesis */
    vector<uint> gaps ;
    findGaps(lambdas,gaps) ;
    /* the last indices of the intervals are now in the vector gaps */

    vector<double> faras(nFaraday) ;
    if (method == Meth_RMSynth) { // Classical RM-Synthesis
      fillFaras(phi_min, phi_max, nFaraday, faras) ;
    }
    else {   // Wiener Filter
      fillFaras(phi_min, phi_max, nFaraday, faras) ;
    }
    createOutDirs(ausName,q_name,u_name) ;

    /* set prameters for wienerfiltering */
    if (method == Meth_Wiener) {  
      Wiener.setFrequencies(freqs) ;
      Wiener.setFaradayDepths(faras) ;
      Wiener.alpha = alpha;
      Wiener.nu_0 = nu_0 ;
      Wiener.epsilon_0 = epsilon ;
      Wiener.generateWienerFiltering(noNoise, flatPrior, signalVar, sigCorrLength, noiseVar, gaps, eps,2) ;
      cout << "Wiener filter generated" << endl ;
    }
    else if (method == Meth_Eigen) { // wiener filtering with iterating the powerspectrum 
      Wiener.setFrequencies(freqs) ;
      Wiener.setFaradayDepths(faras) ;
      Wiener.alpha = alpha;
      Wiener.nu_0 = nu_0 ;
      Wiener.epsilon_0 = epsilon ;
    }
    /* perform the actual computation for each line of site  */
    uint nx = cubeIn.getXSize() ;
    uint ny = cubeIn.getYSize() ;;
    if ((method==Meth_RMSynth) || (method==Meth_Wiener)) {
      for (uint ij=0 ; ij<ny; ij++) {
        cout << "ij= " << ij << endl ;
        for (uint ii=0; ii<nx; ii++) {
          vector<complex<double> > P_lambda;
	  cubeIn.getLineOfSight(ii,ij,P_lambda) ;
          vector<complex<double> > P_farad ;
          if (method == Meth_RMSynth)        
            P_farad = cubeIn.performRMSynthesis(P_lambda,lambdas,faras, gaps, nu_0, alpha, epsilon) ;
          else
            P_farad =Wiener.applyWF(P_lambda) ;

          cubeOut.setLineOfSight(ii,ij,P_farad);
        }
      }
    }
    else if (method==Meth_Eigen){  // case iterating the signal power spectrum
	/* the following function calculates the covariance matrix for the datas */
	Wiener.iterateForAll(cubeIn,cubeOut, signalVar, sigCorrLength, noiseVar,gaps,smoothSteps, powerEps) ;
	/* perform the Wiener filtering with the calculated signal covariance */
	for (uint ij=0 ; ij<ny; ij++) {
          cout << "ij= " << ij << endl ;
          for (uint ii=0; ii<nx; ii++) {
              vector<complex<double> > P_lambda;
              cubeIn.getLineOfSight(ii,ij,P_lambda) ;
              vector<complex<double> > P_farad ;
              P_farad =Wiener.applyWF(P_lambda) ;
              cubeOut.setLineOfSight(ii,ij,P_farad);
          }
        }
    } // end of iterating the signal powerspectrum
    cubeOut.writeRM_Cube(q_out_Path,u_out_Path,out_names) ;
}
/*! integration over the lamda squared of the instrument. In the case of gaps the lamda 
    squared intervals are splited into two separat intervals */

complex<double> integrateLamdaSq(vector<complex<double> > &QU,vector<double> &lambC, vector<double> &lambI,double faraday, double nu_0, double alpha, double epsilon) {
    double lam_a ;  // start of the integration interval for phi integration 
    double lam_b ;  // end of the integration interval for phi integration 
    uint Nlambdas = lambC.size() ;
    complex<double> exp_lambdafactor;
    complex<double> erg = 0.0 ;
   // calculate the interval in faradaydepths
    /* loop for the porduct of one row of the transformation matrix with the vector
     * which must be transformed */
    for	(uint j=0; j<Nlambdas; j++) { 
        /* get the boarders of the current lamda interval */
        lam_a = lambC[j]-0.5*lambI[j] ;
        lam_b = lambC[j]+0.5*lambI[j] ;
        double arg = -2.0*faraday*lambC[j] ;
        exp_lambdafactor=complex<double>(cos(arg), sin(arg));	
        erg = erg+exp_lambdafactor*QU[j]*(lam_b-lam_a) ;
      
    }
    return erg ;
}

void processLine(cvec &data, wienerfilter &Wiener, vector<complex<double> > &P_farad , vec &freqsC, vec &freqsI, vector<double> &faras, double phi_min, double phi_max, uint nFara, int method, double nu_0, double alpha, double epsilon, string outDat, int useClean, double rmFakt, uint maxIter, double cleanRatio, int addRes, double minWave, double maxWave, double stepWave) {
  /* perform the actual calculation of the polarized emission 
     as a function of the faraday depth */
  if (!useClean) {
    Wiener.prepare(freqsC.data, freqsI.data, faras, nu_0, alpha, epsilon, method) ;
    P_farad = signalReconst(Wiener, data,freqsC.data,freqsI.data,faras,phi_min,phi_max,nFara,method,nu_0,alpha,epsilon, minWave, maxWave, stepWave);
  }
  else {
    
    cout << "clean is used: " << endl ;
    P_farad = performRMClean(data, freqsC, freqsI, faras, method, nFara,  phi_min, phi_max,nu_0, alpha, epsilon, useClean,rmFakt, maxIter, cleanRatio, addRes, minWave, maxWave, stepWave) ;
  }
}

void convLine(cvec &data, vec &freqsC, vec &freqsI, double phi_min, double phi_max, uint nFara, int method, double nu_0, double alpha, double epsilon, string outDat, int useClean, double rmFakt, uint maxIter, double cleanRatio, int addRes, double minWave, double maxWave, double stepWave) {
  vector<double> faras(nFara) ;
  wienerfilter Wiener;
  fillFaras(phi_min, phi_max, nFara, faras) ;
  /* get the gaps of the lambdasquared intervals for the rm-synthesis */
  vector<double> lcenter ;
  vector<double> linterv ;
  /* convert frequencies to lambda squared values,if this is neccesary */
  if ((method == Meth_RMSynth) || (method == Meth_Wavelet)) {     
     freqToLambdaSq(freqsC.data,freqsI.data,lcenter,linterv) ;
  }
  
  /* process the actal line of sight */
//  findGaps(lambdas,gaps) ;
  vector<complex<double> > P_farad ;
  rmIO aus ;
  processLine(data, Wiener, P_farad , freqsC, freqsI, faras, phi_min, phi_max, nFara, method, nu_0, alpha, epsilon, outDat, useClean, rmFakt, maxIter, cleanRatio, addRes, minWave, maxWave, stepWave) ;
  /* perform the actual calculation of the polarized emission 
     as a function of the faraday depth */
  aus.saveFaraLineToFits(outDat,P_farad,faras);
}
/*! Procedure to read a test dataset of one line of sight 
    of polarized antennasignal into a cvec object.
    The data set must be stored into two ascii files of 
    real and imaginary data, named by nameR and nameI */
void testRead(string nameR, string nameI, cvec &vals) {
  ifstream f1(nameR.c_str(), ios::in);
  ifstream f2(nameI.c_str(), ios::in);
  double re;
  double im ;
  int i=0;
  while ((f1 !=NULL) && (f2!=NULL)) {
    f1 >> re ;
    f2 >> im ;
    if((f1 != NULL) &&(f2 != NULL)) {
      complex<double> val(re,im) ;
      vals.set(i,val) ;
      i++ ;
    }
  }
  f1.close();
  cout << "erste Datei geschlossen" << endl ;
  f2.close();
  cout << "zweite Datei geschlossen" << endl ;
}

/*! Procedure writes out the logging information, which is generated by the 
    rm synthesis software itself.
    Here the rm Method is mentioned and wether rm cleaning is used or not */
void writeLogging(PagedImage<Float> image, int method, int useClean) {
  image.logger().logio() << "RM Synthesis" << endl;
  switch(method) {
    case Meth_RMSynth:
      image.logger().logio() << "Method: RM Synthesis" << endl;
      break ;
    case Meth_Wiener:
    case Meth_Eigen:
      image.logger().logio() << "Method: Wiener Filtering" << endl;
      break ;
    case Meth_Wavelet:
      image.logger().logio() << "Method: Wavelet Transformation " << endl;
      break ;
    default:
      image.logger().logio() << "Method: unknown" << endl ;
  }
  image.logger().logio() << "Cleaning method: " << useClean << LogIO::POST;
}

void processCube(double phi_min, double phi_max, uint nFara, int method, double nu_0, double alpha, double epsilon, string input, string casaQuery, string outDat, int useClean, double rmFakt, uint maxIter, double cleanRatio, int addRes, double minWave, double maxWave, double stepWave) {
  Vector<int> inds;
  rmCube cube(input,"",casaQuery, inds) ;
  vec freqsC = cube.getFreqsCenters() ;
  vec freqsI = cube.getFreqsLengths() ;
  cout << cube.getXSize() << " " << cube.getYSize() << " " << nFara << " " << inds << endl ;
  vector<double> faras(nFara) ;
  wienerfilter Wiener;
  fillFaras(phi_min, phi_max, nFara, faras) ;
  int xSize = 10;//cube.getXSize() ; 
  int ySize = 10;//cube.getYSize() ;
  rmCube result(xSize, ySize, faras) ;
  Wiener.prepare(freqsC.data, freqsI.data, faras, nu_0, alpha, epsilon, method) ;
//   Array<Float> feld = arr2Col.getColumn( );
  /*performing the rm-synthesis for all pixels of the image. For each pixel, the
    rmsynthesis is done separatly by using the line procedure */
  #pragma omp parallel 
  {
    for (int x=0; x<result.getXSize(); x++ ) {
      cout << "x= "<< x+1 << " / " << cube.getXSize() << endl ;
//       #pragma omp for
      for (int y=0; y<result.getYSize(); y++ ) {
        cvec data(freqsC.size()) ;
        cube.getLineOfSight(x,y,data) ;
        vector<complex<double> > P_farad ;
        print=((x==0) && (y==0));
        processLine(data, Wiener, P_farad , freqsC, freqsI, faras, phi_min, phi_max, nFara, method, nu_0, alpha, epsilon, outDat, useClean, rmFakt, maxIter, cleanRatio, addRes, minWave, maxWave, stepWave) ;
        result.setLineOfSight(x,y,P_farad) ;
      }
    }
  }
  CoordinateSystem coors=cube.get_cs() ;
  CoordinateSystem neu;
  IPosition size=cube.get_shape() ; 
  size[inds[0]] = xSize ;
  size[inds[1]] = ySize ;
  size[inds[2]] = nFara ;
  TiledShape tShape(size);
  double dphi=(phi_max-phi_min)/(nFara-1);
  for (uint i=0; i<coors.nCoordinates(); i++) {
    Coordinate const &coor=coors.coordinate(i) ;
    if (coor.type()!= Coordinate::SPECTRAL) {
      neu.addCoordinate(coor) ;
    }
    else {
      TabularCoordinate coord(phi_min, dphi, 0, "rad/m^2", "faradaydepth");
      neu.addCoordinate(coord) ;
    }
  }
  /* create new image object to save the result to file */
  PagedImage<Float> imag2(tShape,neu,outDat);
  /* generate logging information for the new image object 
     1. copy the logging information from the input image object */
  imag2.logger().append(cube.get_log()) ;
  writeLogging(imag2,method, useClean) ;
  cube.copyMetaData(imag2);
  IPosition pos(/*ipos.size()+1*/4) ;
  for (uint j=0; j<pos.size(); j++) {
    pos[j] =0 ;
  }
  uint curIndx = 0 ;
  for (uint j=0; j<nFara ; j++) {
    pos[inds[2]] = j ;  // set the frequency coordinate inside the current image
    /* loop over the first dimension of the current image */
    for (int x=0; x<result.getXSize(); x++) {
      pos[inds[0]]=x ;  // set the position for the first spartial image dimension
      /* loop over the seccond dimension of the current image */
      for (int y=0; y<result.getYSize(); y++) {
        pos[inds[1]]=y ; // set the position for the seccond spartial image dimension
        pos[inds[3]]=1 ; // select pos component for Stokes Q
        complex<double> const val=result.getVal(curIndx,x,y) ;
        imag2.putAt(real(val),pos) ;
        pos[inds[3]]=2 ; // select pos component for Stokes U
        imag2.putAt(imag(val),pos) ;
        
      }
    }
    curIndx++ ;
  }
}
/*! procedure to create a linear frequency vector 
  \param nu_min minimal frequency
  \param nu_max maximal frequency
  \param freqs fill in the frequenc values */
void fillFreqs(double nu_min, double nu_max, vec &freqs) {
  uint num= freqs.size() ;
  double delta = (nu_max-nu_min)/(num-1) ;
  /* loop to fill the frequency falue vector */
  for (uint i=0; i<num; i++) {
    freqs.set(i,nu_min+delta*i);
  }
}

complex<double> testComplexPerform(vector<complex<double> > &v1, vector<complex<double> > &v2) {
  complex<double> fin(0,0) ;
//  double t0 = omp_get_wtime() ;
  for (uint l=0; l<v1.size(); l++) {
    fin = fin+v1[l]*v2[l] ;
  }
//  double t1 = omp_get_wtime() ;
//  cout <<  "  Zeit: " << (t1-t0) << " Ergebnis: " << fin <<  endl ;
  return fin ;
} 
/*! main routine for the pulsar rm-synthesis */
int main (int argc, char * const argv[]) {

if ( argc <= 1 ) {
    cout << endl << "Too few parameters..." << endl << endl;
    cout << "Procedure needs the name of the used parameter file" << endl;
    cout << endl;
    return -1;
  }  // end of checking parameter
  paramfile inp(argv[1]);
  string input = inp.find("input");
  string outDat = inp.find("output") ;
  rmIO io(outDat,true) ;
  double minPhi = inp.findDouble("faraday_min") ;
  double maxPhi = inp.findDouble("faraday_max") ;
  double nu_0 = inp.findDouble("nu_0") ;
  double alpha = inp.findDouble("alpha");
  double epsilon = inp.findDouble("epsilon_0");
  int indFreq = inp.findInt("indFreq") ;
  int nFaraday = inp.findInt("faraday_num");
  int method = inp.findInt("method") ;  
  int useClean = inp.findInt("useClean") ;
  int addRes = inp.findInt("addResidual");
  int rmCube = inp.findInt("rmCube") ;
  string casaQuery = inp.find("casaQuery") ;
  uint maxIter = inp.findInt("cleanIterations");
  double cleanRatio1 = inp.findDouble("cleanRatio");
  double minWave = inp.findDouble("wavelet_scale_min");
  double maxWave = inp.findDouble("wavelet_scale_max");
  double stepWave = inp. findDouble("wavelet_scale_step");
  double cleanRatio = 1.0/cleanRatio1;
  double rmFakt=0;
  if(useClean>=1) { // read faktor for weighting "point source in rm cleaning" only if clean is selected 
    rmFakt = inp.findDouble("clean_weight");
  }
  int fits = inp.findInt("file_format") ;
  cvec vals ;
  vec freqsC ; // centers of frequencies
  vec freqsI ; // interval widths of frequencies
  rmIO ein ;
  // discriminate the cases
  // only one line of frequencies is to be processed 
  if (rmCube == 0) {
    ein.readFreqLine(input,vals,freqsC, freqsI,indFreq,fits);
    convLine(vals, freqsC, freqsI, minPhi, maxPhi, nFaraday, method, nu_0, alpha, epsilon, outDat, useClean, rmFakt, maxIter, cleanRatio, addRes, minWave, maxWave, stepWave) ;
  }
  // a complete rm cube is to be processed 
  else {
    processCube(minPhi, maxPhi, nFaraday, method, nu_0, alpha, epsilon, input, casaQuery, outDat, useClean, rmFakt, maxIter, cleanRatio, addRes, minWave, maxWave, stepWave) ;
  }
 
}
int mainold (int argc, char * const argv[]) 
{

  //__________________________________________________________________
  // Check parameters provided from the command line
  if ( argc <= 1 ) {
    cout << endl << "Too few parameters..." << endl << endl;
    cout << "Procedure needs the name of the used parameter file" << endl;
    cout << endl;
    return -1;
  }  // end of checking parameter
  paramfile inp(argv[1]);
  string input = inp.find("input");
  string outDat = inp.find("output") ;
//   double minPhi = inp.findDouble("faraday_min") ;
//   double maxPhi = inp.findDouble("faraday_max") ;
//   double nu_0 = inp.findDouble("nu_0") ;
//   double nu_min = inp.findDouble("nu_min") ;
//   double nu_max = inp.findDouble("nu_max") ;
  double alpha = inp.findDouble("alpha");
//   double epsilon = inp.findDouble("epsilon_0");
//   int indFreq = inp.findInt("indFreq") ;
//   int nFaraday = inp.findInt("faraday_num");
  int method = inp.findInt("method") ;
  double signalVar = 0.0 ;
  double noiseVar = 0.0 ;
  double sigCorrLength = 0.0 ;
  int noNoise = 0 ;
  int flatPrior = 0 ;
//   double eps = 0.1 ;
  uint smoothSteps ; 
  double powerEps ;
  if (method > Meth_RMSynth) // 2 for pure Wiener Filter 3 for iterating the powerspectrum
  { // method wiener fileter was seleceted
    if (method == Meth_Wiener) {
      noNoise = inp.findInt("no_noise") ;
      flatPrior = inp.findInt("flat_Prior") ;
    
      if (flatPrior == 0) 
      { // no signalvariance and correlation length need for flat prior
         signalVar = inp.findDouble("signal_variance") ;  
         sigCorrLength = inp.findDouble("signal_correlation_length") ;
      }
    
      if (noNoise == 0) 
      { // no noise variance need for "noNoise"
        noiseVar = inp.findDouble("noise_variance") ;
      }
//      eps = inp.findDouble("epsilon_int") ;
    }
    else if (method ==Meth_Eigen) {
      signalVar = inp.findDouble("signal_variance") ;  
      sigCorrLength = inp.findDouble("signal_correlation_length") ;
      noiseVar = inp.findDouble("noise_variance") ;
      smoothSteps = inp.findInt("smoothSteps") ;
      powerEps = inp.findDouble("powerEps") ;
    }
  }
  
  //__________________________________________________________________
  // Run the tests
  cout << "test " << alpha << endl ;
  //Convert(inpDatQ, inpDatU, outDat, outNames, minPhi, maxPhi, nFaraday, nu_0, nu_min, nu_max, alpha, epsilon, method, noNoise, flatPrior, signalVar, sigCorrLength, noiseVar, eps, smoothSteps, powerEps) ;
  return 0 ;
}
