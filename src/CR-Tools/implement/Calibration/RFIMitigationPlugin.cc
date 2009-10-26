/***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Kalpana Singh (<k.singh@astro.ru.nl>)                                                     *
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

/* $Id$*/

#include <Calibration/RFIMitigationPlugin.h>

namespace CR { //namespace CR -- begin

// ==============================================================================
//
//  Construction
//
// ==============================================================================

RFIMitigationPlugin::RFIMitigationPlugin ()
                    : PluginBase<DComplex> ()
{

  Int tmpDataBlockSize=1024*2 ;
  Int tmpNOfsegments = 25 ;
  
  Matrix<Double> tmpOPtimisedWeights ;
  
   parameters_p.define("dataBlockSize",tmpDataBlockSize ) ;
   parameters_p.define("nOfSegments",tmpNOfsegments ) ; 
   parameters_p.define("optimisedWeights",tmpOPtimisedWeights ) ;
   }




RFIMitigationPlugin::RFIMitigationPlugin (RFIMitigationPlugin const &other)
  : PluginBase<DComplex> ()
{
  copy (other);
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

RFIMitigationPlugin::~RFIMitigationPlugin ()
{
  destroy();
}

void RFIMitigationPlugin::destroy ()
{;}

// ==============================================================================
//
//  Operators
//
// ==============================================================================

RFIMitigationPlugin& RFIMitigationPlugin::operator = (RFIMitigationPlugin const &other)
{
  if (this != &other) {
    destroy ();
    copy (other);
  }
  return *this;
}

void RFIMitigationPlugin::copy (RFIMitigationPlugin const &other)
{;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

 Bool RFIMitigationPlugin :: setDataBlockSize( Int NewDataBlockSize )
 {
  
  try {
  parameters_p.define("dataBlockSize", NewDataBlockSize ) ;
  }
  catch( AipsError x){
  cerr << "RFIMitigationPlugin:: setDataBlockSize" << x.getMesg () << endl ;
  return False ;
  }
  return True ;
 
}


 Bool RFIMitigationPlugin :: setNOfSegments( Int NewNOfSegments )
 {
  
  try {
  parameters_p.define("nOfSegments",NewNOfSegments ) ;
  }
  catch( AipsError x){
  cerr << "RFIMitigationPlugin:: setNOfSegments" << x.getMesg () << endl ;
  return False ;
  }
  return True ;
 
}
// ==============================================================================
//
//  Methods
//
// ==============================================================================

Bool RFIMitigationPlugin :: calcWeights(const Matrix<DComplex> &spectra)
{
 
 try{
 
   Matrix<Double> optimizedWeights ;
   
   optimizedWeights = rfi_p.getOptimizedSpectra( spectra,
   						  parameters_p.asInt("dataBlockSize"),
						  parameters_p.asInt("nOfSegments") ) ;
						  
  parameters_p.define("optimisedWeights", optimizedWeights) ;
  if (!optimizedWeights.shape().isEqual(spectra.shape())) {
    cerr << "RFIMitigation :: calcWeights Input and output matrices have different shape!" << endl;
  };
  weights_p.resize(optimizedWeights.shape());
  convertArray(weights_p,optimizedWeights);
  }
  
  catch(AipsError x){
  cerr << "RFIMitigation :: calcWeights" << x.getMesg() << endl ;
  return False ;
  }
 return True ;
  
 }

}  // Namespace CR -- end





