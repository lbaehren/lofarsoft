/***************************************************************************
 *   Copyright (C) 2005 by Sven Lafebre                                    *
 *   s.lafebre@astro.ru.nl                                                 *
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

#ifndef _PGPLOTYERCR_H_
#define _PGPLOTTERCR_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <Data/Data.h>

using namespace std;

namespace CR { // Namespace CR -- begin
  
  /*!
    \class PGPlotterCR
    
    \ingroup Display
    
    \brief Class for plotting with PGPlot
    
    \author Sven Lafebre    
  */
  class PGPlotterCR {
    //! Minimum for range of values on the x-axis
    double xAxisMin_;
    //! Maximum for range of values on the x-axis
    double xAxisMax_;
    //! Minimum for range of values on the y-axis
    double yAxisMin_;
    //! Maximum for range of values on the y-axis
    double yAxisMax_;
    //! Title of the plot
    string title_;
    //! Label for the x-axis
    string xLabel_;
    //! Label for the y-axis
    string yLabel_;
    
    string mode_;
    
  public:
    /*!
      \brief Constructor
    */
    PGPlotterCR();
    /*!
      \brief Destructor
    */
    ~PGPlotterCR();
    /*!
      \brief Set the range of values on the x-axis

      \param min -- Minimum for range of values on the x-axis
      \param max -- Maximum for range of values on the x-axis
    */
    void xAxis(double min,
	       double max);
    /*!
      \brief Set the range of values on the y-axis

      \param min -- Minimum for range of values on the y-axis
      \param max -- Maximum for range of values on the y-axis
    */
    void yAxis(double min,
	       double max);

    /*!
      \brief Get the minimum for range of values on the x-axis

      \return xAxisMin -- Minimum for range of values on the x-axis
    */
    double& xAxisMin() {return xAxisMin_;}
    /*!
      \brief Get the maximum for range of values on the x-axis

      \return xAxisMax -- Maximum for range of values on the x-axis
    */
    double& xAxisMax() {return xAxisMax_;}
    /*!
      \brief Get the minimum for range of values on the y-axis

      \return yAxisMin -- Minimum for range of values on the y-axis
    */
    double& yAxisMin() {return yAxisMin_;}
    /*!
      \brief Get the maximum for range of values on the y-axis
      \return yAxisMax -- Maximum for range of values on the y-axis
    */
    double& yAxisMax() {return yAxisMax_;}

    /*!
      \brief Get the title of the plot
      \param title -- The title of the plot
    */
    string& title()    {return title_;}
    /*!
      \brief Get the label on the x-axis
      \return xLabel -- The label on the x-axis
    */
    string& xLabel()   {return xLabel_;}
    /*!
      \brief Get the label on the y-axis
      \return yLabel -- The label on the y-Axis
    */
    string& yLabel()   {return yLabel_;}
    /*!
      \brief Get the plotting mode
      \return mode -- The plotting mode
    */
    string& mode()     {return mode_;}
    
    void initPlot();
  };
  
} // Namespace CR -- end

#endif /* _PGPLOTTER_H_ */
