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
/** @file SphereTreeModel.h
 *  @brief Definition of the class SphereTreeModel and SingleSphere;
 *
 *  Recommended: Put SingleSphere out of this file
 *
 *  @author Cesar Mendoza
 *  @bug Not known so far
 *
 */

#ifndef SOFA_COMPONENT_COLLISION_SPHERETREEMODEL_H
#define SOFA_COMPONENT_COLLISION_SPHERETREEMODEL_H

#include <sofa/core/CollisionModel.h>
#include <sofa/core/VisualModel.h>
#include <sofa/component/MechanicalObject.h>
#include <sofa/component/topology/MeshTopology.h>
#include <sofa/defaulttype/Vec3Types.h>
#include <vector>
#include <iostream>
#include <fstream>




namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;
using namespace std;

class SphereTreeModel;

/** @class SingleSphere
    @brief Define a "glorious" pointer to an element (sphere) in the SphereTreeModel.
	 It should not contain any data information. We are trying not to store
	 virtual classes for each element. In this way we avoid overheading
*/
class SingleSphere : public core::TCollisionElementIterator<SphereTreeModel>
{
public:
	/** @brief Constructor. */
	SingleSphere(SphereTreeModel* model, int index);

	/** @brief Constructor */ 
	explicit SingleSphere(core::CollisionElementIterator& i);

	/** @brief Returns center of the sphere. It points (according to the index "i")
	to the DOF's of the collision model i.e. the centers of the spheres*/
	const Vector3& center() const;

	/** @brief  Set center of sphere. It modifies the center of the sphere with index "i"
	i.e. it modifies one element of the DOF's of the collision model*/
	void setCenter( double x, double y, double z );
	
	const Vector3& v() const;

	/** @brief Returns the radius of the sphere "i" */
	double r() const;
	
};

class SphereTreeModel : public component::MechanicalObject<Vec3Types>, public core::CollisionModel, public core::VisualModel
{
public:
	typedef Vec3Types DataTypes;
	typedef SingleSphere Element;
	friend class SingleSphere;
	DataTypes::VecCoord VecCoord;

	/** @brief Constructor */
	SphereTreeModel(double radius = 1.0);
		
	/** @brief Add a new sphere to the tree. It actually increases the size of the DOF's of the
	model setting the radius and the center*/
	int addSphere(const Vector3& pos, double radius);

	/** @brief */ 
	void setSphere(int index, const Vector3& pos, double radius);

	/** @brief Load SphereTree Model. The file is generated offline using a medial axis approximation
		Windows executables to obtain the file are available at cesarmendoza_serrano@yahoo.fr*/
	bool load(const char* filename);

	// -- CollisionModel interface
	/* @brief It "resizes" the vector of DOF's of the collision model. It is normally used after
		adding a new element to the tree*/
	virtual void resize(int size);

	/** @brief It updates the hierarchical bounding tree*/
	virtual void computeBoundingTree(int maxDepth=0);

	/** @brief Returns a pair of iterators. The first corresponds to the first child of iterator given by the index.
	The second element of the pair corresponds to the last children of collisionElementIterator index*/
	virtual std::pair<core::CollisionElementIterator,core::CollisionElementIterator> getInternalChildren(int index) const;

	/** @brief To do */
	virtual void computeContinuousBoundingTree(double dt, int maxDepth=0);

	void draw(int index);
	
	/** @brief Levels of the tree of the collision model */
	int levels;

	/** @brief Branching factor of the KTree (number of children for each node) */
	int degree;

	/** @brief Load sphere tree given a filename. To obtain executables (win32) to
	obtain this sphere tree files hierarchies, please email: cesarmendoza_serrano@yahoo.fr */ 
	bool loadSphereTree( const char *fileName );
    
	/** @brief It returns true is the element iterator is a leaf of the spheretree collision model*/
	virtual bool isLeaf( int index ) const;

	// -- VisualModel interface	
	void draw();
	void initTextures() { }
	void update() { }

protected:

	/** @brief  vector of radii of spheres in the model */
	std::vector<double> radius;	

	/** @brief default radius */
	DataField<double> defaultRadius;	

	virtual void init( void );

};

/// Inline definitions of class SingleSphere

/// Constructor
inline SingleSphere::SingleSphere(SphereTreeModel* model, int index)
: core::TCollisionElementIterator<SphereTreeModel>(model, index)
{
}

inline SingleSphere::SingleSphere(core::CollisionElementIterator& i)
: core::TCollisionElementIterator<SphereTreeModel>(static_cast<SphereTreeModel*>(i.getCollisionModel()), i.getIndex())
{
	
}

inline const Vector3& SingleSphere::center() const
{
	return (*model->getX())[index]; 
}

inline const Vector3& SingleSphere::v() const
{
	return (*model->getV())[index];
}

inline void SingleSphere::setCenter( double x, double y, double z ) 
{
	(*model->getX())[index] = Vector3(x,y,z);
}

inline double SingleSphere::r() const
{
	return model->radius[index];
}


} // namespace collision

} // namespace component

} // namespace sofa

#endif


