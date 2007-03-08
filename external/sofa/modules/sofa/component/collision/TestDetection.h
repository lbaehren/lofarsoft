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
/** @file TestDetection.h
*  @brief Implementation of a hierarchically collision detection
*
*  Comments: 
*
*  @author C�ar Mendoza
*  @bug1  interruptibility
*  @bug2  Search bug2 in the code TestDetection.cpp
*
*/
#ifndef SOFA_COMPONENT_COLLISION_TESTDETECTION_H
#define SOFA_COMPONENT_COLLISION_TESTDETECTION_H

#include <sofa/core/componentmodel/collision/BroadPhaseDetection.h>
#include <sofa/core/componentmodel/collision/NarrowPhaseDetection.h>
#include <sofa/core/CollisionElement.h>
#include <sofa/core/VisualModel.h>
#include <sofa/defaulttype/Vec.h>
#include <set>


namespace sofa
{

namespace component
{

namespace collision
{

using namespace sofa::defaulttype;
using namespace sofa::core::componentmodel::collision;

/** @class Test detection
    @brief Define the interaction betwee two collision models
	@brief Inherit of VisualModel, NarrowPhaseDetection and BroadPhaseDetection
*/
class TestDetection : public BroadPhaseDetection, public NarrowPhaseDetection, public core::VisualModel
{
private:
	std::vector<core::CollisionModel*> collisionModels;
	DataField<bool> bDraw;

public:

	/** @brief Constructor */ 
	TestDetection();
	
	void setDraw(bool val) { bDraw.setValue(val); }
 
	
	/** @brief Function comes from addCollisionModel from BroadPhaseDetection which
			which comes from DefaultPipeline.cpp addCollisionModels.
			It executes the broadphase collision detection using a 
			hierarchical collision model
		  @param[in] collision model
  */
	void addCollisionModel (core::CollisionModel *cm);


	void addCollisionPair (const std::pair<core::CollisionModel*, core::CollisionModel*>& cmPair);
	
	virtual void clearBroadPhase()
	{
		BroadPhaseDetection::clearBroadPhase();
		collisionModels.clear();
	}
	
	/* for debugging, VisualModel */
	void draw();
	void initTextures() { }
	void update() { }


};// class testDetection

} // namespace collision

} // namespace component

} // namespace sofa

#endif
