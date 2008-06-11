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
 
  \date 2008/05/29
    
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
// date conversion can be done with:
// date -u -d '1970-01-01 978350400 seconds'
const unsigned int LOPES10_start = 978350400; // Mo Jan  1 12:00:00 UTC 2001
const unsigned int LOPES30_start = 1104580800; //Sa Jan  1 12:00:00 UTC 2005 
// Dual polarization setup
const unsigned int LOPES_pol_start =  1165574694; // Fr Dez  8 10:44:54 UTC 2006
// Delay measurement of 26. / 27.04.2007
const unsigned int delay_apr_07_start = 1158142580; // Mi Sep 13 10:16:20 UTC 2006
// Delay measurement of 04.09.2007
const unsigned int delay_sep_07_start = 1184241600; // Do Jul 12 12:00:00 UTC 2007
// Delay measurement of 27.11.2007
const unsigned int delay_nov_07_start = 1189171000; //Fr Sep  7 13:16:40 UTC 2007 

// Start of 63.5 and 68.1 MHz Phase Reference signal
const unsigned int roof_setup_2_freq_start = 1202479000; // Fr Feb  8 13:56:40 UTC 2008

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

void adjust_height_of_ant_14(void)
{
  // GPS height measurement was wrong, set new height to -0.2 m
  // x and y position did not change
  Vector<Double> position_14(3);
  position_14(0) = -52.069;
  position_14(1) = -54.610;
  position_14(2) = -0.2;
  
  
  cout << "Writing correct height for antenna: " << 40202 << endl;

  if (!writer.AddData(position_14,40202,"Position",LOPES30_start) )
    cerr << "\nERROR while writing field: Position" << endl;
  if (!writer.AddData(position_14,40202,"Position",LOPES_pol_start) )
    cerr << "\nERROR while writing field: Position" << endl;
}

