/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Horizon.h"
#include "Meridian.h"

#include <slalib.h>

using namespace std;

void check (double a, double b, const char* name)
{
  if ( fabs( a - b ) < 1e-12 )
    return;

  throw Error (InvalidState, "", "%s %lf != %lf", name, a, b);
}

int main ()
{
  time_t temp = time(NULL);
  struct tm date = *gmtime(&temp);
  cerr << "Using current date/time: " << asctime(&date) << endl;
  MJD mjd (date);

  Horizon horizon;
  Meridian meridian;

  Directional* directional = &horizon;

  for (unsigned itest=0; itest < 2; itest ++)
  {
    directional->set_epoch (mjd);

    for (int lon=0; lon <= 180; lon+=10)
      for (int lat=-80; lat <= 80; lat+= 10)
      {
	directional->set_observatory_latitude( lat * M_PI/180 );
	directional->set_observatory_longitude( lon * M_PI/180 );

	for (int ra=0; ra <= 24; ra++)
	  for (int dec=-80; dec <= 80; dec+= 10) try
	  {
	    sky_coord coordinates;
	    coordinates.ra().setRadians( ra * M_PI/12.0 );
	    coordinates.dec().setRadians( dec * M_PI/180 );
	    
	    directional->set_source_coordinates( coordinates );
	    
	    if (itest == 0)
            {
	      double ignore;
	      double azimuth;
	      double elevation;
	      double parallactic_angle;
	      
	      slaAltaz (horizon.get_hour_angle(), 
			coordinates.dec().getRadians(),
			horizon.get_observatory_latitude(),
			&azimuth,                &ignore, &ignore,
			&elevation,              &ignore, &ignore,
			&parallactic_angle,      &ignore, &ignore);
	      
	      check (horizon.get_azimuth(), azimuth,
		     "azimuth");
	      check (horizon.get_elevation(), elevation,
		     "elevation");
	      check (horizon.get_vertical(), parallactic_angle,
		     "parallactic_angle");
	    }

	    double vertical = directional->get_vertical ();

	    double hour_angle = directional->get_hour_angle();

	    // "forget" the current hour angle
	    directional->set_hour_angle (M_PI*2);
	    directional->get_vertical ();

	    // test that set_hour_angle works
	    directional->set_hour_angle (hour_angle);

	    check (vertical, directional->get_vertical(), "vertical");

	  }
	  catch (Error& error)
	    {
	      cerr << error.get_message() << endl
		   << "ra=" << ra << " dec=" << dec << endl
		   << "lat=" << lat << " lon=" << lon << endl;
	      
	      return -1;
	    }
      }
    
    directional = & meridian;
  }

  cerr << "All tests passed" << endl;
  return 0;
}
