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
#include "Node.h"
#include <sofa/simulation/automatescheduler/Edge.h>
#include <sofa/simulation/automatescheduler/StateMachine.h>
#include "Automate.h"

using namespace std;
using namespace sofa::helper::system::thread;

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

//////////
// Node //
//////////

Node::Node() : NodeGFX("Node",0,0) 
{ 
	Automate::getInstance()->tabNodes.push_back(this);
};


Node::Node(char *s, double xx, double xy) : NodeGFX(s,xx,xy)
{
	Automate::getInstance()->tabNodes.push_back(this);
};


void Node::onEnter(StateMachine*){ };


void Node::onExit(StateMachine*){ };


void Node::execute(char *){ };


void Node::addOutput(Edge *e){ tabOutputs.push_back( e ); }


void Node::update(StateMachine *sm)
{
	for(unsigned int i = 0; i < tabOutputs.size(); i++)
	{
		if(tabOutputs[i]->evalCond())
		{
			sm->queueSerial(tabOutputs[i]->getNext());
		}
	}
}


//////////////
// NodePush //
//////////////


void NodePush::onExit(StateMachine *sm)
{ 
	sm->queueParralel( src );
}

/////////////////////
// NodePushDisplay //
/////////////////////

void NodePushDisplay::onExit(StateMachine *sm)
{ 
	sm->queueDisplay( src );
}

////////////////////
// NodePushPeriph //
////////////////////

void NodePushPeriph::onExit(StateMachine *sm)
{ 
	sm->queuePeriph( src );
}

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

