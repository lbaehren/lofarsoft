/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2008                                                  *
 *   Frank Schroeder (<mail>)                                                     *
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

#ifndef COMPLETEPIPELINE_H
#define COMPLETEPIPELINE_H

// Standard library header files
#include <iostream>
#include <string>

// This class is derived form CRinvFFT an adds upsampling
#include "Analysis/CRinvFFT.h"
#include "Display/SimplePlot.h"


namespace CR { // Namespace CR -- begin
  
  /*!
    \class CompletePipeline
    
    \ingroup Analysis
    
    \brief Brief description for class CompletePipeline
    
    \author Frank Schroeder

    \date 2008/01/30

    \test tCompletePipeline.cc
    
    <h3>Prerequisite</h3>
    
    <ul type="square">
      <li>[start filling in your text here]
    </ul>
    
    <h3>Synopsis</h3>

    This pipeline shall contain upsampling capabilities but is not working yet.
    
    <h3>Example(s)</h3>
    
  */  
  class CompletePipeline : public CRinvFFT {

  protected:
    //! Start time of the interval diplayed in the plot
    double plotStart_p;
    //! Stop time of the interval diplayed in the plot
    double plotStop_p;
    
  public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    CompletePipeline ();
    
    /*!
      \brief Copy constructor
      
      \param other -- Another CompletePipeline object from which to create this new
      one.
    */
    CompletePipeline (CompletePipeline const &other);
    
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~CompletePipeline ();
    
    // ---------------------------------------------------------------- Operators
    
    /*!
      \brief Overloading of the copy operator
      
      \param other -- Another CompletePipeline object from which to make a copy.
    */
    CompletePipeline& operator= (CompletePipeline const &other); 
    
    // --------------------------------------------------------------- Parameters
    
    /*!
      \brief Get the name of the class
      
      \return className -- The name of the class, CompletePipeline.
    */
    std::string className () const {
      return "CompletePipeline";
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
      \brief Get the start time of the interval diplayed in the plot

      \return plotStart -- Start time of the interval diplayed in the plot
    */
    inline double getPlotStart () {
      return plotStart_p;
    }
    
    /*!
      \brief Set the start time of the interval diplayed in the plot

      \param plotStart -- Start time of the interval diplayed in the plot
    */
    inline void setPlotStart (double const &plotStart) {
      plotStart_p = plotStart;
    }
    
    /*!
      \brief Get the stop time of the interval diplayed in the plot

      \return plotStop -- Stop time of the interval diplayed in the plot
    */
    inline double getPlotStop () {
      return plotStop_p;
    }
    
    /*!
      \brief Set the stop time of the interval diplayed in the plot

      \param plotStop -- Stop time of the interval diplayed in the plot
    */
    inline void setPlotStop (double const &plotStop) {
      plotStop_p = plotStop;
    }

    /*!
      \brief Set the interval diplayed in the plot

      \param plotStart -- Start time of the interval diplayed in the plot
      \param plotStop  -- Stop time of the interval diplayed in the plot
    */
    inline void setPlotInterval (double const &plotStart,
				 double const &plotStop) {
      plotStart_p = plotStart;
      plotStop_p  = plotStop;
    }


    /*!
      \brief Plots the CC-beam

      \param filename         -- Name of the .ps file (without ".ps") the plot will be written to
      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- Selection of antennas considered for the plot
    */

    void plotCCbeam(const string filename, DataReader *dr,
 			     Vector<Bool> antennaSelection = Vector<Bool>());

    /*!
      \brief Plots the X-beam

      \param filename         -- Name of the .ps file (without ".ps") the plot will be written to
      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- Selection of antennas considered for the plot
    */

    void plotXbeam(const string filename, DataReader *dr,
 			     Vector<Bool> antennaSelection = Vector<Bool>());

    /*!
      \brief Plots the fieldstrength of all antennas after beam forming

      \param filename         -- Name of the .ps file (without ".ps") the plot will be written to
      \param DataReader       -- DataReader (LopesEventIn)
      \param antennaSelection -- Selection of antennas considered for the plot
      \param seperated        -- true = an individual plot for each antenna will be created
                                 (the antenna number serves as file name appendix).
    */

    void plotAllAntennas(const string filename, DataReader *dr,
 			     Vector<Bool> antennaSelection = Vector<Bool>(), bool seperated = false);

    
    
    
  private:
    
    /*!
      \brief Unconditional copying
    */
    void copy (CompletePipeline const &other);
    
    /*!
      \brief Unconditional deletion 
    */
    void destroy(void);
    
  };
  
} // Namespace CR -- end

#endif /* COMPLETEPIPELINE_H */
  
