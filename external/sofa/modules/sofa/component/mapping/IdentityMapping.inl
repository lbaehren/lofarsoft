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
#ifndef SOFA_COMPONENT_MAPPING_IDENTITYMAPPING_INL
#define SOFA_COMPONENT_MAPPING_IDENTITYMAPPING_INL

#include <sofa/component/mapping/IdentityMapping.h>
#include <sofa/core/componentmodel/behavior/MechanicalMapping.inl>


namespace sofa
{

namespace component
{

namespace mapping
{

template <class BasicMapping>
void IdentityMapping<BasicMapping>::apply( typename Out::VecCoord& out, const typename In::VecCoord& in )
{
	out.resize(in.size());
	//if (this->fromModel->getContext()->getTopology()!=NULL)
	//	this->toModel->getContext()->setTopology(this->fromModel->getContext()->getTopology());
	for(unsigned int i=0;i<out.size();i++)
	{
		out[i] = in[i];
	}
}

template <class BasicMapping>
void IdentityMapping<BasicMapping>::applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in )
{
	out.resize(in.size());
	for(unsigned int i=0;i<out.size();i++)
	{
		out[i] = in[i];
	}
}

template <class BasicMapping>
void IdentityMapping<BasicMapping>::applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in )
{
	for(unsigned int i=0;i<out.size();i++)
	{
		out[i] += in[i];
	}
}

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
