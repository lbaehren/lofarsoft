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
#include <map>
#include <sofa/core/BehaviorModel.h>
#include <sofa/simulation/automatescheduler/Automate.h>
#include <sofa/simulation/automatescheduler/AutomateUtils.h>
#include <sofa/simulation/automatescheduler/StateMachine.h>
#include <sofa/simulation/tree/xml/Element.h>
#include <sofa/simulation/automatescheduler/Edge.h>
#include <sofa/simulation/automatescheduler/ObjSubAutomate.h>
#include <sofa/simulation/automatescheduler/ObjSubAutomateNodeExec.h>
#include <sofa/simulation/automatescheduler/utils.h>



namespace sofa
{

namespace simulation
{

namespace automatescheduler
{

extern MonoFrequencyObjSubAutomate* subAutomate;

using namespace core::objectmodel;

Automate *Automate::automate = NULL;
Automate::DrawCB* Automate::drawCB = NULL;

Automate::Automate()
{
	//drawCB = NULL;
}

void Automate::execDrawCB(void)
{
	if (drawCB!=NULL)
		drawCB->drawFromAutomate();
}

Automate* Automate::getInstance()
{
	if (automate == NULL)
	{
		automate = new Automate();		
	}

	return automate;
}

void Automate::load(char *_automateType, bool _periphInSimulation)
{
	if (strcmp(_automateType, "simpleMonoFrequency") == 0)
	{
		std::cout << "-- SCHEDULING -- SimpleMonoFrequency Scheduler\n";

		MonoFrequencyObjSubAutomate *monoFreqSubAuto = new MonoFrequencyObjSubAutomate(2);

		Node *begin = new Node("begin", 0, 0);
		
		NodeExec *beginLoop = new NodeExec("beginLoop", 1, 0);
		beginLoop->setAction(beginLoopFunc);

		NodeExec *endLoop = new NodeExec("endLoop", 6, 0);
		endLoop->setAction(endLoopFunc);

		Node *end = new Node("end", 12, 0);

		NodeExec *incrb = new NodeExec("b++", 11, 0);
		incrb->setAction(incrB);

		NodeExec *rezb = new NodeExec("b=0", 7, 0);
		rezb->setAction(rezB);

		CondExec *condB	= new CondExec();
		condB->setAction(testB);

		Cond *tjVrai = new Cond();
		
		// SubAutomate

		begin->addOutput(new Edge(tjVrai, begin, beginLoop));

		beginLoop->addOutput(new Edge(tjVrai, beginLoop, monoFreqSubAuto->entry));

		NodeExec *disp = new NodeExec("disp", 9, 2);
		NodePushDisplay *pushDisp = new NodePushDisplay(" ", 9, 2, disp);
		disp->setAction(drawCode);
		//disp->addOutput(new Edge(tjVrai, disp, pushDisp));
		disp->addOutput(new Edge(tjVrai, disp, incrb));
		//begin->addOutput(new Edge(tjVrai, begin, pushDisp));

		if (_periphInSimulation)
		{
			NodeExec *periph = new NodeExec("periph", 2, -2);
			NodePushPeriph *pushPeriph = new NodePushPeriph(" ", 2, -2, periph);
			periph->setAction(periphCode);
			periph->addOutput(new Edge(tjVrai, periph, pushPeriph));
			begin->addOutput(new Edge(tjVrai, begin, pushPeriph));
		}		

		NodeExec *collR = new NodeExec("collR", 10, -2);
		NodePushDisplay *pushCollR = new NodePushDisplay(" ", 10, -2, collR);
		collR->setAction(collisResponseCode);
		//collR->addOutput(new Edge(tjVrai, collR, end));
		collR->addOutput(new Edge(tjVrai, collR, incrb));

		NodeExec *collD = new NodeExec("collD", 9, -2);
		NodePush *pushCollD = new NodePush(" ", 9, -2, collD);
		//NodePushDisplay *pushCollD = new NodePushDisplay(" ", 8, -2, collD);
		collD->setAction(collisDetectionCode);
		collD->addOutput(new Edge(tjVrai, collD, pushCollR));

		NodeExec *coll = new NodeExec("coll", 8, -2);
		NodePushDisplay *pushColl = new NodePushDisplay(" ", 8, -2, coll);
		coll->setAction(collisResetCode);
		coll->addOutput(new Edge(tjVrai, coll, pushCollD));

		//(monoFreqSubAuto->getExitNode())->addOutput(new Edge(tjVrai, monoFreqSubAuto->getExitNode(), pushColl));
		(monoFreqSubAuto->getExitNode())->addOutput(new Edge(tjVrai, monoFreqSubAuto->getExitNode(), endLoop));
		endLoop->addOutput(new Edge(tjVrai, endLoop, rezb));

		std::vector< MonoFrequencyObjSubAutomateAttribute* > tmp;

		std::map < std::string, core::BehaviorModel* > behaviorModels; // = Scene::getInstance()->getBehaviorModels();
		/// \todo get list of behavior models from scenegraph

		std::map < std::string, core::BehaviorModel* >::iterator itCur = behaviorModels.begin();
		std::map < std::string, core::BehaviorModel* >::iterator itEnd = behaviorModels.end();

		while (itCur != itEnd)
		{
			if ((*itCur).first != "Mouse1")
			{
			//	tmp.push_back(new MonoFrequencyObjSubAutomateAttribute(obj_nextStepCode, (*itCur).first));
			}
			itCur++;
		}
		
		monoFreqSubAuto->setActionList(tmp);
		monoFreqSubAuto->setExitCondition(tmp.size());
		subAutomate = monoFreqSubAuto;

		rezb->addOutput(new Edge(tjVrai, rezb, pushColl));
		rezb->addOutput(new Edge(tjVrai, rezb, pushDisp));

		incrb->addOutput(new Edge(condB, incrb, end));
		
		// \SubAutomate

		end->addOutput(new Edge(tjVrai, end, beginLoop));

		begin->addOutput(new Edge(tjVrai, beginLoop, exitCompteur()));

		StateMachine::getInstance()->queueSerial(begin);
	}
	else if (strcmp(_automateType, "complexMonoFrequency") == 0)
	{
		std::cout << "-- SCHEDULING -- ComplexMonoFrequency Scheduler\n";

		MonoFrequencyObjSubAutomate *monoFreqSubAuto = new MonoFrequencyObjSubAutomate(2);

		Node *begin = new Node("begin", 0, 0);
		
		NodeExec *beginLoop = new NodeExec("beginLoop", 1, 0);
		beginLoop->setAction(beginLoopFunc);

		NodeExec *endLoop = new NodeExec("endLoop", 6, 0);
		endLoop->setAction(endLoopFunc);

		Node *end = new Node("end", 12, 0);

		Cond *tjVrai = new Cond();
		
		// SubAutomate

		begin->addOutput(new Edge(tjVrai, begin, beginLoop));

		beginLoop->addOutput(new Edge(tjVrai, beginLoop, monoFreqSubAuto->entry));

		NodeExec *disp = new NodeExec("disp", 1, 3);
		NodePushDisplay *pushDisp = new NodePushDisplay(" ", 1, 3, disp);
		disp->setAction(drawCode);
		disp->addOutput(new Edge(tjVrai, disp, pushDisp));

		begin->addOutput(new Edge(tjVrai, begin, pushDisp));

		NodeExec *collR = new NodeExec("collR", 10, 0);
		NodePushDisplay *pushCollR = new NodePushDisplay(" ", 10, 0, collR);
		collR->setAction(collisResponseCode);
		collR->addOutput(new Edge(tjVrai, collR, end));

		NodeExec *collD = new NodeExec("collD", 9, 0);
		NodePush *pushCollD = new NodePush(" ", 9, 0, collD);
		//NodePushDisplay *pushCollD = new NodePushDisplay(" ", 8, -2, collD);
		collD->setAction(collisDetectionCode);
		collD->addOutput(new Edge(tjVrai, collD, pushCollR));

		NodeExec *coll = new NodeExec("coll", 8, 0);
		NodePushDisplay *pushColl = new NodePushDisplay(" ", 8, 0, coll);
		coll->setAction(collisResetCode);
		coll->addOutput(new Edge(tjVrai, coll, pushCollD));

		(monoFreqSubAuto->getExitNode())->addOutput(new Edge(tjVrai, monoFreqSubAuto->getExitNode(), endLoop));
		endLoop->addOutput(new Edge(tjVrai, endLoop, pushColl));

		subAutomate = monoFreqSubAuto;
		
		// \SubAutomate

		end->addOutput(new Edge(tjVrai, end, beginLoop));

		begin->addOutput(new Edge(tjVrai, beginLoop, exitCompteur()));

		StateMachine::getInstance()->queueSerial(begin);
	}
	else if (strcmp(_automateType, "test") == 0)
	{
		std::cout << "Test Scheduler chosen for execution\n";

		Node *begin = new Node("begin", 0, 0);
		Node *end = new Node("end", 10, 0);

		// A
		NodeExec *incra = new NodeExec("a++", 5, 0);
		incra->setAction(incrA);

		NodeExec *reza = new NodeExec("a=0", 1, 0);
		reza->setAction(rezA);

		CondExec *condA	= new CondExec();
		condA->setAction(testA);

		// B
		NodeExec *incrb = new NodeExec("b++", 8, 0);
		incrb->setAction(incrB);

		NodeExec *rezb = new NodeExec("b=0", 6, 0);
		rezb->setAction(rezB);

		CondExec *condB	= new CondExec();
		condB->setAction(testB);

		// 1
		Cond *tjVrai = new Cond();

		begin->addOutput(new Edge(tjVrai, begin, reza));
		
		// BM
		NodeExec *obj0 = new NodeExec("obj0", 4, -2);
		NodePush *pushObj0 = new NodePush(" ", 4, -2, obj0);
		obj0->setAction(o0);
		obj0->addOutput(new Edge(tjVrai, obj0, incra));
		reza->addOutput(new Edge(tjVrai, reza, pushObj0));

		NodeExec *obj1 = new NodeExec("obj1", 4, 0);
		NodePush *pushObj1 = new NodePush(" ", 4, 0, obj1);
		obj1->setAction(o1);
		obj1->addOutput(new Edge(tjVrai, obj1, incra));
		reza->addOutput(new Edge(tjVrai, reza, pushObj1));

		NodeExec *obj2 = new NodeExec("obj2", 4, 2);
		NodePush *pushObj2 = new NodePush(" ", 4, 2, obj2);
		obj2->setAction(o2);
		obj2->addOutput(new Edge(tjVrai, obj2, incra));
		reza->addOutput(new Edge(tjVrai, reza, pushObj2));

		NodeExec *obj3 = new NodeExec("obj3", 4, 4);
		NodePush *pushObj3 = new NodePush(" ", 4, 4, obj3);
		obj3->setAction(o3);
		obj3->addOutput(new Edge(tjVrai, obj3, incra));
		reza->addOutput(new Edge(tjVrai, reza, pushObj3));

		incra->addOutput(new Edge(condA, incra, rezb));

		// Display
		NodeExec *disp = new NodeExec("disp", 7, 2);
		NodePushDisplay *pushDisp = new NodePushDisplay(" ", 7, 2, disp);
		disp->setAction(drawCode);
		disp->addOutput(new Edge(tjVrai, disp, incrb));

		// Collision
		NodeExec *coll = new NodeExec("coll", 7, -2);
		NodePush *pushColl = new NodePush(" ", 7, -2, coll);
		coll->setAction(collisCode);
		coll->addOutput(new Edge(tjVrai, coll, incrb));

		rezb->addOutput(new Edge(tjVrai, rezb, pushColl));
		rezb->addOutput(new Edge(tjVrai, rezb, pushDisp));

		incrb->addOutput(new Edge(condB, incrb, end));
		
		end->addOutput(new Edge(tjVrai, end, begin));

		StateMachine::getInstance()->queueSerial(begin);
	}
	else if (strcmp(_automateType, "MultiFrequency") == 0)
	{
		
	}
}


Node* Automate::exitCompteur(void)
{
	NodeExec *dec = new NodeExec("exit", 2, 1);
	NodeExec *ex  = new NodeExec("", 2, 1);
	CondExec *test = new CondExec();

	dec->setAction( decompteur );
	test->setAction( isFini );
	ex->setAction( exitCall );

	dec->addOutput( new Edge(test, dec, ex) );
	return dec;
}

} // namespace automatescheduler

} // namespace simulation

} // namespace sofa

