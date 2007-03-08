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
#include <sofa/simulation/tree/AnimateAction.h>
#include <sofa/simulation/tree/MechanicalAction.h>
//#include "MechanicalIntegration.h"

namespace sofa
{

namespace simulation
{

namespace tree
{

void AnimateAction::processBehaviorModel(GNode*, core::BehaviorModel* obj)
{
	obj->updatePosition(getDt());
}

void AnimateAction::fwdInteractionForceField(GNode*, core::componentmodel::behavior::InteractionForceField* obj)
{
	obj->addForce();
}

void AnimateAction::processSolver(GNode* node, core::componentmodel::behavior::OdeSolver* obj)
{
	double nextTime = node->getTime() + node->getDt();
	MechanicalBeginIntegrationAction beginAction(getDt());
	node->execute(&beginAction);
	//MechanicalIntegration* group = new MechanicalIntegration(node);
	//obj->setGroup(node);
	obj->solve(getDt());
	obj->propagatePositionAndVelocity(nextTime,core::componentmodel::behavior::OdeSolver::VecId::position(),core::componentmodel::behavior::OdeSolver::VecId::velocity());
	//delete group;
	MechanicalEndIntegrationAction endAction(getDt());
	node->execute(&endAction);
}

Action::Result AnimateAction::processNodeTopDown(GNode* node)
{
	if (dt == 0) setDt(node->getDt());
	for_each(this, node, node->behaviorModel, &AnimateAction::processBehaviorModel);
	if (node->solver != NULL)
	{
		ctime_t t0 = begin(node, node->solver);
		processSolver(node, node->solver);
		end(node, node->solver, t0);
		return RESULT_PRUNE;
	}
	/*
	else if (node->mechanicalModel != NULL)
	{
		std::cerr << "Graph Error: MechanicalState without solver." << std::endl;
		return RESULT_PRUNE;
	}
	*/
	else
	{
		// process InteractionForceFields
		for_each(this, node, node->interactionForceField, &AnimateAction::fwdInteractionForceField);
		return RESULT_CONTINUE;
	}
}

// void AnimateAction::processNodeBottomUp(GNode* node)
// {
//     node->setTime( node->getTime() + node->getDt() );
// }

} // namespace tree

} // namespace simulation

} // namespace sofa

