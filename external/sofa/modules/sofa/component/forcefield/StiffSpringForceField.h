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
// Author: François Faure, INRIA-UJF, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
#ifndef SOFA_COMPONENT_FORCEFIELD_STIFFSPRINGFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_STIFFSPRINGFORCEFIELD_H

#include <sofa/component/forcefield/SpringForceField.h>

namespace sofa
{

namespace component
{

namespace forcefield
{

  /** SpringForceField able to evaluate and apply its stiffness. 
  This allows to perform implicit integration.
  Stiffness is evaluated and stored by the addForce method.
  When explicit integration is used, SpringForceField is slightly more efficient.
  */
template<class DataTypes>
class StiffSpringForceField : public SpringForceField<DataTypes>
{
public:
	typedef SpringForceField<DataTypes> Inherit;
	typedef typename DataTypes::VecCoord VecCoord;
	typedef typename DataTypes::VecDeriv VecDeriv;
	typedef typename DataTypes::Coord Coord;
	typedef typename DataTypes::Deriv Deriv;
	typedef typename Coord::value_type Real;
	typedef typename Inherit::Spring Spring;
	typedef core::componentmodel::behavior::MechanicalState<DataTypes> MechanicalState;
	class Mat3 : public fixed_array<Deriv,3>
	{
	public:
		Deriv operator*(const Deriv& v)
		{
			return Deriv((*this)[0]*v,(*this)[1]*v,(*this)[2]*v);
		}
	};

	//virtual const char* getTypeName() const { return "StiffSpringForceField"; }
	
protected:
	std::vector<Mat3> dfdx;
	double m_potentialEnergy;
	
        /// Accumulate the spring force and compute and store its stiffness
	void addSpringForce(double& potentialEnergy, VecDeriv& f1, const VecCoord& p1, const VecDeriv& v1, VecDeriv& f2, const VecCoord& p2, const VecDeriv& v2, int i, const Spring& spring);
	
        /// Apply the stiffness, i.e. accumulate df given dx
	void addSpringDForce(VecDeriv& df1, const VecDeriv& dx1, VecDeriv& df2, const VecDeriv& dx2, int i, const Spring& spring);
	
public:
	StiffSpringForceField(MechanicalState* object1, MechanicalState* object2, double ks=100.0, double kd=5.0)
	: SpringForceField<DataTypes>(object1, object2, ks, kd)
	{
	}
	
	StiffSpringForceField(double ks=100.0, double kd=5.0)
	: SpringForceField<DataTypes>(ks, kd)
	{
	}
	
	virtual void init();
	
        /// Accumulate f corresponding to x,v
        virtual void addForce();
	
        /// Accumulate df corresponding to dx
        virtual void addDForce();

        /// Return the potential energy of the springs
        virtual double getPotentialEnergy() { return m_potentialEnergy; }
};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
