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
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <sofa/simulation/automatescheduler/utils.h>
#include <sofa/simulation/automatescheduler/CPU.h>
#include <sofa/simulation/automatescheduler/Node.h>
#include <sofa/helper/system/thread/CTime.h>
#include <sofa/simulation/automatescheduler/ExecBus.h>
#include <sofa/simulation/automatescheduler/StateMachine.h>


using namespace std;

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

////////
// PU //
////////

int PU::sid = 0;

PU::PU()
{
	id = sid++;
	sprintf(name, "PU%d", id);
}


/////////
// CPU //
/////////

CPU::CPU(ExecBus *xb, CPU_TYPE _type) 
{
	xbus = xb;
	cout << name << ": init\n";
	type = _type;
}


void CPU::init()
{

}


#ifdef WIN32
void CPU::loop(void *sarg)
#else 
void* CPU::loop(void *sarg)
#endif
{
	Arg *arg = (Arg*)sarg;

	CPU *cpu = arg->pu;
	ExecBus *xbus = arg->xbus;
	//int timestamp = 0; //unused variable

	cout << cpu->name  << ": start\n";
//	ctime_t debut;
	// ctime_t fin; // unused variable
//	ctime_t totalTime = 0;
//	ctime_t total2 = 0;
//	ctime_t total3 = 0;
	// int myCounter = 0; //unused variable
	Node *currentNode = NULL;

//	xbus->getStateMachine()->onEnter();

	while( 1 )
	{    
	//	ctime_t begin3 = MesureTemps();

		switch (cpu->type)
		{
			case STANDARD:
				currentNode = xbus->getNext(cpu->name, currentNode);
				break;

			case PERIPH:
				currentNode = xbus->getNext("periphThread", currentNode);
				break;

			default:
#ifdef WIN32
				return;
#else
				return NULL;
#endif 
				break;
		}

		if( currentNode != NULL )
		{
		//	ctime_t begin2 = MesureTemps();
		//	debut = MesureTemps();		    
			currentNode->execute(cpu->name);
		//	totalTime += MesureTemps() - debut;		    
		//	total2 += MesureTemps() - begin2;
		}

	//	total3 += MesureTemps()-begin3;
	}

	cout << cpu->name << ": end\n";
}

void CPU::start()
{
	cout << name << " : thread creation\n";

	Arg *arg = new Arg(this, xbus);

#ifdef WIN32
	threadID = _beginthread( CPU::loop, 0, arg);
#else
	pthread_create( &threadID, NULL, CPU::loop, arg );
#endif
}


void CPU::finalize()
{
	cout << name << " : waiting end of thread\n";

#ifdef WIN32
	
#else
	pthread_join(threadID,NULL);
#endif

	cout << name << " : finalized\n";
}

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

