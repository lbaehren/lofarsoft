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
#ifndef SOFA_COMPONENT_FORCEFIELD_MESHSPRINGFORCEFIELD_H
#define SOFA_COMPONENT_FORCEFIELD_MESHSPRINGFORCEFIELD_H

#include <sofa/component/forcefield/StiffSpringForceField.h>
#include <set>

namespace sofa
{

namespace component
{

namespace forcefield
{

template<class DataTypes>
class MeshSpringForceField : public StiffSpringForceField<DataTypes>
{
public:
	typedef typename DataTypes::VecCoord VecCoord;
	typedef typename DataTypes::VecDeriv VecDeriv;
	typedef typename DataTypes::Coord Coord;
	typedef typename DataTypes::Deriv Deriv;
	typedef typename Coord::value_type Real;
	
protected:
	Real linesStiffness;
	Real linesDamping;
	Real trianglesStiffness;
	Real trianglesDamping;
	Real quadsStiffness;
	Real quadsDamping;
	Real tetrasStiffness;
	Real tetrasDamping;
	Real cubesStiffness;
	Real cubesDamping;

	void addSpring(std::set<std::pair<int,int> >& sset, int m1, int m2, Real stiffness, Real damping);
	
public:
	MeshSpringForceField()
	: linesStiffness(0), linesDamping(0),
	  trianglesStiffness(0),  trianglesDamping(0),
	  quadsStiffness(0), quadsDamping(0),
	  tetrasStiffness(0), tetrasDamping(0),
	  cubesStiffness(0), cubesDamping(0)
	{
	}
        
        virtual double getPotentialEnergy();
        
	
	Real getStiffness() const { return linesStiffness; }
	Real getLinesStiffness() const { return linesStiffness; }
	Real getTrianglesStiffness() const { return trianglesStiffness; }
	Real getQuadsStiffness() const { return quadsStiffness; }
	Real getTetrasStiffness() const { return tetrasStiffness; }
	Real getCubesStiffness() const { return cubesStiffness; }
	void setStiffness(Real val)
	{
		linesStiffness = val;
		trianglesStiffness = val;
		quadsStiffness = val;
		tetrasStiffness = val;
		cubesStiffness = val;
	}
	void setLinesStiffness(Real val)
	{
		linesStiffness = val;
	}
	void setTrianglesStiffness(Real val)
	{
		trianglesStiffness = val;
	}
	void setQuadsStiffness(Real val)
	{
		quadsStiffness = val;
	}
	void setTetrasStiffness(Real val)
	{
		tetrasStiffness = val;
	}
	void setCubesStiffness(Real val)
	{
		cubesStiffness = val;
	}
	
	Real getDamping() const { return linesDamping; }
	Real getLinesDamping() const { return linesDamping; }
	Real getTrianglesDamping() const { return trianglesDamping; }
	Real getQuadsDamping() const { return quadsDamping; }
	Real getTetrasDamping() const { return tetrasDamping; }
	Real getCubesDamping() const { return cubesDamping; }
	void setDamping(Real val)
	{
		linesDamping = val;
		trianglesDamping = val;
		quadsDamping = val;
		tetrasDamping = val;
		cubesDamping = val;
	}
	void setLinesDamping(Real val)
	{
		linesDamping = val;
	}
	void setTrianglesDamping(Real val)
	{
		trianglesDamping = val;
	}
	void setQuadsDamping(Real val)
	{
		quadsDamping = val;
	}
	void setTetrasDamping(Real val)
	{
		tetrasDamping = val;
	}
	void setCubesDamping(Real val)
	{
		cubesDamping = val;
	}
	
	virtual void init();

    void parse(core::objectmodel::BaseObjectDescription* arg);

};

} // namespace forcefield

} // namespace component

} // namespace sofa

#endif
