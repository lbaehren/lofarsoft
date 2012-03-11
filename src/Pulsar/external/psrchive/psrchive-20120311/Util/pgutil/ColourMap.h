//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/pgutil/ColourMap.h,v $
   $Revision: 1.6 $
   $Date: 2010/01/05 05:41:57 $
   $Author: jonathan_khoo $ */

#ifndef __ColourMap_H
#define __ColourMap_H

#include "TextInterface.h"

namespace pgplot {

  //! Convenience interface to a bunch of regularly used colour maps
  class ColourMap : public Reference::Able {

  public:
    //! Available colour map names
    enum Name {
      GreyScale,
      Inverse,
      Heat,
      Cold,
      Plasma,
      Forest,
      AlienGlow,
      Test
    };

    //! Default constructor
    ColourMap ();

    ColourMap (const Name _name);

    //! Set the colour map name
    void set_name (Name name);
    Name get_name () const { return name; }

    //! Set the contrast (0 to 1; normally 1)
    void set_contrast (float contrast);
    float get_contrast () const { return contrast; }

    //! Set the brightness (0 to 1; normally 5)
    void set_brightness (float brightness);
    float get_brightness () const { return brightness; }

    //! Set the image transfer function to logarithmic
    void set_logarithmic (bool log) { logarithmic = log; }
    bool get_logarithmic () const { return logarithmic; }

    //! Apply the current attributes
    void apply ();

    //! The text interface to this class
    class Interface;
    
  protected:

    //! Colour map name
    Name name;

    //! Contrast
    float contrast;

    //! Brightness
    float brightness;

    //! Logarithmic image transfer function
    bool logarithmic;

  };

  class ColourMap::Interface : public TextInterface::To<ColourMap> {
  public:
    Interface (ColourMap* = 0);
  };

  //! Basis output operator
  std::ostream& operator << (std::ostream&, ColourMap::Name);

  //! Basis input operator
  std::istream& operator >> (std::istream&, ColourMap::Name&);


}

#endif
