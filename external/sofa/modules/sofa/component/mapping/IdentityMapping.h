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
#ifndef SOFA_COMPONENT_MAPPING_IDENTITYMAPPING_H
#define SOFA_COMPONENT_MAPPING_IDENTITYMAPPING_H

#include <sofa/core/componentmodel/behavior/MechanicalMapping.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <vector>

namespace sofa
{

namespace component
{

namespace mapping
{

template <class BasicMapping>
class IdentityMapping : public BasicMapping
{
public:
	typedef BasicMapping Inherit;
	typedef typename Inherit::In In;
	typedef typename Inherit::Out Out;
	typedef typename Out::VecCoord VecCoord;
	typedef typename Out::VecDeriv VecDeriv;
	typedef typename Out::Coord Coord;
	typedef typename Out::Deriv Deriv;
	
	IdentityMapping(In* from, Out* to)
	: Inherit(from, to)
	{
	}
	
	virtual ~IdentityMapping()
	{
	}
	
	void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
	
	void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
	
	void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
};

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
