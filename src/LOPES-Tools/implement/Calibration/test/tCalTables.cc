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

/* $Id: tCalTables.cc,v 1.4 2006/11/10 16:54:24 horneff Exp $*/

#include <lopes/Calibration/CalTableCreator.h>
#include <lopes/Calibration/CalTableWriter.h>
#include <lopes/Calibration/CalTableReader.h>
#include <tables/Tables/ExprNodeSet.h>
#include <casa/Utilities/ValType.h>


/*!
  \file tCalTableCreator.cc

  \brief A collection of test routines for CalTableCreator
 
  \author Andreas Horneffer
 
  \date 2006/01/18
*/

// -----------------------------------------------------------------------------

/*!
  \brief Test constructors for a new CalTableCreator object

  \return nofFailedTests -- The number of failed tests.
*/
Int test_CalTableCreator () {
  try {
    Bool ok;
    CalTableCreator maker;         
    cout << "CalTableCreator object created." << endl;
    ok = maker.newTable("testTable");
    cout << "testTable created:" << ok << endl;      
  } catch (AipsError x) {
      cerr << "Creator Test:" << x.getMesg() << endl;
      return 1;
  };
  return 0;
}

Int test_CalTableWriter () {
  Int i,j,failed=0;
  try {
    CalTableWriter writer;
    if (!writer.AttachTable("testTable")) {
      cout << "AttachTable failed!" << endl;
      failed++;
    };
    cout << "Start of CalTableWriter : ";
    writer.PrintSummary();
    if (!writer.AddAntenna(1,"Antenna number one!")) failed++;
    if (!writer.AddField("Position","The antenna position","Array<Double>",True,IPosition(1,3))) failed++;
    if (!writer.AddField("Gain","The electronics Gain","Array<DComplex>")) failed++;
    if (!writer.AddField("GainFreqAx","The frequency axis of the Gain","Array<Double>",False,IPosition(1,3),False,"Gain")) failed++;
    if (!writer.AddField("GainPosAx","The position axis of the Gain","Array<Double>",False,IPosition(1,3),False,"Gain")) failed++;
    Vector<Double> PosData(3);
    for (i=0;i<3;i++) { PosData(i) = i+1; };
    if (!writer.AddData(PosData,1,"Position",1000)) failed++;
    Matrix<DComplex> GainData(3,3);
    for (i=0;i<3;i++) {
      for (j=0;j<3;j++) {
	GainData(IPosition(2,i,j)) = DComplex(i+1,j+1);
      };
    };
    if (!writer.AddData(GainData,1,"Gain",1000)) failed++;
     for (i=0;i<3;i++) {
      for (j=0;j<3;j++) {
	GainData(IPosition(2,i,j)) = DComplex(i+1,j+1)*2.;
      };
    };   
    if (!writer.AddData(GainData,1,"Gain",2000)) failed++;
    if (!writer.AddData(PosData,1,"GainFreqAx",1000)) failed++;
    PosData = PosData*10.;
    if (!writer.AddData(PosData,1,"GainPosAx",1000)) failed++;
    cout << "Here has to follow an error message:" << endl;
    if (writer.AddData(PosData,1,"GainFreqAx",1500)) failed++;
    cout << "End of CalTableWriter : ";
    writer.PrintSummary();
  } catch (AipsError x) {
      cerr << "Writer Test:" << x.getMesg() << endl;
      return 1+failed;
  };
  return failed;
}

Int test_CalTableReader () {
  Int failed (0);
  try {
    CalTableReader reader;
    if (!reader.AttachTable("testTable")) {
      cout << "AttachTable failed!" << endl;
      failed++;
    };
    Matrix<DComplex> GainData;
    if (!reader.GetData(1001,1,"Gain",&GainData)) failed++;
    cout << "gain Data 1001" << GainData << endl;  
    Vector<Double> GainAx;
    if (!reader.GetData(1001,1,"GainFreqAx",&GainAx)) failed++;
    cout << "gain freq axis 1001" << GainAx << endl;
    if (!reader.GetData(2001,1,"GainFreqAx",&GainAx)) failed++;
    cout << "gain freq axis 2001" << GainAx << endl;
    if (!reader.GetData(1001,1,"GainPosAx",&GainAx)) failed++;
    cout << "gain Pos axis 2001" << GainAx << endl;
    cout << "gain 1001 1501 identical: " << reader.isIdentical(1001,1501,1,"Gain") << endl;
    cout << "gain 1001 2001 identical: " << reader.isIdentical(1001,2001,1,"Gain") << endl;
    cout << "gain axis 1001 2001 identical: " << reader.isIdentical(1001,2001,1,"GainFreqAx") << endl;
  } catch (AipsError x) {
      cerr << "Reader Test:" << x.getMesg() << endl;
      return 1+failed;
  };
  return failed;
}

