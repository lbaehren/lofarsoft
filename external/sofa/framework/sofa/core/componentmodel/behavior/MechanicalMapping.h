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
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MECHANICALMAPPING_H
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MECHANICALMAPPING_H

#include <sofa/core/Mapping.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalMapping.h>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

template <class In, class Out>
class MechanicalMapping : public Mapping<In,Out>, public BaseMechanicalMapping
{
public:
	MechanicalMapping(In* from, Out* to);
	
	virtual ~MechanicalMapping();
	
	virtual void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in ) = 0;

	// Constraint applyJt Method
	virtual void applyJT( typename In::VecConst& /*out*/, const typename Out::VecConst& /*in*/ ){};

	// Mapping Interface
	virtual void init();

	virtual void propagateX();
	virtual void propagateV();
	virtual void propagateDx();

	virtual void accumulateForce();
	virtual void accumulateDf();

	virtual void accumulateConstraint();

	virtual BaseMechanicalState* getMechFrom();
	virtual BaseMechanicalState* getMechTo();
};

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
