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
#ifndef SOFA_COMPONENT_COLLISION_RAYPICKINTERACTOR_H
#define SOFA_COMPONENT_COLLISION_RAYPICKINTERACTOR_H

#include <sofa/component/collision/RayModel.h>
#include <sofa/core/BehaviorModel.h>
#include <sofa/component/MechanicalObject.h>
#include <sofa/simulation/tree/GNode.h>
#include <sofa/component/forcefield/StiffSpringForceField.h>
//#include <sofa/component/constraint/LagrangianMultiplierAttachConstraint.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/defaulttype/Quat.h>
#include <sofa/core/componentmodel/collision/DetectionOutput.h>


namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;

class RayPickInteractor : public RayModel, public core::BehaviorModel
{
protected:
	typedef forcefield::StiffSpringForceField<Vec3Types> ContactForceField1;
//	typedef constraint::LagrangianMultiplierAttachConstraint<Vec3Types> ContactForceField2;
	std::vector<core::componentmodel::behavior::BaseForceField*> forcefields;
	std::vector<simulation::tree::GNode*> nodes;
	std::vector< std::pair<int,double> > attachedPoints;
	component::MechanicalObject<Vec3Types>* mm;
	enum { DISABLED, PRESENT, ACTIVE, ATTACHED } state;
	int button; // index of activated button (only valid in ACTIVE state)
	Mat4x4d transform;
	core::componentmodel::collision::DetectionOutput* findFirstCollision(const Ray& ray, double* dist);
public:
	RayPickInteractor();
	~RayPickInteractor();
	
	virtual void init();
	
	virtual bool isActive();
	
	/// Computation of a new simulation step.
	virtual void updatePosition(double dt);
	
	/// Interactor interface
	virtual void newPosition(const Vector3& translation, const Quat& rotation, const Mat4x4d& transformation);
	virtual void newEvent(const std::string& command);
	
	virtual void draw();
};

} // namespace collision

} // namespace component

} // namespace sofa

#endif
