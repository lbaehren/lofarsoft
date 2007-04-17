/***************************************************************************
 *   Copyright (C) 2005                                                    *
 *   Lars Bahren (bahren@astron.nl)                                        *
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

/* $Id: DataBlockSettings.h,v 1.7 2006/04/26 10:21:36 bahren Exp $*/

#ifndef _DATABLOCKSETTINGS_H_
#define _DATABLOCKSETTINGS_H_

#include <stdlib.h>
#include <iostream>
using namespace std;

/*!
  \class DataBlockSettings.h

  \ingroup IO

  \brief Check parameters used for progression through the data volume

  \author Lars B&auml;hren

  \date 2005/07/28

  \test tDataBlockSettings.cc

  \todo Add concept of a stride, controlling wether subsequent data blocks
  overlap, are contiguous or separated; for an example see the data progressing
  scheme used used with the DataReader.

  <h3>Prerequisite</h3>

  None.

  <h3>Synopsis</h3>

  This class is based on the original Glish routine <i>updateBlockSettings()</i>
  as introduced for the <i>skymapper</i>.

*/

class DataBlockSettings {

 public:

  enum method {
    //! Reduce the number of iterations
    ITERATIONS,
    //! Reduce the number of blocks per iteration
    BLOCKS,
    //! Reduce the number of samples per block (aka blocksize)
    SAMPLES
  };

  // --- Construction ----------------------------------------------------------

  /*!
    \brief Constructor
  */
  DataBlockSettings ();

  /*!
    \brief Constructor

    \param filesize -- Size the data file; [samples].
  */
  DataBlockSettings (const int& filesize);

  /*!
    \brief Constructor

    \param filesize -- Size the data file; [samples].
    \param which    -- Method by which the total accessed data volume is adjusted.
  */
  DataBlockSettings (const int& filesize,
		     DataBlockSettings::method which);

  // --- Destruction -----------------------------------------------------------

  /*!
    \brief Destructor
  */
  ~DataBlockSettings ();

  // --- Parameters ------------------------------------------------------------

  /*!
    \brief Get the size of the data file.

    \return filesize -- Size the data file; [samples].
  */
  int filesize () {
    return filesize_p;
  }

  /*!
    \brief Set the size of the data file.

    \param filesize -- Size the data file; [samples].
  */
  void setFilesize (const int& filesize) {
    filesize_p = filesize;
  }

  DataBlockSettings::method methodType () {
    return method_p;
  }

  /*!
    \brief Method by which the total accessed data volume is adjusted.
  */
  string methodName ();

  // --- Check settings --------------------------------------------------------

  /*!
    \brief Check the setting for progressing through the data set.

    \param nofIterations -- Number of iteration steps.
    \param nofBlocks     -- Number of blocks per iteration step.
    \param nofSamples    -- Number of samples per block of data.
   */
  void check (int& nofIterations,
	      int& nofBlocks,
	      int& nofSamples);
  
  /*!
    \brief Check the setting for progressing through the data set.

    \param nofIterations -- Number of iteration steps.
    \param nofBlocks     -- Number of blocks per iteration step.
    \param nofSamples    -- Number of samples per block of data.
    \param startOffset   -- Offset from which on to start progressing through
                            the data volume, in [samples].
   */
  void check (int& nofIterations,
	      int& nofBlocks,
	      int& nofSamples,
	      int& startOffset);

 private:
  
  //! Size the data file in samples.
  int filesize_p;

  //! Method by which the total accessed data volume is adjusted.
  DataBlockSettings::method method_p;

};

#endif
