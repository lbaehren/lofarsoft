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
#ifndef SOFA_SIMULATION_TREE_MECHANICALACTION_H
#define SOFA_SIMULATION_TREE_MECHANICALACTION_H

#if !defined(__GNUC__) || (__GNUC__ > 3 || (_GNUC__ == 3 && __GNUC_MINOR__ > 3))
#pragma once
#endif


#include <sofa/simulation/tree/Action.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalState.h>
#include <sofa/core/componentmodel/behavior/BaseMechanicalMapping.h>
#include <sofa/core/componentmodel/behavior/Mass.h>
#include <sofa/core/componentmodel/behavior/ForceField.h>
#include <sofa/core/componentmodel/behavior/InteractionForceField.h>
#include <sofa/core/componentmodel/behavior/InteractionConstraint.h>
#include <sofa/core/componentmodel/behavior/Constraint.h>
#include <sofa/defaulttype/SofaBaseMatrix.h>
#include <iostream>
using std::cerr;
using std::endl;

namespace sofa
{

namespace simulation
{

namespace tree
{

using namespace sofa::defaulttype;
/** Base class for easily creating new actions for mechanical simulation.
 
During the first traversal (top-down), method processNodeTopDown(GNode*) is applied to each GNode. Each component attached to this node is processed using the appropriate method, prefixed by fwd.
 
During the second traversal (bottom-up), method processNodeBottomUp(GNode*) is applied to each GNode. Each component attached to this node is processed using the appropriate method, prefixed by bwd.
 
The default behavior of the fwd* and bwd* is to do nothing. Derived actions typically overload these methods to implement the desired processing.
 
*/
class MechanicalAction : public Action
{
public:
    typedef sofa::core::componentmodel::behavior::BaseMechanicalState::VecId VecId;

    /**@name Forward processing
    Methods called during the forward (top-down) traversal of the data structure.
    Method processNodeTopDown(GNode*) calls the fwd* methods in the order given here. When there is a mapping, it is processed first, then method fwdMappedMechanicalState is applied to the BaseMechanicalState. 
    When there is no mapping, the BaseMechanicalState is processed first using method fwdMechanicalState.
    Then, the other fwd* methods are applied in the given order.
     */
    ///@{

    /// This method calls the fwd* methods during the forward traversal. You typically do not overload it.
    virtual Result processNodeTopDown(GNode* node);
    
    /// Process the BaseMechanicalMapping
    virtual Result fwdMechanicalMapping(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMechanicalState if it is mapped from the parent level
    virtual Result fwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMechanicalState if it is not mapped from the parent level
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process the BaseMass
    virtual Result fwdMass(GNode* /*node*/, core::componentmodel::behavior::BaseMass* /*mass*/)
    {
        return RESULT_CONTINUE;
    }

    /// Process all the BaseForceField
    virtual Result fwdForceField(GNode* /*node*/, core::componentmodel::behavior::BaseForceField* /*ff*/)
    {
        return RESULT_CONTINUE;
    }


    /// Process all the InteractionForceField
    virtual Result fwdInteractionForceField(GNode* node, core::componentmodel::behavior::InteractionForceField* ff)
    {
        return fwdForceField(node, ff);
    }

    /// Process all the BaseConstraint
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* /*c*/)
    {
        return RESULT_CONTINUE;
    }

	/// Process all the InteractionConstraint
    virtual Result fwdInteractionConstraint(GNode* node, core::componentmodel::behavior::InteractionConstraint* c)
    {
        return fwdConstraint(node, c);
    }

    ///@}

    /**@name Backward processing
    Methods called during the backward (bottom-up) traversal of the data structure.
    Method processNodeBottomUp(GNode*) calls the bwd* methods. 
    When there is a mapping, method bwdMappedMechanicalState is applied to the BaseMechanicalState. 
    When there is no mapping, the BaseMechanicalState is processed using method bwdMechanicalState.
    Finally, the mapping (if any) is processed using method bwdMechanicalMapping.
     */
    ///@{

    /// This method calls the bwd* methods during the backward traversal. You typically do not overload it.
    virtual void processNodeBottomUp(GNode* node);
    
    /// Process the BaseMechanicalState when it is not mapped from parent level
    virtual void bwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {}

    /// Process the BaseMechanicalState when it is mapped from parent level
    virtual void bwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {}
    
    /// Process the BaseMechanicalMapping
    virtual void bwdMechanicalMapping(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* /*map*/)
    {}
    ///@}


