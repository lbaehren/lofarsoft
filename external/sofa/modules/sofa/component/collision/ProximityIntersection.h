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
#ifndef SOFA_COMPONENT_COLLISION_PROXIMITYINTERSECTION_H
#define SOFA_COMPONENT_COLLISION_PROXIMITYINTERSECTION_H

#include <sofa/component/collision/DiscreteIntersection.h>
#include <sofa/helper/FnDispatcher.h>
#include <sofa/component/collision/SphereModel.h>
#include <sofa/component/collision/TriangleModel.h>
#include <sofa/component/collision/LineModel.h>
#include <sofa/component/collision/PointModel.h>
#include <sofa/component/collision/CubeModel.h>
#include <sofa/component/collision/RayModel.h>

namespace sofa
{

namespace component
{

namespace collision
{

class ProximityIntersection : public DiscreteIntersection
{
public:
    DataField<bool> useTriangleTriangle;
    DataField<bool> useLineTriangle;
    DataField<bool> usePointTriangle;
    DataField<bool> useSphereTriangle;
    DataField<double> alarmDistance;
    DataField<double> contactDistance;

    ProximityIntersection();

    virtual void init();

    /// Return the intersector class handling the given pair of collision models, or NULL if not supported.
    virtual core::componentmodel::collision::ElementIntersector* findIntersector(core::CollisionModel* object1, core::CollisionModel* object2);

    /// returns true if algorithm uses continous detection
    virtual bool useProximity() const { return true; }

    /// Return the alarm distance (must return 0 if useProximity() is false)
    double getAlarmDistance() const { return alarmDistance.getValue(); }

    /// Return the contact distance (must return 0 if useProximity() is false)
    double getContactDistance() const { return contactDistance.getValue(); }
};

// Jeremie A. : put the methods inside a namespace instead of a class,
// for g++ 3.4 compatibility

namespace ProximityIntersections
{

bool intersectionCubeCube(Cube& ,Cube&);
bool intersectionSphereSphere(Sphere & ,Sphere &);
bool intersectionSphereTriangle(Sphere &, Triangle &);
bool intersectionTriangleTriangle(Triangle& ,Triangle&);
bool intersectionLineTriangle(Line& ,Triangle&);
bool intersectionPointTriangle(Point& ,Triangle&);
bool intersectionSphereRay(Sphere & ,Ray &);
bool intersectionRayTriangle(Ray & ,Triangle &);

core::componentmodel::collision::DetectionOutput* distCorrectionCubeCube(Cube&, Cube&);
core::componentmodel::collision::DetectionOutput* distCorrectionSphereSphere(Sphere & ,Sphere &);
core::componentmodel::collision::DetectionOutput* distCorrectionSphereRay(Sphere & ,Ray &);
core::componentmodel::collision::DetectionOutput* distCorrectionSphereTriangle(Sphere &, Triangle &);
core::componentmodel::collision::DetectionOutput* distCorrectionTriangleTriangle (Triangle& ,Triangle&);
core::componentmodel::collision::DetectionOutput* distCorrectionLineTriangle (Line& ,Triangle&);
core::componentmodel::collision::DetectionOutput* distCorrectionPointTriangle (Point& ,Triangle&);
core::componentmodel::collision::DetectionOutput* distCorrectionRayTriangle(Ray & ,Triangle &);
} // namespace ProximityIntersections

} // namespace collision

} // namespace component

} // namespace sofa

#endif
