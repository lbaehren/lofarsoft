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

#ifndef _PGPLOT_H_
#define _PGPLOT_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <Data/Data.h>

namespace lopestools {
  
  /*!
    \class PGPlot
    
    \brief Class for plotting with PGPlot
    
    \author Sven Lafebre
    
  */
  class PGPlot {
    double xAxisMin_;
    double xAxisMax_;
    
    double yAxisMin_;
    double yAxisMax_;
    
    std::string title_;
    std::string xLabel_;
    std::string yLabel_;
    
    std::string mode_;
    
  public:

    //! Constructor
    PGPlot();

    //! Destructor
    ~PGPlot();

    /*!
      \brief Set the range of values on the x-axis

      \param min -- 
      \param max -- 
    */
    void xAxis(double min,
	       double max);

    /*!
      \brief Set the range of values on the y-axis

      \param min -- 
      \param max -- 
    */
    void yAxis(double min,
	       double max);

    /*!
      \brief Get the minimum value for the x-axis

      \return xAxisMin
     */
    double& xAxisMin() {return xAxisMin_;}
    /*!
      \brief Get the maximum value for the x-axis

      \return xAxisMax
     */
    double& xAxisMax() {return xAxisMax_;}
    /*!
      \brief Get the minimum value for the y-axis

      \return yAxisMin
     */
    double& yAxisMin() {return yAxisMin_;}
    /*!
      \brief Get the maximum value for the y-axis

      \return yAxisMax
     */
    double& yAxisMax() {return yAxisMax_;}

    /*!
      \brief Get the title of the plot

      \return title -- 
    */
    std::string& title()    {return title_;}

    /*!
      \brief Get the the label for the x-axis

      \return xLabel -- 
    */
    std::string& xLabel()   {return xLabel_;}

    /*!
      \brief Get the label for the y-axis

      \return yLabel -- 
    */
    std::string& yLabel()   {return yLabel_;}

    /*!
      \brief Get the mode

      \return mode -- 
    */
    std::string& mode()     {return mode_;}
 
    //! Initialize the internal parameters of the object
    void initPlot();
  };
  
}

#endif /* _PGPLOT_H_ */
  