// rotate Antenna Modell for dual polarisation setup
void rotate_antenna_model(void)
{
  Vector<bool> rotate(30);

  // find antennas which must be rotatet (all NS polarized antennas)
  // read in values, rotate them if neccessary and write them back as new values
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Reading antenna polaristaion for: " << antennaIDs[i] << endl;
    
    String polarization;
    if (!reader.GetData(LOPES_pol_start, antennaIDs[i], "Polarization", &polarization))
    { 
      cerr << "Error while reading field: Polarization" << endl;
      return;
    }
    // set rotation flag to true, if antenna is NS polarized
    if (polarization == "NS") rotate(i) = true;
      else rotate(i) = false; 
    
    
    // read in the AntennaGainFaktor
    Cube<Double> oldAntennaGainFaktor;  
     if (!reader.GetData(LOPES30_start, antennaIDs[i], "AntennaGainFaktor", &oldAntennaGainFaktor))
    { 
      cerr << "Error while reading field: AntennaGainFaktor" << endl;
      return;
    }
    Cube<Double> newAntennaGainFaktor = oldAntennaGainFaktor.copy();  
    // rotate AntennaGainFaktor by -90°
    if ( rotate(i) )
    {
      cout << "Rotating AntennaGainFaktor by -90° for antenna: " << i+1 << endl;
      // shift values between 90° and 360° by -90° (fields 9 - 36 --> fields 0 - 27)
      for (int j = 0 ; j < 27; j++) newAntennaGainFaktor.xzPlane(j) = oldAntennaGainFaktor.xzPlane(j+9);
      // values from 270° - 360° = values from 10° - 90°
      for (int j = 27; j < 37; j++) newAntennaGainFaktor.xzPlane(j) = oldAntennaGainFaktor.xzPlane(j-27);
    } 
    // write the data back as new values
    if (!writer.AddData(newAntennaGainFaktor, antennaIDs[i],"AntennaGainFaktor",LOPES_pol_start) )
      cerr << "\nERROR while writing field: AntennaGainFaktor" << endl;  
    
    
    // read in the AntennaAziGain
    Cube<Double> oldAntennaAziGain;  
     if (!reader.GetData(LOPES30_start, antennaIDs[i], "AntennaAziGain", &oldAntennaAziGain))
    { 
      cerr << "Error while reading field: AntennaAziGain" << endl;
      return;
    }
    Cube<Double> newAntennaAziGain = oldAntennaAziGain.copy();  
    // rotate AntennaAziGain by -90°
    if ( rotate(i) )
    {
      // shift values between 90° and 360° by -90° (fields 9 - 36 --> fields 0 - 27)
      for (int j = 0 ; j < 27; j++) newAntennaAziGain.xzPlane(j) = oldAntennaAziGain.xzPlane(j+9);
      // values from 270° - 360° = values from 10° - 90°
      for (int j = 27; j < 37; j++) newAntennaAziGain.xzPlane(j) = oldAntennaAziGain.xzPlane(j-27);
    } 
    // write the data back as new values
    if (!writer.AddData(newAntennaAziGain, antennaIDs[i],"AntennaAziGain",LOPES_pol_start) )
      cerr << "\nERROR while writing field: AntennaAziGain" << endl;  
    
    
    // read in the AntennaZeniGain
    Cube<Double> oldAntennaZeniGain;  
     if (!reader.GetData(LOPES30_start, antennaIDs[i], "AntennaZeniGain", &oldAntennaZeniGain))
    { 
      cerr << "Error while reading field: AntennaZeniGain" << endl;
      return;
    }
    Cube<Double> newAntennaZeniGain = oldAntennaZeniGain.copy();  
    // rotate AntennaZeniGain by -90°
    if ( rotate(i) )
    {
      // shift values between 90° and 360° by -90° (fields 9 - 36 --> fields 0 - 27)
      for (int j = 0 ; j < 27; j++) newAntennaZeniGain.xzPlane(j) = oldAntennaZeniGain.xzPlane(j+9);
      // values from 270° - 360° = values from 10° - 90°
      for (int j = 27; j < 37; j++) newAntennaZeniGain.xzPlane(j) = oldAntennaZeniGain.xzPlane(j-27);
    } 
    // write the data back as new values
    if (!writer.AddData(newAntennaZeniGain, antennaIDs[i],"AntennaZeniGain",LOPES_pol_start) )
      cerr << "\nERROR while writing field: AntennaZeniGain" << endl;  
    
    
    // read in the AntennaAziPhase
    Cube<Double> oldAntennaAziPhase;  
     if (!reader.GetData(LOPES30_start, antennaIDs[i], "AntennaAziPhase", &oldAntennaAziPhase))
    { 
      cerr << "Error while reading field: AntennaAziPhase" << endl;
      return;
    }
    Cube<Double> newAntennaAziPhase = oldAntennaAziPhase.copy();  
    // rotate AntennaAziPhase by -90°
    if ( rotate(i) )
    {
      // shift values between 90° and 360° by -90° (fields 9 - 36 --> fields 0 - 27)
      for (int j = 0 ; j < 27; j++) newAntennaAziPhase.xzPlane(j) = oldAntennaAziPhase.xzPlane(j+9);
      // values from 270° - 360° = values from 10° - 90°
      for (int j = 27; j < 37; j++) newAntennaAziPhase.xzPlane(j) = oldAntennaAziPhase.xzPlane(j-27);
    } 
    // write the data back as new values
    if (!writer.AddData(newAntennaAziPhase, antennaIDs[i],"AntennaAziPhase",LOPES_pol_start) )
      cerr << "\nERROR while writing field: AntennaAziPhase" << endl;  
    
    
    // read in the AntennaZeniPhase
    Cube<Double> oldAntennaZeniPhase;  
     if (!reader.GetData(LOPES30_start, antennaIDs[i], "AntennaZeniPhase", &oldAntennaZeniPhase))
    { 
      cerr << "Error while reading field: AntennaZeniPhase" << endl;
      return;
    }
    Cube<Double> newAntennaZeniPhase = oldAntennaZeniPhase.copy();  
    // rotate AntennaZeniPhase by -90°
    if ( rotate(i) )
    {
      // shift values between 90° and 360° by -90° (fields 9 - 36 --> fields 0 - 27)
      for (int j = 0 ; j < 27; j++) newAntennaZeniPhase.xzPlane(j) = oldAntennaZeniPhase.xzPlane(j+9);
      // values from 270° - 360° = values from 10° - 90°
      for (int j = 27; j < 37; j++) newAntennaZeniPhase.xzPlane(j) = oldAntennaZeniPhase.xzPlane(j-27);
    } 
    // write the data back as new values
    if (!writer.AddData(newAntennaZeniPhase, antennaIDs[i],"AntennaZeniPhase",LOPES_pol_start) )
      cerr << "\nERROR while writing field: AntennaZeniPhase" << endl;  
      
   }
}


