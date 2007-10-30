/*-------------------------------------------------------------------------*
 | $Id$ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SIMPLEPLOT_H
#define SIMPLEPLOT_H

// Standard library header files
#include <iostream>
#include <string>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>

//use plplot as default
#ifdef HAVE_PLPLOT
  #include <plplot/plConfig.h>
  #include <plplot/plplot.h>
  #ifdef DEBUGGING_MESSAGES
    #warning Using plplot for output
  #endif
  //this makes life easier!
  #undef HAVE_PGPLOT
#else
  //use pgplot if plplot is not available
  #ifdef HAVE_PGPLOT
    // #include <casa/System/PGPlotter.h>
    #include <Display/PGPlotterLocal.h>
    #ifdef DEBUGGING_MESSAGES
      #warning Using the old PGPlot for output
    #endif
  #endif
#endif

using casa::Matrix;
using casa::String;
using casa::Vector;

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \class SimplePlot
    
    \ingroup Display
    
    \brief Generate simple (postscript) plots with PLplot or the CASA pgplotter interface
    
    \author Andreas Horneffer

    \date 2007/06/19

    \test tSimplePlot.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  
  class SimplePlot {
    
#ifdef HAVE_PGPLOT
    // \brief Pointer to the local pgplotter object
    PGPlotterLocal *plotter_p;
#endif

    Int ppCharacterHeight;
    Int ppLineWidth;

  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    SimplePlot ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~SimplePlot ();
        
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, SimplePlot.
    */
    std::string className () const {
      return "SimplePlot";
    }

    /*!
      \brief Provide a summary of the internal status
    */
    inline void summary () {
      summary (std::cout);
    }

    /*!
      \brief Provide a summary of the internal status
    */
    void summary (std::ostream &os);    

    // ------------------------------------------------------------------ Methods
    

    /*!
      \brief Initialize a new plot. An old plot gets flushed.
      
      \param file - pathname of the file
      \param xmin - Minimal x-value of the plotting area
      \param xmax - Maximal x-value of the plotting area
      \param ymin - Minimal y-value of the plotting area
      \param ymax - Maximal y-value of the plotting area
      \param axis=0 - axis parameter of "env()" see PLplot documentation
      \param just=0 - set to 1 to have x and y axes on the same scale
      \param col=1 - color index for the coordinate axes
      \param cheight=1 - size of the characters
      \param linewidth=1 - thickness of the lines

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool InitPlot (String file,
		   Double xmin,
		   Double xmax,
		   Double ymin,
		   Double ymax, 
		   Int axis=0,
		   Int just=0,
		   Int col=1,
		   Int cheight=1,
		   Int linewidth=1);

    /*!
      \brief Plot a line into the current plot
      
      \param xvals - x-values of the line to be plotted
      \param yvals - y-values of the line to be plotted
      \param col=2 - color index for the line
      \param style=1 - index of the line style (see pllsty() or sls() documentation)

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool PlotLine(Vector<Double> xvals,
		  Vector<Double> yvals,
		  Int col=2,
		  Int style=1);

    /*!
      \brief Plot symbols (and errorbars) into the current plot
      
      \param xvals - x-values of the symbols to be plotted
      \param yvals - y-values of the symbols to be plotted
      \param yerrs - error (length of the error-bar) of the y-axis
      \param xerrs - error (length of the error-bar) of the x-axis
      \param col=2 - color index for the ymbols and error-bars
      \param symbol=2 - index of the symbol type
      \param ticklength=1 - ticklength of the error-bars

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool PlotSymbols (Vector<Double> xvals,
		      Vector<Double> yvals, 
		      Vector<Double> yerrs=Vector<Double>(),
		      Vector<Double> xerrs=Vector<Double>(),
		      Int col=2, 
		      Int symbol=2,
		      Int ticklength=1);
    
    /*!
      \brief Add labels to the plot
      
      \param xlabel - label of the x-axis
      \param ylabel - label of the y-axis
      \param toplabel - label above the plot
      \param col=1 - color index for the labels

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */
    Bool AddLabels (String xlabel=String(),
		    String ylabel=String(), 
		    String toplabel=String(),
		    Int col=1);
    

    /*!
      \brief Do a complete plot with one call
      
      \param file - pathname of the file
      \param xvals - x-values of the symbols to be plotted
      \param yvals - y-values of the symbols to be plotted
      \param yerrs - error (length of the error-bar) of the y-axis
      \param xerrs - error (length of the error-bar) of the x-axis
      \param xlabel - label of the x-axis
      \param ylabel - label of the y-axis
      \param toplabel - label above the plot
      \param linecol - color index for the line or symbols
      \param plotASline - plot the data as a line or as symbols
      \param style - linestyle or symbol type 
      \param logx - make the x-axis on a logarithmic scale
      \param logy - make the y-axis on a logarithmic scale
      \param printingplot - make a plot suitable for inclusion in talks or papers

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.
    */

    Bool quickPlot (String file,
		    Vector<Double> xvals,
		    Vector<Double> yvals, 
		    Vector<Double> yerrs=Vector<Double>(),
		    Vector<Double> xerrs=Vector<Double>(),
		    String xlabel=String(),
		    String ylabel=String(),
		    String toplabel=String(), 
		    Int linecol=1,
		    Bool plotASline=True,
		    Int style=1,
		    Bool logx=False, 
		    Bool logy=False,
		    Bool printingplot=False);


   /*!
      \brief Do a complete 2-dimensional plot with one call
      
      \param file     - pathname of the file
      \param zvals    - the data to be plotted (i.e. the z-values)
      \param xmin     - minimum x-value (the column zvals.column(0) has x-value xmin)
      \param xmax     - maximum x-value (the column zvals.column(zvals.ncolumn()-1) has x-value xmax)
      \param ymin     - minimum y-value (the row zvals.row(0) has y-value ymin)
      \param ymax     - maximum y-value (the row zvals.row(zvals.nrow()-1) has y-value ymax)
      \param xlabel   - label of the x-axis
      \param ylabel   - label of the y-axis
      \param toplabel - label above the plot
      \param printingplot - make a plot suitable for inclusion in talks or papers
      \param nLevels  - number of different (color) levels to use
      \param nCLevels - number of contour lines to plot (<tt>nCLevels<=0</tt> means no contours)
      \param cCol     - color index for the contour lines 

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.

      Generate a 2d color-map plot with the plplot library, i.e. no output with pgplot. 
      Currently (29. Okt. 2007) only a simple black to white colormap is supported. <br>
      <b>Note:</b> The <tt>plshades()</tt> function does a linear interpolation while plotting.
    */

    Bool quick2Dplot(String file, Matrix<Double> zvals, Double xmin, Double xmax, 
		     Double ymin, Double ymax,	     
		     String xlabel=String(), String ylabel=String(), String toplabel=String(),
		     Bool printingplot=False, int nLevels=20, int nClevels=0, int cCol=2);

   /*!
      \brief Add contour lines to an existing 2d plot
      
      \param zvals    - the data to be plotted (i.e. the z-values)
      \param xmin     - minimum x-value (the column zvals.column(0) has x-value xmin)
      \param xmax     - maximum x-value (the column zvals.column(zvals.ncolumn()-1) has x-value xmax)
      \param ymin     - minimum y-value (the row zvals.row(0) has y-value ymin)
      \param ymax     - maximum y-value (the row zvals.row(zvals.nrow()-1) has y-value ymax)
      \param nCLevels - number of contour lines to plot 
      \param cCol     - color index for the contour lines 

      \return ok  -- Was operation successful? Returns <tt>True</tt> if yes.

      Uses the plplot library, i.e. no output with pgplot. 
    */
    Bool addContourLines(Matrix<Double> zvals, Double xmin, Double xmax, Double ymin, Double ymax,
			 int nClevels, int cCol=2);


  private:
        
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
 
    /*!
      \brief Unconditional deletion 
    */
    void init(void);

  }; // class SimplePlot -- end
  
} // Namespace CR -- end

#endif /* SIMPLEPLOT_H */
  
