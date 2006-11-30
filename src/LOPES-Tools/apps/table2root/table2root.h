
/* $Id: table2root.h,v 1.1 2005/07/13 09:46:57 bahren Exp $ */

#include <root/TObject.h>
#include <root/TClonesArray.h>

/*!
  \file table2root.h

  \brief Conversion of an AIPS++ table to a ROOT file.

  \author Maaike Mevius, Lars B&auml;hren

  \date 2005

  <h3>Synopsis</h3>

  \verbatim
  table2root 
  \endverbatim
  After running the conversion program you can inspect the created ROOT file; for
  this start up ROOT
  \verbatim
  root [0] TFile f1 ("3C343.MS.root")
  root [1] TBrowser hg
  root [2] <TCanvas::MakeDefCanvas>: created default TCanvas with name c1
  \endverbatim
*/

typedef struct {
  Int_t arrayID;
  Double_t U;
  Double_t V;
  Double_t W;
  Double_t time;
} AntennaPair;

static char naam[53]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz";

void dummy(int);

void inittree(const char* tableName);

/*!
  \brief Open AIPS++ table for reading.

  \param tableName -- Name of the table from which the data are read.
*/
void openTable (const char* tableName);
