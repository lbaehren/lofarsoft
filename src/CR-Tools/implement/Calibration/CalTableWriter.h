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

#ifndef _CALTABLEWRITER_H_
#define _CALTABLEWRITER_H_

#include <tables/Tables/TableRow.h>
#include <tables/Tables/SubTabDesc.h>

#include <Calibration/CalTableReader.h>

namespace CR {  //  Namespace CR -- begin
  
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
    virtual Bool AttachTable(const String& tableFilename);
    
    //@{    
    /*!
      \brief Add a new value to an existing field
      
      \param data -- datum that is to be stored
      \param AntID -- ID of the antenna to which the data belongs 
      \param FieldName -- Name of the field that to which the data belongs
      \param startDate -- First date when this entry is valid
      \param stopDate=0 -- last date when this entry is valid or 0 if no end date 
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
      
      Maybe not too elegant:
      The data is added as the last row, so no sorting by date is done.
      
    */
    Bool AddData(String data,
		 Int const AntID,
		 String const FieldName, 
		 uInt const startDate,
		 uInt const stopDate=0);
    Bool AddData(Double data,
		 Int const AntID,
		 String const FieldName, 
		 uInt const startDate,
		 uInt const stopDate=0);
    Bool AddData(DComplex data,
		 Int const AntID,
		 String const FieldName, 
		 uInt const startDate,
		 uInt const stopDate=0);
    Bool AddData(Array<Double> data,
		 Int const AntID,
		 String const FieldName, 
		 uInt const startDate,
		 uInt const stopDate=0);
    Bool AddData(Array<DComplex> data,
		 Int const AntID,
		 String const FieldName, 
		 uInt const startDate,
		 uInt const stopDate=0);
    //@}
    
    /*!
      \brief Add a new column for a new field
      
      \param FieldName -- Name of the field that is added
      \param FieldDesc -- Human readable description of the field, 
      \param DataType -- Type of the data that is stored in this field
      \param shapeFixed=False -- If it is an array type: Is the shape fixed?
      \param shape=IPosition(0) -- Shape of the fixed shape array.
      \param SingleTables=False -- Put the data into seperate tables for each antenna
      \param FriendField="" --  Name of the senior friend field of the field. (Read the 
      (no yet) written document about friend fields.)
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
      
      Does not fill the new entries with any data. So a call to <tt>AddColumn()</tt> 
      is usually followed by several calls to <tt>AddData()</tt> 
    */
    Bool AddField(const String FieldName,
		  const String FieldDesc,
		  const String DataType, 
		  const Bool shapeFixed=False,
		  const IPosition shape=IPosition::IPosition(),
		  const Bool SingleTables=False,
		  const String FriendField="");
    
    /*!
      \brief Add a new antenna
      
      \param AntID -- ID of the new antenna. Must be unique!
      \param AntName -- Name of the new antenna.
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
      
      Does not fill the new entries with any data. So a call to <tt>AddAntenna()</tt> 
      is usually followed by several calls to <tt>AddData()</tt> 
    */
    Bool AddAntenna(Int AntID,
		    String AntName);
    
    //@{
    /*!
      \brief Sets the value of a keyword
      
      \param data -- datum that is to be stored
      \param KeywordName -- Name of the field that to which the data belongs
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
      
      Sets the value of a keyword. If a keyword with this name already exists then 
      the old keyword is overwritten.
    */
    Bool SetKeyword(String data, String const KeywordName);
    Bool SetKeyword(Double data, String const KeywordName);
    Bool SetKeyword(DComplex data, String const KeywordName);
    Bool SetKeyword(Array<Double> data, String const KeywordName);
    Bool SetKeyword(Array<DComplex> data, String const KeywordName);
    //@}
    
  private:
    
    /*!
      \brief Get the preceding and succeding row numbers and reset the stopDate
      
      \param AntID -- ID of the antenna.
      \param colTable -- column table with the data
      \param startDate -- startDate for the new entry
      \param *stopDate -- filled with the stopDate for the new entry, may be updated
      \param *preRow -- returns the row number of the preceding row or -1 if none
      \param *succRow -- returns the row number of the succeding row or -1 if none
      
      \return ok -- Was operation successful? Returns <tt>True</tt> if yes.
      
      Maybe not too elegant:
      The data is added as the last row, so no sorting by date is done.
      
    */
    Bool GetPreSucRows(Int const AntID, Table const colTab, uInt const startDate, 
		       uInt *stopDate, Int *preRow, Int *succRow);
    
  };

}  // Namespace CR -- end
  
#endif /* _CALTABLEWRITER_H_ */
  
