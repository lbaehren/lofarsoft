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

/* $Id: CGaccessClient.h,v 1.1 2007/07/26 14:37:22 horneff Exp $*/

#ifndef _CGACCESSCLIENT_H_
#define _CGACCESSCLIENT_H_


// Glish includes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>

// general includes
#include <iostream>
#include <sstream>

#include <Analysis/analyseLOPESevent.h>
#include <Data/tbbctlIn.h>
#include <ApplicationSupport/Glish.h>

/*!
  \file CGaccessClient.h
  
  \ingroup CR_Glish

  \brief Wrapper to access the stages of the C++-based pipeline from Glish
  
  \author Andreas Horneffer
  
  \date 2007/07/19
  
  <h3>Prerequisite</h3>
  
  <ul>
    <li>analyseLOPESevent 
  </ul>
  
  <h3>Synopsis</h3>
  This is technically a Glish client. It is intended to make the intermediate 
  of the steps C++-based pipeline accessible from Glish so that they can be 
  displayed in the gui.
  
  <h3>Example(s)</h3>
  
*/

// ----------------------------------------------------------------------------

/*!
  \brief Initialize the pipeline

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool initPipeline(GlishSysEvent &event, void *);

/*!
  \brief Set and get the status 

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered

   Currently controls if RFI-mitigation and/or TV-calibration is done
*/
Bool setStatus(GlishSysEvent &event, void *);

/*!
  \brief process one event

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool LoadEvent(GlishSysEvent &event, void *);

/*!
  \brief Get the Time

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetTime(GlishSysEvent &event, void *);

/*!
  \brief Get the Frequency

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetFrequency(GlishSysEvent &event, void *);

/*!
  \brief Get the FX

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetFX(GlishSysEvent &event, void *);

/*!
  \brief Get the Voltage

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetVoltage(GlishSysEvent &event, void *);

/*!
  \brief Get the FFT

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetFFT(GlishSysEvent &event, void *);

/*!
  \brief Get the CalFFT

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetCalFFT(GlishSysEvent &event, void *);

/*!
  \brief Get the FilteredFFT

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetFilteredFFT(GlishSysEvent &event, void *);

/*!
  \brief Get the (time-)shifted FFT

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetShiftedFFT(GlishSysEvent &event, void *);

/*!
  \brief Get the Beams etc.

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetTCXP(GlishSysEvent &event, void *);

/*!
  \brief Get upsampled data

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetUpsample(GlishSysEvent &event, void *);



/*!
  \brief Get the FieldStrength

  \return status -- Status of the operation; return <tt>false</tt> if an error
                    was encountered
*/
Bool GetFieldStrength(GlishSysEvent &event, void *);

/*!
  \brief Get the CCBeam
*/
Bool GetCCBeam(GlishSysEvent &event, void *);

/*!
  \brief Get the XBeam
*/
Bool GetXBeam(GlishSysEvent &event, void *);





#endif /* _CGACCESSCLIENT_H_ */
