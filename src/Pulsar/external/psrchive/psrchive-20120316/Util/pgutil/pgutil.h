//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/pgutil/pgutil.h,v $
   $Revision: 1.2 $
   $Date: 2009/01/29 08:12:09 $
   $Author: straten $ */

#ifndef __pgutil_H
#define __pgutil_H

#include <iostream>

namespace pgplot {

  /* UNITS: 0 => answer in normalized device coordinates
            1 => answer in inches
	    2 => answer in mm
	    3 => answer in absolute device coordinates (dots)
	    4 => answer in world coordinates
	    5 => answer as a fraction of the current viewport size
  */

  //! Coordinate frame of reference
  enum Units
    {
      //! normalized device coordinates
      Device = 0,
      //! imperial
      Inches = 1,
      //! metric
      Millimetres = 2,
      //! pixels
      Pixels = 3,
      //! world coordinates
      World = 4,
      //! fraction of the current viewport size
      Viewport = 5,
    };

  //! Basis output operator
  std::ostream& operator << (std::ostream&, Units);

  //! Basis input operator
  std::istream& operator >> (std::istream&, Units&);

  //! Get the current aspect ratio of the paper
  float get_aspect_ratio ();

  //! Get the current aspect ratio of the viewport
  float get_viewport_aspect_ratio ();

  //! Get scale factors to convert from 'from' units to 'to' units
  void get_scale (Units from, Units to, float& width, float& height);

  //! Set the paper size, adjusting either width or aspect ratio or both
  void set_paper_size (float width_cm, float aspect_ratio);

  //! Set the paper dimensions in pixels
  void set_dimensions (unsigned width_pixels, unsigned height_pixels);

}

#endif
