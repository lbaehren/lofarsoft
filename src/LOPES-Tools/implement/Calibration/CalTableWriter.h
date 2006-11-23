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

/* $Id: CalTableWriter.h,v 1.4 2006/11/08 16:06:41 bahren Exp $*/

#ifndef CALTABLEWRITER_H
#define CALTABLEWRITER_H

// CASA header files
#include <tables/Tables/TableRow.h>
#include <tables/Tables/SubTabDesc.h>

// LOPES-Tools header files
#include <Calibration/CalTableReader.h>

using std::cout;
using std::cerr;
using std::endl;

using casa::AipsError;
using casa::Array;
using casa::ArrayColumn;
using casa::ArrayColumnDesc;
using casa::ColumnDesc;
using casa::ColumnsIndex;
using casa::Complex;
using casa::DComplex;
/* using casa::FieldDesc; */
using casa::IPosition;
/* using casa::KeywordName; */
using casa::RecordFieldPtr;
using casa::ROScalarColumn;
using casa::ScalarColumn;
using casa::String;
using casa::Table;
using casa::TableRow;
using casa::Vector;

namespace LOPES {
  
  /*!
    \class CalTableWriter
    
    \ingroup Calibration
    
    \brief Class that can add entries to a CalTable
    
    \author Andreas Horneffer
    
    \date 2006/01/18
    
    \test tCalTableWriter.cc
    
    <h3>Prerequisite</h3>
    
    <h3>Synopsis</h3>
    
    <h3>Example(s)</h3>
    
  */
  class CalTableWriter : public CalTableReader {
    
  public:
    
    // --------------------------------------------------------------- Construction
    
    /*!
      \brief Default constructor
    */
    CalTableWriter ();
    
    /*!
      \brief Copy constructor
      
      \param other -- A CalTableReader object from which to create this new one.
    */
    CalTableWriter (CalTableReader const& other);
    
    // ---------------------------------------------------------------- Destruction
    
    /*!
      \brief Destructor
    */
    ~CalTableWriter ();
    
    // ------------------------------------------------------------------ Operators
    
    // ----------------------------------------------------------------- Parameters
    
    // -------------------------------------------------------------------- Methods
    
    /*!
      \brief Attach the table file to the writer object
      
      \param tableFilename -- filename of the table to be edited.
    */
    virtual bool AttachTable(const String& tableFilename);
    
    
    /*!
      \brief Add a new value to an existing field
      
      \param data -- datum that is to be stored
      \param AntID -- ID of the antenna to which the data belongs 
      \param FieldName -- Name of the field that to which the data belongs
      \param startDate -- First date when this entry is valid
      \param stopDate=0 -- last date when this entry is valid or 0 if no end date 
      
      \return ok -- Was operation successful? Returns <tt>true</tt> if yes.
      
      Maybe not too elegant:
      The data is added as the last row, so no sorting by date is done.
      
    */
    bool AddData (String data,
		  int const AntID,
		  String const FieldName, 
		  unsigned int const startDate,
		  unsigned int const stopDate=0);
    bool AddData (double data,
		  int const AntID,
		  String const FieldName, 
		  unsigned int const startDate,
		  unsigned int const stopDate=0);
    bool AddData (DComplex data,
		  int const AntID,
		  String const FieldName, 
		  unsigned int const startDate,
		  unsigned int const stopDate=0);
    bool AddData (Array<double> data,
		  int const AntID,
		  String const FieldName, 
		  unsigned int const startDate,
		  unsigned int const stopDate=0);
    bool AddData (Array<DComplex> data,
		  int const AntID,
		  String const FieldName, 
		  unsigned int const startDate,
		  unsigned int const stopDate=0);
    
    /*!
      \brief Add a new column for a new field
      
      \param FieldName -- Name of the field that is added
      \param FieldDesc -- Human readable description of the field, 
      \param DataType -- Type of the data that is stored in this field
      \param shapeFixed=false -- If it is an array type: Is the shape fixed?
      \param shape=IPosition(0) -- Shape of the fixed shape array.
      \param SingleTables=false -- Put the data into seperate tables for each antenna
      \param FriendField="" --  Name of the senior friend field of the field. (Read the 
      (no yet) written document about friend fields.)
      
      \return ok -- Was operation successful? Returns <tt>true</tt> if yes.
      
      Does not fill the new entries with any data. So a call to <tt>AddColumn()</tt> 
      is usually followed by several calls to <tt>AddData()</tt> 
    */
    bool AddField (const String FieldName,
		   const String FieldDesc,
		   const String DataType, 
		   const bool shapeFixed=false,
		   const IPosition shape=IPosition::IPosition(),
		   const bool SingleTables=false,
		   const String FriendField="");
    
  /*!
    \brief Add a new antenna
    
    \param AntID -- ID of the new antenna. Must be unique!
    \param AntName -- Name of the new antenna.
    
    \return ok -- Was operation successful? Returns <tt>true</tt> if yes.

    Does not fill the new entries with any data. So a call to <tt>AddAntenna()</tt> 
    is usually followed by several calls to <tt>AddData()</tt> 
  */
  bool AddAntenna(int AntID,
		  String AntName);

  /*!
    \brief Sets the value of a keyword

    \param data -- datum that is to be stored
    \param KeywordName -- Name of the field that to which the data belongs

    \return ok -- Was operation successful? Returns <tt>true</tt> if yes.

    Sets the value of a keyword. If a keyword with this name already exists then 
    the old keyword is overwritten.
  */
  bool SetKeyword(String data,
		  String const KeywordName);
  bool SetKeyword(double data,
		  String const KeywordName);
  bool SetKeyword(DComplex data,
		  String const KeywordName);
  bool SetKeyword(Array<double> data,
		  String const KeywordName);
  bool SetKeyword(Array<DComplex> data,
		  String const KeywordName);

 private:

  /*!
    \brief Get the preceding and succeding row numbers and reset the stopDate

    \param AntID -- ID of the antenna.
    \param colTable -- column table with the data
    \param startDate -- startDate for the new entry
    \param *stopDate -- filled with the stopDate for the new entry, may be updated
    \param *preRow -- returns the row number of the preceding row or -1 if none
    \param *succRow -- returns the row number of the succeding row or -1 if none
    
    \return ok -- Was operation successful? Returns <tt>true</tt> if yes.
    
    Maybe not too elegant:
    The data is added as the last row, so no sorting by date is done.
    
  */
  bool GetPreSucRows(int const AntID,
		     Table const colTab,
		     unsigned int const startDate, 
		     unsigned int *stopDate,
		     int *preRow,
		     int *succRow);

};

}  // Namespace LOPES -- END

#endif /* _CALTABLEWRITER_H_ */
