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
#ifndef SOFA_COMPONENT_MAPPING_SURFACEIDENTITYMAPPING_H
#define SOFA_COMPONENT_MAPPING_SURFACEIDENTITYMAPPING_H

#include <sofa/core/componentmodel/behavior/MechanicalMapping.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>
#include <vector>

namespace sofa
{

namespace component
{

namespace mapping
{

/**
 * @class SurfaceIdentityMapping
 * @brief Map a surface-only model to a plain volumetric one
 * This mapping establish a 1-to-1 correspondance between the points of out and
 * (some of) the points of in.
 * Will come in handy for mapping a visual 'skin' on a volumetric model.
 */
template <class BasicMapping>
class SurfaceIdentityMapping : public BasicMapping
{
private:
	/// Correspondance array
	std::vector<unsigned int> correspondingIndex;

public:
	typedef BasicMapping Inherit;
	typedef typename Inherit::In In;
	typedef typename Inherit::Out Out;
	typedef typename Out::VecCoord OutVecCoord;
	typedef typename Out::VecDeriv OutVecDeriv;
	typedef typename Out::Coord OutCoord;
	typedef typename Out::Deriv OutDeriv;
	typedef typename In::VecCoord InVecCoord;
	typedef typename In::VecDeriv InVecDeriv;
	typedef typename In::Coord InCoord;
	typedef typename In::Deriv InDeriv;
	typedef typename InCoord::value_type Real;

	
	SurfaceIdentityMapping(In* from, Out* to);

	void init();
	
	virtual ~SurfaceIdentityMapping();
	
	void apply( typename Out::VecCoord& out, const typename In::VecCoord& in );
	
	void applyJ( typename Out::VecDeriv& out, const typename In::VecDeriv& in );
	
	void applyJT( typename In::VecDeriv& out, const typename Out::VecDeriv& in );
};

} // namespace mapping

} // namespace component

} // namespace sofa

#endif