    /// Return a category name for this action.
    /// Only used for debugging / profiling purposes
    virtual const char* getCategoryName() const
    {
        return "animate";
    }
};

/** Reserve an auxiliary vector identified by a symbolic constant.
*/
class MechanicalVAllocAction : public MechanicalAction
{
public:
    VecId v;
    MechanicalVAllocAction(VecId v) : v(v)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->vAlloc(v);
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
            mm->vAlloc(v);
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Free an auxiliary vector identified by a symbolic constant */
class MechanicalVFreeAction : public MechanicalAction
{
public:
    VecId v;
    MechanicalVFreeAction(VecId v) : v(v)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->vFree(v);
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
            mm->vFree(v);
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Perform a vector operation v=a+b*f
*/
class MechanicalVOpAction : public MechanicalAction
{
public:
    VecId v;
    VecId a;
    VecId b;
    double f;
    MechanicalVOpAction(VecId v, VecId a = VecId::null(), VecId b = VecId::null(), double f=1.0)
            : v(v), a(a), b(b), f(f)
    {}

    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        //cerr<<"    MechanicalVOpAction::fwdMechanicalState, model "<<mm->getName()<<endl;
        mm->vOp(v,a,b,f);
        return RESULT_CONTINUE;
    }
    virtual Result fwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {
        //cerr<<"    MechanicalVOpAction::fwdMappedMechanicalState, model "<<mm->getName()<<endl;
        //mm->vOp(v,a,b,f);
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
            mm->vOp(v,a,b,f);
        return RESULT_CONTINUE;
    }

