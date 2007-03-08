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
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_BASEMECHANICALMAPPING_H
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_BASEMECHANICALMAPPING_H

#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <sofa/core/objectmodel/BaseObject.h>
#include <sofa/core/BehaviorModel.h>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

class BaseMechanicalState;

/*! \class BaseMechanicalMapping
  *  \brief An interface to convert a mechanical model to an other mechanical model
  *  
  */

class BaseMechanicalMapping : public virtual objectmodel::BaseObject
{
public:
	virtual ~BaseMechanicalMapping() { }
	
	// Mapping Interface
	virtual void init() = 0;
	
	virtual void propagateX() = 0;
	virtual void propagateV() = 0;
	virtual void propagateDx() { }
	virtual void accumulateForce() { }
	virtual void accumulateDf() { }
	virtual void accumulateConstraint() {};
	
	virtual BaseMechanicalState* getMechFrom() = 0;
	virtual BaseMechanicalState* getMechTo() = 0;
};

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
