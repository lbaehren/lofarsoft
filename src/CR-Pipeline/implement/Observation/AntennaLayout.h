/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B"ahren (bahren@astron.nl)                                       *
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

#ifndef ANTENNALAYOUT_H
#define ANTENNALAYOUT_H

/* $Id: AntennaLayout.h,v 1.5 2006/11/27 10:54:04 bahren Exp $ */

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <coordinates/Coordinates/ObsInfo.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/StandardStMan.h>
#include <tables/Tables/IncrementalStMan.h>

using casa::Matrix;
using casa::MEpoch;
using casa::ObsInfo;
using casa::Quantity;
using casa::String;
using casa::Table;
using casa::Vector;

namespace CR {  // namespace CR -- begin

/*!
  \class AntennaLayout

  \ingroup Observation

  \brief Read in the antenna layout for a given telescope and epoch

  \author Lars B&auml;hren

  \date 2005/05

  \test tAntennaLayout.cc

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/tables/implement/Tables.html">Tables</a> -- Tables are the fundamental storage mechanism for AIPS++
  </ul>

  <h3>Synopsis</h3>

  Access and maintain the information about the antenna positions for a
  phased array. This requires considering not only the name of the observatory
  but also the epoch of the observation, as the antenna layout may change as
  function of time. For this reason antenna layout information are stored in a 
  number of AIPS++ tables, which are scanned for telecope name and epoch.

  <h4>(a) Layout of the antenna table</h4>

  The antennna table is intended not only not hold the 3-dim antenna positions
  but also possible information regarding feeds for different polarizations for
  a given antenna. Based on these demands the layout of the AIPS++ table is
  chosen as follows:

  <table cellpadding="3">
    <tr>
      <td class="indexkey">telescope</td>
      <td class="indexkey">epoch</td>
      <td class="indexkey">Antenna</td>
      <td class="indexkey">Feed</td>
      <td class="indexkey">Polarization</td>
      <td class="indexkey">Position</td>
      <td class="indexkey">comment</td>
    </tr>
    <tr>
      <td class="indexvalue">String</td>
      <td class="indexvalue">MEpoch</td>
      <td class="indexvalue">Vector<Int></td>
      <td class="indexvalue">Vector<Int></td>
      <td class="indexvalue">Vector<String></td>
      <td class="indexvalue">Matrix<double></td>
      <td class="indexvalue">String</td>
    </tr>
  </table>

  As an example consider antenna information for the LOFAR ITS -- both at 60 
  single polarization dipoles and 30 dual polarization dipoles.

  <table cellpadding="3">
    <tr>
      <td class="indexkey">telescope</td>
      <td class="indexkey">epoch</td>
      <td class="indexkey">Antenna</td>
      <td class="indexkey">Feed</td>
      <td class="indexkey">Polarization</td>
      <td class="indexkey">Position</td>
      <td class="indexkey">comment</td>
    </tr>
    <tr>
      <td class="indexvalue">ITS</td>
      <td class="indexvalue">2003/12/01</td>
      <td class="indexvalue">
      [001,<br>
       002,<br>
       003,<br>
       004,<br>
       ...]</td>
      <td class="indexvalue">
      [001,<br>
       002,<br>
       003,<br>
       004,<br>
       ...]</td>
      <td class="indexvalue">
      [y,<br>
       y,<br>
       y,<br>
       y,<br>
       ...]</td>
      <td class="indexvalue">
      [18.7287, -14.0166, 0.0],<br>
      [23.5054, -12.539,  0.0],<br>
      [28.6266, -10.0498, 0.0],<br>
      [33.9609,  -6.3622, 0.0],<br>
      ...
      </td>
      <td class="indexvalue">60 x single-polarization </td>
    </tr>
    <tr>
      <td class="indexvalue">ITS</td>
      <td class="indexvalue">2005/03/30</td>
      <td class="indexvalue">
      [001,<br>
       001,<br>
       002,<br>
       002,<br>
       ...]</td>
      <td class="indexvalue">
      [003,<br>
       001,<br>
       005,<br>
       002,<br>
       ...]</td>
      <td class="indexvalue">
      [y,<br>
       x,<br>
       y,<br>
       x,<br>
       ...]</td>
      <td class="indexvalue">
      [18.7287, -14.0166, 0.0],<br>
      [18.7287, -14.0166, 0.0],<br>
      [23.5054, -12.539,  0.0],<br>
      [23.5054, -12.539,  0.0],<br>
      ...
      </td>
      <td class="indexvalue">30 x dual polarization</td>
    </tr>
  </table>

 */

class AntennaLayout {

