#include <crtools.h>
#include <fstream>
#include <Analysis/Utils/Simulator.h>
#include <Analysis/Utils/ContSourceHBATile.h>


double mySource(double time){
  return 1.e4*sin(time*2*3.14*169.65e6); //the pager signal

}


void get_antennas(const char * antPosFile,vector<double> &antPositions){
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



int simulTimeseries(const  char * antPosFile){
  int nofFailedTests (0);
  try{
    vector<double> antPos;
    get_antennas(antPosFile,antPos);
    
    double az=3.14*110./180; //position of the source
    double el=3.14*60./180; //position of the source

    CR::Simulator sim;
    sim.setAntennas(antPos);
    sim.setFunction(CR::Simulator::CONTINUOUS_SOURCE);

    //you canalso define your own function
    //either derive from the SimulFunction class
    CR::ContSourceHBATile hbasimul;
    hbasimul.setAzEl(az,el); //simulates a tile beam poting to zenith with a contineous source at az,el

    CR::Simulator sim2;
    sim2.setAntennas(antPos);
    sim2.setFunction(hbasimul);
 

    //or define your own function of time

    CR::Simulator sim3;
    sim3.setAntennas(antPos);
    sim3.setFunction(&mySource);
 
    vector<double> out;
    uint iant = 0;//antenna number
    uint blocksize=1024;
    double timebin_size=1./200e6;
    uint starttime = 10; //first bin t=starttime*timebin_size
    
    sim.getTimeSeries(out,blocksize,az,el, iant, starttime, timebin_size);

    //or set azel and samplefreq before

    sim2.setAzEl(az,el);
    sim2.setSampleFreq(200.e6);
    sim2.getTimeSeries(out,blocksize,iant, starttime);
  
    sim3.getTimeSeries(out,blocksize,iant, starttime);
    
   
//     for(uint i=0;i<blocksize;i++)
//       cout<<out[i]<<" ";
//     cout<<endl;
  }
  catch(std::string message) {
    std::cerr << message << std::endl;
    nofFailedTests++;
  }
  
  return nofFailedTests;
 
}

int main (int argc,
	  char *argv[])
{
  int nofParameters (2);
  int nofFailedTests (0);
  
  char * antposfile;
  
  if (argc < nofParameters) {
    std::cerr << "[tSimulator] Incomplete list of input parameters!" << std::endl;
    return -1;
  } else {
  
  antposfile  = argv[1];
  nofFailedTests+=simulTimeseries(antposfile);
  
  
    
  return nofFailedTests;
  }

}
