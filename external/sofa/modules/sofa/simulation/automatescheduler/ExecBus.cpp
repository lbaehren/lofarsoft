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

#ifndef WIN32
#include <pthread.h>
#endif

#include <sofa/simulation/automatescheduler/ExecBus.h>
#include <sofa/simulation/automatescheduler/StateMachine.h>
#include <sofa/simulation/automatescheduler/Node.h>
#include <sofa/simulation/automatescheduler/utils.h>
#include <sofa/helper/system/thread/CTime.h>

using namespace std;

namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

ExecBus *ExecBus::EXECBUS_instance = NULL;
bool ExecBus::instanceInitialized  = false;

// ExecBus()

ExecBus::ExecBus()
{
#ifdef WIN32
	mutex = CreateMutex( NULL, FALSE, NULL);
#else
	pthread_mutex_init(&mutex, NULL);
#endif
}

// initInstance()

void ExecBus::initInstance(StateMachine *_stateMachine)
{
	this->stateMachine = _stateMachine;
	instanceInitialized = true;
}

// getInstance()

ExecBus* ExecBus::getInstance()
{
	if (EXECBUS_instance == NULL)
	{
		EXECBUS_instance = new ExecBus();
		return EXECBUS_instance;
	}
	else if (!instanceInitialized)
	{
		std::cerr << "Trying to get ExecBus Instance before initializing\n";
		return NULL;
	}
	else
	{
		return EXECBUS_instance;
	}
}

// getStateMachine()

StateMachine* ExecBus::getStateMachine()
{ 
	return stateMachine;
}

// getNext()

Node* ExecBus::getNext(char *src, Node *oldNode)
{

#ifdef WIN32
	WaitForSingleObject(mutex, INFINITE);
#else
	pthread_mutex_lock( &mutex );
#endif

	Node *node  = stateMachine->getNext(src, oldNode);

#ifdef WIN32	
	ReleaseMutex(mutex);
#else
	pthread_mutex_unlock( &mutex );
#endif

	return node;
}

// isEmpty()

bool ExecBus::isEmpty(char *)
{	

#ifdef WIN32
	WaitForSingleObject(mutex, INFINITE);
#else
	pthread_mutex_lock( &mutex );
#endif
	
	if( stateMachine->isEmpty() )
	{
#ifdef WIN32
		ReleaseMutex(mutex);
#else
		pthread_mutex_unlock( &mutex );
#endif
		return true;
	}
	
#ifdef WIN32
	ReleaseMutex(mutex);
#else
	pthread_mutex_unlock( &mutex );
#endif

	return false;
}

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa
