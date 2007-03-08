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
#ifndef SOFA_COMPONENT_INTERACTIONFORCEFIELD_EXTERNALFORCEFIELD_H
#define SOFA_COMPONENT_INTERACTIONFORCEFIELD_EXTERNALFORCEFIELD_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif

#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/component/MechanicalObject.h>
#include <sofa/core/VisualModel.h>
#include <sofa/component/topology/MeshTopology.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/helper/vector.h>

namespace sofa
{

namespace component
{

namespace interactionforcefield
{

using namespace sofa::defaulttype;
/** Apply given forces to given particles
*/
template<class DataTypes>
class ExternalForceField : public core::componentmodel::behavior::ForceField<DataTypes>, public core::VisualModel
{
public:
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef VecCoord Vector;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef typename Coord::value_type Real;


protected:
    /// Value of the applied forces
    VecDeriv m_forces;
    /// Indices of the particles where the forces are applied
    vector<unsigned> m_indices;


public:
    ExternalForceField();
    
    /// Set the value of the force applied to the given particle.
    // void setForce( unsigned particleIndex, const Deriv& force );

    virtual void addForce (VecDeriv& f, const VecCoord& x, const VecDeriv& v);

    virtual void addDForce (VecDeriv& /*df*/, const VecDeriv& /*dx*/){}
    
    virtual double getPotentialEnergy( const VecCoord& x );



    // -- VisualModel interface
    void draw(){}
    void initTextures()
    { }
    void update()
    { }

};

} // namespace interactionforcefield

} // namespace component

} // namespace sofa

#endif
