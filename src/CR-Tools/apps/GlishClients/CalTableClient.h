/***************************************************************************
 *   Copyright (C) 2006                                                  *
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

/* $Id: CalTableClient.h,v 1.5 2006/11/08 17:58:44 bahren Exp $*/

#ifndef _CALTABLECLIENT_H_
#define _CALTABLECLIENT_H_

// CallTables includes
#include <ApplicationSupport/Glish.h>
#include <Calibration/CalTableWriter.h>
#include <Calibration/CalTableReader.h>
#include <Calibration/CalTableCreator.h>

// Glish includes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>

// general includes
#include <iostream>
#include <sstream>


/*!
  \file CalTableClient.h
  
  \ingroup CR_Glish

  \brief Wrapper to use the CalTables from Glish
  
  \author Andreas Horneffer
  
  \date 2006/01/21
  
  <h3>Prerequisite</h3>
  
  <h3>Synopsis</h3>
  This is technically a Glish client, and functionally a wrapper that maps the 
  CalTables API to functions callable from glish.
  
  <h3>Example(s)</h3>
  
*/

// ----------------------------------------------------------------------------

/*!
  \brief Open a calibration table for further operation
*/
Bool openTable(GlishSysEvent &event, void *);

/*!
  \brief Get data from a calibration table 
*/
Bool getData(GlishSysEvent &event, void *);

//------------------------------------------------

/*!
  \brief Get the value of a keyword
*/
Bool GetKeyword(GlishSysEvent &event, void *);

/*!
  \brief Set the value of a (new) keyword
*/
Bool SetKeyword(GlishSysEvent &event, void *);


//------------------------------------------------

/*!
  \brief Add data to a calibration table 
*/
Bool addData(GlishSysEvent &event, void *);

/*!
  \brief Add a field to a calibration table
*/
Bool addField(GlishSysEvent &event, void *);

//------------------------------------------------

/*!
  \brief Add a new antenna to a calibration table
*/
Bool addAntenna(GlishSysEvent &event, void *);

/*!
  \brief Print a summary of a calibration table
*/
Bool PrintSummary(GlishSysEvent &event, void *);

/*!
  \brief Open the calibration table for writing
*/
Bool openWrite(GlishSysEvent &event, void *);

/*!
  \brief Create a new, empty calibration table
*/
Bool CreateEmptyTable(GlishSysEvent &event, void *);

/*!
  \brief Report an error happing during trying to get data

  \param Field -- Name of the field in the data table
  \param AntID -- Antenna ID
  \param Date  -- Date for which the data where requested
*/
void ErrorGetData (String const &Field,
		   Int const &AntID,
		   uInt const &Date)
{
  std::cerr
    << "CalTableClient:GetData  Could not get '"
    << Field
    << "', Ant: "
    << AntID
    << ", date: "
    << Date
    << "!"
    << std::endl;
}

/*!
  \brief Report an error happing during trying to get data

  \param Field     -- Name of the field in the data table
  \param AntID     -- Antenna ID
  \param startdate -- Start date of the time interval, for which the value
                      was to be added
  \param stopdate  -- Stop date of the time internal, for which the value was
                      to be added
*/
void ErrorAddData (String const &Field,
		   Int const &AntID,
		   uInt const &startdate,
		   uInt const &stopdate)
{
  std::cerr
    << "CalTableClient:addData  Could not add data to '"
    << Field
    << "', Ant: "
    << AntID
    << ", startdate: "
    << startdate
    << ", stopdate: "
    << stopdate
    << "!"
    << std::endl;
}

#endif /* _CALTABLECLIENT_H_ */
