/***************************************************************************
 *   Copyright (C) 2005                                                    *
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

/* $Id: table2root.cc,v 1.4 2006/02/07 18:18:30 bahren Exp $ */

#include <root/TString.h>
#include <root/TClass.h>
#include <root/TFile.h>
#include <root/TTree.h>

#include <casa/aips.h>
#include <casa/iostream.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/BasicSL/Complex.h>
//
#include <tables/Tables/Table.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ScaRecordColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/StandardStMan.h>
#include <tables/Tables/IncrementalStMan.h>
#include <tables/Tables/ExprNode.h>

#include <table2root.h>

#include <casa/namespace.h>
    
/*!
  \file table2root.cc

  \brief Conversion of an AIPS++ table to a ROOT file.

  \author Maaike Mevius, Lars B&auml;hren

  \date 2005

  <h3>Usage</h3>

  \verbatim
  table2root <ms>
  \endverbatim
  where <tt>ms</tt> is an AIPS++ MeasurementSet.
*/

// --- Global variables --------------------------------------------------------

TFile *hfile;
TTree *tree;
AntennaPair *p12; 

void dummy(int dom)
{
  if(dom>52 || dom<=0) {
    cout << "dombo" << endl;
  } else { 
    cout << naam[dom-1] << endl;  
  }
}

// --- Open AIPS++ table for reading -------------------------------------------

void openTable (const char* tableName)
{
  inittree(tableName);
  p12 = new AntennaPair();
  cout << "addres p12 " << p12 << " " << tree << endl;
  char rootfilename[100];
  sprintf(rootfilename,"%s.root",tableName);

  hfile = new TFile(rootfilename,"RECREATE","testtable");
  hfile->cd();
  
  tree = new TTree("tree","pairinfo");
  tree->Branch("AntennaPair1",p12,"U/D:V:W:arrayID:time");
  tree->Branch("AntennaPair2",p12,"U/D:V:W:arrayID:time");

  // -----------------------------------------------------------------
  // Open AIPS++ table for reading

  cout << " - Opening input table ..." << endl;  
  casa::Table tab (tableName);

  cout << " - Opening table selection ... " << endl;
  casa::Table result = tab (tab.col("ANTENNA1") == 1 && tab.col("ANTENNA2") == 2);
 
  cout << " - Opening column ARRAY_ID ... " << endl;
  casa::ROScalarColumn<Int> arrayIDCol (result, "ARRAY_ID");
//   cout << " - Opening column TIME(UTC) ... " << endl;
//   casa::ROScalarColumn<Double> timeCol (result, "TIME(UTC)");
//   cout << " - Opening column UVW(ITRF) ... " << endl;
//   casa::ROArrayColumn<Float> uvwCol (result, "UVW(ITRF)");
//   cout << "addres p12 now " << p12 << " " << tree << endl;

  for (unsigned int i=0; i<result.nrow(); i++) {
//     casa::Vector<Double> array = uvwCol(i);
//     p12->time = timeCol(i);
     p12->arrayID = arrayIDCol(i);
     p12->time = 1.;
//     p12.U=array[0];
//     p12.V=array[1];
//     p12.W=array[2];
    p12->U=1.;
    p12->V=2.;
    p12->W=3.;
    
    tree->Fill();

    // Feedback
    cout<<"Filling ArrayID "<< p12->arrayID<<endl;
    tree->Print();
  }

  delete p12;
  hfile->cd();
  hfile->Write();
  hfile->Close();

  // Summary

  cout << "\n[table2root] Summary\n" << endl;
  cout << " - Original AIPS++ table  : " << tab;
  cout << " - AIPS++ table selection : " << result;
  cout << " - Created ROOT file .... : " << rootfilename << endl;
  
}

// --- Initialize ROOT Tree object ---------------------------------------------

void inittree (const char* tableName)
{
  p12 = new AntennaPair();
  cout << "addres p12 " << p12 << " " << tree << endl;
  char rootfilename[100];
  sprintf(rootfilename,"%s.root",tableName);

  hfile = new TFile(rootfilename,"RECREATE","testtable");
  hfile->cd();
  cout<<"opened rootfile "<<rootfilename<<endl;
  
  tree = new TTree("tree","pairinfo");

}

// --- Main function -----------------------------------------------------------

int main (int argc, const char** argv)
{

  if (argc < 2) {
    cerr << "[table2root] Too few parameters." << endl;
    return 1;
  } else {
    const char* tableName = argv[1];
    //
    openTable (tableName);
    //
    return 0;
  }
}
