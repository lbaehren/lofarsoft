/***************************************************************************
 *   Copyright (C) 2007                                                  *
 *   Andreas Horneffer (<mail>)                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/* $Id: CGuiPipeClient.h,v 1.2 2007/06/21 08:37:16 horneff Exp $*/

#ifndef _CGUIPIPECLIENT_H_
#define _CGUIPIPECLIENT_H_

// CGuipipeline includes
#include <Analysis/analyseLOPESevent.h>
#include <ApplicationSupport/Glish.h>

// Glish includes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>

// general includes
#include <iostream>
#include <sstream>


/*!
  \file CGuiPipeClient.h
  
  \ingroup CR_Glish

  \brief Wrapper to call the C++-based pipeline.
  
  \author Andreas Horneffer
  
  \date 2007/06/20
  
  <h3>Prerequisite</h3>
  
  analyseLOPESevent 
  
  <h3>Synopsis</h3>
  This is technically a Glish client. It is intended to make the C++-based
  pipeline callable from Glish so that parameters known under Glish can be 
  passed to it.
  
  <h3>Example(s)</h3>
  
*/

// ----------------------------------------------------------------------------

/*!
  \brief initialize the pipeline 
*/
Bool initPipeline(GlishSysEvent &event, void *);

/*!
  \brief process one event
*/
Bool ProcessEvent(GlishSysEvent &event, void *);



#endif /* _CGUIPIPECLIENT_H_ */
