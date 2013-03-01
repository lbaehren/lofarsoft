//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2013 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_PlotAnnotation_h
#define __Pulsar_PlotAnnotation_h

namespace Pulsar {

  class SimplePlot;

  //! Add generic annotations to any SimplePlot
  /*! This class is meant to provide an easy way to put
   * arbitrary simple/flexible annotations (circles, lines, 
   * text, etc) on to any plot derived from the SimplePlot
   * class.  Derived classes can put arbitrary pgplot
   * code into the draw() method, which will be executed
   * after drawing the main plot.  
   *
   * TODO: could eventually add a text interface for
   * common annotation types.
   */
  class PlotAnnotation : public Reference::Able {

  public:

    //! Draw the stuff
    virtual void draw(const Archive*) = 0;

  protected:

    //! Reference to the plot that the annotation goes on
    Reference::To<SimplePlot> parent;

  private:

    friend class SimplePlot;

  };

}

#endif
