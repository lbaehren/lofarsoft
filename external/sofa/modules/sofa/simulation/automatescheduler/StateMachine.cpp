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
#include <iostream>
#include <sofa/simulation/automatescheduler/StateMachine.h>
#include <sofa/simulation/automatescheduler/Node.h>

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

StateMachine *StateMachine::STATEMACHINE_instance = NULL;

int StateMachine::totalNode = 0;
ctime_t StateMachine::debutTemps = 0;


StateMachine::StateMachine()
{
	totalNode = 0;
}


StateMachine* StateMachine::getInstance()
{
	if (STATEMACHINE_instance == NULL)
	{
		STATEMACHINE_instance = new StateMachine();
	}

	return STATEMACHINE_instance;
}


void StateMachine::onEnter()
{ 
	debutTemps = CTime::getTime();
}


void StateMachine::onExit()
{
/*
	double totalTime = Timer::ticksToMS*(MesureTemps()-debutTemps);

	std::cout << "StateMachine statistics: \n";
	std::cout << "\t automaton size: " << tabNodes.size() << "\n";
	std::cout << "\t automaton total node executed: " << totalNode << "\n";
	std::cout << "\t total simulation time: " << totalTime << " ms \n";
	std::cout << "\t time per timestep: " << (totalTime)/( totalNode/tabNodes.size() ) <<" (in ms)\n";

	FILE *fpt = fopen("dump/automaton.dot","wt");
	fprintf(fpt,"digraph G {\n");

	for(int i=0;i<tabNodes.size();i++)
	{
		tabNodes[i]->toDot( fpt );
	}

	for(int i=0;i<tabNodes.size();i++)
	{
		for(int j=0;j<tabNodes[i]->tabOutputs.size();j++)
		{
			tabNodes[i]->tabOutputs[j]->toDot( fpt );
		}
	}

	fprintf(fpt,"}\n");
	fclose(fpt);
*/
}


void StateMachine::queueSerial(Node *n)
{
	tabSerial.push_back( n );
	n->status = STATUS_READY;
}


void StateMachine::queueParralel(Node *n)
{
	tabParralel.push_back( n );
	n->status = STATUS_READY;
}


void StateMachine::queueDisplay(Node *n)
{
	tabDisplay.push_back( n );
	n->status = STATUS_READY;
}


void StateMachine::queuePeriph(Node *n)
{
	tabPeriph.push_back( n );
	n->status = STATUS_READY;
}


Node* StateMachine::getNext(char *str, Node *n)
{
	if( n != NULL )
	{
		n->status = STATUS_BLOCK;
		n->onExit(this);
		n->update(this);
	}

	handleAutomaton(str);
	
	if (strcmp(str, "displayThread") == 0)
	{
		if( tabDisplay.size() != 0 )
		{
			n = tabDisplay.front();
			tabDisplay.pop_front();

			totalNode++;
			n->status = STATUS_EXEC;
			return n;
		}
	}

	if (strcmp(str, "periphThread") == 0)
	{
		if( tabPeriph.size() != 0 )
		{
			n = tabPeriph.front();
			tabPeriph.pop_front();

			totalNode++;
			n->status = STATUS_EXEC;
			return n;
		}
	}

	if( tabParralel.size() != 0 )
	{
		n = tabParralel.front();
		tabParralel.pop_front();

		totalNode++;
		n->status = STATUS_EXEC;
		return n;
	}

	return NULL;
}


void StateMachine::handleAutomaton(char *str)
{
	while( tabSerial.size() != 0 )
	{
		totalNode++;
		Node *n = tabSerial.front();
		tabSerial.pop_front(); 
		n->status = STATUS_EXEC;
		n->onEnter(this);
		n->execute(str);
		n->onExit(this);
		n->update(this);
		n->status = STATUS_BLOCK;
	}
}


bool StateMachine::isEmpty()
{ 
	return ((tabSerial.size() == 0) && (tabParralel.size() == 0) && (tabDisplay.size() == 0) && (tabPeriph.size() == 0));
}

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa
