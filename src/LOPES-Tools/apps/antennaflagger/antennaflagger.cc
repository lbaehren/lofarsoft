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

/* $Id: antennaflagger.cc,v 1.15 2006/10/31 18:33:49 bahren Exp $ */

#include <iomanip>

// ROOT includes
// #include <root/TString.h>
// #include <root/TClass.h>
// #include <root/TFile.h>
// #include <root/TTree.h>
// #include <root/TH2F.h>
// #include <root/TGraph2D.h>

// AIPS++ includes
#include <casa/iostream.h>
#include <casa/stdio.h>
#include <casa/Arrays.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/BasicSL/String.h>
#include <casa/Exceptions/Error.h>
#include <Glish/Client.h>

// LOPES includes 
#include <lopes/Data/ITSCapture.h>
#include <lopes/IO/DataReader.h>
#include <lopes/IO/DataReaderTools.h>
#include <lopes/Analysis/ClippingFraction.h>
#include <lopes/Utilities/Masking.h>
#include <lopes/Utilities/ProgressBar.h>
#include <lopes/Utilities/StringTools.h>
#include <antennaflagger.h>

// --- global variables --------------------------------------------------------

ClientData *clientData = new ClientData ();

ClippingFraction<Float> clip;

// --- main function -----------------------------------------------------------

int main (int argc, char **argv)
{
  try {
    // Set up the Glish event stream
    GlishSysEventSource glishStream (argc, argv);
    glishStream.addTarget (init_event, "init");
    glishStream.addTarget (flag_event, "flag");
    glishStream.addTarget (stats_event, "stats");
    glishStream.loop();
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }

  return 0;
}

// --- Initialize the flagger --------------------------------------------------

Bool init_event (GlishSysEvent &event, void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishValue glishVal = event.val();

  // Check that argument is a record.
  if (glishVal.type() != GlishValue::RECORD) {
    glishBus->reply(GlishArray(False));
    return False;
  }

  GlishRecord glishRec = glishVal;
  GlishArray gtmp;
  
  //------------------------------------------------------------------
  // Extract parameters for DataReader object
  try {
    Int block;
    Int blocksize;
    Vector<String> datafiles;
    //
    gtmp = glishRec.get("Files");
    gtmp.get(datafiles);
    gtmp = glishRec.get("blocksize");
    gtmp.get(blocksize);
    gtmp = glishRec.get("block");
    gtmp.get(block);
    //
    cout << "blocksize=" << blocksize
	 << ", # files=" << datafiles.nelements()
	 << endl;
    //
    clientData->dr = DataReaderTools::assignFromRecord (glishRec,true);
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }
    
  //------------------------------------------------------------------
  // Extract parameters for ClippingFraction object
  try {
    Vector<Float> clippingLimits;
    Float clippingFraction;
    //
    gtmp = glishRec.get("clippingLimits");
    gtmp.get(clippingLimits);
    gtmp = glishRec.get("clippingFraction");
    gtmp.get(clippingFraction);
    //
    ClippingFraction<Float> cf (clippingLimits,clippingFraction);
    clip = cf;
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }

  //------------------------------------------------------------------
  // Extract global parameters
  try {
    gtmp = glishRec.get("nofBlocks");
    gtmp.get(clientData->nofBlocks);
    gtmp = glishRec.get("threshold");
    gtmp.get(clientData->threshold);
    //
  } catch (AipsError x) {
    cerr << x.getMesg() << endl;
  }  

  //------------------------------------------------------------------
  // Set additional global parameter
  clientData->nofAntennas = clientData->dr->nofAntennas();
  //
  clientData->antennaFlags.resize(clientData->nofAntennas);
  clientData->antennaFlags = True;
  //
  clientData->nofTimesFlagged.resize(clientData->nofAntennas);
  clientData->nofTimesFlagged = 0;

  //------------------------------------------------------------------
  // Report computing parameters

  cout << "[antennaflagger] Settings" << endl;
  cout << " - Number of blocks analyzed : " << clientData->nofBlocks << endl;
  cout << " - Block size [samples] .... : " << clientData->dr->blocksize() << endl;
  cout << " - Clipping limits ......... : " << clip.limits() << endl;
  cout << " - Clipping fraction ....... : " << clip.clipFraction() << endl;
  cout << " - Flagging threshold ...... : " << clientData->threshold << endl;

  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(True));
  } else {
    cerr << "[antennaflagger] Unable to report status." << endl;
    return False;
  }
  
  return True;
}

// --- Run the flagger ---------------------------------------------------------

Bool flag_event (GlishSysEvent &event, void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishValue glishVal = event.val();
  Bool ok (True);

  ok = flag_func ();

  if (glishBus->replyPending()) {
    glishBus->reply(GlishArray(ok));
  } else {
    cerr << "[antennaflagger] Unable to report status." << endl;
    return False;
  }
  
  return True;
}

// --- Retrieve the generated statistics ---------------------------------------

Bool stats_event (GlishSysEvent &event, void *)
{
  GlishSysEventSource *glishBus = event.glishSource();
  GlishValue glishVal = event.val();
  Bool ok (True);

  GlishRecord rec;

  rec.add ("antennaFlags",clientData->antennaFlags);
  rec.add ("nofTimesFlagged",clientData->nofTimesFlagged);

  if (glishBus->replyPending()) {
    glishBus->reply(rec);
  } else {
    cerr << "[antennaflagger] Unable to report status." << endl;
    return False;
  }

  return ok;
}

// --- Analyze the data --------------------------------------------------------

Bool flag_func ()
{
  int antenna (0);
  int nofFlagsSet (0);
  Matrix<Double> data (clientData->dr->blocksize(),
		      clientData->dr->nofSelectedAntennas());
  Double mean_block (0.0);
  Vector<Double> mean_antenna (clientData->nofAntennas);

  clientData->nofTimesFlagged = 0;

  LOPES::ProgressBar bar (clientData->nofBlocks);

  cout << "[antennaflagger] Scanning data ..." << endl;

  for (int block (0); block<clientData->nofBlocks; block++) {

    nofFlagsSet = 0;
    mean_block  = 0.0;

    // --- read one block of data
    data = clientData->dr->fx();

    // --- extract statistics
    for (antenna=0; antenna<clientData->nofAntennas; antenna++) {
      mean_antenna(antenna) = mean(abs(data.column(antenna)));
    }
    mean_block = mean(mean_antenna);

    // --- check statistics
    for (antenna=0; antenna<clientData->nofAntennas; antenna++) {
      if (clientData->threshold*mean_antenna(antenna) < mean_block) {
	clientData->nofTimesFlagged(antenna)++;
	nofFlagsSet++;
      }
      else if (clientData->threshold*mean_block < mean_antenna(antenna)) {
	clientData->nofTimesFlagged(antenna)++;
	nofFlagsSet++;
      }
    }

    bar.update(block);
    // forward pointer to next block of data
    clientData->dr->nextBlock();
  }

  // --- wrap up the results form the parsing of the data blocks
  for (antenna=0; antenna<clientData->nofAntennas; antenna++) {
    if (clientData->nofTimesFlagged(antenna) > 0) {
      clientData->antennaFlags(antenna) = False;
    }
  }

  bar.update(clientData->nofBlocks);
  
  return True;
}
