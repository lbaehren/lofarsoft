//# GlishRecordExpr.h: Filter GlishRecords using a TaQL expression
//# Copyright (C) 2000,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: GlishRecordExpr.h,v 19.4 2004/11/30 17:51:10 ddebonis Exp $

#ifndef TASKING_GLISHRECORDEXPR_H
#define TASKING_GLISHRECORDEXPR_H

//# Includes
#include <tables/Tables/TableExprData.h>
#include <casa/Containers/Record.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations
class TableExprNode;
class GlishRecord;
class GlishArray;


// <summary>
// Filter GlishRecords using a TaQL expression.
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tGlishRecordExpr">
// </reviewed>

// <prerequisite>
//   <li> <linkto class="TableExprData">TableExprData</linkto>.
//   <li> <linkto class="GlishRecord">GlishRecord</linkto>.
// </prerequisite>

// <synopsis>
// This class builds a TaQL expression tree from an expression string
// and a prototype <linkto class=GlishRecord>GlishRecord</linkto>.
// See <a href=../../../notes/199/199.html>note 199</a> for a full
// description of TaQL.
// Thereafter the <src>matches</src> function can be used to test
// if a given GlishRecord matches the expression.
// This function can repreatedly be called for a series of records.
// <p>
// If the record contains subrecords, it is possible to use fields
// in those subrecords (or subsubrecords, etc.) by specifying all
// parts of the full field name separated by dots (as shown in the example).
// Array fields can also be handled in the expression.
// <p>
// The GlishRecord given to the <src>matches</src> function must in
// principle have the same structure as the prototype record, otherwise
// bad things (like an exception, or even worse a segmentation fault)
// can happen when evaulating the expression, because checking would
// take too much time.
// However, by using the TaQL function <src>isdefined</src> one
// can explicitly check if a field is defined and has the data type
// as defined in the prototype record.
// </synopsis>

// <example>
// In the following example a block of GlishRecords is matched
// against an expression.
// <srcblock>
// void someFunc (const Block<GlishRecord>& recs)
// {
//   AlwaysAssert (recs.nelements() > 0, AipsError);
//   GlishRecordExpr expr("ant_data.ON_SOURCE", recs[0]);
//   for (uInt i=0; i<recs.nelements(); i++) {
//     cout << expr.matches (recs[i]) << endl;
//   }
// }
// </srcblock>
// The following example is the same as above, but uses the isdefined
// function in the expression to make sure no unexpected things
// can happen.
// <srcblock>
// void someFunc (const Block<GlishRecord>& recs)
// {
//   AlwaysAssert (recs.nelements() > 0, AipsError);
//   GlishRecordExpr expr
//       ("isdefined(ant_data.ON_SOURCE) && ant_data.ON_SOURCE", recs[0]);
//   for (uInt i=0; i<recs.nelements(); i++) {
//     cout << expr.matches (recs[i]) << endl;
//   }
// }
// </srcblock>
// </example>

class GlishRecordExpr : public TableExprData
{
public:
  // Construct it from an expression which gets parsed.
  // The proto GlishRecord is used to get a description of the records
  // to be compared with the expression.
  GlishRecordExpr (const GlishRecord& proto, const String& expr);

  // Construct it from an already created expression tree.
  // The proto Record should be the same as the one used to create
  // the expression (in the
  // <linkto group="RecordExpr:h#RecordExpr>makeRecordExpr</linkto> function).
  GlishRecordExpr (const Record& proto, const TableExprNode& expr);

  // Construct it from an expression which gets parsed.
  // The proto Record is used to get a description of the records
  // to be compared with the expression.
  GlishRecordExpr (const Record& proto, const String& expr);

  // Copy constructor (copy semantics).
  GlishRecordExpr (const GlishRecordExpr&);

  virtual ~GlishRecordExpr();

  // Assignment (copy semantics).
  GlishRecordExpr& operator= (const GlishRecordExpr&);

  // Does this record match the expression?
  // The record must have the same description as the proto record
  // used when constructing the object.
  // <note role=caution>
  // It is not checked if the description is the same, because it
  // is too time-consuming.
  // </note>
  Bool matches (const GlishRecord& record);

  // Get a scalar in the given type.
  // <group>
  virtual Bool     getBool     (const Block<Int>& fieldNrs) const;
  virtual Double   getDouble   (const Block<Int>& fieldNrs) const;
  virtual DComplex getDComplex (const Block<Int>& fieldNrs) const;
  virtual String   getString   (const Block<Int>& fieldNrs) const;
  virtual Array<Bool>     getArrayBool     (const Block<Int>& fieldNrs) const;
  virtual Array<Double>   getArrayDouble   (const Block<Int>& fieldNrs) const;
  virtual Array<DComplex> getArrayDComplex (const Block<Int>& fieldNrs) const;
  virtual Array<String>   getArrayString   (const Block<Int>& fieldNrs) const;
  // </group>

  // Get the shape of the of the given (array) field.
  virtual IPosition shape (const Block<Int>& fieldNrs) const;

  // Get the data type of the given field.
  virtual DataType dataType (const Block<Int>& fieldNrs) const;

private:
  // Get the value of the given field in the subject GlishRecord.
  GlishArray getValue (const Block<Int>& fieldNrs) const;


  TableExprNode*     itsExpr;
  Record             itsRecord;
  const GlishRecord* itsSubject;
};



} //# NAMESPACE CASA - END

#endif