    //virtual void processNodeBottomUp(GNode* node);

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Compute the dot product of two vectors */
class MechanicalVDotAction : public MechanicalAction
{
public:
    VecId a;
    VecId b;
    double* total;
    MechanicalVDotAction(VecId a, VecId b, double* t) : a(a), b(b), total(t)
    {}
    /// Sequential code
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        *total += mm->vDot(a,b);
        return RESULT_CONTINUE;
    }
    /// Sequential code
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
            *total += mm->vDot(a,b);
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
    /// Parallel code
    virtual Result processNodeTopDown(GNode* node, LocalStorage* stack)
    {
        double* localTotal = new double(0.0);
        stack->push(localTotal);
        if (node->mechanicalModel && !node->mechanicalMapping)
        {
            core::componentmodel::behavior::BaseMechanicalState* mm = node->mechanicalModel;
            *localTotal += mm->vDot(a,b);
        }
        for (GNode::Sequence<core::componentmodel::behavior::BaseConstraint>::iterator it = node->
                constraint.begin();
                it != node->constraint.end();
                ++it)
        {
            core::componentmodel::behavior::BaseConstraint* c = *it;
            core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
            if (mm)
                *localTotal += mm->vDot(a,b);
        }
        return RESULT_CONTINUE;
    }
    /// Parallel code
    virtual void processNodeBottomUp(GNode* /*node*/, LocalStorage* stack)
    {
        double* localTotal = static_cast<double*>(stack->pop());
        double* parentTotal = static_cast<double*>(stack->top());
        if (!parentTotal)
            *total += *localTotal; // root
        else
            *parentTotal += *localTotal;
        delete localTotal;
    }
};

/** Apply a hypothetical displacement.
This action does not modify the state (i.e. positions and velocities) of the objects.
It is typically applied before a MechanicalComputeDfAction, in order to compute the df corresponding to a given dx (i.e. apply stiffness).
Dx is propagated to all the layers through the mappings.
 */
class MechanicalPropagateDxAction : public MechanicalAction
{
public:
    VecId dx;
    MechanicalPropagateDxAction(VecId dx) : dx(dx)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->setDx(dx);
        return RESULT_CONTINUE;
    }
    virtual Result fwdMechanicalMapping(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map)
    {
        map->propagateDx();
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
            mm->setDx(dx);
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Accumulate the product of the mass matrix by a given vector.
Typically used in implicit integration solved by a Conjugate Gradient algorithm 
*/
class MechanicalAddMDxAction : public MechanicalAction
{
public:
    VecId res;
    VecId dx;
    MechanicalAddMDxAction(VecId res, VecId dx) : res(res), dx(dx)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->setF(res);
        mm->setDx(dx);
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
        {
            mm->setF(res);
            mm->setDx(dx);
        }
        return RESULT_CONTINUE;
    }
    virtual Result fwdMass(GNode* /*node*/, core::componentmodel::behavior::BaseMass* mass)
    {
        mass->addMDx();
        return RESULT_PRUNE;
    }

    virtual Result fwdMechanicalMapping(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map)
    {
        map->propagateDx();
        return RESULT_CONTINUE;
    }
    virtual Result fwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->resetForce();
        return RESULT_CONTINUE;
    }
    virtual void bwdMechanicalMapping(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map)
    {
        map->accumulateForce();
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Compute accelerations generated by given forces 
 */
class MechanicalAccFromFAction : public MechanicalAction
{
public:
    VecId a;
    VecId f;
    MechanicalAccFromFAction(VecId a, VecId f) : a(a), f(f)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->setDx(a);
        mm->setF(f);
        /// \todo Check presence of Mass
        return RESULT_CONTINUE;
    }
    virtual Result fwdMass(GNode* /*node*/, core::componentmodel::behavior::BaseMass* mass)
    {
        mass->accFromF();
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
        {
            mm->setDx(a);
            mm->setF(f);
        }
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Propagate positions and velocities to all the levels of the hierarchy.
At each level, the mappings form the parent to the child is applied.
After the execution of this action, all the (mapped) degrees of freedom are consistent with the independent degrees of freedom.
This action is typically applied after time integration of the independent degrees of freedom.
 */
class MechanicalPropagatePositionAndVelocityAction : public MechanicalAction
{
public:
    double t;
    VecId x;
    VecId v;
    MechanicalPropagatePositionAndVelocityAction(double time=0, VecId x = VecId::position(), VecId v = VecId::velocity());
    virtual Result processNodeTopDown(GNode* node);
    virtual void processNodeBottomUp(GNode* node);

    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->setX(x);
        mm->setV(v);
        return RESULT_CONTINUE;
    }
    virtual Result fwdMechanicalMapping(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map)
    {
        map->propagateX();
        map->propagateV();
        return RESULT_CONTINUE;
    }
    virtual void bwdConstraint( GNode*, core::componentmodel::behavior::BaseConstraint* /*c*/)
    {}
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        // FF, 14/07/06: I do not understand the purpose of this method
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
        {
            mm->setX(x);
            mm->setV(v);
        }
        c->projectVelocity();
        c->projectPosition();
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Reset the force in all the MechanicalModel 
This action is typically applied before accumulating all the forces.
 */
class MechanicalResetForceAction : public MechanicalAction
{
public:
    VecId res;
    MechanicalResetForceAction(VecId res) : res(res)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->setF(res);
        mm->resetForce();
        return RESULT_CONTINUE;
    }
    virtual Result fwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->resetForce();
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
        {
            mm->setF(res);
            mm->resetForce();
        }
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Accumulate the forces (internal and interactions).
This action is typically called after a MechanicalResetForceAction.
 */
class MechanicalComputeForceAction : public MechanicalAction
{
public:
    VecId res;
    MechanicalComputeForceAction(VecId res) : res(res)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->setF(res);
        mm->accumulateForce();
        return RESULT_CONTINUE;
    }
    virtual Result fwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->accumulateForce();
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
        {
            mm->setF(res);
            mm->accumulateForce();
        }
        return RESULT_CONTINUE;
    }
    //virtual Result fwdMass(GNode* /*node*/, core::componentmodel::behavior::BaseMass* mass)
    //{
    //	mass->computeForce();
    //	return RESULT_CONTINUE;
    //}
    virtual Result fwdForceField(GNode* /*node*/, core::componentmodel::behavior::BaseForceField* ff)
    {
        ff->addForce();
        return RESULT_CONTINUE;
    }
    virtual void bwdMechanicalMapping(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map)
    {
        map->accumulateForce();
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Compute the variation of force corresponding to a variation of position.
This action is typically called after a MechanicalPropagateDxAction.
 */
class MechanicalComputeDfAction : public MechanicalAction
{
public:
    VecId res;
    MechanicalComputeDfAction(VecId res) : res(res)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->setF(res);
        mm->accumulateDf();
        return RESULT_CONTINUE;
    }
    virtual Result fwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->accumulateDf();
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
        {
            mm->setF(res);
            mm->accumulateDf();
        }
        return RESULT_CONTINUE;
    }
    //virtual Result fwdMass(GNode* /*node*/, core::componentmodel::behavior::BaseMass* mass)
    //{
    //	mass->computeDf();
    //	return RESULT_CONTINUE;
    //}
    virtual Result fwdForceField(GNode* /*node*/, core::componentmodel::behavior::BaseForceField* ff)
    {
        ff->addDForce();
        return RESULT_CONTINUE;
    }
    virtual void bwdMechanicalMapping(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map)
    {
        map->accumulateDf();
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};


class MechanicalResetConstraintAction : public MechanicalAction
{
public:
    VecId res;
    MechanicalResetConstraintAction(VecId res) : res(res)
    {}

    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->setC(res);
        mm->resetConstraint();
        return RESULT_CONTINUE;
    }

    virtual Result fwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->resetConstraint();
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};


class MechanicalAccumulateConstraint : public MechanicalAction
{
public:
	MechanicalAccumulateConstraint(void)
	{}

	virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        c->applyConstraint();
        return RESULT_CONTINUE;
    }

