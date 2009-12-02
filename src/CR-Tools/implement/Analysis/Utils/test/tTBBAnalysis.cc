#include <crtools.h>
#include <fstream>
#include <Analysis/Utils/TBBData.h>
#include <Analysis/Utils/PolyPhaseFilter.h>
#include <Analysis/Utils/InvPolyPhaseFilter.h>
#include <Analysis/Utils/SimpleBeamFormer.h>

//_______________________________________________________________________________
//                                                                   get_antennas

/*!
  \brief Get the antenna position from a file

  \param antPosFile    -- File with the antenna positions
  \retval antPositions -- Antenna positions, [y1,x1,z1,y2,x2,z2,...]
*/
void get_antennas (const char * antPosFile,
		   vector<double> &antPositions){
  //reference position =(0,0,0)!
  //get antenna positions;  
  ifstream pos;
  pos.open(antPosFile);
  double x,y,z;

  antPositions.clear();
  while(!pos.eof() ) {
    pos>>x>>y>>z;
    if(!pos.eof()){
      antPositions.push_back(y);  //Convert NEU to ENU
      antPositions.push_back(x);
      antPositions.push_back(z);
    }
  }
  pos.close();
}

//_______________________________________________________________________________
//                                                                       test_TBB

/*
  \param filelist       -- List of data files
  \param antPosFile     -- File with the antenna positions
  \param weightsfile    -- File with the weights for the PPF
  \param invweightsfile -- File with the weights for the PPF inversion
*/
int test_TBB (const char *filelist,
	      const char *antPosFile,
	      const char *weightsfile,
	      const char *invweightsfile ){
  //example how to form a beam from TBB data using ppf and ppfinv to get back to timedomain
  int nofFailedTests (0);

  try{
    //init the data
    vector<string> names;
    char name[256];
    ifstream ffile(filelist);
    while(!ffile.eof()) {
      ffile>>name;
      if(!ffile.eof()) names.push_back(name);
      cout<<"getting file: "<<name<<endl;
     }
    ffile.close();
  
  
    CR::TBBData tbb;
    bool odd=true; //odd dipole nrs (X)
    bool even=false;//no even dipole nrs (Y)
    tbb.initData(names,even,odd);  //initializes nofantennas and offsets per antenna, 
    uint nofAntennas=tbb.getNofAntennas();
    int length=tbb.getLength(); //The maximum nr of samples
   
    
    //select channel numbers, 
    vector<int> freqVec(3);
    freqVec[0]=101;freqVec[1]=201;freqVec[2]=450;
    int nr_channels=freqVec.size();

    //select az, el positions, just select 2 positions for the example
    vector<double> az(2);
    vector<double> el(2);
    az[0]=130*3.14/180.;az[1]=230*3.14/180.;
    el[0]=59*3.14/180.;el[1]=30*3.14/180.;
    uint nr_pixels=az.size();
    
    //init antenna positions
    vector<double> antPos;
    get_antennas(antPosFile,antPos);
    assert(nofAntennas==(antPos.size()/3));//did we get  the right number of antenna positions

    //init the simple beamformer
    CR::SimpleBeamFormer beam(nr_channels);
    beam.setAntennas(antPos);
    beam.setAzEl(az,el);
    

    //init the ppf
    CR::PolyPhaseFilter ppf;
    
    vector<double> weights(1024*16);
    int fpos=0;
  
    ifstream coeffs(weightsfile);
    while(!coeffs.eof() && !(fpos>=1024*16)) {
      coeffs>>weights[fpos];
      //if(fpos%1024==0) cout<<"weights "<<weights[fpos]<<endl;
      fpos++;
    }
    coeffs.close();
    ppf.setWeights(weights);
 

    //init the inv ppfs, Since invppf keeps the data in a circular buffer like structure, you'd better create one per pixel
    vector<CR::InvPolyPhaseFilter *> invppf(nr_pixels);
    vector<double> invweights(1024*16);
    fpos=0;
    ifstream invcoeffs(invweightsfile);
    while(!invcoeffs.eof() && !(fpos>=1024*16)) {
      invcoeffs>>invweights[fpos];
      //if(fpos%1024==0) cout<<"weights "<<weights[fpos]<<endl;
      fpos++;
    }
    invcoeffs.close();
    
    for(uint ix=0;ix<nr_pixels;ix++){
      invppf[ix]=new CR::InvPolyPhaseFilter();
      invppf[ix]->setWeights(invweights);
    }
    

    
    
    int blocksize=1024;
    int readsize=16*1024; //eg. for ppf read data in blocks of 16*1024 shift by 1024
    int totlength = min(length,20480);//read 20480 samples (if available) 

    //NOW start processing
    vector<double> out;
    vector<complex double> in_invppf;
    in_invppf.assign(blocksize/2+1,0+0*I);



    for(int startsample=0;startsample<totlength;startsample+=blocksize){
      //remove previous beamformed 
      beam.Clear();
      cout<<"reading block:"<<startsample/1024<<endl;
      for(uint iant=0;iant<nofAntennas;iant++)
	{
	  //get the datablock
	  tbb.getTimeSeries(out,readsize,iant,startsample);  
	  //convert to freq domain using ppf
	  ppf.Convert(out);
	  
	  for(int ifreq=0;ifreq<nr_channels;ifreq++){
	    double freq=100.e6+freqVec[ifreq]*100.e6/513;
	    //get the converted data from ppf
	    complex double data = conj(ppf.GetComplex(blocksize/2-freqVec[ifreq]));  //2nd nyquistzone
	    for(uint ix=0;ix<nr_pixels;ix++)
	      //form the beam
	      beam.Add(data,iant,freq,ix,ifreq);
	    
	    
	  }
	}
      //Convert back to the time domain using invPPF, remember the first 15 timeframes do not make sense since the invpf is collecting data 
      for(uint ix=0;ix<nr_pixels;ix++){
	for(int ifreq=0;ifreq<nr_channels;ifreq++){
	  in_invppf[blocksize/2-freqVec[ifreq]] = beam.GetComplex(ix,ifreq); //second nyquistzone, convert back....
	  //Maybe I better make an option to set nyquistZone for PPF and PPFInv
	}
	invppf[ix]->Convert(in_invppf);
	//and do something nice with the data
	cout<<"Converted data: "<<invppf[ix]->GetAmp(130)<<endl;//eg. print the timeseries data of timestamp 130...
      }

    }//loop over frames  
   
    //clean up
   //delete PPF's
  for(uint ix=0;ix<nr_pixels;ix++)
    delete invppf[ix];
  
  invppf.clear();
   
  }
  catch(std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;

}

//_______________________________________________________________________________
//                                                                           main

int main (int argc,
	  char *argv[])
{

  int nofFailedTests (0);
  int nofParameters (5);
  char * filelist;
  char * antposfile;
  char * ppfweights;
  char * ppfinvweights;

  if (argc < nofParameters) {
    std::cerr << "[tTBBAnalysis] Incomplete list of input parameters! Usage tTBBAnalysis filelist antPos ppfweights invppfweigthts" << std::endl;
    return -1;
  } else {
    filelist  = argv[1];
    antposfile  = argv[2];
    ppfweights  = argv[3];
    ppfinvweights = argv[4];
    // [1] Test for the constructor(s)
    nofFailedTests += test_TBB (filelist,antposfile,ppfweights,ppfinvweights);
  }
   
  return nofFailedTests;
 
 }
