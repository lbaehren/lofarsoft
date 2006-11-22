/***************************************************************************
 *   Copyright (C) 2006                                                    *
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

/* $Id: CalTableCreator.h,v 1.3 2006/11/08 16:06:41 bahren Exp $*/

#ifndef CALTABLECREATOR_H
#define CALTABLECREATOR_H

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/StManAipsIO.h>
#include <tables/Tables/SubTabDesc.h>
/* Don't know yet whether I need them: */
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/StandardStMan.h>
#include <tables/Tables/IncrementalStMan.h>

using std::cout;
using std::cerr;
using std::endl;

using casa::AipsError;
using casa::String;
using casa::ScalarColumnDesc;
using casa::SetupNewTable;
using casa::Table;
using casa::TableDesc;

namespace LOPES {  // Namespace LOPES -- BEGIN

/*!
  \class CalTableCreator

  \ingroup Calibration

  \brief Tool to create an empty calibration table

  \author Andreas Horneffer

  \date 2006/01/18

  \test tCalTableCreator.cc

  <h3>Prerequisite</h3>

  <h3>Synopsis</h3>

  <h3>Example(s)</h3>

  \code
  CalTableCreator create;
  create.newTable("empty_table");
  \endcode
*/

class CalTableCreator {
  
 private:
  
#define minDate 0
  //! 2106.02.07.06:28:15.000 end of the 32bit unsigned unix epoch...
#define maxDate 0xffffffff
  
  
 public:
  
  // --------------------------------------------------------------- Construction

  /*!
    \brief Default constructor
  */
  CalTableCreator ();

  /*!
    \brief Argumented constructor

    \param filename -- Name of the new table
  */
  CalTableCreator (String filename);


  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  ~CalTableCreator ();

  // ------------------------------------------------------------------ Operators


  // ----------------------------------------------------------------- Parameters

  // -------------------------------------------------------------------- Methods

  /*!
    \brief Create a new empty table

    \param filename -- Name of the new table

    \return  ok -- Was operation successful? Returns \c true if yes.
  */
  bool newTable(String filename);
};

}  // Namespace LOPES -- END

#endif /* _CALTABLECREATOR_H_ */