Int test_stuff(){
  try {
    ColumnDesc newCol;
    newCol = ScalarColumnDesc<String>("Hurtz!","Das Hurtz Feld.");    
    TableDesc entryDesc("master",TableDesc::Scratch);
    entryDesc.addColumn(ScalarColumnDesc<Int>("StartDate", "First date when this entry is valid"));
    entryDesc.addColumn(ScalarColumnDesc<Int>("StopDate", "Last date when this entry is valid"));
    entryDesc.addColumn(ScalarColumnDesc<Int>("AntID", "ID of the antenna"));
    entryDesc.addColumn(ScalarColumnDesc<String>("String", "ID of the antenna"));
    entryDesc.addColumn(ScalarColumnDesc<Double>("Double", "ID of the antenna"));
    entryDesc.addColumn(ScalarColumnDesc<DComplex>("DComplex", "ID of the antenna"));
    entryDesc.addColumn(ArrayColumnDesc<Double>("Array<Double"));
    newCol = ArrayColumnDesc<Double>("Array<fixed>",IPosition(1,3),ColumnDesc::Direct);
    entryDesc.addColumn(newCol);
    SetupNewTable entriesMaker("dataTable",entryDesc,Table::Scratch);
    Table entries(entriesMaker);
    entries.addRow(6);
    Double tmpd,tmpd2;
    tmpd = 0./0.; //Set the variables to NaN
    tmpd2 = 0./0.;
    ScalarColumn<Int>(entries,"AntID").put(0,10);
    ScalarColumn<String>(entries,"String").put(1,"String");
    ScalarColumn<Double>(entries,"Double").put(2,ValType::undefDouble());
    ScalarColumn<DComplex>(entries,"DComplex").put(3,ValType::undefDouble());
    Array<Double> arr(IPosition(1,3),tmpd);
    ArrayColumn<Double>(entries,"Array<Double").put(4,arr);
    ArrayColumn<Double>(entries,"Array<fixed>").put(4,arr);
    Vector<uInt> rowNrs;
    Table tmptab;
    tmptab =entries(entries.col("AntID")>5);
    cout << tmptab << endl;
    rowNrs = tmptab.rowNumbers(entries); 
    cout << "AntID" << rowNrs << endl;
    rowNrs.resize();
    tmptab = entries(isdefined(entries.col("AntID")));
    cout << tmptab << endl;
    rowNrs = tmptab.rowNumbers(entries); 
    cout << "AntID" << rowNrs << endl;
    rowNrs.resize();
    rowNrs = entries(isdefined(entries.col("String"))).rowNumbers(entries); 
    cout << "String" << rowNrs << endl;
    rowNrs.resize();
    rowNrs = entries(isdefined(entries.col("Double"))).rowNumbers(entries); 
    cout << "Double" << rowNrs << endl;
    rowNrs.resize();
    rowNrs = entries( isdefined(entries.col("DComplex")) && isNaN(entries.col("DComplex")(TableExprNodeSet(IPosition(1,1)))) ).rowNumbers(entries);
    cout << "DComplex" << rowNrs << endl;
    rowNrs.resize();
    rowNrs = entries( isdefined(entries.col("Array<Double")) && isNaN(entries.col("Array<Double")(TableExprNodeSet(IPosition(1,1)))) ).rowNumbers(entries); 
    cout << "Array<Double>" << rowNrs << endl;
    rowNrs.resize();
    rowNrs = entries(isdefined(entries.col("Array<fixed>"))).rowNumbers(entries); 
    cout << "Array<fixed>" << rowNrs << endl;
    cout << "tmpd:" << tmpd << endl;
    cout << ArrayColumn<Double>(entries,"Array<fixed>")(4) << endl;
    rowNrs.resize();
    rowNrs = entries( isNaN(entries.col("Array<fixed>")(TableExprNodeSet(IPosition(1,1))))  ).rowNumbers(entries); 
    cout << "Array<fixed>" << rowNrs << endl;
    Bool tmpbool;
    tmpbool = isnan(tmpd);
    cout << tmpbool << endl;
    tmpbool = True;
    cout << tmpbool << endl;
    } catch (AipsError x) {
      cerr << "Stuff Test:" << x.getMesg() << endl;
      return 1;
  };
  return 0;
};


// -----------------------------------------------------------------------------

int main ()
{
  Int nofFailedTests (0);

  // Test for the constructor(s)
  {
    nofFailedTests += test_CalTableCreator ();
    nofFailedTests += test_CalTableWriter ();
    nofFailedTests += test_CalTableReader ();
  }

  cout << "Total number of failed tests: " << nofFailedTests << endl; 
  return nofFailedTests;
}