void writeDelays(void)
{
  // Set delays (three measurements in 2007)
  Vector<Double> delay_apr(30);
  Vector<Double> delay_sep(30);
  Vector<Double> delay_nov(30);

  delay_apr(0)  = 0.		;
  delay_apr(1)  = 2.52		;
  delay_apr(2)  = -418.18	;
  delay_apr(3)  = -414.67	;
  delay_apr(4)  = -456.85	;
  delay_apr(5)  = -459.98	;
  delay_apr(6)  = -418.96	;
  delay_apr(7)  = -418.57	;
  delay_apr(8)  = -429.46	;
  delay_apr(9)  = -431.46	;
  delay_apr(10) = -1257.24	;
  delay_apr(11) = -1258.	;
  delay_apr(12) = -1252.56	;
  delay_apr(13) = -1257.24	;
  delay_apr(14) = -1260.8	;
  delay_apr(15) = -1252.24	;
  delay_apr(16) = -838.86	;
  delay_apr(17) = -839.85	;
  delay_apr(18) = -1256.78	;
  delay_apr(19) = -1260.37	;
  delay_apr(20) = -1002.88	;
  delay_apr(21) = -1002.17	;
  delay_apr(22) = -1012.89	;
  delay_apr(23) = -1010.76	;
  delay_apr(24) = -581.07	;
  delay_apr(25) = -589.67	;
  delay_apr(26) = -595.14	;
  delay_apr(27) = -594.46	;
  delay_apr(28) = -591.91	;
  delay_apr(29) = -613.1	;
  
  delay_sep(0)  = 0.		;
  delay_sep(1)  = 2.43		;
  delay_sep(2)  = -416.82	;
  delay_sep(3)  = -414.62	;
  delay_sep(4)  = -456.8	;
  delay_sep(5)  = -459.77	;
  delay_sep(6)  = -417.7	;
  delay_sep(7)  = -417.89	;
  delay_sep(8)  = -428.75	;
  delay_sep(9)  = -431.73	;
  delay_sep(10) = -1227.02	;
  delay_sep(11) = -1227.57	;
  delay_sep(12) = -1222.09	;
  delay_sep(13) = -1226.6	;
  delay_sep(14) = -1229.98	;
  delay_sep(15) = -1220.18	;
  delay_sep(16) = -809.79	;
  delay_sep(17) = -807.1	;
  delay_sep(18) = -1224.47	;
  delay_sep(19) = -1227.81	;
  delay_sep(20) = -973.11	;
  delay_sep(21) = -972.68	;
  delay_sep(22) = -982.48	;
  delay_sep(23) = -979.88	;
  delay_sep(24) = -549.54	;
  delay_sep(25) = -558.95	;
  delay_sep(26) = -564.82	;
  delay_sep(27) = -564.83	;
  delay_sep(28) = -562.29	;
  delay_sep(29) = -582.99	;
				
  delay_nov(0)  = 0.		;
  delay_nov(1)  = 2.35		;
  delay_nov(2)  = -416.79	;
  delay_nov(3)  = -415.23	;
  delay_nov(4)  = -457.38	;
  delay_nov(5)  = -459.76	;
  delay_nov(6)  = -418.59	;
  delay_nov(7)  = -418.56	;
  delay_nov(8)  = -429.29	;
  delay_nov(9)  = -432.03	;
  delay_nov(10) = -1229.64	;
  delay_nov(11) = -1230.31	;
  delay_nov(12) = -1225.	;
  delay_nov(13) = -1229.68	;
  delay_nov(14) = -1233.2	;
  delay_nov(15) = -1223.43	;
  delay_nov(16) = -810.07	;
  delay_nov(17) = -810.93	;
  delay_nov(18) = -1228.12	;
  delay_nov(19) = -1231.8	;
  delay_nov(20) = -951.95	;
  delay_nov(21) = -951.95	;
  delay_nov(22) = -960.93	;
  delay_nov(23) = -959.37	;
  delay_nov(24) = -529.29	;
  delay_nov(25) = -537.89	;
  delay_nov(26) = -544.14	;
  delay_nov(27) = -543.75	;
  delay_nov(28) = -541.01	;
  delay_nov(29) = -562.5	;
  
  // converstion from ns to s
  delay_apr /= 1e9;
  delay_sep /= 1e9;
  delay_nov /= 1e9;
  
  // Add the delays for all antennas to the CalTable
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Writing delays of apr 2007 for antenna: " << antennaIDs[i] << endl;

    if (!writer.AddData(delay_apr(i),antennaIDs[i],"Delay",delay_apr_07_start) )
      cerr << "\nERROR while writing field: Delay" << endl;
  }
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Writing delays of sep 2007 for antenna: " << antennaIDs[i] << endl;

    if (!writer.AddData(delay_sep(i),antennaIDs[i],"Delay",delay_sep_07_start) )
      cerr << "\nERROR while writing field: Delay" << endl;
  }
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Writing delays of nov 2007 for antenna: " << antennaIDs[i] << endl;

    if (!writer.AddData(delay_nov(i),antennaIDs[i],"Delay",delay_nov_07_start) )
      cerr << "\nERROR while writing field: Delay" << endl;
  }
}