	virtual void bwdMechanicalMapping(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalMapping* map)
    {
        map->accumulateConstraint();
    }
};

/** Apply the constraints as filters to the given vector.
This works for simple independent constraints, like maintaining a fixed point.
*/
class MechanicalApplyConstraintsAction : public MechanicalAction
{
public:
    VecId res;
    MechanicalApplyConstraintsAction(VecId res) : res(res)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->setDx(res);
        //mm->projectResponse();
        return RESULT_CONTINUE;
    }
    virtual Result fwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* /*mm*/)
    {
        //mm->projectResponse();
        return RESULT_CONTINUE;
    }

    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
        {
            mm->setDx(res);
        }
        c->projectResponse();
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Action used to prepare a time integration step. Typically, does nothing.
*/
class MechanicalBeginIntegrationAction : public MechanicalAction
{
public:
    double dt;
    MechanicalBeginIntegrationAction (double dt)
            : dt(dt)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->beginIntegration(dt);
        return RESULT_CONTINUE;
    }
    virtual Result fwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->beginIntegration(dt);
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
        {
            mm->beginIntegration(dt);
        }
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Action applied after a time step has been applied. Does typically nothing.
*/
class MechanicalEndIntegrationAction : public MechanicalAction
{
public:
    double dt;
    MechanicalEndIntegrationAction (double dt)
            : dt(dt)
    {}
    virtual Result fwdMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->endIntegration(dt);
        return RESULT_CONTINUE;
    }
    virtual Result fwdMappedMechanicalState(GNode* /*node*/, core::componentmodel::behavior::BaseMechanicalState* mm)
    {
        mm->endIntegration(dt);
        return RESULT_CONTINUE;
    }
    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        core::componentmodel::behavior::BaseMechanicalState* mm = c->getDOFs();
        if (mm)
        {
            mm->endIntegration(dt);
        }
        return RESULT_CONTINUE;
    }

    /// Specify whether this action can be parallelized.
    virtual bool isThreadSafe() const
    {
        return true;
    }
};

/** Compute the size of a dynamics matrix (mass or stiffness) of the whole scene */
class MechanicalGetMatrixDimensionAction : public MechanicalAction
{
public:
    unsigned int * const nbRow;
    unsigned int * const nbCol;
    MechanicalGetMatrixDimensionAction(unsigned int * const _nbRow, unsigned int * const _nbCol)
            : nbRow(_nbRow), nbCol(_nbCol)
    {}

    virtual Result fwdForceField(GNode*, core::componentmodel::behavior::BaseForceField *ff)
    {
        ff->contributeToMatrixDimension(nbRow, nbCol);
        return RESULT_CONTINUE;
    }
};


/** Accumulate the entries of a dynamics matrix (mass or stiffness) of the whole scene */
class MechanicalComputeMatrixAction : public MechanicalAction
{
public:
    SofaBaseMatrix *mat;
    double m, b, k;
    unsigned int offset, offsetBckUp;

    MechanicalComputeMatrixAction(SofaBaseMatrix *_mat, double _m=1.0, double _b=1.0, double _k=1.0, unsigned int _offset=0)
            : mat(_mat),m(_m),b(_b),k(_k),offset(_offset), offsetBckUp(_offset)
    {}

    virtual Result fwdForceField(GNode* /*node*/, core::componentmodel::behavior::BaseForceField* ff)
    {
        ff->computeMatrix(mat,m,b,k,offset);
        return RESULT_CONTINUE;
    }

    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        c->applyConstraint(mat, offsetBckUp);
        offsetBckUp = offset;
        return RESULT_CONTINUE;
    }
};


/** Accumulate the entries of a dynamics vector (e.g. force) of the whole scene */
class MechanicalComputeVectorAction : public MechanicalAction
{
public:
    SofaBaseVector *vect;
    unsigned int offset, offsetBckUp;

    MechanicalComputeVectorAction(SofaBaseVector *_vect, unsigned int _offset=0)
            : vect(_vect), offset(_offset), offsetBckUp(_offset)
    {}

    virtual Result fwdForceField(GNode* /*node*/, core::componentmodel::behavior::BaseForceField* ff)
    {
        ff->computeVector(vect, offset);
        return RESULT_CONTINUE;
    }

    virtual Result fwdConstraint(GNode* /*node*/, core::componentmodel::behavior::BaseConstraint* c)
    {
        c->applyConstraint(vect, offsetBckUp);
        offsetBckUp = offset;
        return RESULT_CONTINUE;
    }
};


class MechanicalMatResUpdatePositionAction : public MechanicalAction
{
public:
    SofaBaseVector *vect;
    unsigned int offset;

    MechanicalMatResUpdatePositionAction(SofaBaseVector *_vect, unsigned int _offset=0)
            : vect(_vect), offset(_offset)
    {}

    virtual Result fwdForceField(GNode* /*node*/, core::componentmodel::behavior::BaseForceField* ff)
    {
        ff->matResUpdatePosition(vect, offset);
        return RESULT_CONTINUE;
    }
};


} // namespace tree

} // namespace simulation

} // namespace sofa

#endif
