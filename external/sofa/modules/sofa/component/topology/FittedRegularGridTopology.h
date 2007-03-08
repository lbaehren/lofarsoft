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
#ifndef SOFA_COMPONENT_TOPOLOGY_FITTEDREGULARGRIDTOPOLOGY_H
#define SOFA_COMPONENT_TOPOLOGY_FITTEDREGULARGRIDTOPOLOGY_H

#include <sofa/component/topology/RegularGridTopology.h>
#include <sofa/defaulttype/Vec.h>

namespace sofa
{

namespace component
{

namespace topology
{

using namespace sofa::defaulttype;

class FittedRegularGridTopology : public RegularGridTopology
{
public:
	typedef Vec3d Vec3;
	
	FittedRegularGridTopology();
	
	FittedRegularGridTopology(int nx, int ny, int nz);
	
	/// return the cube containing the given point (or -1 if not found).
	virtual int findCube(const Vec3& pos);
	
	/// return the nearest cube (or -1 if not found).
	virtual int findNearestCube(const Vec3& pos);
	
	/// return the cube containing the given point (or -1 if not found),
	/// as well as deplacements from its first corner in terms of dx, dy, dz (i.e. barycentric coordinates).
	virtual int findCube(const Vec3& pos, double& fx, double &fy, double &fz);
	
	/// return the cube containing the given point (or -1 if not found),
	/// as well as deplacements from its first corner in terms of dx, dy, dz (i.e. barycentric coordinates).
	virtual int findNearestCube(const Vec3& pos, double& fx, double &fy, double &fz);

	/// set a given cube as active
	void setCubeActive(int index);

	/// set a given cube as active
	void setCubeActive(int x, int y, int z);

	/// return true if the given cube is active, i.e. it contains or is surrounded by mapped points.
	bool isCubeActive(int index);

	/// return true if the given cube is active, i.e. it contains or is surrounded by mapped points.
	bool isCubeActive(int x, int y, int z);

	//virtual const char* getTypeName() const { return "TrimmedRegularGrid"; }

protected:

	/// active flag for each cube (true if the given cube contains or is surrounded by mapped points).
	std::vector<bool> activeCubeFlag;

	/// true if the activeCubeFlag vector is up-to-date.
	bool validActiveCubeFlags;

	/// Update activeCubeFlags.
	void updateActiveCubeFlags();

    virtual void setSize();

};

} // namespace topology

} // namespace component

} // namespace sofa

#endif
