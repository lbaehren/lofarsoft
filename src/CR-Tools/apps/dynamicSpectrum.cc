#include<iostream>
#include <string>
#include <sstream>
#include <list>

#include <Analysis/analyseLOPESevent2.h>
#include <crtools.h>

using CR::analyseLOPESevent2;
using CR::LopesEventIn;

/*!
  \file dynamicSpectrum.cc

  \ingroup CR_Applications

  \brief Generates a dynamic spectrum of the timerange of a given thunderstorm mode LOPES-event"
 
  \author Moses Ender
 
  \date 2009/03/08

  <h3>Usage</h3>

  \verbatim
   ./dynamicSpectrum lopes1.event [ lopes2.event ...]
  \endverbatim
  
  At least one event is required but you can also provide several events at once.
 
  A number of 512 blocks is used. This is optimized for thunderstorm events with a length of 2^19 samples.
*/

int main(int argc, char* argv[])
{
    if(argc<2)
    {
	cerr<<"Wrong number of arguments!"<<endl
	    <<"Syntax: ./dynamicSpectrum first.event [second.event ...]"<<endl;
	exit(1);
    }
    
    String fn;
    string fname,eventname;
    for(int i=1; i<argc; i++)
    {
	fname=argv[i];
	eventname=fname;

	//check if argument is a lopes events
	if(fname.find(".event")==string::npos)
	{
	    cout<<argv[i]<<" does not seem to be a LOPES event file\n";
	    continue;
	}
      fname.erase(fname.find_last_of("."), 6);
           fname.append("-dynSpec.eps");


	cout<<"Processing "<<argv[i]<<" ...\n  writing "<<fname<<endl;
	fn=argv[i];
        LopesEventIn dr(fn);
	dr.setBlocksize(dr.blocksize()/512);


 	// writing postscript output
	ofstream psout(fname.c_str());
	// writing ps header
        psout<<"%!PS-Adobe-2.0 EPSF-2.0\n"
           <<"%%DocumentFonts: Courier-Bold Helvetica-Bold\n"
           <<"%%BoundingBox: -55 -50 520 280\n"
           <<"%%Title: "<< fname << endl
           <<"%%EndComments\n"
           <<"gsave\n"
           <<"/PSDict 200 dict def\n"
           <<"PSDict begin\n"
           <<"/a {arc} def\n"
           <<"/s {stroke} def\n"
           <<"/l {lineto} def\n"
           <<"/m {moveto} def\n"
           <<"/n {newpath} def\n"
           <<"/rgb {setrgbcolor} def\n"
           <<"/c {closepath} def\n"
           <<"/f {fill} def\n"
           <<"/gs {gsave} def\n"
           <<"/gr {grestore} def\n"
           <<"/cf {closepath fill} def\n"
           <<"/lw {setlinewidth} def\n"
           <<"end\n"
           <<"PSDict begin\n"
           <<"0.1 lw \n\n";
        psout.precision(3);

	float r,g,b;
	double power;
	unsigned int j=0;
	Matrix<Double> spectrum;
	while(j<dr.Filesize()/dr.blocksize())
	{
	    dr.setBlock(j);
	    dr.setADC2Voltage(dr.ADC2Voltage());

	    Matrix<Double> spectrum = log10(amplitude(dr.calfft()));
	    for(unsigned int k=0; k<spectrum.nrow(); k++)
	    {
                power=mean(spectrum.row(k));
		if(power<0||power>10)
		{
		    if(power>5.) power=5.;
		    else power=0;
		}
                r=0.2*power*0.65;
                g=0.2*power*0.7;
                b=0.2*power;


		psout<<r<<" "<<g<<" "<<b<<" rgb n "<< j <<" "<< k/2. <<" m "<< (j+1) <<" "<< k/2. <<" l  "<< (j+1) <<" "<< (k+1)/2. <<" l  "<< j <<" "<< (k+1)/2. <<"  l c gs f gr s\n";
	    }
	    j++;
	}
psout<<"0.5 lw 0 0 0 rgb n 0 0 m 512 0 l 512 257 l 0 257 l c s\n"
     <<"/Helvetica 16 selectfont " 
     <<"n 180 265 m ("<<eventname<<") show c\n"
     <<"n -25 -6 m (40) show c n -25 57 m (50) show c n -25 121 m (60) show c n -25 185 m (70) show c n -25 250 m (80) show c n 0 63 m 5 63 l s n 0 127 m 5 127 l s n 0 191 m 5 191 l s\n"   
     <<"n 32 0 m 32 5 l s n 12 -25 m (0.00) show c \n"
     <<"n 96 0 m 96 5 l s n 76 -25 m (0.82) show c \n"
     <<"n 160 0 m 160 5 l s n 140 -25 m (1.64) show c \n"
     <<"n 224 0 m 224 5 l s n 204 -25 m (2.46) show c \n"
     <<"n 288 0 m 288 5 l s n 268 -25 m (3.28) show c \n"
     <<"n 352 0 m 352 5 l s n 332 -25 m (4.10) show c \n"
     <<"n 416 0 m 416 5 l s n 396 -25 m (4.92) show c \n"
     <<"n 480 0 m 480 5 l s n 460 -25 m (5.73) show c \n"
     <<"n 255 -45 m (t [ms]) show c \n"
     <<"gs n -40 95 m 90 rotate (f [MHz]) show c gr\n"
     <<"grestore\n"
     <<"end\n";
psout.close();
}
}
