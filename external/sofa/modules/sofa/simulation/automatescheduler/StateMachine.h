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
#ifndef SOFA_SIMULATION_AUTOMATESCHEDULER_STATEMACHINE_H
#define SOFA_SIMULATION_AUTOMATESCHEDULER_STATEMACHINE_H

#include <vector>
#include <deque>
#include <iostream>

#ifdef WIN32
#include <process.h>
#else
#include <pthread.h>
#endif

#include <sofa/helper/system/thread/CTime.h>
#include "utils.h"

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

using namespace sofa::helper::system::thread;

class Node;

//
// The state machine is a cyclic graph that stores the simulation process
//

class StateMachine
{
private:

	static StateMachine *STATEMACHINE_instance;

	std::deque<simulation::automatescheduler::Node*> tabSerial;
	std::deque<simulation::automatescheduler::Node*> tabParralel;
	std::deque<simulation::automatescheduler::Node*> tabDisplay;
	std::deque<simulation::automatescheduler::Node*> tabPeriph;

	static int totalNode;
	static ctime_t debutTemps;

 public:

	StateMachine();
	~StateMachine();

	static StateMachine* getInstance();

	void onEnter();
	void onExit();

	void queueSerial(simulation::automatescheduler::Node *);
	void queueParralel(simulation::automatescheduler::Node *);
	void queueDisplay(simulation::automatescheduler::Node *);
	void queuePeriph(simulation::automatescheduler::Node *);
  
	void handleAutomaton(char*);

	simulation::automatescheduler::Node* getNext(char *str, simulation::automatescheduler::Node *oldNode);
	bool isEmpty();
};

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

#endif
