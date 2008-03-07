/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

/* $Id: DynamicSpectrum.cc,v 1.1 2006/02/22 18:53:34 bahren Exp $*/

#include "DynamicSpectrum.h"

namespace CR {  // Namespace CR -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  // ------------------------------------------------------------ DynamicSpectrum
  
  DynamicSpectrum::DynamicSpectrum ()
  {;}
  
  // ------------------------------------------------------------ DynamicSpectrum
  
  DynamicSpectrum::DynamicSpectrum (const Vector<double>& crval,
				    const Vector<double>& cdelt,
				    const Vector<String>& units)
  {
  }
  
  // ------------------------------------------------------------ DynamicSpectrum
  
  DynamicSpectrum::DynamicSpectrum (casa::ObsInfo obsInfo,
				    LinearCoordinate timeAxis,
				    SpectralCoordinate freqAxis)
  {
    obsInfo_p  = obsInfo;
    timeAxis_p = timeAxis;
    freqAxis_p = freqAxis;
  }
  
  // ------------------------------------------------------------ DynamicSpectrum
  
  DynamicSpectrum::DynamicSpectrum (DynamicSpectrum const& other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  DynamicSpectrum::~DynamicSpectrum ()
  {
    destroy();
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================

  // ------------------------------------------------------------------ operator=
  
  DynamicSpectrum &DynamicSpectrum::operator= (DynamicSpectrum const &other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  // ----------------------------------------------------------------------- copy

  void DynamicSpectrum::copy (DynamicSpectrum const& other)
  {
    dynamicSpectrum_p.resize(other.dynamicSpectrum_p.shape());
    dynamicSpectrum_p = other.dynamicSpectrum_p;
    //
    obsInfo_p  = other.obsInfo_p;
    timeAxis_p = other.timeAxis_p;
    freqAxis_p = other.freqAxis_p;
  }
  
  // -------------------------------------------------------------------- destroy

  void DynamicSpectrum::destroy ()
  {;}
  
  // ============================================================================
  //
  //  Parameters
  //
  // ============================================================================

  // ---------------------------------------------------------------- setTimeAxis
  
  void DynamicSpectrum::setTimeAxis (const double& crval,
				     const double& cdelt,
				     const String& unit)
  {
    Quantum<double> referenceValue (crval,unit);
    Quantum<double> increment (cdelt,unit);
    
    setTimeAxis (referenceValue,
		 increment);
  }
  
  // ---------------------------------------------------------------- setTimeAxis
  
  void DynamicSpectrum::setTimeAxis (const Quantum<double>& crval,
				     const Quantum<double>& cdelt)
  {
    Vector<Quantum<double> > refVal (1);
    Vector<Quantum<double> > increment (1);
    Vector<double> refPix (1,0.0);
    Matrix<double> pc(1,1);
    Vector<String> name(1);
    
    // set proper values
    refVal(0)     = crval;
    increment(0)  = cdelt;
    pc            = 0;
    pc.diagonal() = 1.0;
    name          = "time";
    
    LinearCoordinate lc (name,
			 refVal,
			 increment,
			 pc,
			 refPix);
    timeAxis_p = lc;
  }
  
  // ----------------------------------------------------------- setFrequencyAxis
  
  void DynamicSpectrum::setFrequencyAxis (const double& crval,
					  const double& cdelt,
					  const String& unit)
  {
    Quantum<double> referenceValue (crval,unit);
    Quantum<double> increment (cdelt,unit);
    
    setFrequencyAxis (referenceValue,
		      increment);
  }
  
  // ----------------------------------------------------------- setFrequencyAxis

  void DynamicSpectrum::setFrequencyAxis (const Quantum<double>& crval,
					  const Quantum<double>& cdelt)
  {
    double crpix (0.0);
    Quantum<double> restfreq (0.0,"Hz");
    
    SpectralCoordinate sc (casa::MFrequency::TOPO,
			   crval,
			   cdelt,
			   crpix,
			   restfreq);
    freqAxis_p = sc;
  }
  
  // ----------------------------------------------------------- coordinateSystem

  casa::CoordinateSystem DynamicSpectrum::coordinateSystem ()
  {
    casa::CoordinateSystem cs;
    
    //   cs.setObsInfo (csys_p.obsInfo());
    cs.addCoordinate (freqAxis_p);
    cs.addCoordinate (timeAxis_p);
    
    return cs;
  }
  
  // -------------------------------------------------------------------- summary

  void DynamicSpectrum::summary (std::ostream &os)
  {
    os << "[DynamicSpectrum] Summary of object properties" << std::endl;
    os << "-- Shape = " << dynamicSpectrum_p.shape() << std::endl;
  }
  
  // ==============================================================================
  //
  //  Methods
  //
  // ==============================================================================

  // -------------------------------------------------------------- averageSpectrum
  
  Vector<float> DynamicSpectrum::averageSpectrum ()
  {
    casa::IPosition shape (dynamicSpectrum_p.shape());
    Vector<float> averageSpectrum (shape(0));
    
    // do something
    for (int freq(0); freq<shape(0); freq++) {
      averageSpectrum(freq) = abs(sum(conj(dynamicSpectrum_p.column(freq))
				      *dynamicSpectrum_p.column(freq)));
    }
    
    // return the result
    return averageSpectrum;
  }

  // ------------------------------------------------------------------- totalPower
  
  Vector<float> DynamicSpectrum::totalPower ()
  {
    casa::IPosition shape (dynamicSpectrum_p.shape());
    Vector<float> totalPower (shape(0));
    
    // compute
    for (int n(0); n<shape(0); n++) {
      totalPower(n) = abs(sum(conj(dynamicSpectrum_p.row(n))
			      *dynamicSpectrum_p.row(n)));
    }
    
    // return the result
    return totalPower;
  }
  
}  // Namespace CR -- end
