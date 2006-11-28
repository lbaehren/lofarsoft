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

/* $Id: PluginBase.cc,v 1.9 2006/11/02 12:40:49 horneff Exp $*/

#include <Calibration/PluginBase.h>

using std::cerr;
using std::endl;

/*!
  \class PluginBase
*/

namespace LOPES {  // Namespace LOPES -- begin
  
  // ============================================================================
  //
  //  Construction
  //
  // ============================================================================
  
  template <class T>
  PluginBase<T>::PluginBase ()
  {;}
  
  template <class T>
  PluginBase<T>::PluginBase (const Matrix<T>& weights)
  {
    bool status (true);
    status = setWeights (weights);
  }
  
  template <class T>
  PluginBase<T>::PluginBase (const PluginBase& other)
  {
    copy (other);
  }
  
  // ============================================================================
  //
  //  Destruction
  //
  // ============================================================================
  
  template <class T>
  PluginBase<T>::~PluginBase ()
  {
    destroy();
  }
  
  // ============================================================================
  //
  //  Operators
  //
  // ============================================================================
  
  template <class T>
  PluginBase<T>& PluginBase<T>::operator= (const PluginBase<T>& other)
  {
    if (this != &other) {
      destroy ();
      copy (other);
    }
    return *this;
  }
  
  template <class T>
  void PluginBase<T>::copy (const PluginBase<T>& other)
  {
    // weights
    weights_p.resize(other.weights_p.shape());
    weights_p = other.weights_p;
    
    // misc parameters
    parameters_p = other.parameters_p;
  }
  
  template <class T>
  void PluginBase<T>::destroy ()
  {;}

// ==============================================================================
//
//  Parameters
//
// ==============================================================================

template <class T>
Record&  PluginBase<T>::parameters ()
{
  return parameters_p;
}

template <class T>
bool  PluginBase<T>::setParameters (Record const &param)
{
  bool status (true);
  try {
    uint i,nFields = param.nfields();
    casa::DataType type;
    for (i=0; i<nFields; i++){
      type = param.dataType(i);
      switch (type) {
      case casa::TpString:
	parameters_p.define(param.name(i),param.asString(i));
      case casa::TpBool:
	parameters_p.define(param.name(i),param.asBool(i));
      case casa::TpInt:
	parameters_p.define(param.name(i),param.asInt(i));
      case casa::TpUInt:
	parameters_p.define(param.name(i),param.asuInt(i));
      case casa::TpFloat:
	parameters_p.define(param.name(i),param.asFloat(i));
      case casa::TpDouble:
	parameters_p.define(param.name(i),param.asDouble(i));
      case casa::TpComplex:
	parameters_p.define(param.name(i),param.asComplex(i));
      case casa::TpDComplex:
	parameters_p.define(param.name(i),param.asDComplex(i));
      case casa::TpArrayBool:
	parameters_p.define(param.name(i),param.asArrayBool(i));
      case casa::TpArrayInt:
	parameters_p.define(param.name(i),param.asArrayInt(i));
      case casa::TpArrayUInt:
	parameters_p.define(param.name(i),param.asArrayuInt(i));
      case casa::TpArrayFloat:
	parameters_p.define(param.name(i),param.asArrayFloat(i));
      case casa::TpArrayDouble:
	parameters_p.define(param.name(i),param.asArrayDouble(i));
      case casa::TpArrayComplex:
	parameters_p.define(param.name(i),param.asArrayComplex(i));
      case casa::TpArrayDComplex:
	parameters_p.define(param.name(i),param.asArrayDComplex(i));
      default:
	cerr << "Sorry, setting a parameter of type " << type << 
	  " not implemented yet." << endl <<
	  "Please complain to the author!" << endl;
	status = false;
      };
    };
  } catch (AipsError x) {
    status = false;
  }
  return status;
}


template <class T>
bool PluginBase<T>::setWeights (const Matrix<T>& weights)
{
  bool status (true);

  weights_p.resize(weights.shape());
  weights_p = weights;

  return status;
}

// ==============================================================================
//
//  Methods
//
// ==============================================================================

template <class T>
bool PluginBase<T>::calcWeights (const Matrix<T>& data)
{
  bool status (true);

  try {
    weights_p.resize (data.shape());
    weights_p = T(1);
  } catch (AipsError x) {
    status = false;
  }

  return status;
}

template <class T>
bool PluginBase<T>::apply (Matrix<T>& data,
			   const bool& computeWeights)
{
  bool status (true);
  
  if (computeWeights) {
    status = calcWeights (data);
  }
  
  try {
//     data *= weights_p;
  } catch (AipsError x) {
    status = false;
  }

  return status;
}

template <class T>
bool PluginBase<T>::apply (Matrix<T> &outputData,
			   const Matrix<T>& inputData,
			   const bool& computeWeights)
{
  bool status (true);
  
  if (computeWeights) {
    status = calcWeights (inputData);
  }
  
  try {
    outputData.resize(inputData.shape());
//     outputData = inputData*weights_p;
  } catch (AipsError x) {
    status = false;
  }

  return status;
}


// =============================================================================
//
//  Template instantiation
//
// =============================================================================

template class PluginBase<double>;
template class PluginBase<DComplex>;

// These are old versions which should no be used!
template class PluginBase<int>;
template class PluginBase<float>;
template class PluginBase<Complex>;

}  // Namespace LOPES -- end
