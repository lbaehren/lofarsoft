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
#ifndef SOFA_COMPONENT_CONSTRAINT_BOXCONSTRAINT_H
#define SOFA_COMPONENT_CONSTRAINT_BOXCONSTRAINT_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/component/constraint/FixedConstraint.h>
#include <sofa/defaulttype/Vec.h>
//#include <sofa/component/topology/MultiResSparseGridTopology.h>

namespace sofa
{

namespace component
{

namespace constraint
{

  /** Keep fixed all the particles located inside a given box.
  */
template <class DataTypes>
class BoxConstraint : public FixedConstraint<DataTypes>
{
public:
	typedef FixedConstraint<DataTypes> Inherited;
	typedef typename DataTypes::VecCoord VecCoord;
	typedef typename DataTypes::VecDeriv VecDeriv;
	typedef typename DataTypes::Coord Coord;
	typedef typename DataTypes::Deriv Deriv;
        typedef typename DataTypes::Real Real;
		typedef typename Inherited::SetIndex SetIndex;
        typedef defaulttype::Vec<6,Real> Vec6;

protected:
	
	//float Xmin,Xmax,Ymin,Ymax,Zmin,Zmax;

public:
	BoxConstraint();
	
	~BoxConstraint();
	
	void init();
        
        //virtual const char* getTypeName() const { return "BoxConstraint"; }
        DataField<Vec6> box;
	
	
	// -- VisualModel interface
	
	void draw();
	
	bool addBBox(double* minBBox, double* maxBBox);
};

} // namespace constraint

} // namespace component

} // namespace sofa

#endif
