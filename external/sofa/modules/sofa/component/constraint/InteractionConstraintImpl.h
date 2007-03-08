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
#ifndef SOFA_COMPONENT_CONSTRAINT_INTERACTIONCONSTRAINTIMPL_H
#define SOFA_COMPONENT_CONSTRAINT_INTERACTIONCONSTRAINTIMPL_H

#include <sofa/core/componentmodel/behavior/InteractionConstraint.h>
#include <sofa/core/componentmodel/behavior/MechanicalState.h>

namespace sofa
{

namespace component
{

namespace constraint
{

template<class DataTypes>
class InteractionConstraintImpl : public core::componentmodel::behavior::InteractionConstraint
{
public:
	typedef typename DataTypes::VecCoord VecCoord;
	typedef typename DataTypes::VecDeriv VecDeriv;
	typedef typename DataTypes::Coord Coord;
	typedef typename DataTypes::Deriv Deriv;
	typedef typename Coord::value_type Real;
	typedef typename core::componentmodel::behavior::MechanicalState<DataTypes> MechanicalState;

protected:
	MechanicalState* object1;
	MechanicalState* object2;

public:
	
	MechanicalState* getObject1() { return object1; }
	MechanicalState* getObject2() { return object2; }
	core::componentmodel::behavior::BaseMechanicalState* getMechModel1() { return object1; }
	core::componentmodel::behavior::BaseMechanicalState* getMechModel2() { return object2; }

	virtual void applyConstraint(){};

	// Previous Constraint Interface
	virtual void projectResponse(){};
	virtual void projectVelocity(){};
	virtual void projectPosition(){};

    /// Pre-construction check method called by ObjectFactory.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        if (arg->getAttribute("object1") || arg->getAttribute("object2"))
        {
            if (dynamic_cast<MechanicalState*>(arg->findObject(arg->getAttribute("object1",".."))) == NULL)
                return false;
            if (dynamic_cast<MechanicalState*>(arg->findObject(arg->getAttribute("object2",".."))) == NULL)
                return false;
        }
        else
        {
            if (dynamic_cast<MechanicalState*>(context->getMechanicalState()) == NULL)
                return false;
        }
        return core::componentmodel::behavior::InteractionConstraint::canCreate(obj, context, arg);
    }

    /// Construction method called by ObjectFactory.
    template<class T>
    static void create(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        core::componentmodel::behavior::InteractionConstraint::create(obj, context, arg);
        if (arg->getAttribute("object1") || arg->getAttribute("object2"))
        {
            obj->object1 = dynamic_cast<MechanicalState*>(arg->findObject(arg->getAttribute("object1","..")));
            obj->object2 = dynamic_cast<MechanicalState*>(arg->findObject(arg->getAttribute("object2","..")));
        }
        else
        {
            obj->object1 =
            obj->object2 =
                dynamic_cast<MechanicalState*>(context->getMechanicalState());
        }
    }
};
} // namespace constraint

} // namespace component

} // namespace sofa

#endif // SOFA_COMPONENTS_INTERACTIONCONSTRAINTIMPL_H
