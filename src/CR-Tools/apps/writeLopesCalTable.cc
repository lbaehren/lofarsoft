/***************************************************************************
 *   Copyright (C) 2008                                                    *
 *   Frank Schröder (<mail>)                                               *
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

#include <Calibration/CalTableWriter.h>
#include <Calibration/CalTableReader.h>
#include <string>

using CR::CalTableWriter;
using CR::CalTableReader;

/*!
  \file writeLopesCalTable.cc
  
  \ingroup apps

  \brief for writing LOPES-CalTables
 
  \author Frank Schröder
 
  \date 2008/05/15
    
  <h3>Motivation</h3>

  This application allows to write values to LOPES-Calibration-Tables.
  In this first version new values must be written into the source code.
  

  <h3>Usage</h3>
  \verbatim
  Change source code and recompile.
  Then go to the apps directory and type:
  ./writeLopesCalTable
  \endverbatim
*/

// define constants for the start date of LOPES 10 and LOPES 30
const unsigned int LOPES10_start = 978350400;
const unsigned int LOPES30_start = 1104580800;
// Feb. 2008 begin of 63.5 and 68.1 MHz Phase Reference signal
const unsigned int roof_setup_2_freq_start = 1202479000;	

// define antenna IDs
const int MAX_Antennas = 30;
const int antennaIDs[] = 
          { 10101, 10102, 10201, 10202, 20101, 20102, 20201, 20202, 30101, 30102,
            40101, 40102, 40201, 40202, 50101, 50102, 50201, 50202, 60101, 60102,
            70101, 70102, 70201, 70202, 80101, 80102, 80201, 80202, 90101, 90102 };

// CalTableWriter as global object (accessable to all functions)
CalTableWriter writer;
// Reader to access existing values
CalTableReader reader;

