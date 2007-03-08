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
#include <sofa/simulation/tree/MechanicalAction.h>
#include <sofa/simulation/tree/GNode.h>
#include <iostream>
using std::cerr;
using std::endl;

namespace sofa
{

namespace simulation
{

namespace tree
{

Action::Result MechanicalAction::processNodeTopDown(GNode* node)
{
	Result res = RESULT_CONTINUE;
	if (node->mechanicalModel != NULL) {
		if (node->mechanicalMapping != NULL) {
			//cerr<<"MechanicalAction::processNodeTopDown, node "<<node->getName()<<" is a mapped model"<<endl;
			Result res2;
			ctime_t t0 = begin(node, node->mechanicalMapping);
			res = this->fwdMechanicalMapping(node, node->mechanicalMapping);
			end(node, node->mechanicalMapping, t0);
			t0 = begin(node, node->mechanicalModel);
			res2 = this->fwdMappedMechanicalState(node, node->mechanicalModel);
			end(node, node->mechanicalModel, t0);
			if (res2 == RESULT_PRUNE)
				res = res2;
		} else {
			//cerr<<"MechanicalAction::processNodeTopDown, node "<<node->getName()<<" is a no-map model"<<endl;
			ctime_t t0 = begin(node, node->mechanicalModel);
			res = this->fwdMechanicalState(node, node->mechanicalModel);
			end(node, node->mechanicalModel, t0);
		}
	}
	if (res != RESULT_PRUNE) {
		if (node->mass != NULL) {
			ctime_t t0 = begin(node, node->mass);
			res = this->fwdMass(node, node->mass);
			end(node, node->mass, t0);
		}
	}
	if (res != RESULT_PRUNE) {
		res = for_each(this, node, node->forceField, &MechanicalAction::fwdForceField);
	}
        if (res != RESULT_PRUNE) {
            res = for_each(this, node, node->interactionForceField, &MechanicalAction::fwdInteractionForceField);
        }
        if (res != RESULT_PRUNE) {
            res = for_each(this, node, node->constraint, &MechanicalAction::fwdConstraint);
        }
        return res;
}

void MechanicalAction::processNodeBottomUp(GNode* node)
{
	if (node->mechanicalModel != NULL) {
		if (node->mechanicalMapping != NULL) {
			ctime_t t0 = begin(node, node->mechanicalModel);
			this->bwdMappedMechanicalState(node, node->mechanicalModel);
			end(node, node->mechanicalModel, t0);
			t0 = begin(node, node->mechanicalMapping);
			this->bwdMechanicalMapping(node, node->mechanicalMapping);
			end(node, node->mechanicalMapping, t0);
		} else {
			ctime_t t0 = begin(node, node->mechanicalModel);
			this->bwdMechanicalState(node, node->mechanicalModel);
			end(node, node->mechanicalModel, t0);
		}
	}
}

MechanicalPropagatePositionAndVelocityAction::MechanicalPropagatePositionAndVelocityAction(double t, VecId x, VecId v) : t(t), x(x), v(v)
{
    //cerr<<"::MechanicalPropagatePositionAndVelocityAction"<<endl;
}


Action::Result MechanicalPropagatePositionAndVelocityAction::processNodeTopDown(GNode* node)
{
	//cerr<<" MechanicalPropagatePositionAndVelocityAction::processNodeTopDown "<<node->getName()<<endl;   
	node->setTime(t);
        node->updateContext();
        return MechanicalAction::processNodeTopDown( node);
}

void MechanicalPropagatePositionAndVelocityAction::processNodeBottomUp(GNode* node)
{
	//cerr<<" MechanicalPropagatePositionAndVelocityAction::processNodeBottomUp "<<node->getName()<<endl;   
	for_each(this, node, node->constraint, &MechanicalPropagatePositionAndVelocityAction::bwdConstraint);

}



// void MechanicalVOpAction::processNodeBottomUp(GNode* node)
// {
//    MechanicalAction::processNodeBottomUp(node);
//    if( node->getDebug() && node->mechanicalModel ) {
//       cerr<<"MechanicalVopAction::processNodeBottomUp, node "<<node->getName()<<", x=";
//       (*node->mechanicalModel).printX(cerr);
//       cerr<<endl;
//       cerr<<"MechanicalVopAction::processNodeBottomUp, node "<<node->getName()<<", v=";
//       (*node->mechanicalModel).printV(cerr);
//       cerr<<endl;
//       cerr<<"MechanicalVopAction::processNodeBottomUp, node "<<node->getName()<<", f=";
//       (*node->mechanicalModel).printF(cerr);
//       cerr<<endl;
//       cerr<<"MechanicalVopAction::processNodeBottomUp, node "<<node->getName()<<", dx=";
//       (*node->mechanicalModel).printDx(cerr);
//       cerr<<endl;
//    }
// }

} // namespace tree

} // namespace simulation

} // namespace sofa

