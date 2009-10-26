/***************************************************************************
 *   Copyright (C) 2006                                                  *
 *   Andreas Horneffer (<mail>)                                                     *
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

/* $Id$*/

#include <Calibration/CalTableCreator.h>

// ==============================================================================
//
//  Construction
//
// ==============================================================================

CalTableCreator::CalTableCreator ()
{;}

CalTableCreator::CalTableCreator (String filename)
{
  Bool status (true);

  status = newTable (filename);

  if (!status) {
    cerr << "[CalTableCreator] Error when trying to create a new table" << endl;
  }
}

// ==============================================================================
//
//  Destruction
//
// ==============================================================================

CalTableCreator::~CalTableCreator ()
{
  ;
}

// ==============================================================================
//
//  Operators
//
// ==============================================================================


// ==============================================================================
//
//  Parameters
//
// ==============================================================================



// ==============================================================================
//
//  Methods
//
// ==============================================================================


Bool CalTableCreator::newTable(String filename)
{
  try {
    //// Create the table description for the empty entries table
    //    TableDesc entryDesc("master",TableDesc::Scratch);
    //
    //    entryDesc.addColumn(ScalarColumnDesc<Int>("StartDate", "First date when this entry is valid"));
    //    entryDesc.addColumn(ScalarColumnDesc<Int>("StopDate", "Last date when this entry is valid"));

       
// Create the table description for the master table
    TableDesc masterDesc("master",TableDesc::Scratch);
    
    masterDesc.addColumn(ScalarColumnDesc<Int>("AntID", "ID of the antenna"));
    masterDesc.addColumn(ScalarColumnDesc<String>("AntName", "Name of the Antenna (e.g. the ID as string)"));
    
//Create the master table
    SetupNewTable masterMaker(filename,masterDesc,Table::NewNoReplace);
    Table master(masterMaker);
    
    ////Create an empty entries table
    //    String tmpstring;
    //    tmpstring = filename +"/entries";
    //    SetupNewTable entriesMaker(tmpstring,entryDesc,Table::NewNoReplace);
    //    Table entries(entriesMaker);
    //    
    ////Add the empty entries table as a keyword
    //    master.rwKeywordSet().defineTable("emptyEntry",Table(entries));

//Add some default keywords
    master.rwKeywordSet().define("minDate",minDate);
    master.rwKeywordSet().define("maxDate",maxDate);

//Add a stupid default keyword //to be deleted...
    master.rwKeywordSet().define("Observatory","LOPES");

    cout << "endian:" << master.endianFormat() << " type:" << master.tableType() << endl;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
    return False;
  };
  
  return True;
}

