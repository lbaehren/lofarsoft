/***************************************************************************
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

/* $Id: dgfitgauss.h,v 1.3 2007/06/21 16:07:31 horneff Exp $*/

#ifndef DFITGAUSS_H
#define DFITGAUSS_H

#include <crtools.h>

#include <unistd.h>
#include <assert.h>

#include <casa/stdio.h>
#include <casa/string.h>
#include <casa/iostream.h>
#include <casa/BasicSL/String.h>
#include <casa/Arrays/Vector.h>
#include <casa/Exceptions/Error.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/ArrayIO.h>
#include <scimath/Functionals/Gaussian1D.h>
#include <scimath/Fitting/NonLinearFitLM.h>

#include <Analysis/dGaussFitState.h>

#include <casa/namespace.h>


namespace CR { // Namespace CR -- begin
  
  /*!
    \class dfitgauss
    
    \ingroup Analysis
    
    \brief Addaption of the glish gausfitter client
    
    \author Andreas Horneffer

    \date 2007/06/17

    \test tdfitgauss.cc


    <h3>Prerequisite</h3>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */  

  class dfitgauss {

    // \brief stoarage for the state
    dGaussFitState state_;

    public:
    
    // ------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    dfitgauss ();
        
    // -------------------------------------------------------------- Destruction

    /*!
      \brief Destructor
    */
    ~dfitgauss ();
    
    // -------------------------------------------------------------- Destruction
    
    Record fitXY(Vector<Double> &x, Vector<Double> &y, 
		 Vector<Double> sigma=Vector<Double>(), Bool verbose=False);
    Vector<Double> evalGaussian(Vector <Double> x);

    Record getState(){
      return state_.state();
    };

    void setState(Record newstate){
      state_.setState(newstate);
    };

  };
  
} // Namespace CR -- end

#endif /* SECONDSTAGEPIPELINE_H */
  
