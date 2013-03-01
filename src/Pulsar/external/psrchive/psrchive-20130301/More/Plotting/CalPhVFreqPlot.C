



#include <Pulsar/CalPhVFreqPlot.h>
#include <Pulsar/Archive.h>
#include <Pulsar/Integration.h>
#include <cpgplot.h>

#include <string.h>

using namespace std;
using namespace Pulsar;


const void CalPhVFreqPlot::minmaxval( vector<float> thearray, float &min, float &max )
{
    min = thearray[0];
    max = thearray[0];

    int n = thearray.size();
    for( int i = 1; i < n; i ++ )
    {
        if( thearray[i] > max )
            max = thearray[i];
        if( thearray[i] < min )
            min = thearray[i];
    }
}






CalPhVFreqPlot::CalPhVFreqPlot()
{
  get_frame()->hide_axes();
}



TextInterface::Parser* CalPhVFreqPlot::get_interface ()
{
    return new CalPhVFreqPlot::Interface( this );
}


//! Get the default label for the x axis
string CalPhVFreqPlot::get_xlabel (const Archive*)
{
    string label = "x";

    return label;
}

//! Get the default label for the y axis
string CalPhVFreqPlot::get_ylabel (const Archive*)
{
    string label = "y";

    return label;
}

//! prepare for rendering
void CalPhVFreqPlot::prepare ( const Archive *data )
{


    for(unsigned n=0; n<npol; n++)
    {


    }
}

//! draw the angle as a function of pulse phase
void CalPhVFreqPlot::draw (const Archive *data )
{
  copy = data->clone();

  if (copy->get_state() != Signal::Coherence)
  {
        // convert back to original
    copy->convert_state(Signal::Coherence);
  }

  copy->tscrunch();

  nchan = copy->get_nchan();
  npol = copy->get_npol();

  vector<vector<Estimate<double> > > calon;
  vector<vector<Estimate<double> > > caloff;

  copy->get_Integration(0)->cal_levels(calon, caloff);

    // Form calon = calon - caloff
  for( unsigned n=0; n<npol; n++)
  {
    for(unsigned i=0; i<nchan; i++)
    {
      calon[n][i] -= caloff[n][i];
    }
  }

  cal.resize(nchan);
  sys.resize(nchan);
  xx.resize(nchan);




  for(unsigned i=0; i<nchan; i++)
  {
    xx[i] = (float)i;
  }

  
    for(unsigned n=0; n<npol; n++)
    {

      for(unsigned i=0; i<nchan; i++)
      {
	if(n < 2)
	{
	  cal[i] = (float)(calon[n][i].val);
	  sys[i] = (float)(caloff[n][i].val);
	}
	else
	{
                // Cal Cross
	  double x = calon[2][i].val;
	  double y = calon[3][i].val;
	  if(n == 2)
	  {
	    cal[i] = (float)(sqrt(x*x + y*y));
	  }
	  else
	  {
	    cal[i] = (float)(180.0/M_PI * atan2(y,x));
	  }
                // System Cross
	  x = caloff[2][i].val;
	  y = caloff[3][i].val;
	  if(n == 2)
	  {
	    sys[i] = (float)(sqrt(x*x + y*y));
	  }
	  else
	  {
	    sys[i] = (float)(180.0/M_PI * atan2(y,x));
	  }
	}
      }
//         for(unsigned i=0; i<nchan; i++)
//         {
            char cpol[4];

            if(n < 3)
            {
                // Amplitudes
                minmaxval ( cal, mincal, maxcal);
                minmaxval ( sys, minsys, maxsys);

                maxcal += 0.05 * (maxcal - mincal);
                maxsys += 0.05 * (maxsys - minsys);
                mincal -= 0.05 * (maxcal - mincal);
                minsys -= 0.05 * (maxsys - minsys);

                cpgsvp(0.1, 0.9, 0.1+(float)n/npol*0.8, 0.1+(float)(n+1)/npol*0.8);

                // Plot cal
                cpgswin(-1., nchan, mincal, maxcal);

                if(n==0)
                {
                    cpgbox("bcnst", 0., 0, "bcnst", 0., 0);
                    cpglab("Channel Number", "Cal Amp", " ");
                }
                else
                {
                    cpgbox("bcst", 0., 0, "bcnst", 0., 0);
                    cpglab(" ", "Cal Amp", " ");
                }
                cpgsci(3);
                cpgline(nchan, &xx[0], &cal[0]);
                cpgsci(1);

                // Plot sys
                cpgswin(-1., nchan, minsys, maxsys);
                cpgbox(" ", 0., 0, "mst",0., 0);
                cpgmtxt("R", 3.0, 0.5, 0.5, "Sys Amp");
                cpgsci(4);
                cpgline(nchan, &xx[0], &sys[0]);

                cpgsci(1);

                if(n==0)
                    strcpy(cpol,"AA");
                if(n==1)
                    strcpy(cpol,"BB");
                if(n==2)
                    strcpy(cpol,"AB");
                cpgtext((float)(0.02*nchan), maxsys-0.15*(maxsys-minsys),cpol);

            }
            else
            {
                // Cross phase
                cpgsvp(0.1, 0.9, 0.7, 0.9);

                // Plot cal
                cpgswin(-1., nchan, -185.0, 185.0);
                cpgbox("bcst", 0., 0, "bcnst", 0., 0);
                cpglab(" ", "Cal Phs", " ");
                cpgbox(" ", 0., 0, "mst",0., 0);
                cpgmtxt("R", 3.0, 0.5, 0.5, "Sys Phs");

                cpgsci(5);
                cpgline(nchan, &xx[0], &cal[0]);

                // Plot sys
                cpgsci(4);
                cpgline(nchan, &xx[0], &sys[0]);

                cpgsci(1);
                cpgtext((float)(0.02*nchan), 130., cpol);

            }
//         }

        if( n == npol - 1 )
        {
//             char info[512];
//             sprintf(info,"File: %s  Centre Freq: %.3f MHz, BW: %.3f MHz",
//                     data->get_filename().c_str(), copy->get_centre_frequency(),
//                     copy->get_bandwidth());
//             cpgmtxt("T", 1.0, 0.5, 0.5, info);
        }
    }
}

