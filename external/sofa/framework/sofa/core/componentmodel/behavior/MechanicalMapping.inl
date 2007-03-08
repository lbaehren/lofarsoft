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
#ifndef SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MECHANICALMAPPING_INL
#define SOFA_CORE_COMPONENTMODEL_BEHAVIOR_MECHANICALMAPPING_INL

#include <sofa/core/Mapping.h>
#include <sofa/core/Mapping.inl>

namespace sofa
{

namespace core
{

namespace componentmodel
{

namespace behavior
{

template <class In, class Out>
MechanicalMapping<In,Out>::MechanicalMapping(In* from, Out* to)
: Mapping<In,Out>(from, to)
{
}
	
template <class In, class Out>
MechanicalMapping<In,Out>::~MechanicalMapping()
{
}

template <class In, class Out>
BaseMechanicalState* MechanicalMapping<In,Out>::getMechFrom()
{
	return this->fromModel;
}

template <class In, class Out>
BaseMechanicalState* MechanicalMapping<In,Out>::getMechTo()
{
	return this->toModel;
}

template <class In, class Out>
void MechanicalMapping<In,Out>::init()
{
	this->updateMapping();
}

template <class In, class Out>
void MechanicalMapping<In,Out>::propagateX()
{
    if (this->fromModel!=NULL && this->toModel->getX()!=NULL && this->fromModel->getX()!=NULL)
		apply(*this->toModel->getX(), *this->fromModel->getX());
}

template <class In, class Out>
void MechanicalMapping<In,Out>::propagateV()
{
    if (this->fromModel!=NULL && this->toModel->getV()!=NULL && this->fromModel->getV()!=NULL)
		applyJ(*this->toModel->getV(), *this->fromModel->getV());
}

template <class In, class Out>
void MechanicalMapping<In,Out>::propagateDx()
{
    if (this->fromModel!=NULL && this->toModel->getDx()!=NULL && this->fromModel->getDx()!=NULL)
		applyJ(*this->toModel->getDx(), *this->fromModel->getDx());
}

template <class In, class Out>
void MechanicalMapping<In,Out>::accumulateForce()
{
	if (this->fromModel!=NULL && this->toModel->getF()!=NULL && this->fromModel->getF()!=NULL)
		applyJT(*this->fromModel->getF(), *this->toModel->getF());
}

template <class In, class Out>
void MechanicalMapping<In,Out>::accumulateDf()
{
    if (this->fromModel!=NULL && this->toModel->getF()!=NULL && this->fromModel->getF()!=NULL)
		applyJT(*this->fromModel->getF(), *this->toModel->getF());
}

template <class In, class Out>
void MechanicalMapping<In,Out>::accumulateConstraint()
{
    if (this->fromModel!=NULL && this->toModel->getC()!=NULL && this->fromModel->getC()!=NULL)
		applyJT(*this->fromModel->getC(), *this->toModel->getC());
}

} // namespace behavior

} // namespace componentmodel

} // namespace core

} // namespace sofa

#endif
