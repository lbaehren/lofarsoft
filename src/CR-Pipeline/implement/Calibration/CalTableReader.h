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

/* $Id: CalTableReader.h,v 1.5 2007/01/15 14:41:09 horneff Exp $*/

#ifndef _CALTABLEREADER_H_
#define _CALTABLEREADER_H_


#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Containers/RecordField.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/ColumnsIndex.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/ExprNode.h>

/* Don't know yet whether I need them: */
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/StandardStMan.h>
#include <tables/Tables/IncrementalStMan.h>


#include <casa/namespace.h>

/*!
  \class CalTableReader

  \ingroup Calibration

  \brief Class to open a CalTable and access single datasets

  \author Andreas Horneffer

  \date 2006/01/17

  \test tCalTableReader.cc (Not implemented yet.)

  <h3>Prerequisite</h3>

  <ul type="square">
    <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/tables/implement/Tables.html">Tables</a> -- Tables are the fundamental storage mechanism for AIPS++
  </ul>

  <h3>Synopsis</h3>

  This is the class a normal computing task uses. Upon construction the table
  with the calibration data is opened and then the bits of data can be 
  extracted.

  <h3>Example(s)</h3>

  \code
  String tablename;
  CalTableWriter table;
  // open the table
  table.AttachTable(tablename);
  // define which value we are interested in
  String fieldName="AntennaPosition";
  Int AntennaID = 010101; //antenna 1 in LOPES notation
  uInt date = time(); //get the value that is valid today
  // get the data
  Vector<Double> Pos;
  if (!table.get(date, AntennaID, fieldName, &Pos)) {
    cerr << "Error while retrieving data" << endl;
  };
  cout << "The " << fieldName << " of antenna " << AntennaID <<
       " for the time label " << date << " is: " << Pos << endl;
   \endcode

*/

class CalTableReader {

 protected:

  // The AIPS++ table with the calibration data
  Table *masterTable_p;
  
  // Index over the AntID Column
  ColumnsIndex *AntIDIndex_p;
  // Pointer to the index value
  RecordFieldPtr<Int> *indexedAnt_p;

  /*!
    \brief Get the column table identified by AntID and FieldName
    
    \param AntId -- ID of the antenna for which the data is requested
    \param FieldName -- Name of the field that is requested
    \param *subtab -- return the table
    \param *rowNr -- return the row number of the antenna
    \param *DataType=NULL -- return the data type of the field
    \param *isJunior=NULL -- return whether this field has a senior friend
    
    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
  */
  Bool GetColumnTable(Int const AntID,
		      String const FieldName,
		      Table *subtab, 
		      Int *rowNr,
		      String *DataType=NULL,
		      Bool *isJunior=NULL);

  /*!
    \brief Get the row in the column table that corresponds to the date
    
    \param AntId -- ID of the antenna for which the data is requested
    \param colTable -- column table with the data
    \param date -- date for which the data is requested
    \param mindEmpty -- complain about empty result
    
    \return rowNr -- row number or -1 if row not found
  */
  Int GetDateRow(Int const AntID,
		 Table const colTable,
		 uInt const date,
		 const Bool mindEmpty=True);

  /*!
    \brief Get the row in the column table that corresponds to the date for a
           junior friend field
    
    \param AntId     -- ID of the antenna for which the data is requested
    \param colTable  -- column table with the data
    \param date      -- date for which the data is requested
    \param MaskField -- name of the mask field
        
    \return rowNr -- row number or -1 if row not found
  */
  Int GetJuniorDateRow(Int const AntID,
		       Table const colTable,
		       uInt const date,
		       String const MaskField,
		       const Bool mindEmpty=True);
  
public:
    
  // --------------------------------------------------------------- Construction
    
  /*!
    \brief Default constructor
    
    This constructor just creates a new object but does not do anything else.
   */
  CalTableReader ();
 
 /*!
    \brief Augmented constructor
    
    \param tableFilename -- filename of the table to be read.
  */
  CalTableReader (const String& tableFilename);

//  /*!
//    \brief Copy constructor
//
//    \param other -- Another CalTableReader object from which to create this new
//                    one.
//  */
//  CalTableReader (CalTableReader const& other);

  // ---------------------------------------------------------------- Destruction

  /*!
    \brief Destructor
  */
  virtual ~CalTableReader ();

  // ----------------------------------------------------------------- Parameters

  // -------------------------------------------------------------------- Methods

 /*!
    \brief Attach the table file to the reader object
    
    \param tableFilename -- filename of the table to be read.

    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
  */
  virtual Bool AttachTable(const String& tableFilename);

 /*!
    \brief Is this reader object attached to a table?
    
    \return  yes -- <tt>True</tt> if the object is attached.
 */
  virtual Bool isAttached();


  /*!
    \brief Get a piece of data from the table

    \param date -- date for which the data is requested
    \param AntID -- ID of the antenna for which the data is requested
    \param FieldName -- Name of the field that is requested
    \param *result -- Place where the returned data can be stored

    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.

    The methods to get the data. I expect this to be called a lot.

  */
  Bool GetData(uInt const date,
	       Int const AntID,
	       String const FieldName,
	       String *result);
  Bool GetData(uInt const date,
	       Int const AntID,
	       String const FieldName,
	       Double *result);
  Bool GetData(uInt const date,
	       Int const AntID,
	       String const FieldName,
	       DComplex *result);
  Bool GetData(uInt const date,
	       Int const AntID,
	       String const FieldName,
	       Array<Double> *result);
  Bool GetData(uInt const date,
	       Int const AntID,
	       String const FieldName,
	       Array<DComplex> *result);

  /*!
    \brief Get the value of a keyword
    
    \param KeywordName -- Name of the field that is requested
    \param *result -- Place where the returned data can be stored

    \return ok -- Was operation successful? Returns <tt>True</tt> if yes.

    The methods to retrieve the data that was stored in a keyword. If the types 
    mismatch then type promotion of scalars will be done if possible. If not 
    possible, the function returns false.
  */
  Bool GetKeyword(String const KeywordName, String *result);
  Bool GetKeyword(String const KeywordName, Double *result);
  Bool GetKeyword(String const KeywordName, DComplex *result);
  Bool GetKeyword(String const KeywordName, Array<Double> *result);
  Bool GetKeyword(String const KeywordName, Array<DComplex> *result);

  /*!
    \brief Get the name of the data type of the keyword
    
    \param KeywordName -- Name of the keyword that is requested
    
    \return TypeName -- the name of the data type
  */
  String GetKeywordType(String const KeywordName);

  /*!
    \brief Has the value of this field changed between the two given dates?
    
    \param date1 -- first date
    \param date2 -- second date 
    \param AntID -- ID of the antenna for which the data is requested
    \param FieldName -- Name of the field that is requested

    \return yes -- True if both dates point to the same entry, False otherwise 

    This method checks whether the two given dates point to the same entry in
    the calibration table. (I.e. to the same row in the column table.) Useful
    e.g. to check whether one has to recompute calibration values.
  */
  Bool isIdentical(uInt const date1,
		   uInt const date2,
		   Int const AntID,
		   String const FieldName);
  
  /*!
    \brief Get the name of the data type of the field
    
    \param FieldName -- Name of the field that is requested
    
    \return TypeName -- the name of the data type
  */
  String GetFieldType(String const FieldName);


  /*!
    \brief Print the summary (number of rows and columns) of the master table
    
    \return True
  */
  Bool PrintSummary();
 
private:
  
  Bool init();
  Bool cleanup();
};

#endif /* _CALTABLEREADER_H_ */