// adds a new field, which contains the reference antenna for the reference phase differences
void addRefAntField(void)
{
  if (!writer.AddField("PhaseRefAnt", "Reference antenna for calculation of phase differences", "Double",
  			False,  IPosition::IPosition(), False, "Delay") )
    cerr << "\nERROR while adding field: PhaseRefAnt" << endl;
  
  // set PhaseRefAnt to AntennaID of Antenna 1 for LOPES 10 and 30
  for (int i = 0; i < 10; i++)
  {
    cout << "Writing PhaseRefAnt of LOPES 10 for antenna: " << antennaIDs[i] << endl;

    if (!writer.AddData(10101,antennaIDs[i],"PhaseRefAnt",LOPES10_start) )
      cerr << "\nERROR while writing field: PhaseRefAnt" << endl;
  }
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Writing PhaseRefAnt of LOPES 30 for antenna: " << antennaIDs[i] << endl;

    if (!writer.AddData(10101,antennaIDs[i],"PhaseRefAnt",LOPES30_start) )
      cerr << "\nERROR while writing field: PhaseRefAnt" << endl;
  }

}

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
  PhaseRefPhases(12,0) = -83.45	;	  PhaseRefPhases(12,1) = -135.8	;
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
      // Reference Antenna 4 for new phase differences
      if (!writer.AddData(10202,antennaIDs[i],"PhaseRefAnt",roof_setup_2_freq_start) )
        cerr << "\nERROR while writing field: PhaseRefAnt" << endl;
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
    
    // execute requested function to change CalTable
    //adjust_height_of_ant_14(); // allready checked in
    
    // Changes to Delay-Table , allready checked in
    // TV-Ref phases for LOPES_POL are missing
    //writeDelays();
    //addRefAntField();
    //writeRefPhases();

    // Rotate Antennagainfaktors for NS-polarised antennas:
    //rotate_antenna_model();
    
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