void writeRefPhases(void)
{
  // Set new reference frequencies
  Matrix<Double> PhaseRefFreqs(2,2);
  PhaseRefFreqs(0,0) = 63500000;
  PhaseRefFreqs(0,1) = 63500000;
  PhaseRefFreqs(1,0) = 68100000;
  PhaseRefFreqs(1,1) = 68100000;

  // Set reference Phases
  Matrix<Double> PhaseRefPhases(30,2);
  PhaseRefPhases(0,0)  = -132.	;	  PhaseRefPhases(0,1)  = 108.	;
  PhaseRefPhases(1,0)  = 105.4	;	  PhaseRefPhases(1,1)  = -79.83	;
  PhaseRefPhases(2,0)  = 158.5	;	  PhaseRefPhases(2,1)  = -53.56	;
  PhaseRefPhases(3,0)  = 0.	;	  PhaseRefPhases(3,1)  = 0.	;
  PhaseRefPhases(4,0)  = 46.22	;	  PhaseRefPhases(4,1)  = 166.7	;
  PhaseRefPhases(5,0)  = 75.92	;	  PhaseRefPhases(5,1)  = -150.2	;
  PhaseRefPhases(6,0)  = -150.8	;	  PhaseRefPhases(6,1)  = 28.63	;
  PhaseRefPhases(7,0)  = -1.523	;	  PhaseRefPhases(7,1)  = 143.	;
  PhaseRefPhases(8,0)  = -137.6	;	  PhaseRefPhases(8,1)  = 53.53	;
  PhaseRefPhases(9,0)  = 101.4	;	  PhaseRefPhases(9,1)  = 154.7	;
  PhaseRefPhases(10,0) = -116.2	;	  PhaseRefPhases(10,1) = 102.9	;
  PhaseRefPhases(11,0) = 61.83	;	  PhaseRefPhases(11,1) = 70.08	;
  PhaseRefPhases(12,0) = -51.19	;	  PhaseRefPhases(12,1) = -135.8	;
  PhaseRefPhases(13,0) = -2.295	;	  PhaseRefPhases(13,1) = -45.93	;
  PhaseRefPhases(14,0) = 164.8	;	  PhaseRefPhases(14,1) = 37.24	;
  PhaseRefPhases(15,0) = 41.63	;	  PhaseRefPhases(15,1) = 63.69	;
  PhaseRefPhases(16,0) = -179.6	;	  PhaseRefPhases(16,1) = 97.7	;
  PhaseRefPhases(17,0) = 12.72	;	  PhaseRefPhases(17,1) = 37.5	;
  PhaseRefPhases(18,0) = -132.9	;	  PhaseRefPhases(18,1) = -24.66	;
  PhaseRefPhases(19,0) = 124.8	;	  PhaseRefPhases(19,1) = -102.5	;
  PhaseRefPhases(20,0) = 97.1	;	  PhaseRefPhases(20,1) = -1.764	;
  PhaseRefPhases(21,0) = -58.16	;	  PhaseRefPhases(21,1) = 13.59	;
  PhaseRefPhases(22,0) = 37.91	;	  PhaseRefPhases(22,1) = -22.52	;
  PhaseRefPhases(23,0) = 75.2	;	  PhaseRefPhases(23,1) = 153.	;
  PhaseRefPhases(24,0) = 113.1	;	  PhaseRefPhases(24,1) = 177.2	;
  PhaseRefPhases(25,0) = -11.58	;	  PhaseRefPhases(25,1) = -13.73	;
  PhaseRefPhases(26,0) = 141.5	;	  PhaseRefPhases(26,1) = 125.	;
  PhaseRefPhases(27,0) = 132.	;	  PhaseRefPhases(27,1) = 139.2	;
  PhaseRefPhases(28,0) = 66.59	;	  PhaseRefPhases(28,1) = -26.2	;
  PhaseRefPhases(29,0) = -0.29	;	  PhaseRefPhases(29,1) = 152.6	;

  // Set Sample jumps
  Vector<Double> SampleJumps(4);
  SampleJumps(0) = -2;
  SampleJumps(1) = 2;
  SampleJumps(2) = -1;
  SampleJumps(3) = 1;

  // Add the value for all antennas
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Writing values for antenna: " << antennaIDs[i] << endl;

    // It is neccessary to write the Delay again, as the other fields
    // are junior fields an cannot be written alone
    Double old_delay = 0.;
    if (!reader.GetData(roof_setup_2_freq_start, antennaIDs[i], "Delay", &old_delay))
    { 
      cerr << "Error while reading field: Delay" << endl;
    } else
    {
      if (!writer.AddData(old_delay,antennaIDs[i],"Delay",roof_setup_2_freq_start) )
        cerr << "\nERROR while writing field: Delay" << endl;
      if (!writer.AddData(PhaseRefFreqs,antennaIDs[i],"PhaseRefFreqs",roof_setup_2_freq_start) )
        cerr << "\nERROR while writing field: PhaseRefFreqs" << endl;
      if (!writer.AddData(SampleJumps,antennaIDs[i],"SampleJumps",roof_setup_2_freq_start) )
        cerr << "\nERROR while writing field: SampleJumps" << endl;
      if (!writer.AddData(PhaseRefPhases.row(i),antennaIDs[i],"PhaseRefPhases",roof_setup_2_freq_start) )
        cerr << "\nERROR while writing field: PhaseRefPhases" << endl;
    }
  }
}

int main (int argc, char *argv[])
{
  // Default CalTable-Path
  const string CalTablePath="/home/schroeder/usg/data/lopes/LOPES-CalTable";
//const string CalTablePath="/home/schroeder/lopestools/lopescasa/data/LOPES/LOPES-CalTable";
  
  try
  {
    cout << "Starting writeLopesCalTable...\n"
         << "Make sure that you made the right changes to the source code.\n" << endl;
 
    if (!reader.AttachTable(CalTablePath.c_str()))
    {
      cerr << "AttachTable for reading failed!" << endl;
      return 1;
    };

    if (!writer.AttachTable(CalTablePath.c_str()))
    {
      cerr << "AttachTable for writing failed!" << endl;
      return 1;
    };

    cout << "Opened table for writing: " << endl;
    writer.PrintSummary();

    writeRefPhases();

    cout << "Writing finished: " << endl;
    writer.PrintSummary();
  } 
  catch (AipsError x) 
  {
      cerr << "writeLopesCalTable: " << x.getMesg() << endl;
      return 1;
  };

  return 0;
}
