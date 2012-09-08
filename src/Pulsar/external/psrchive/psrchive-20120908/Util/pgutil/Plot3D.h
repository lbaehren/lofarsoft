//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/pgutil/Plot3D.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $*/

#ifndef __CurvePlotter2D3_H
#define __CurvePlotter2D3_H

#include "Cartesian.h"
#include <vector>

namespace pgplot {
  
  class Plot3D {    

  public:

    static bool verbose;
    
    Plot3D() { init(); }
    virtual ~Plot3D() {}
    
    // primitives
    void move (const Cartesian& pt);
    void draw (const Cartesian& pt);
    void plot (const Cartesian& pt, int symbol);
    void text (const Cartesian& pt, const char* text, float align=.5);

    void arrow (const Cartesian& from, const Cartesian& to);

    void poly (const std::vector<Cartesian>& pts);

    // make hatching run parallel to pp
    void set_hatch (const Cartesian& hp);

    // set camera position - phi and theta in degrees
    void set_camera (double theta, double phi);

    void where (float& x, float& y, const Cartesian& pt);

    const Cartesian& get_xaxis () { return vx_axis; };
    const Cartesian& get_yaxis () { return vy_axis; };

  protected:
    Cartesian vx_axis;
    Cartesian vy_axis;

    void init(); 
  };
}

#endif

