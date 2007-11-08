/*-------------------------------------------------------------------------*
 | $Id:: templates.h 391 2007-06-13 09:25:11Z baehren                    $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2004                                                    *
 *   Andreas Horneffer ()                                                  *
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

#if !defined(READDAT_H)
#define READDAT_H

// Standard header files
#include <unistd.h>
#include <netinet/in.h>

// AIPS++/CASA header files
#include <casa/aips.h>
#include <casa/stdio.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Vector.h>
#include <casa/Arrays/Matrix.h>
#include <casa/BasicSL/Complex.h>

// LOPES-Tools header files
#include <ApplicationSupport/Glish.h>
#include <ApplicationSupport/GlishEvent.h>
#include <ApplicationSupport/GlishRecord.h>
#include <ApplicationSupport/DataCC.h>
#include <Utilities/StringTools.h>
#include <Utilities/Masking.h>
#include <ApplicationSupport/ObservationMeta.h>

#include <casa/namespace.h>

/*!
  \file readdat.h
  
  \ingroup CR_Applications

  \brief C-programm Glish client to read a datafile

  <h3>Prerequisite</h3>

  <ul>
    <li><a href="http://aips2.nrao.edu/docs/aips/implement/Glish.html">Module
    Glish</a> - AIPS++ wrapper classes for Glish values and events. AIPS++ is
    using Glish as a user command line interpreter and as the communications
    infrastructure. Functionality is bound to the CLI by creating new clients
    which pass events back and forth between the client and the CLI.
  </ul>

  <h3>Notes:</h3>

  <ul>
    <li>Currently only int16-files and ascii-tables are supported
  </ul>

  <h3>Examples:</h3>
  
  <ol>
    <li>Given that at some point in your Glish code you have defined <i>readdat</i>
    to be a client program, you can later call it - given you provide the
    necessary input:
    \verbatim
    # Construct the Glish CLI by providing the path to the program executable
    readdat_client := client(spaste(mydir,'/readdat',sep=''));

    # Set up the record to pass parameters to the CLI
    feld          := [];
    file.filename := infile;
    file.block    := blocksize;
    file.offset   := offset
    file.type     := type;

    # Send event with parameters to the CLI
    feld := readdat_client->readfile(file);
    \endverbatim
  </ol>
 */

/*!
  \brief Read data from data file

  \param filename -- Name of the file, from which the data are read.
  \param type     -- Type of the file; currently supported: ("int16","tab")
  \param block    -- Length of the block.
  \param offset   -- data-offset.
  
  \return retval -- GlishArray with the data.
 */
GlishArray readDataFromFile (const String filename,
			     const int type,
			     const int block,
			     const int offset);

/*!
  \fn Bool readfile(GlishSysEvent &event, void *)
  
  \brief Event Callback Function to read in a data file.

  The access itself of the datafile is carried out by the function 
  readDataFromFile
  
  \param filerec Record describing the file; record fields are:
  \param filerec.filename Name of the file [String]
  \param filerec.type type of the file [String]; currently supported:
         ("int16","tab")
  \param filerec.block length of the block [int]
  \param filerec.offset data-offset [int]
  
  \return GlishArray with the data
*/
Bool readfile (GlishSysEvent &event, void *);

/*!
  \fn Bool writefile(GlishSysEvent &event, void *)
  
  \brief Event Callback Function to write to a datafile
  
  \param filerec          Record describing the file - record fields are:
  \param filerec.data     Array with the data
  \param filerec.filename Name of the file [String]
  \param filerec.type     type of the file [String]; currently supportet:
                          ("int16","tab")
  \return True or False
*/
Bool writefile(GlishSysEvent &event, void *);

/*!
  \fn Bool getVisibilities (GlishSysEvent &event, void *)

  \brief Get visibilities from a cross-correlation dataset.

  \param record             -- Record describing the file - record fields are:
  \param record.metafile    -- Path to the experiment metafile; for an ITS
                               experiment this will by <i>experiment.meta</i>
			       located in the experiment data directory. The 
			       full path to the metafile must be specified.
  \param record.antennafile -- File with the antenna position; full path to
                               file must be specified.

  \retval record              -- Glish record passing the results -- record
                                 fields are:
  \retval record.visibilities -- Complex visibilities; matrix of shape
                                 [antennas,frequencies].
  \retval record.baselines    -- Numbering of the baselines, for which the
                                 visibility values are returned; this is the 
				 running index when accessing the set of antenna
				 combinations.

  \return ok -- Status of the routine.
*/
Bool getVisibilities (GlishSysEvent &event, void *);


#endif
