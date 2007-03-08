/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#ifndef SOFA_CORE_BASICMECHANICALMAPPING_H
#define SOFA_CORE_BASICMECHANICALMAPPING_H

#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include "Sofa-old/Abstract/BaseObject.h"
#include "Sofa-old/Abstract/BehaviorModel.h"

namespace Sofa
{

namespace Core
{

class BasicMechanicalModel;

/*! \class BasicMechanicalMapping
  *  \brief An interface to convert a mechanical model to an other mechanical model
  *  
  */

class BasicMechanicalMapping : public virtual Abstract::BaseObject
{
public:
	virtual ~BasicMechanicalMapping() { }
	
	// Mapping Interface
	virtual void init() = 0;
	
	virtual void propagateX() = 0;
	virtual void propagateV() = 0;
	virtual void propagateDx() { }
	virtual void accumulateForce() { }
	virtual void accumulateDf() { }
	virtual void accumulateConstraint() {};
	
	virtual BasicMechanicalModel* getMechFrom() = 0;
	virtual BasicMechanicalModel* getMechTo() = 0;
};

} // namespace Core

} // namespace Sofa

#endif