  // Observation information: epoch, telescope, observer.
  ObsInfo obsInfo_p;

  // Table holding the antenna layout information
  Table table_p;

  // Row of the antenna layout table containing the data for the given
  // combination of telescope and epoch.
  int tableRow_p;
  
 public:
  
  // -----------------------------------------------------------------
  // Construction

  /*!
    \brief Empty constructor.

    Since we need a minimum of information to set up the internal data, this
    constructor just creates a new object but does not do anything else.
  */
  AntennaLayout ();

  /*!
    \brief Argumented constructor
    
    \param telescope -- Name of the telescope.
    \param epoch     -- Epoch of the observation.
  */
  AntennaLayout (const String& tableFilename,
		 const String& telescope,
		 const Quantity& epoch);

  /*!
    \brief Argumented constructor.

    \param obsInfo -- Epoch, telescope and observer info.
   */
  AntennaLayout (const String& tableFilename,
		 const ObsInfo& obsInfo);

  // -----------------------------------------------------------------
  // 

  /*!
    \brief 
  */
  void setEpoch (const MEpoch& epoch);

  /*!
    \brief 
  */
  void setTelescope (const String& telescope);
  
  // -----------------------------------------------------------------
  // Access to the antenna positions

  /*!
    \brief Get the position of a specific antenna.
    
    \param antenna -- The number of the antenna.

    return position -- The 3-dim antenna position.
  */
  Vector<double> antennaPosition (const int& antenna);

  /*!
    \brief Get the antenna positions.

    \return antennaPositions -- 3-dimension antenna positions w.r.t. to the
                                telescope phase center.
  */
  Matrix<double> antennaPositions ();

  /*!
    \brief Get antenna positions for feeds of a given polarization.

    \param polarization -- Polarization of the feeds, for which the antenna
                           position information are extracted.

    \retval antennaPositions --
    \retval antennaNumber    -- 
    \retval feedNumber       -- 
  */
  void antennaPositions (const String& polarization,
			 Matrix<double>& antennaPositions,
			 Vector<int>& antennaNumber,
			 Vector<int>& feedNumber);

  // -----------------------------------------------------------------
  // Manipulation of the layout table itself

  /*!
    \brief Read a complete entry from the antenna layout table

    \param telescope -- Name of the telescope.
    \param epoch     -- Epoch for which the setting is requested.

    \retval epoch            -- Epoch from which on the returned setup is valid;
                                closest time earlier than the request date.
    \retval antennaNumber    -- Numbering of the telescope antennas.
    \retval feedNumber       -- Numbering of the telescope feeds.
    \retval polarization     -- Polarization of the feeds.
    \retval antennaPositions -- 3-dimension antenna positions w.r.t. to the
                                telescope phase center.
    \retval comment          -- User comment on the stored setup.

    \return ok -- Status of the operation; returns <i>False</i> if an error
                  was encoutered.
  */
  bool readEntry (const String& telescope,
		  MEpoch& epoch,
		  Vector<int>& antennaNumber,
		  Vector<int>& feedNumber,
		  Vector<String>& polarization,
		  Matrix<double> antennaPositions,
		  String& comment);

  /*!
    \brief Add a new entry to the antenna layout table.

    \param telescope        -- Name of the telescope.
    \param epoch            -- Epoch from which on this setting is valid.
    \param antennaNumber    -- Numbering of the telescope antennas.
    \param feedNumber       -- Numbering of the telescope feeds.
    \param polarization     -- Polarization of the feeds.
    \param antennaPositions -- 3-dimension antenna positions w.r.t. to the
                               telescope phase center.
    \param comment          -- User comment on the stored setup.

    \return ok -- Status of the operation; returns <i>False</i> if an error
                  was encoutered.
  */
  bool addNewEntry (const String& telescope,
		    const MEpoch& epoch,
		    const Vector<int>& antennaNumber,
		    const Vector<int>& feedNumber,
		    const Vector<String>& polarization,
		    const Matrix<double> antennaPositions,
		    const String& comment);

};  // class AntennaLayout -- end

}  // namespace CR --- end

#endif
