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

// Writes reference phase differences for roof setup
void writeRoofRefPhases(void)
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

// Writes TV reference phase differences 
void writeTVRefPhases(void)
{
  // References Frequencies, Sample Jumps amd reference antenna stay as they are

  // Set reference phases from nov 2006
  Matrix<Double> PhaseRefPhases(30,3);
  PhaseRefPhases(0,0)  = 0	;	  PhaseRefPhases(0,1)  = 0	;	  PhaseRefPhases(0,2)  = 0	;
  PhaseRefPhases(1,0)  = 119.1	;	  PhaseRefPhases(1,1)  = -121.7	;	  PhaseRefPhases(1,2)  = -109.5	;
  PhaseRefPhases(2,0)  = -120	;	  PhaseRefPhases(2,1)  = 119.1	;	  PhaseRefPhases(2,2)  = 164.6	;
  PhaseRefPhases(3,0)  = -57.1	;	  PhaseRefPhases(3,1)  = -174.4	;	  PhaseRefPhases(3,2)  = -121	;
  PhaseRefPhases(4,0)  = 45.1	;	  PhaseRefPhases(4,1)  = -120.8	;	  PhaseRefPhases(4,2)  = -19.3	;
  PhaseRefPhases(5,0)  = -165.1	;	  PhaseRefPhases(5,1)  = 21.2	;	  PhaseRefPhases(5,2)  = 99	;
  PhaseRefPhases(6,0)  = 71.1	;	  PhaseRefPhases(6,1)  = 153.4	;	  PhaseRefPhases(6,2)  = -131.3	;
  PhaseRefPhases(7,0)  = 31.5	;	  PhaseRefPhases(7,1)  = -78.1	;	  PhaseRefPhases(7,2)  = -7.7	;
  PhaseRefPhases(8,0)  = 39.3	;	  PhaseRefPhases(8,1)  = -73.5	;	  PhaseRefPhases(8,2)  = -11.4	;
  PhaseRefPhases(9,0)  = -97.1	;	  PhaseRefPhases(9,1)  = 118.9	;	  PhaseRefPhases(9,2)  = 166.8	;
  PhaseRefPhases(10,0) = 122.9	;	  PhaseRefPhases(10,1) = 35.7	;	  PhaseRefPhases(10,2) = -169.8	;
  PhaseRefPhases(11,0) = -98.65	;	  PhaseRefPhases(11,1) = 124.3	;	  PhaseRefPhases(11,2) = -73.1	;
  PhaseRefPhases(12,0) = 154.5	;	  PhaseRefPhases(12,1) = 13.1	;	  PhaseRefPhases(12,2) = 167.7	;
  PhaseRefPhases(13,0) = 69.9	;	  PhaseRefPhases(13,1) = 29.5	;	  PhaseRefPhases(13,2) = -152.4	;
  PhaseRefPhases(14,0) = 144.6	;	  PhaseRefPhases(14,1) = 125.4	;	  PhaseRefPhases(14,2) = -62	;
  PhaseRefPhases(15,0) = 78.2	;	  PhaseRefPhases(15,1) = 34.2	;	  PhaseRefPhases(15,2) = -155.3	;
  PhaseRefPhases(16,0) = -132.8	;	  PhaseRefPhases(16,1) = 79.7	;	  PhaseRefPhases(16,2) = -133.5	;
  PhaseRefPhases(17,0) = 28.2	;	  PhaseRefPhases(17,1) = -1.3	;	  PhaseRefPhases(17,2) = 146.6	;
  PhaseRefPhases(18,0) = 172.8	;	  PhaseRefPhases(18,1) = 152	;	  PhaseRefPhases(18,2) = -71.8	;
  PhaseRefPhases(19,0) = 26.64	;	  PhaseRefPhases(19,1) = 112.2	;	  PhaseRefPhases(19,2) = -97.4	;
  PhaseRefPhases(20,0) = -68.1	;	  PhaseRefPhases(20,1) = 68.1	;	  PhaseRefPhases(20,2) = -168.2	;
  PhaseRefPhases(21,0) = 34.6	;	  PhaseRefPhases(21,1) = -172.1	;	  PhaseRefPhases(21,2) = -60.3	;
  PhaseRefPhases(22,0) = 99.7	;	  PhaseRefPhases(22,1) = 13.2	;	  PhaseRefPhases(22,2) = 147.7	;
  PhaseRefPhases(23,0) = 15.4	;	  PhaseRefPhases(23,1) = -104.8	;	  PhaseRefPhases(23,2) = 30.9	;
  PhaseRefPhases(24,0) = -73.1	;	  PhaseRefPhases(24,1) = 6.2	;	  PhaseRefPhases(24,2) = 124	;
  PhaseRefPhases(25,0) = -72.1	;	  PhaseRefPhases(25,1) = 152.8	;	  PhaseRefPhases(25,2) = -107.5	;
  PhaseRefPhases(26,0) = -77.7	;	  PhaseRefPhases(26,1) = -96.4	;	  PhaseRefPhases(26,2) = -33.2	;
  PhaseRefPhases(27,0) = -158	;	  PhaseRefPhases(27,1) = -152.9	;	  PhaseRefPhases(27,2) = -86.5	;
  PhaseRefPhases(28,0) = -116.4	;	  PhaseRefPhases(28,1) = -4.5	;	  PhaseRefPhases(28,2) = 80.7	;
  PhaseRefPhases(29,0) = 155.4	;	  PhaseRefPhases(29,1) = -114.2	;	  PhaseRefPhases(29,2) = -41	;

  // Add the value for all antennas
  cout << "Writing TV reference phases for LOPES 30, GT = " << delay_apr_07_start << endl;
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Writing values for antenna: " << antennaIDs[i] << endl;

    if (!writer.AddData(PhaseRefPhases.row(i),antennaIDs[i],"PhaseRefPhases",delay_apr_07_start) )
      cerr << "\nERROR while writing field: PhaseRefPhases" << endl;
  }

  // Set reference phases from apr/may 2007 for LOPES Dual Pol
  PhaseRefPhases(0,0)  = 0	;	  PhaseRefPhases(0,1)  = 0	;	  PhaseRefPhases(0,2)  = 0	;
  PhaseRefPhases(1,0)  = -123.9	;	  PhaseRefPhases(1,1)  = 14.7	;	  PhaseRefPhases(1,2)  = -27.2	;
  PhaseRefPhases(2,0)  = -163.8	;	  PhaseRefPhases(2,1)  = 134.7	;	  PhaseRefPhases(2,2)  = 126	;
  PhaseRefPhases(3,0)  = 79.9	;	  PhaseRefPhases(3,1)  = -42.1	;	  PhaseRefPhases(3,2)  = -36.2	;
  PhaseRefPhases(4,0)  = -176.2	;	  PhaseRefPhases(4,1)  = 14.1	;	  PhaseRefPhases(4,2)  = 62	;
  PhaseRefPhases(5,0)  = 86	;	  PhaseRefPhases(5,1)  = -78.6	;	  PhaseRefPhases(5,2)  = -59.8	;
  PhaseRefPhases(6,0)  = 68.9	;	  PhaseRefPhases(6,1)  = 134	;	  PhaseRefPhases(6,2)  = 166.2	;
  PhaseRefPhases(7,0)  = -31.3	;	  PhaseRefPhases(7,1)  = 174.1	;	  PhaseRefPhases(7,2)  = -174.4	;
  PhaseRefPhases(8,0)  = 178.5	;	  PhaseRefPhases(8,1)  = 64.7	;	  PhaseRefPhases(8,2)  = 82.6	;
  PhaseRefPhases(9,0)  = -95.1	;	  PhaseRefPhases(9,1)  = 104.4	;	  PhaseRefPhases(9,2)  = 118	;
  PhaseRefPhases(10,0) = 89	;	  PhaseRefPhases(10,1) = 0.1	;	  PhaseRefPhases(10,2) = 80.5	;
  PhaseRefPhases(11,0) = 35.5	;	  PhaseRefPhases(11,1) = -98.6	;	  PhaseRefPhases(11,2) = 20	;
  PhaseRefPhases(12,0) = -70.3	;	  PhaseRefPhases(12,1) = 142.2	;	  PhaseRefPhases(12,2) = -107.5	;
  PhaseRefPhases(13,0) = 9.4	;	  PhaseRefPhases(13,1) = -6.1	;	  PhaseRefPhases(13,2) = 88.9	;
  PhaseRefPhases(14,0) = 113	;	  PhaseRefPhases(14,1) = 64.7	;	  PhaseRefPhases(14,2) = 159.2	;
  PhaseRefPhases(15,0) = -147.4	;	  PhaseRefPhases(15,1) = 165.8	;	  PhaseRefPhases(15,2) = -69.9	;
  PhaseRefPhases(16,0) = -1.3	;	  PhaseRefPhases(16,1) = -150.8	;	  PhaseRefPhases(16,2) = -52.3	;
  PhaseRefPhases(17,0) = -32.6	;	  PhaseRefPhases(17,1) = -19.6	;	  PhaseRefPhases(17,2) = 72.4	;
  PhaseRefPhases(18,0) = -49.9	;	  PhaseRefPhases(18,1) = -67.7	;	  PhaseRefPhases(18,2) = 24.2	;
  PhaseRefPhases(19,0) = -134.6	;	  PhaseRefPhases(19,1) = -169.8	;	  PhaseRefPhases(19,2) = -45.3	;
  PhaseRefPhases(20,0) = -88.5	;	  PhaseRefPhases(20,1) = 93.8	;	  PhaseRefPhases(20,2) = 169.4	;
  PhaseRefPhases(21,0) = 24.8	;	  PhaseRefPhases(21,1) = -134.6	;	  PhaseRefPhases(21,2) = -67	;
  PhaseRefPhases(22,0) = -68.7	;	  PhaseRefPhases(22,1) = -139.8	;	  PhaseRefPhases(22,2) = -43.3	;
  PhaseRefPhases(23,0) = 171.2	;	  PhaseRefPhases(23,1) = 101	;	  PhaseRefPhases(23,2) = -168.2	;
  PhaseRefPhases(24,0) = -101.1	;	  PhaseRefPhases(24,1) = 34.4	;	  PhaseRefPhases(24,2) = 103.4	;
  PhaseRefPhases(25,0) = 55.9	;	  PhaseRefPhases(25,1) = -30.7	;	  PhaseRefPhases(25,2) = 40.4	;
  PhaseRefPhases(26,0) = 49.3	;	  PhaseRefPhases(26,1) = 122	;	  PhaseRefPhases(26,2) = 137.1	;
  PhaseRefPhases(27,0) = 174.2	;	  PhaseRefPhases(27,1) = -150.5	;	  PhaseRefPhases(27,2) = -117.3	;
  PhaseRefPhases(28,0) = -128.7	;	  PhaseRefPhases(28,1) = 23.1	;	  PhaseRefPhases(28,2) = 52.4	;
  PhaseRefPhases(29,0) = 139.6	;	  PhaseRefPhases(29,1) = -71.5	;	  PhaseRefPhases(29,2) = -48.4	;

  // Add the value for all antennas
  cout << "Writing TV reference phases for LOPES POL, GT = " << LOPES_pol_start << endl;
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Writing values for antenna: " << antennaIDs[i] << endl;

    // It is neccessary to write the Delay again, as the other fields
    // are junior fields an cannot be written alone
    Double old_delay = 0.;
    if (!reader.GetData(LOPES_pol_start, antennaIDs[i], "Delay", &old_delay))
    { 
      cerr << "Error while reading field: Delay" << endl;
    } else
    {
      if (!writer.AddData(old_delay,antennaIDs[i],"Delay",LOPES_pol_start) )
        cerr << "\nERROR while writing field: Delay" << endl;
      if (!writer.AddData(PhaseRefPhases.row(i),antennaIDs[i],"PhaseRefPhases",LOPES_pol_start) )
        cerr << "\nERROR while writing field: PhaseRefPhases" << endl;
    }
  }

  // Set reference phases from early sep 2007 for LOPES Dual Pol
  PhaseRefPhases(0,0)  = 0	;	  PhaseRefPhases(0,1)  = 0	;	  PhaseRefPhases(0,2)  = 0	;
  PhaseRefPhases(1,0)  = -66.9	;	  PhaseRefPhases(1,1)  = -3.2	;	  PhaseRefPhases(1,2)  = -9.2	;
  PhaseRefPhases(2,0)  = -113.5	;	  PhaseRefPhases(2,1)  = 105	;	  PhaseRefPhases(2,2)  = 130.3	;
  PhaseRefPhases(3,0)  = 144.8	;	  PhaseRefPhases(3,1)  = -56.8	;	  PhaseRefPhases(3,2)  = -17.4	;
  PhaseRefPhases(4,0)  = -116.7	;	  PhaseRefPhases(4,1)  = -8	;	  PhaseRefPhases(4,2)  = 75.5	;
  PhaseRefPhases(5,0)  = 152.5	;	  PhaseRefPhases(5,1)  = -105.1	;	  PhaseRefPhases(5,2)  = -37.9	;
  PhaseRefPhases(6,0)  = 123.3	;	  PhaseRefPhases(6,1)  = 104.6	;	  PhaseRefPhases(6,2)  = 176.3	;
  PhaseRefPhases(7,0)  = 32.83	;	  PhaseRefPhases(7,1)  = 145.2	;	  PhaseRefPhases(7,2)  = -163.8	;
  PhaseRefPhases(8,0)  = -129.1	;	  PhaseRefPhases(8,1)  = 30.3	;	  PhaseRefPhases(8,2)  = 91.8	;
  PhaseRefPhases(9,0)  = 23.6	;	  PhaseRefPhases(9,1)  = 90.4	;	  PhaseRefPhases(9,2)  = 163.5	;
  PhaseRefPhases(10,0) = -172.3	;	  PhaseRefPhases(10,1) = -55.2	;	  PhaseRefPhases(10,2) = 71.8	;
  PhaseRefPhases(11,0) = 143.6	;	  PhaseRefPhases(11,1) = -144	;	  PhaseRefPhases(11,2) = 9.1	;
  PhaseRefPhases(12,0) = 26.1	;	  PhaseRefPhases(12,1) = 91.5	;	  PhaseRefPhases(12,2) = -119.1	;
  PhaseRefPhases(13,0) = 106.6	;	  PhaseRefPhases(13,1) = -57	;	  PhaseRefPhases(13,2) = 79.6	;
  PhaseRefPhases(14,0) = -143	;	  PhaseRefPhases(14,1) = 9.2	;	  PhaseRefPhases(14,2) = 142.1	;
  PhaseRefPhases(15,0) = -41.6	;	  PhaseRefPhases(15,1) = 122.2	;	  PhaseRefPhases(15,2) = -78.9	;
  PhaseRefPhases(16,0) = 103.3	;	  PhaseRefPhases(16,1) = 162.8	;	  PhaseRefPhases(16,2) = -59.9	;
  PhaseRefPhases(17,0) = 69.7	;	  PhaseRefPhases(17,1) = -66.4	;	  PhaseRefPhases(17,2) = 59.1	;
  PhaseRefPhases(18,0) = 56.9	;	  PhaseRefPhases(18,1) = -117.9	;	  PhaseRefPhases(18,2) = 7.4	;
  PhaseRefPhases(19,0) = -27.6	;	  PhaseRefPhases(19,1) = 137.1	;	  PhaseRefPhases(19,2) = -62.13	;
  PhaseRefPhases(20,0) = -172.8	;	  PhaseRefPhases(20,1) = 178.5	;	  PhaseRefPhases(20,2) = -75.7	;
  PhaseRefPhases(21,0) = -56.11	;	  PhaseRefPhases(21,1) = -52.12	;	  PhaseRefPhases(21,2) = 49.3	;
  PhaseRefPhases(22,0) = -163.5	;	  PhaseRefPhases(22,1) = -64.3	;	  PhaseRefPhases(22,2) = 70.5	;
  PhaseRefPhases(23,0) = 90.1	;	  PhaseRefPhases(23,1) = 174.7	;	  PhaseRefPhases(23,2) = -60.4	;
  PhaseRefPhases(24,0) = 178.5	;	  PhaseRefPhases(24,1) = 115.3	;	  PhaseRefPhases(24,2) = -142.4	;
  PhaseRefPhases(25,0) = -27.2	;	  PhaseRefPhases(25,1) = 51.7	;	  PhaseRefPhases(25,2) = 157.1	;
  PhaseRefPhases(26,0) = 2.9	;	  PhaseRefPhases(26,1) = -158.2	;	  PhaseRefPhases(26,2) = -110.2	;
  PhaseRefPhases(27,0) = 108.6	;	  PhaseRefPhases(27,1) = -58.1	;	  PhaseRefPhases(27,2) = 22.6	;
  PhaseRefPhases(28,0) = 162.6	;	  PhaseRefPhases(28,1) = 112.1	;	  PhaseRefPhases(28,2) = 178.5	;
  PhaseRefPhases(29,0) = 40.9	;	  PhaseRefPhases(29,1) = -7.7	;	  PhaseRefPhases(29,2) = 55.3	;


  // Add the value for all antennas
  cout << "Writing TV reference phases for LOPES POL, GT = " << delay_sep_07_start << endl;
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Writing values for antenna: " << antennaIDs[i] << endl;

    if (!writer.AddData(PhaseRefPhases.row(i),antennaIDs[i],"PhaseRefPhases",delay_sep_07_start) )
      cerr << "\nERROR while writing field: PhaseRefPhases" << endl;
  }

  // Set reference phases from oct 07 for LOPES Dual Pol
  PhaseRefPhases(0,0)  = 0	;	  PhaseRefPhases(0,1)  = 0	;	  PhaseRefPhases(0,2)  = 0	;
  PhaseRefPhases(1,0)  = -107.4	;	  PhaseRefPhases(1,1)  = -1.2	;	  PhaseRefPhases(1,2)  = -35.9	;
  PhaseRefPhases(2,0)  = -163.4	;	  PhaseRefPhases(2,1)  = 114.3	;	  PhaseRefPhases(2,2)  = 116.8	;
  PhaseRefPhases(3,0)  = 80.74	;	  PhaseRefPhases(3,1)  = -54.9	;	  PhaseRefPhases(3,2)  = -40.4	;
  PhaseRefPhases(4,0)  = -174.3	;	  PhaseRefPhases(4,1)  = 2.7	;	  PhaseRefPhases(4,2)  = 58.5	;
  PhaseRefPhases(5,0)  = 85.42	;	  PhaseRefPhases(5,1)  = -94.2	;	  PhaseRefPhases(5,2)  = 64.3	;
  PhaseRefPhases(6,0)  = 65	;	  PhaseRefPhases(6,1)  = 118.6	;	  PhaseRefPhases(6,2)  = 152.5	;
  PhaseRefPhases(7,0)  = -34.7	;	  PhaseRefPhases(7,1)  = 152.5	;	  PhaseRefPhases(7,2)  = 168.7	;
  PhaseRefPhases(8,0)  = 169.9	;	  PhaseRefPhases(8,1)  = 34.9	;	  PhaseRefPhases(8,2)  = 56.7	;
  PhaseRefPhases(9,0)  = -86.7	;	  PhaseRefPhases(9,1)  = 91.2	;	  PhaseRefPhases(9,2)  = 111	;
  PhaseRefPhases(10,0) = 170.1	;	  PhaseRefPhases(10,1) = 10.7	;	  PhaseRefPhases(10,2) = 106.9	;
  PhaseRefPhases(11,0) = 125.2	;	  PhaseRefPhases(11,1) = -87	;	  PhaseRefPhases(11,2) = 33	;
  PhaseRefPhases(12,0) = 14.4	;	  PhaseRefPhases(12,1) = 147.7	;	  PhaseRefPhases(12,2) = -91.4	;
  PhaseRefPhases(13,0) = 104.9	;	  PhaseRefPhases(13,1) = 0.9	;	  PhaseRefPhases(13,2) = 108	;
  PhaseRefPhases(14,0) = -163.5	;	  PhaseRefPhases(14,1) = 75.6	;	  PhaseRefPhases(14,2) = -172	;
  PhaseRefPhases(15,0) = -59.7	;	  PhaseRefPhases(15,1) = -174.2	;	  PhaseRefPhases(15,2) = -51.5	;
  PhaseRefPhases(16,0) = 83.6	;	  PhaseRefPhases(16,1) = -141	;	  PhaseRefPhases(16,2) = -38.3	;
  PhaseRefPhases(17,0) = 54.83	;	  PhaseRefPhases(17,1) = -4.6	;	  PhaseRefPhases(17,2) = 91.6	;
  PhaseRefPhases(18,0) = 39.3	;	  PhaseRefPhases(18,1) = -61.8	;	  PhaseRefPhases(18,2) = 36.3	;
  PhaseRefPhases(19,0) = -46	;	  PhaseRefPhases(19,1) = -152.5	;	  PhaseRefPhases(19,2) = -22.5	;
  PhaseRefPhases(20,0) = -163.4	;	  PhaseRefPhases(20,1) = -96.35	;	  PhaseRefPhases(20,2) = -21.9	;
  PhaseRefPhases(21,0) = -49.5	;	  PhaseRefPhases(21,1) = 35	;	  PhaseRefPhases(21,2) = 106.4	;
  PhaseRefPhases(22,0) = -149.8	;	  PhaseRefPhases(22,1) = 31.3	;	  PhaseRefPhases(22,2) = 134.5	;
  PhaseRefPhases(23,0) = 94.5	;	  PhaseRefPhases(23,1) = -100.5	;	  PhaseRefPhases(23,2) = -8.3	;
  PhaseRefPhases(24,0) = -174	;	  PhaseRefPhases(24,1) = -157.4	;	  PhaseRefPhases(24,2) = -87.8	;
  PhaseRefPhases(25,0) = -15.7	;	  PhaseRefPhases(25,1) = 144.3	;	  PhaseRefPhases(25,2) = -144.6	;
  PhaseRefPhases(26,0) = -3.7	;	  PhaseRefPhases(26,1) = -63.6	;	  PhaseRefPhases(26,2) = -53.3	;
  PhaseRefPhases(27,0) = 111.4	;	  PhaseRefPhases(27,1) = -0.7	;	  PhaseRefPhases(27,2) = 57.1	;
  PhaseRefPhases(28,0) = 155	;	  PhaseRefPhases(28,1) = -159	;	  PhaseRefPhases(28,2) = -135.9	;
  PhaseRefPhases(29,0) = 67.1	;	  PhaseRefPhases(29,1) = 85.5	;	  PhaseRefPhases(29,2) = 122.3	;

  // Add the value for all antennas
  cout << "Writing TV reference phases for LOPES POL, GT = " << delay_nov_07_start << endl;
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Writing values for antenna: " << antennaIDs[i] << endl;

    if (!writer.AddData(PhaseRefPhases.row(i),antennaIDs[i],"PhaseRefPhases",delay_nov_07_start) )
      cerr << "\nERROR while writing field: PhaseRefPhases" << endl;
  }

}


// write dispersion correction
// use values from a measurement of the box which contains the filter
void writePhaseCal(void)
{
  // construct casacore Vectors with frequency values:
  // first create array, secondly a stl-vector and then a casacore Vector
  double freqAxis[] =  {4e+07,4.025e+07,4.05e+07,4.075e+07,4.1e+07,4.125e+07,4.15e+07,4.175e+07,4.2e+07,4.225e+07,4.25e+07,4.275e+07,4.3e+07,4.325e+07,4.35e+07,4.375e+07,4.4e+07,4.425e+07,4.45e+07,4.475e+07,4.5e+07,4.525e+07,4.55e+07,4.575e+07,4.6e+07,4.625e+07,4.65e+07,4.675e+07,4.7e+07,4.725e+07,4.75e+07,4.775e+07,4.8e+07,4.825e+07,4.85e+07,4.875e+07,4.9e+07,4.925e+07,4.95e+07,4.975e+07,5e+07,5.025e+07,5.05e+07,5.075e+07,5.1e+07,5.125e+07,5.15e+07,5.175e+07,5.2e+07,5.225e+07,5.25e+07,5.275e+07,5.3e+07,5.325e+07,5.35e+07,5.375e+07,5.4e+07,5.425e+07,5.45e+07,5.475e+07,5.5e+07,5.525e+07,5.55e+07,5.575e+07,5.6e+07,5.625e+07,5.65e+07,5.675e+07,5.7e+07,5.725e+07,5.75e+07,5.775e+07,5.8e+07,5.825e+07,5.85e+07,5.875e+07,5.9e+07,5.925e+07,5.95e+07,5.975e+07,6e+07,6.025e+07,6.05e+07,6.075e+07,6.1e+07,6.125e+07,6.15e+07,6.175e+07,6.2e+07,6.225e+07,6.25e+07,6.275e+07,6.3e+07,6.325e+07,6.35e+07,6.375e+07,6.4e+07,6.425e+07,6.45e+07,6.475e+07,6.5e+07,6.525e+07,6.55e+07,6.575e+07,6.6e+07,6.625e+07,6.65e+07,6.675e+07,6.7e+07,6.725e+07,6.75e+07,6.775e+07,6.8e+07,6.825e+07,6.85e+07,6.875e+07,6.9e+07,6.925e+07,6.95e+07,6.975e+07,7e+07,7.025e+07,7.05e+07,7.075e+07,7.1e+07,7.125e+07,7.15e+07,7.175e+07,7.2e+07,7.225e+07,7.25e+07,7.275e+07,7.3e+07,7.325e+07,7.35e+07,7.375e+07,7.4e+07,7.425e+07,7.45e+07,7.475e+07,7.5e+07,7.525e+07,7.55e+07,7.575e+07,7.6e+07,7.625e+07,7.65e+07,7.675e+07,7.7e+07,7.725e+07,7.75e+07,7.775e+07,7.8e+07,7.825e+07,7.85e+07,7.875e+07,7.9e+07,7.925e+07,7.95e+07,7.975e+07,8e+07};
  Vector<double> PhaseCalFreq(vector<double> (freqAxis,freqAxis+sizeof(freqAxis)/sizeof(freqAxis[0])) );

  // construct Matrixx with PhaseCal values
  double phasecals[][161] =
  {{-129.569,-129.129,-128.696,-127.648,-125.852,-123.403,-120.111,-116.013,-109.994,-102.707,-92.8544,-81.8489,-71.0527,-62.3397,-54.2214,-45.6668,-35.7816,-25.6155,-16.2622,-9.03965,-3.31797,1.4012,5.24625,7.71226,9.17607,10.8891,12.2222,13.0776,13.1318,13.5286,14.0603,14.7677,15.7958,17.2513,20.2762,23.471,26.6979,29.7201,32.5585,34.5548,35.2773,35.8882,35.945,35.9831,35.0877,33.6831,32.438,30.7097,28.909,26.9117,25.046,23.3524,21.7232,20.1014,18.9755,19.0028,19.5827,20.1867,20.5998,22.3603,24.5491,25.8683,26.9636,27.8563,29.0162,29.0755,27.8564,27.8278,26.7945,25.8771,24.3486,22.7374,21.38,18.1383,15.2436,12.2485,9.56359,7.33087,5.16424,2.64862,-0,-2.65312,-3.96084,-4.43887,-4.06729,-3.24791,-1.73033,0.605845,1.83232,2.6799,3.56749,6.30213,8.92805,10.7737,10.1548,9.21876,7.87832,6.57783,4.94168,2.82775,0.752368,-1.92709,-4.86075,-7.8619,-10.7824,-13.3283,-15.6803,-17.7133,-19.9346,-21.9098,-23.7161,-24.6384,-23.4725,-20.8691,-17.2279,-12.6881,-8.52882,-5.11859,-0.458599,2.99536,5.679,6.65013,6.79536,6.23125,6.00973,4.99202,4.10486,0.844602,-2.11276,-4.22168,-3.1227,-2.14353,-0.257348,1.67383,4.49341,8.81989,15.3127,24.1562,34.4248,43.4068,50.5442,56.1456,61.6247,66.3645,71.6548,78.141,87.1166,97.7728,108.938,119.151,127.989,135.012,140.598,144.989,148.495,151.296,153.512,154.973,155.837,156.191,156.164},
  {-134.914,-134.597,-134.066,-133.064,-131.209,-128.764,-125.15,-121.115,-115.304,-108.409,-99.5208,-89.713,-79.3783,-69.6711,-60.7584,-51.7962,-42.681,-33.3444,-24.9605,-18.0134,-11.7675,-6.9955,-2.61543,0.842914,3.16153,5.47117,7.24584,8.83392,10.0526,10.8868,12.0175,12.9294,14.1186,15.5733,17.4854,19.7472,21.5393,23.6137,25.2398,26.4885,27.2475,27.8195,28.2444,28.4311,28.2555,27.3144,26.6224,25.7245,24.5493,23.3229,22.4836,21.1642,20.4317,19.4695,18.5581,18.7874,18.7245,19.0153,18.9873,19.3788,20.1609,20.5801,20.8197,20.6669,20.731,20.4664,19.7516,19.0571,18.6646,17.6312,16.4949,15.3209,14.1318,12.2085,10.0224,8.24854,6.65181,5.19078,3.66155,2.14053,-0,-1.30993,-1.97805,-2.07528,-1.68191,-1.15634,-0.767963,0.23801,0.0655833,0.169706,0.0495192,1.04345,1.70739,2.00869,1.36085,0.363144,-0.822703,-1.78695,-3.08426,-4.35819,-6.0125,-7.83088,-10.0052,-11.7913,-13.4853,-14.9337,-16.4123,-17.3938,-18.494,-19.6341,-19.9785,-19.7907,-18.7808,-16.8608,-14.7066,-12.4341,-10.359,-8.81508,-6.47227,-4.36151,-2.96988,-2.24501,-1.9324,-1.66819,-1.64013,-1.8588,-1.79749,-2.90307,-3.8672,-3.68253,-2.03775,-0.0354812,2.69379,5.60656,9.65914,14.19,20.2525,27.8815,35.2941,42.2165,48.5949,54.5351,61.244,68.0237,75.9679,85.6917,96.5666,107.542,117.588,125.864,132.819,138.281,142.92,146.116,148.619,150.545,151.871,152.218,152.565,152.169,151.202},
  {-131.633,-131.829,-131.488,-130.714,-129.254,-127.336,-124.796,-121.383,-116.532,-110.658,-102.832,-93.8687,-83.8441,-74.6574,-66.5224,-58.6947,-49.7314,-39.6394,-29.305,-20.5487,-12.936,-7.04631,-1.62374,2.17308,4.16851,6.414,8.11884,9.39189,9.68471,9.68826,10.6357,11.6678,12.3355,13.7027,16.4055,19.7872,22.9183,25.6618,28.9949,30.9017,32.1586,32.878,33.1939,33.4,32.3573,31.2532,29.9208,28.488,26.5768,24.7118,22.9711,21.3748,19.6199,18.0623,17.0819,17.3941,18.1701,18.8426,19.9043,21.7976,24.0335,25.3594,26.6498,27.6242,28.8803,28.5569,27.6385,27.3767,26.6104,25.5153,24.016,22.3919,20.9221,17.7776,14.936,11.8567,9.34203,7.2433,5.29387,2.55783,-0,-2.42473,-3.54197,-3.4451,-2.99954,-1.62507,0.253197,2.75536,3.94343,4.66353,5.47364,8.53888,10.8226,11.9868,11.0976,10.3747,8.74176,7.29169,5.40711,3.48289,0.925615,-1.71669,-4.39682,-7.6479,-10.4044,-12.7482,-14.8131,-17.0933,-18.4558,-20.6473,-21.9233,-22.2392,-20.4662,-17.2463,-13.2053,-8.38824,-4.40516,-0.976959,3.48856,6.82176,9.20556,9.59623,9.46569,8.64745,8.08606,7.25514,6.10025,3.11091,0.52948,-0.794554,0.689512,3.13978,5.73384,8.44391,13.1223,18.8972,27.4492,37.6496,47.2289,54.571,60.4096,64.8574,69.7082,75.1423,81.78,90.9349,103.111,116.853,129.484,139.681,147.992,154.666,160.051,163.848,167.005,169.17,170.767,171.536,171.433,171.355,170.24},
  {-130.796,-130.407,-129.674,-128.258,-126.295,-123.587,-120.041,-115.875,-110.43,-103.823,-95.7062,-86.9544,-78.2795,-70.4382,-63.1179,-55.3711,-46.0799,-35.6297,-25.4635,-17.1862,-10.67,-4.80605,-0.533032,2.30041,3.86806,5.56315,7.20744,8.1731,8.37323,9.13693,10.3993,11.4102,12.8379,15.0638,18.3635,21.8546,24.9249,28.1586,30.3266,31.9394,32.5458,32.6576,32.6889,31.971,30.9277,29.1815,27.6099,26.0259,24.0415,22.3298,20.7813,19.3301,17.9951,16.9913,16.8199,17.718,19.049,20.1799,22.3625,25.0562,27.9973,28.8757,30.0474,30.7098,30.5783,29.4647,27.2717,26.4143,25.0856,23.4671,21.8798,20.3399,18.5831,15.6123,12.7188,9.64388,7.67198,5.78839,4.28639,2.2967,-0,-2.1297,-1.19959,0.425612,2.53452,4.94612,7.99782,10.5518,11.6761,12.2179,12.2207,13.9289,14.5393,14.5187,12.6258,11.0629,8.71877,6.53986,4.42086,1.69327,-0.686177,-3.90999,-6.92881,-9.1887,-11.0357,-12.2627,-13.7771,-14.836,-15.0585,-14.5848,-12.8416,-9.3646,-4.25134,1.68361,7.24082,11.036,13.3431,14.3328,16.2539,17.0907,16.993,15.8987,14.5568,12.6834,11.8867,10.9378,10.276,9.3533,10.6977,14.8844,20.0336,26.2719,33.6131,41.6024,50.4197,59.1031,67.3398,74.0822,79.1396,83.6108,89.7559,99.1989,112.752,129.721,145.383,157.442,166.386,173.041,177.971,-178.314,-175.756,-174.033,-173.33,-173.148,-173.311,-174.441,-175.679,-177.541,179.771,176.65,172.506},
  {-134.115,-134.069,-133.829,-132.978,-131.399,-129.364,-126.624,-123.068,-118.784,-112.827,-105.915,-97.4447,-88.2872,-79.0261,-70.1794,-61.607,-53.0263,-43.9951,-35.0119,-27.241,-20.3071,-14.2022,-8.77229,-4.69947,-1.40418,1.27243,3.55678,5.63467,6.88195,7.97481,9.32069,10.3273,11.3977,13.0295,14.8764,17.0525,19.0342,21.2022,23.0959,24.6514,25.5548,26.191,26.8281,26.9129,26.8283,26.1851,25.5152,24.5943,23.5537,22.3928,21.2705,20.1619,18.7194,17.8188,16.98,17.0536,16.8243,17.024,17.1554,17.465,18.3401,18.5905,19.1287,19.1638,19.9698,19.966,19.3194,19.0911,18.69,17.8699,16.803,15.7881,14.3521,12.6052,10.5519,8.58141,6.87843,5.25795,3.85327,1.70318,-0,-1.77008,-2.81717,-3.44175,-3.44803,-3.47221,-2.908,-2.28608,-2.12936,-1.92445,-1.98113,-0.843852,0.105985,0.872292,0.633389,-0.103814,-0.973367,-1.44696,-2.56328,-3.93353,-5.39031,-6.91589,-8.79917,-10.8981,-12.6002,-14.3335,-15.7949,-17.3062,-18.4852,-19.763,-20.4694,-20.8681,-20.0957,-18.853,-16.995,-14.79,-12.7291,-10.5305,-7.99798,-5.73104,-3.7467,-2.39785,-1.54967,-1.1055,-0.903838,-0.902261,-0.842594,-2.15612,-3.03468,-3.63256,-2.35505,-0.709129,1.34789,3.55671,6.37842,10.201,15.1092,21.0873,28.2563,35.1553,41.6939,47.877,53.9664,60.2725,67.2403,75.3797,85.8908,98.8757,112.84,125.685,136.165,144.554,150.92,155.937,159.538,162.314,163.776,164.842,165.277,164.868,164.031},
  {-139.967,-140.698,-140.697,-140.626,-139.76,-138.303,-136.439,-133.84,-130.28,-125.658,-119.862,-112.727,-104.148,-94.7677,-85.1832,-76.1072,-67.0739,-58.1453,-49.2557,-40.7667,-33.1407,-26.0818,-19.5844,-14.1116,-9.7323,-5.99022,-3.13148,-0.648885,1.04593,2.35189,3.44209,4.67122,5.90741,6.82001,8.57796,10.2373,12.4053,14.5283,16.3722,18.054,19.4151,20.6221,21.4878,22.351,22.638,22.6629,22.2846,21.8679,21.3445,20.2506,19.0517,18.3355,17.1805,16.2787,15.4916,15.2391,15.2093,15.1032,15.3833,15.2999,15.7966,16.3985,16.4019,16.5495,17.386,17.7458,17.3179,17.1576,16.9208,16.5822,15.7494,14.4482,13.5145,11.7916,10.0315,8.20798,6.3854,4.91432,3.46055,1.90598,1.77636e-15,-1.80618,-2.83575,-3.53702,-4.0358,-3.78648,-3.85545,-3.46753,-3.147,-2.91538,-2.87555,-1.72352,-0.59652,-0.047035,-0.29071,-0.526965,-1.36195,-1.93153,-2.38071,-3.3748,-4.53493,-5.93473,-7.32147,-9.17284,-11.0596,-12.2617,-13.7,-15.0407,-16.2517,-17.0496,-17.8878,-18.2066,-17.3335,-16.068,-14.3372,-12.1115,-9.63859,-7.64507,-5.47384,-2.94466,-0.84295,0.451015,1.65364,2.37707,2.96052,3.45088,3.49941,2.91844,2.3535,2.47752,4.17695,6.58358,9.2158,12.4718,16.5593,21.58,27.7036,35.4558,43.815,51.607,58.793,65.9893,73.9161,82.3339,92.4699,103.944,116.014,127.11,136.888,144.38,150.248,154.547,158.237,160.362,162.065,163.042,163.409,163.017,162.355,161.113,159.337},
  {-131.318,-131.377,-130.647,-129.879,-128.283,-125.889,-123.155,-119.273,-114.242,-107.649,-99.2526,-89.5695,-78.886,-68.7258,-59.3232,-50.1063,-40.6115,-31.675,-23.3557,-16.7496,-11.2219,-6.36789,-2.24663,0.920527,3.36919,5.67766,7.47806,9.14362,10.3162,11.3368,12.4035,13.7869,14.7094,16.0329,18.0134,20.1075,21.7485,23.7948,25.6965,26.887,27.559,28.1872,28.5137,28.7144,28.3969,27.8931,27.0062,26.1519,25.2448,24.2038,22.9457,21.9533,20.8786,19.7773,19.3164,19.2134,19.0835,19.384,19.4277,19.6436,20.1903,20.5453,20.4759,20.6263,20.815,20.4933,19.7752,19.2709,18.7272,18.0984,16.5437,15.4878,14.2781,12.5525,10.4324,8.5296,6.83456,5.30532,3.74048,1.94834,-0,-1.80594,-2.21488,-2.86222,-2.91646,-2.842,-2.57475,-1.83429,-1.96523,-1.98177,-2.18445,-1.08847,-0.52992,-0.0652108,-0.413732,-1.22868,-2.33395,-3.07358,-4.15403,-5.38153,-6.8958,-8.47513,-10.5692,-12.3527,-14.3658,-15.8037,-17.2802,-18.5747,-19.6523,-21.0194,-21.8773,-22.1736,-21.6325,-20.3253,-18.6151,-16.6223,-14.5143,-12.9005,-10.2601,-8.50399,-6.06952,-5.23173,-4.40907,-4.08777,-3.61946,-3.65799,-3.62775,-4.79109,-5.75306,-6.3308,-4.96374,-3.53228,-1.40932,0.494936,3.4058,6.95935,11.8935,17.5661,24.4906,31.7925,38.282,44.353,50.9194,57.3339,64.187,71.7778,81.1223,92.2608,103.356,113.805,122.9,130.371,136.311,141.384,145.154,148.085,150.75,152.133,153.278,153.779,153.66},
  {-136.814,-137.265,-137.43,-136.999,-136.581,-135.198,-133.702,-131.074,-127.797,-123.485,-117.729,-110.321,-101.52,-91.5067,-81.613,-72.1056,-62.1144,-52.2913,-42.5286,-33.953,-26.2902,-20.1273,-14.4996,-9.71935,-6.26501,-3.2488,-0.743143,1.40768,2.90816,3.81002,5.10951,6.36851,7.82601,9.07268,11.1307,13.3816,15.4397,17.7887,19.8797,21.5793,22.6455,23.6175,24.3033,24.7207,24.754,24.3223,23.6546,22.9327,22.1498,20.9682,19.718,18.7491,17.5419,16.5246,15.7202,15.6676,15.6552,15.9675,15.9289,16.5927,17.0742,17.5851,18.2595,18.6094,19.2831,19.2944,18.9236,18.6588,18.1218,17.6471,16.5412,15.417,14.3202,12.5492,10.5603,8.37161,6.70737,5.06523,3.73929,2.00514,-0,-1.70774,-2.63219,-3.18973,-3.07817,-2.90301,-2.78516,-1.7182,-1.71354,-1.26179,-1.05711,0.0315979,0.929465,1.91543,1.57857,1.0032,-0.0883767,-0.81346,-1.77027,-2.86626,-4.37667,-5.99371,-7.81891,-9.49449,-11.4827,-12.9879,-14.2914,-15.6215,-16.5207,-17.5405,-18.2102,-18.1858,-17.2207,-15.3438,-13.2712,-10.8439,-8.75837,-6.6101,-3.94758,-1.94106,0.137743,1.2173,1.87911,2.23747,2.40807,2.71897,2.5458,1.62298,0.87644,0.804897,2.72545,4.72471,7.53627,10.4924,14.5479,19.5897,26.0592,33.6399,42.1474,49.3345,56.1827,62.4323,69.449,76.543,84.5508,92.9508,102.193,110.863,118.586,125.608,131.656,136.452,140.757,144.165,147.012,149.058,150.568,151.661,151.815,151.849,151.057},
  {-134.116,-134.457,-134.816,-134.599,-133.939,-132.874,-131.297,-128.444,-125.16,-120.549,-114.298,-106.307,-96.2201,-85.9662,-76.4676,-68.0023,-58.7817,-48.7267,-38.1917,-28.7914,-20.4722,-13.5782,-7.04548,-2.56186,0.688253,3.38915,5.15128,6.94799,7.32628,7.66688,8.065,9.01703,9.83093,10.9806,13.359,16.522,19.5508,22.6887,25.8555,28.5045,29.9693,31.3091,31.6126,32.3705,32.076,31.1491,30.0218,28.7052,27.1853,25.2239,23.4533,21.7,19.9153,18.3855,16.9137,17.174,17.2034,17.8829,18.4094,19.3848,21.6557,22.8423,24.0328,25.3625,26.6991,27.2547,26.7807,26.6699,26.104,25.2895,24.0465,22.5673,21.2112,18.2313,15.1829,12.0662,9.91161,7.54163,5.28946,2.55488,-0,-2.82298,-4.16836,-4.92543,-4.73051,-4.12029,-3.35487,-1.11055,0.106177,1.0498,2.22892,4.96041,7.74507,9.83649,9.47185,8.92965,7.65748,6.8449,5.36178,3.74136,1.58623,-0.874686,-3.82126,-6.80401,-9.57162,-12.4192,-14.7896,-16.9012,-18.9418,-21.3108,-23.1465,-24.6117,-23.8609,-22.4429,-19.8319,-16.2663,-12.1412,-8.92869,-4.68965,-0.585359,3.02164,4.63765,5.55475,5.33833,5.21749,4.51816,3.86273,0.668778,-2.1484,-4.46042,-3.8383,-3.20477,-1.71825,-0.43033,1.87359,4.96951,9.66074,16.2112,25.1267,34.1886,42.1815,48.7117,54.659,59.8373,64.203,69.232,75.8132,84.6052,95.574,106.723,117.419,126.027,133.175,139.419,144.457,148.452,151.83,154.475,156.389,157.782,158.469},
  {-130.356,-130,-129.688,-128.893,-127.868,-126.272,-124.078,-121.106,-117.233,-112.756,-106.614,-99.2625,-90.6671,-81.9138,-73.4603,-64.9714,-55.6857,-46.0464,-36.1227,-27.7582,-20.7116,-14.3657,-8.82948,-4.7372,-1.7677,0.901355,3.03549,4.97697,5.83838,6.91547,7.99904,9.17621,10.4164,11.8331,14.2889,16.8727,19.3469,21.8237,23.6552,25.5044,26.5943,27.2154,27.6128,27.7075,27.514,26.8668,25.6832,24.6909,23.7883,22.3807,20.9245,19.9892,18.7874,17.8533,17.0639,17.0721,17.5342,17.6897,18.2486,18.9035,20.0633,20.8176,21.0411,21.3488,22.1169,21.7776,20.8949,20.6652,19.6838,19.0772,17.7664,16.6449,15.4724,13.0903,10.9573,8.79796,7.01821,5.32116,3.9022,1.75055,-0,-1.66705,-2.2441,-2.21016,-1.67621,-1.14756,-0.384413,1.06204,1.61678,1.76635,1.74475,3.13612,4.34606,4.94084,4.25682,3.21498,2.02913,1.15893,-0.323848,-1.80484,-3.68034,-5.64434,-7.77942,-9.85165,-11.9002,-13.1301,-14.2759,-15.7926,-16.6943,-17.6585,-17.684,-17.0136,-14.9716,-12.3257,-9.32276,-6.0644,-3.77262,-1.62705,1.3207,3.73874,5.1407,5.98038,5.96122,5.6532,5.59948,5.5368,5.3263,4.24665,4.0156,4.64625,7.0043,10.1459,14.1006,18.5821,24.0885,30.5984,39.4293,48.0978,56.4608,63.2684,69.4734,76.3962,84.4763,94.108,104.978,116.633,127.315,136.093,143.137,148.622,153.307,156.822,159.312,161.09,162.577,163.24,163.131,162.909,162.068,160.568,158.361},
  {125.018,141.16,156.17,170.362,-175.763,-162.049,-149.001,-136.536,-124.562,-113.679,-103.621,-94.3774,-86.4622,-80.0091,-74.1715,-68.8706,-63.672,-58.4306,-53.401,-48.5294,-44.0221,-39.4923,-35.3136,-31.3377,-27.9114,-24.8518,-22.0338,-19.6452,-17.7001,-16.3751,-15.0113,-13.879,-12.6884,-11.7444,-10.3174,-9.07945,-7.55514,-5.92872,-4.63905,-2.84529,-1.65482,-0.340186,0.848671,2.08157,2.91272,3.69657,3.98684,4.00554,4.08936,3.70618,3.26337,2.67831,2.2007,1.42738,0.766783,0.405882,0.09967,0.104448,0.0247167,-0.028975,0.336223,0.763692,1.48525,2.29651,3.50567,4.94493,5.92958,6.71094,7.2694,7.85386,7.51862,7.58447,7.38643,6.70889,5.77655,4.86371,4.00697,3.02413,2.14958,1.08224,1.77636e-15,-1.28244,-2.14908,-2.83952,-3.17447,-3.60221,-4.00365,-3.79129,-3.83813,-3.83098,-3.72282,-2.94346,-1.9389,-0.865352,-0.288133,0.306205,0.966103,1.78322,2.58216,3.12121,3.65382,3.92747,4.26351,4.71945,5.26785,6.04178,7.03903,7.9523,9.17779,10.2434,11.4869,12.8676,14.4645,16.2974,17.6651,19.1531,20.49,21.833,23.6203,25.4978,27.6992,29.9847,32.446,35.2654,38.8189,42.9458,47.7587,53.328,59.6822,66.6418,72.9665,79.6325,86.286,93.1316,100.403,108.195,116.979,126.098,135.695,144.861,153.651,161.878,169.478,176.25,-177.642,-171.953,-167.018,-162.567,-159.457,-157.901,-158.325,-162.113,-169.581,178.909,162.212,142.896,122.184,102.057,81.2198,58.5406,34.2491},
  {135.754,150.592,165.508,-179.772,-165.302,-151.672,-138.52,-126.48,-115.72,-105.828,-96.7151,-88.824,-81.7717,-76.0257,-70.7885,-65.6537,-60.8312,-55.7716,-50.4864,-45.7778,-41.2308,-37.0771,-32.9429,-29.133,-25.8879,-23.0477,-20.5327,-18.467,-16.8135,-15.5931,-14.3942,-13.1365,-12.4115,-11.437,-10.288,-9.04746,-7.57451,-6.07011,-4.47194,-2.9445,-1.75494,-0.492649,0.377921,1.52082,2.24733,2.62822,2.93406,3.01377,2.90946,2.45917,2.11856,1.42802,0.664078,0.0527012,-0.527806,-0.622683,-0.56233,-0.129527,-0.0775342,0.296259,0.981742,2.11379,2.90565,4.1635,5.67515,6.76461,7.45296,8.29871,8.78607,9.05062,8.97617,8.56412,7.92718,7.27863,6.15178,5.05314,3.98669,3.18154,2.15579,1.10095,-0,-1.30405,-1.86859,-2.41254,-2.57719,-2.98774,-2.75338,-2.31713,-2.56958,-2.14692,-1.8701,-0.878468,0.518705,1.48988,2.15868,2.80494,3.33352,4.00963,4.88913,5.38506,5.74153,6.08579,6.37569,7.05794,7.68564,8.67292,9.70146,10.5836,11.755,12.9905,14.248,15.5733,16.8785,18.4102,19.5781,20.9558,22.0884,23.4442,24.9935,26.8055,28.9226,31.2862,33.7868,36.8937,40.8025,45.0489,50.118,56.6557,63.9676,71.7847,79.129,86.553,94.7173,103.107,112.209,121.865,132.435,143.536,155.33,166.686,178.123,-171.303,-160.971,-151.705,-143.337,-136.745,-131.812,-128.405,-126.891,-127.293,-129.488,-133.92,-140.352,-149.058,-162.684,-179.196,158.312,133.472,95.9214,59.5375,40.6839},
  {127.865,144.345,159.511,173.522,-172.757,-159.808,-147.38,-135.797,-124.522,-114.033,-104.276,-95.1679,-87.1597,-80.4119,-74.5614,-69.1168,-64.0885,-58.6642,-53.5732,-48.3791,-43.7318,-39.2491,-34.6803,-30.5449,-27.0686,-23.9878,-21.2351,-18.9621,-17.047,-15.5963,-14.4474,-13.3348,-12.1711,-11.5162,-10.2089,-8.97959,-7.55968,-5.98584,-4.67424,-3.10008,-1.87956,-0.728458,0.504811,1.43602,2.47676,3.17224,3.28512,3.41388,3.19237,2.82797,2.34628,1.65302,0.931985,-0.0424488,-0.746963,-0.963996,-1.17626,-1.11652,-1.26973,-1.42935,-0.978825,-0.132969,0.566687,1.58354,3.1306,4.88276,6.17581,6.90937,7.68452,8.28028,8.44884,8.14099,7.84595,7.16171,6.30096,5.48232,4.20417,3.39273,2.38979,1.18964,-1.77636e-15,-1.35914,-2.15929,-3.03343,-3.55928,-4.00732,-4.32386,-4.35081,-4.50705,-4.46019,-4.51454,-3.86008,-3.07321,-1.96651,-1.5255,-0.789866,-0.42252,0.328676,0.940273,1.65953,2.12178,2.20799,2.47073,2.57031,3.18808,3.84939,4.75629,5.74033,7.10751,8.29979,9.76526,11.566,13.4148,15.3214,17.4012,19.0553,20.6016,22.1059,24.1747,26.1596,28.2444,30.5634,32.6651,35.5605,38.8507,42.4355,46.7386,52.3027,58.5029,65.3749,72.3224,79.28,86.7779,94.5558,103.083,112.212,121.928,131.639,141.301,149.848,157.692,164.062,169.593,174.416,178.915,-177.012,-173.465,-169.901,-166.809,-164.365,-163.089,-163.396,-166.138,-171.994,177.819,163.362,142.577,116.43,88.1885,56.2106,33.143},
  {123.492,138.984,153.482,168.13,-177.707,-163.765,-150.363,-137.894,-125.979,-115.154,-105.047,-96.0702,-87.9701,-81.5406,-75.82,-70.5822,-65.5612,-60.3276,-55.4065,-50.3248,-45.6174,-41.3451,-36.7534,-32.6458,-28.8181,-25.7003,-22.9771,-20.3072,-18.5487,-16.9817,-15.7593,-14.4334,-13.475,-12.4279,-11.1949,-9.92585,-8.55362,-7.07485,-5.63649,-4.23621,-2.85363,-1.38313,-0.513199,0.71749,1.59241,2.19329,2.51971,2.79977,2.56008,2.36203,1.78191,1.21505,0.388948,-0.173649,-1.13508,-1.38241,-1.30282,-1.49604,-1.6945,-1.86307,-1.40112,-0.737955,0.153667,1.13358,2.61659,4.00601,5.50332,6.31623,7.15584,7.67776,7.68647,7.63238,7.2043,6.74131,6.09552,5.01134,4.16295,2.96236,2.15577,1.15959,1.77636e-15,-1.22959,-2.19797,-3.05886,-3.30485,-3.81704,-3.93202,-3.89351,-4.1473,-4.08018,-3.97347,-3.16656,-1.95167,-1.11428,-0.356429,0.353404,0.827587,2.02944,2.76798,3.25146,4.02677,4.42391,4.68102,5.19876,5.7675,6.5501,7.5778,8.63634,9.60425,10.7201,12.031,13.1798,14.3944,15.9355,16.8733,18.1305,19.0489,20.0327,21.4036,22.8922,24.2648,25.8936,27.8485,29.8959,32.8986,36.2488,40.2829,45.2399,51.4761,58.7977,66.3562,74.091,82.4052,90.7938,100.004,109.16,118.686,128.434,138.198,147.146,155.663,162.988,169.638,175.212,-179.211,-174.3,-169.846,-165.828,-162.004,-159.184,-157.59,-157.865,-159.871,-164.744,-173.501,173.202,154.516,125.91,89.9233,53.3455,23.0154},
  {110.072,129.68,147.128,163.586,179.158,-165.933,-152.189,-139.556,-128.08,-117.618,-108.028,-99.2297,-91.5835,-85.0806,-79.4587,-74.2775,-69.2522,-64.4063,-59.4157,-54.57,-50.0168,-45.4393,-40.7701,-36.444,-32.5525,-29.3461,-26.3169,-23.3661,-21.1533,-19.4845,-17.9784,-16.7903,-15.5822,-14.4168,-13.2017,-11.8403,-10.556,-9.26432,-7.62251,-6.06976,-4.84706,-3.44406,-2.07202,-1.08891,-0.113375,0.761793,1.12379,1.06136,1.3304,0.997395,0.663012,0.0355204,-0.425822,-1.26051,-1.86852,-2.16437,-2.26088,-2.38246,-2.53616,-2.61907,-2.36326,-1.73436,-0.751203,0.192175,1.72905,3.12543,4.34881,5.46269,6.51516,6.97714,7.04852,7.0507,6.76428,6.36005,5.60913,4.82051,3.91569,2.95317,2.27124,1.10202,-1.77636e-15,-1.06232,-1.99524,-2.72157,-3.16579,-3.64101,-3.90533,-3.82265,-3.98898,-4.0968,-3.93532,-3.20954,-2.52452,-1.39039,-0.788709,-0.0734713,0.637847,1.28326,2.21364,2.83787,3.4039,3.82479,4.20815,4.67283,5.46169,5.96123,6.98013,7.94166,8.98568,10.025,11.0251,12.1741,13.3508,14.7847,15.9437,17.051,18.0782,19.342,20.5847,22.3604,23.9633,25.8164,27.9916,30.6535,33.8991,37.7597,41.9605,47.0576,53.3861,60.6177,67.2815,74.0495,80.8623,87.8625,95.1186,102.56,110.695,119.901,130.102,140.781,152.221,163.513,174.062,-176.066,-166.861,-159.14,-152.249,-146.421,-142.287,-139.481,-138.916,-140.329,-144.352,-151.129,-161.426,-175.163,165.966,145.347,121.654,100.361,76.3989},
  {138.281,152.985,167.021,-179.068,-165.378,-152.334,-139.333,-127.115,-115.541,-104.963,-95.5277,-87.0317,-79.592,-73.6688,-68.1065,-63.1303,-58.1594,-53.1472,-47.996,-43.0986,-38.658,-34.5348,-30.2833,-26.6767,-23.3597,-20.7376,-18.1185,-15.7754,-13.9924,-12.8745,-11.5661,-10.4051,-9.55424,-8.51572,-7.26305,-6.02801,-4.56909,-3.26821,-1.74459,-0.369392,0.941737,2.1005,2.9108,3.72968,4.56236,4.98838,5.12739,5.02073,5.02232,4.4894,3.98128,3.19496,2.48899,1.72952,1.05567,0.940355,0.78654,0.924175,0.680701,0.863386,1.05559,1.7894,2.5266,3.32281,4.74771,5.93152,6.90372,7.59973,8.29923,8.48694,8.36025,8.14065,7.80006,7.13306,6.25737,5.23027,4.14798,3.23978,2.24709,1.31119,-0,-1.13359,-2.00119,-2.66308,-3.14698,-3.30307,-3.46807,-3.27806,-3.07686,-3.05965,-2.85905,-1.68511,-0.658275,0.59046,1.4867,2.02486,2.86379,3.59532,4.5659,5.15586,5.57526,5.97369,6.21703,6.87488,7.28313,8.1189,8.90924,9.66049,10.4151,11.0444,11.9731,12.8355,13.6986,14.6125,15.299,16.0918,16.6052,17.4614,18.3491,19.6059,20.9241,22.5952,24.3871,26.6015,29.5431,32.8243,36.6539,41.5097,47.2736,54.1745,61.1021,68.5602,76.0895,84.2866,92.721,101.748,111.304,121.041,130.758,140.107,148.397,155.204,161.251,166.322,170.645,174.405,178.272,-178.096,-174.708,-171.292,-168.693,-167.143,-167.205,-170.029,-175.962,173.21,156.999,134.787,109.033,78.9726,48.1996},
  {99.6691,120.661,139.199,155.862,171.54,-173.513,-159.202,-145.683,-133.095,-121.353,-110.655,-101.084,-92.6693,-85.699,-79.5497,-74.3146,-68.8827,-63.9417,-58.4336,-53.3795,-48.5685,-43.9174,-39.3387,-35.1689,-31.1742,-28.1043,-25.0369,-22.5029,-20.3722,-18.7156,-17.3178,-15.9567,-14.834,-13.7521,-12.5361,-11.0411,-9.50614,-8.20496,-6.16469,-4.73943,-3.18825,-1.93559,-0.687457,0.437014,1.37352,1.96626,2.35208,2.60319,2.37315,2.0951,1.5507,1.09955,0.505438,-0.284934,-1.00945,-1.09096,-1.19852,-1.05509,-1.20615,-1.0557,-0.651768,0.23709,1.06904,2.04589,3.75593,4.96328,6.07223,6.83738,7.56162,7.99567,7.94922,7.67007,7.45532,6.65906,5.87131,4.94816,4.06971,2.95096,2.25,1.09745,-0,-1.21585,-2.099,-2.62756,-3.02891,-3.40826,-3.44011,-3.23456,-3.37262,-3.09277,-2.97682,-2.00455,-0.934885,0.292073,1.01875,1.613,2.40566,3.21327,3.87677,4.79476,5.12856,5.3706,5.86962,6.25071,6.99403,8.02407,8.7131,9.72341,10.4845,11.47,12.5414,13.778,15.0433,16.3035,17.5025,18.469,19.4344,20.5054,21.9659,23.385,25.101,26.9931,29.1608,31.6374,34.8125,38.5429,42.7982,48.2021,54.3972,61.6417,68.4952,75.5353,82.6325,90.0774,97.9461,106.516,115.71,125.439,136.325,147.545,158.718,169.418,179.942,-170.355,-161.123,-152.294,-144.502,-137.702,-132.58,-129.256,-127.627,-127.599,-129.752,-133.758,-139.869,-147.553,-160.183,-175.492,162.926,133.587,96.6876},
  {116.291,138.1,156.552,172.409,-173.352,-160.316,-147.432,-135.256,-123.623,-112.392,-102.198,-92.8734,-84.8805,-78.3944,-72.7074,-67.4138,-62.4941,-57.3682,-52.6267,-47.9128,-43.3552,-39.2352,-34.7997,-30.9272,-27.2263,-24.1785,-21.5668,-19.0841,-17.0117,-15.7698,-14.3837,-13.4564,-12.2031,-11.2939,-10.2768,-8.83971,-7.31152,-6.00192,-4.2411,-2.51841,-0.937363,0.573299,1.72573,2.93089,4.20928,4.8866,5.25793,5.55312,5.45147,5.01068,4.51263,3.82887,3.02041,2.2355,1.30821,0.998535,0.97205,0.730235,0.234789,0.326524,0.355568,1.02172,1.57538,2.44816,3.74855,5.08573,6.15842,6.7481,7.36288,8.01657,7.86195,7.86994,7.46522,7.00121,6.07359,5.00008,4.18176,3.11405,2.26273,1.15302,1.77636e-15,-1.18062,-2.07313,-2.50915,-2.96136,-2.96138,-2.95899,-2.85701,-2.54612,-2.11454,-1.81295,-0.59147,0.759485,2.22807,2.89878,3.65071,4.32234,4.8651,5.70004,6.34367,6.57847,6.91955,6.82108,7.04601,7.32182,7.95012,8.48998,9.01005,9.53696,10.3422,11.1824,12.2028,13.0385,14.1779,15.2349,16.2654,16.6412,17.7673,18.9871,20.1012,21.8003,23.1606,24.9603,26.9725,29.4824,32.3504,35.9129,40.1133,45.4677,51.9714,59.013,66.4297,74.0493,82.2963,90.8227,99.6724,108.594,118.517,127.918,137.381,146.219,153.976,161.26,167.435,173.296,178.655,-176.031,-171.114,-166.378,-162.237,-159.461,-158.263,-159.835,-164.523,-172.47,174.25,156.435,133.189,109.719,84.6979,58.4216},
  {135.983,155.016,172.02,-172.495,-157.923,-144.476,-131.792,-119.948,-108.908,-99.3066,-90.3289,-82.6343,-75.7838,-70.321,-65.3589,-60.9113,-56.1089,-51.1503,-46.1639,-41.2986,-36.9899,-32.9668,-28.6897,-24.9069,-21.8637,-18.8675,-16.8469,-14.6962,-13.0985,-12.0005,-11.0168,-10.1454,-9.22919,-8.38807,-7.27994,-5.94311,-4.09996,-2.91714,-1.19332,0.475736,1.94827,3.36733,4.44978,5.49119,6.53147,6.97559,7.11654,7.09055,6.79184,6.31057,5.59827,4.74531,3.70804,2.92319,1.96846,1.63003,1.4882,1.4058,1.23875,1.27389,1.66037,2.2194,3.06897,4.03575,5.47142,6.85569,8.06217,8.80294,9.19622,9.55179,9.57556,9.20114,8.98881,7.99618,7.01786,5.69873,4.5914,3.67928,2.49965,1.32513,-0,-1.13333,-2.06515,-2.76718,-2.98541,-3.30933,-3.20216,-3.23468,-3.06981,-2.66924,-2.43976,-1.3272,-0.321475,1.27061,1.85049,2.70371,3.26856,4.19752,4.6434,5.10748,5.40887,5.64816,5.67153,5.95748,6.42781,7.06052,7.94211,8.89863,9.61202,10.7128,11.8068,12.8801,14.4526,15.5761,16.7174,17.802,18.763,19.7108,20.8988,22.3081,23.8963,25.4478,27.3135,29.5537,32.3688,35.6756,39.658,44.9394,50.9688,58.6193,66.1155,74.2107,82.2464,90.8817,99.7068,108.872,117.998,127.225,136.252,144.489,151.706,158.043,163.618,168.585,172.934,177.155,-178.84,-174.818,-170.902,-167.337,-165.052,-164.414,-165.903,-170.886,-179.996,166.49,145.989,124.408,99.2739,75.0787,51.4722},
  {123.59,139.912,155.129,170.255,-175.249,-161.082,-147.74,-135.408,-123.618,-113.11,-103.144,-94.247,-86.188,-79.6474,-73.9707,-68.4919,-63.3629,-58.1466,-52.9674,-48.1459,-43.4108,-39.2411,-34.99,-31.2157,-27.4457,-24.6391,-21.794,-19.5504,-17.7808,-16.2969,-14.9532,-13.841,-12.8553,-11.8008,-10.7071,-9.28802,-7.84395,-6.3878,-4.80907,-3.13827,-1.56879,-0.312856,1.03787,2.05099,3.04324,3.67871,4.21776,4.2117,4.35056,3.88515,3.58033,2.89056,2.25871,1.3819,0.71005,0.394815,0.41148,0.432705,0.0245,0.170445,0.58078,1.01314,1.76437,2.53681,4.13204,5.39478,6.42841,7.10804,7.67758,8.16901,8.02545,7.81668,7.72142,6.90336,6.04549,5.00423,4.07916,3.2662,2.31863,1.28666,-0,-1.02406,-1.83403,-2.30379,-2.45896,-2.55823,-2.44299,-1.88206,-1.41872,-1.11319,-0.86465,0.458315,1.67758,3.00979,3.68404,4.43426,4.93872,5.64393,6.24213,6.75751,6.95737,7.40301,7.27759,7.86544,8.27822,9.11729,9.66762,10.9351,11.9866,13.2704,14.2983,15.7063,17.4935,19.1363,20.6978,22.2604,23.7535,25.1642,27.1946,29.0723,31.1454,33.4055,35.9567,39.0702,42.6587,47.0391,51.9378,58.0894,65.2162,73.095,80.8902,89.3724,97.4819,106.646,115.63,124.786,133.962,142.311,150.467,156.926,162.646,167.865,172.229,176.261,179.773,-176.806,-173.96,-171.67,-170.319,-171.206,-174.308,179.654,167.386,150.265,125.328,92.3889,58.9365,30.8642,7.65711,-10.0098,-24.3704},
  {138.761,153.618,168.18,-177.608,-163.874,-150.522,-137.524,-124.956,-113.331,-102.571,-93.2812,-84.8013,-77.9264,-72.5484,-67.7552,-63.2078,-58.3608,-53.5215,-48.0516,-43.2467,-38.7682,-34.1478,-29.5759,-25.6252,-22.4088,-19.4708,-17.1763,-14.9876,-13.8199,-12.8733,-12.1429,-11.2697,-10.7254,-10.0229,-8.98836,-7.45609,-5.77562,-4.17747,-1.78687,0.0744404,1.73475,3.3004,4.71836,6.298,7.265,8.07542,8.37875,7.9532,7.79226,6.86472,6.16657,5.06266,3.9386,2.76875,1.53517,1.16485,0.89904,0.695523,0.651756,0.980929,1.32783,2.44809,3.62325,4.8673,6.99865,8.78311,10.0479,10.9744,11.3946,12.0641,11.6466,11.3232,10.7336,9.49173,8.24788,6.48184,5.18859,4.15024,2.81939,1.31975,-1.77636e-15,-1.61655,-2.60389,-3.03634,-3.14969,-3.14764,-2.88958,-2.11803,-1.44498,-0.823024,-0.131771,1.59296,3.38986,4.9589,5.89067,6.48185,6.9362,7.25205,7.7266,7.68539,7.34724,7.07895,6.82367,6.5165,6.58953,7.36022,7.69938,8.55091,9.26472,10.2754,11.7659,13.1451,14.8463,16.7321,18.3933,19.697,21.2122,22.3264,24.1187,25.7835,27.5353,29.3223,31.0079,33.3539,36.2531,39.7382,43.7945,48.939,55.7115,63.422,70.1987,76.917,83.7507,90.5848,97.7399,105.591,113.66,122.56,132.095,142.058,152.102,161.759,171.089,-179.936,-171.274,-162.675,-154.668,-147.667,-141.66,-137.935,-135.526,-136.039,-138.349,-143.324,-151.282,-162.209,-177.291,160.395,132.03,97.2776,65.0557},
  {135.867,153.692,169.313,-176.921,-164.041,-151.927,-139.909,-128.603,-117.292,-106.724,-96.9587,-88.2569,-80.8449,-75.0387,-69.9255,-65.3496,-60.5926,-55.7436,-50.363,-45.6988,-41.1022,-37.0639,-32.3675,-28.324,-24.7358,-21.971,-19.0124,-16.8636,-15.387,-14.2739,-13.2483,-12.442,-11.6045,-10.8153,-9.66494,-8.18283,-6.65022,-4.96788,-3.15621,-1.20619,0.600053,2.11109,3.54475,4.90078,5.81297,6.69577,6.92729,6.86806,6.67292,6.07029,5.38255,4.30953,3.43493,2.25743,1.15374,0.748173,0.64328,0.729927,0.0442233,0.01335,0.435617,1.40027,2.06978,3.13889,4.98649,6.8449,7.90581,8.86361,9.72282,10.0973,9.79133,9.73784,9.30015,8.35805,7.11076,5.75647,4.76407,3.63028,2.62669,1.13669,1.77636e-15,-1.70759,-2.18159,-2.77258,-3.13077,-3.17827,-2.50116,-1.87155,-1.45185,-0.75994,-0.0552333,1.63708,3.32401,5.14364,6.15399,6.885,7.34148,8.10703,8.53859,8.78075,8.87033,8.68944,8.45596,8.51581,8.94181,9.67579,10.2914,10.9674,12.0243,12.9976,14.0104,15.4312,16.9716,18.6386,20.2708,21.6321,22.7469,23.9849,25.8617,27.9922,29.9063,31.9964,34.3625,37.1987,40.7982,44.7409,49.6431,56.1288,63.5217,71.7856,79.8514,87.9826,96.5472,105.325,114.231,123.286,132.213,140.352,147.357,153.297,157.737,161.415,164.313,166.376,167.704,168.66,169.406,169.251,169.133,167.821,164.773,159.305,148.924,132.756,108.319,79.822,48.9817,23.2794,3.38939,-14.1577,-27.0221},
  {120.514,141.88,160.698,177.438,-167.144,-153.209,-140.236,-128.078,-117.049,-106.852,-97.7605,-89.468,-82.247,-76.4206,-71.0495,-65.7167,-60.7018,-55.5248,-50.3992,-45.6147,-41.0307,-37.0464,-32.9358,-29.3116,-25.9631,-23.3176,-20.7481,-18.4208,-16.9245,-15.5224,-14.2979,-12.9949,-11.8949,-11.0829,-9.67374,-8.21318,-6.43297,-5.00149,-3.07749,-1.50366,-0.405549,0.885694,2.10143,3.22597,3.86559,4.38672,4.38597,4.48395,4.2448,3.93334,3.3684,2.90798,2.02802,1.32672,0.85753,0.60005,0.75478,0.80085,0.62896,0.91417,1.7655,2.39742,3.12274,4.19926,5.39788,6.7412,7.17492,7.70734,8.06036,8.45248,8.0659,7.88332,7.35294,6.68616,5.54738,4.7469,4.04712,3.10864,2.02776,1.17148,-8.88178e-16,-1.18218,-1.85696,-2.09214,-2.35892,-2.2894,-2.19628,-1.99096,-1.94174,-1.68432,-1.41109,-0.38662,0.8519,1.8047,2.2938,2.99367,3.35869,4.09282,4.6415,4.97735,5.53454,5.89423,6.01396,6.58385,7.60924,8.68288,9.61426,10.6118,12.2249,13.5512,15.0803,16.8083,18.9024,20.3865,22.4769,23.9994,25.7012,27.3364,29.5366,31.6007,34.1532,36.687,39.5464,43.0281,47.0975,51.9799,57.2708,64.0729,71.5137,79.0611,86.5726,93.9417,102.309,110.929,120.315,130.621,141.586,153.233,164.867,176.568,-171.974,-160.93,-150.471,-141.268,-133.451,-127.589,-123.859,-121.746,-121.557,-122.826,-126.587,-131.616,-138.77,-150.676,-164.265,178.565,151.12,119.59,82.0277,47.4866,21.7762},
  {119.232,138.524,156.131,173.413,-170.932,-156.582,-143.377,-131.641,-120.896,-111.19,-102.388,-94.01,-86.3529,-80.0523,-74.123,-68.7385,-63.6513,-58.4133,-53.3219,-48.2957,-43.6391,-39.3328,-35.1818,-31.0753,-27.605,-24.6077,-21.8765,-19.7366,-17.7067,-16.2372,-14.9926,-13.8934,-12.7501,-11.7928,-10.6581,-9.45598,-7.97182,-6.67025,-5.23901,-3.45826,-2.13733,-1.14447,0.263057,1.07376,2.37384,3.13423,3.65064,3.75147,3.78055,3.65666,3.20583,2.69967,1.95555,1.36077,0.459819,0.59296,0.15659,0.22723,0.0508308,-0.186719,0.276812,0.709892,1.30913,2.25615,3.43507,4.74389,5.67501,6.64303,7.03415,7.35148,7.6211,7.22632,7.16634,6.49526,5.49678,4.7091,3.93182,2.95694,2.08666,1.01328,-0,-1.22138,-1.98886,-2.63134,-3.11992,-3.1255,-3.19238,-2.89686,-2.86234,-2.62282,-2.557,-1.66308,-0.600025,0.540185,1.208,1.89566,2.21081,3.05257,3.4645,3.97235,4.36034,4.2798,4.78972,4.58172,5.00219,5.85439,6.32583,7.08689,8.00226,8.93191,10.1479,11.358,12.6876,14.5222,16.1065,17.27,18.6885,19.8865,21.8627,23.4925,25.1495,27.2134,29.2913,31.4839,34.3781,37.5702,41.2292,45.5986,51.5807,57.9804,64.8864,72.306,80.4307,88.8801,97.8631,106.807,116.35,125.345,133.898,141.307,148.073,153.56,157.925,161.975,165.086,168.024,170.664,173.477,175.993,178.635,-179.113,-177.233,-176.86,-179.096,176.099,166.883,152.438,132.811,107.619,82.7214,56.2114},
  {100.28,121.556,140.24,156.858,172.596,-172.183,-158.104,-144.899,-132.419,-121.369,-111.134,-101.79,-93.4804,-86.4302,-80.403,-74.7009,-69.4587,-64.1607,-58.9649,-53.7337,-49.1306,-44.3481,-39.5787,-35.3872,-31.2966,-28.0521,-24.9853,-22.3221,-20.0804,-18.2973,-17.1393,-15.6577,-14.575,-13.5372,-12.1206,-10.9786,-9.64258,-8.23985,-6.62578,-5.26076,-3.56623,-2.38192,-0.952705,0.212568,1.24637,2.12424,2.74485,2.58946,2.85709,2.62211,2.2144,1.82955,1.10663,0.420442,-0.326908,-0.536069,-0.95595,-0.638601,-0.912722,-1.31525,-0.677383,-0.398524,0.530735,1.09974,2.79035,4.01306,5.22697,5.94718,6.85189,7.373,7.51271,7.43032,7.08933,6.59704,5.78444,4.86115,3.99276,3.11567,2.13448,1.22489,-0,-1.18409,-2.06778,-2.63017,-2.90976,-3.39275,-3.40045,-3.21384,-3.42603,-3.15062,-2.79101,-1.8731,-0.87139,0.376739,1.26654,1.80581,2.47445,3.11097,4.02384,4.68465,5.14994,5.22207,5.71118,5.60662,6.34586,6.89911,7.72035,8.45988,9.60858,10.2593,11.6016,12.3921,14.0592,15.4455,16.9107,18.1242,19.0989,20.3118,21.9019,23.3573,25.0758,26.839,28.6407,31.0535,33.9648,37.386,41.0264,46.0719,51.8256,58.7969,65.828,73.3774,80.9086,89.1861,97.9066,106.906,116.037,125.128,134.067,142.367,149.651,155.844,161.276,165.715,169.867,173.212,176.456,179.489,-177.531,-174.714,-173.198,-172.557,-174.289,-179.615,171.49,156.212,135.303,107.251,76.9209,44.8214,19.6861},
  {123.905,144.056,162.045,178.614,-165.922,-152.008,-138.966,-126.958,-116.079,-106.381,-97.4425,-89.5473,-82.3937,-76.5384,-71.2145,-66.3051,-61.5993,-56.4969,-51.524,-46.693,-42.2454,-37.9879,-33.6323,-29.6202,-26.0781,-23.258,-20.432,-18.0985,-16.4675,-15.0762,-13.8998,-12.8219,-11.6521,-10.8352,-9.70199,-8.26939,-6.87415,-5.46158,-4.35303,-2.48932,-1.35577,-0.243544,0.849506,1.85832,2.54265,3.26991,3.55567,3.5257,3.38669,3.10029,2.6499,1.87002,1.12869,0.340177,-0.356782,-0.435121,-0.70304,-0.636499,-0.852658,-0.635668,-0.100577,0.783314,1.56444,2.54748,4.35611,5.41884,6.54477,7.2661,7.99283,8.46936,8.39209,8.13542,7.79475,7.21278,6.15391,5.21065,4.18428,3.22301,2.24114,1.08487,-0,-1.23487,-2.21104,-2.69001,-3.42458,-3.78525,-3.84992,-3.90038,-3.74595,-3.73092,-3.71279,-2.73476,-1.62263,-0.792259,0.269162,0.859312,1.58999,2.64552,3.25807,3.84538,4.49559,4.93197,5.20105,5.7301,6.51186,7.3451,8.26874,9.32773,10.3092,11.2938,12.2023,13.1222,14.3817,15.5348,16.6112,17.4074,18.3061,19.0884,20.2532,21.66,23.0293,24.7269,26.63,28.8628,31.8448,34.936,39.2314,44.0887,50.3802,57.7262,65.4147,73.2214,81.5576,90.2468,99.2835,108.685,118.971,129.655,140.893,152.3,163.24,173.614,-176.469,-167.462,-158.649,-151.052,-144.437,-139.283,-135.565,-133.807,-134.242,-136.806,-141.695,-148.765,-160.514,-175.369,164.392,136.868,107.406,79.6568,52.3615},
  {101.706,123.283,141.473,158.036,173.468,-171.71,-157.853,-144.837,-132.403,-121.118,-110.609,-101.171,-93.0865,-86.1883,-80.236,-74.7893,-69.5848,-64.59,-59.5709,-54.641,-49.7947,-45.504,-40.5494,-36.2295,-32.399,-29.1288,-26.1612,-23.308,-21.448,-19.6925,-18.3642,-17.0418,-15.8072,-15.0046,-13.7949,-12.5088,-10.9089,-9.57662,-7.9671,-6.12899,-4.5918,-3.12463,-1.89018,-0.497492,0.786617,1.52839,1.9154,2.18952,2.36944,2.17922,1.54074,1.2833,0.324018,-0.391384,-1.25208,-1.40221,-1.4831,-1.67264,-1.83822,-1.9058,-1.90581,-1.053,-0.487203,0.427175,2.03035,3.54443,4.59921,5.76959,6.51516,7.05854,7.06622,7.0684,6.66548,6.39595,5.61593,4.67101,4.02489,2.97037,1.96314,1.02962,1.77636e-15,-1.05742,-1.98454,-2.41657,-2.82639,-2.79201,-2.90193,-2.43465,-2.26978,-2.0445,-1.44192,-0.330843,1.17296,2.84659,3.69106,4.62121,5.53332,6.47276,7.23972,8.17679,8.89658,9.23209,9.69215,10.0843,10.952,11.9266,12.7194,13.9837,14.9688,15.9567,17.1365,18.3067,19.4961,20.8905,21.9967,23.0241,24.0386,25.2195,26.7838,28.6992,30.5982,32.8723,35.4989,38.6605,42.4953,47.2039,52.9097,59.8157,67.9771,76.8037,85.3816,94.1796,102.941,111.997,121.19,130.679,139.681,148.16,155.691,162.103,167.45,171.838,175.542,178.778,-178.499,-176.547,-174.969,-174.444,-175.327,-178.667,175.167,163.438,145.864,123.429,97.6746,69.1492,42.9764,18.7998,0.706677,-14.4687,-26.5217},
  {124.46,143.899,161.042,176.74,-168.572,-154.716,-142.054,-129.684,-118.193,-107.438,-97.6006,-89.0777,-81.6162,-75.3283,-70.1754,-65.0326,-59.8884,-54.805,-49.6465,-44.904,-40.2309,-36.3821,-31.7356,-28.1435,-24.8738,-22.1647,-19.8438,-17.3603,-15.7905,-14.777,-13.6208,-12.758,-11.5937,-10.5658,-9.32493,-8.24348,-6.59547,-5.0253,-3.53692,-1.85911,-0.613591,0.465761,1.6623,2.70256,3.31019,4.16065,4.27907,4.27726,3.92686,3.53444,2.86083,2.23994,1.27899,0.616189,-0.217338,-0.269564,-0.3552,0.0132537,-0.234483,0.187951,0.984625,1.77383,2.70183,4.05874,5.74794,6.95384,7.96425,8.53265,9.06465,9.46746,9.07456,8.70437,8.33597,7.22917,6.25318,5.26918,4.32548,3.09539,2.04539,1.0347,-0,-1.2334,-2.17289,-2.52109,-2.74279,-2.76788,-2.77258,-2.47377,-2.11687,-2.05627,-1.75406,-0.755619,0.560035,1.85721,2.22654,2.88462,3.13509,3.76481,4.42892,4.70175,5.02222,5.24311,5.34943,5.59273,6.13776,7.06722,7.87668,9.02799,9.77966,11.1205,12.7479,13.7806,15.5892,17.0652,18.3867,19.7813,20.9803,22.0615,23.7588,25.5586,27.3452,29.3741,31.7038,34.4768,37.4403,41.3913,45.8536,51.4755,58.4668,66.327,73.7758,81.6724,89.8822,98.2067,107.309,116.912,127.144,138.148,149.636,161.321,172.896,-176.157,-165.697,-156.004,-147.311,-139.807,-134.178,-130.395,-127.994,-127.529,-128.712,-131.773,-136.887,-143.314,-153.645,-166.689,174.611,152.27,123.072,90.8896,51.834},
  {129.357,149.531,167.46,-176.418,-161.908,-148.346,-135.742,-123.729,-112.644,-102.809,-93.5061,-85.5511,-78.8557,-73.1913,-68.3462,-63.9038,-58.9481,-53.9121,-48.6583,-43.7703,-39.015,-34.599,-30.2302,-26.1609,-22.8827,-20.0479,-17.5607,-15.1385,-13.6951,-12.7785,-12.072,-11.0472,-10.1311,-9.54153,-8.33847,-7.00188,-5.36684,-3.53206,-1.5958,0.248724,1.94304,3.24142,4.6254,5.94369,6.69129,7.54931,7.44817,7.1656,6.86878,6.23769,5.41142,4.19508,3.04295,2.02397,1.07051,0.710804,0.3506,0.513106,0.301363,0.264069,0.836905,1.74264,2.98041,4.35287,6.56654,8.28131,9.18087,10.1993,11.1169,11.2653,11.3024,10.8909,10.5986,9.18524,7.9933,6.70067,5.25953,4.1441,2.85817,1.44443,-1.77636e-15,-1.53073,-2.42237,-3.1512,-3.55264,-4.09137,-4.0014,-3.76604,-3.61137,-3.5641,-3.35194,-2.39177,-0.935565,0.433151,1.02682,1.77282,2.23463,2.96361,3.26528,3.63087,3.72584,3.64755,3.43843,3.37512,3.56473,4.4563,5.10139,6.27109,7.30404,8.76559,10.3324,12.1528,14.3374,16.4811,18.3451,20.0614,21.3646,22.6901,24.4631,26.1077,27.8507,29.7306,31.5665,33.8174,36.6421,39.9729,44.1474,49.2193,55.8892,63.1564,70.1639,77.4855,84.5581,92.3127,100.217,108.66,117.912,128.006,138.413,149.448,159.834,169.745,178.76,-172.92,-164.698,-157.238,-150.429,-144.792,-140.24,-137.53,-136.951,-138.586,-142.067,-148.826,-158.42,-171.803,169.607,144.944,114.158,81.7869,51.947},
  {96.6457,118.316,137.308,154.393,170.55,-174.453,-159.964,-146.725,-133.935,-122.126,-111.017,-101.15,-92.2554,-85.0779,-79.1076,-73.658,-68.646,-63.6209,-58.4047,-53.5364,-48.6024,-44.2941,-39.6563,-34.8774,-31.1097,-27.6101,-24.5953,-21.7809,-19.8077,-18.0185,-16.965,-15.6392,-14.7884,-14.0077,-13.2134,-11.7388,-10.3171,-9.11333,-7.33867,-5.69485,-3.80592,-2.27727,-0.591943,1.06979,2.69398,3.91844,4.73516,4.93702,4.73733,4.94278,4.46182,3.71091,2.99944,2.01615,0.932763,0.466842,0.2343,0.0805783,-0.633253,-1.0319,-1.06824,-0.430868,-0.0231,0.865278,2.68073,4.43887,5.66042,6.80147,7.55582,8.46247,8.57582,8.31136,7.97731,7.59206,6.69801,5.48476,4.49561,3.49265,2.7192,1.41845,-3.55271e-15,-1.30115,-2.3356,-3.18736,-3.54681,-3.78046,-3.58411,-3.30946,-3.04431,-2.38897,-2.02522,-0.124368,1.01928,2.92124,3.7702,4.63481,5.30279,6.08562,6.73902,7.17928,7.53009,7.54262,7.21849,7.09182,6.98678,7.07883,7.70568,8.37542,8.95593,9.65572,10.4332,11.7792,13.1546,14.816,16.3909,17.5974,18.8404,20.1516,21.5951,23.3478,24.9128,26.4666,28.1904,30.0834,32.1936,34.9364,38.1787,41.9251,46.9858,53.5426,60.5464,68.1409,76.3453,84.8266,93.5174,102.517,111.531,120.719,129.69,138.631,146.86,154.392,161.206,167.346,172.891,177.952,-176.974,-172.095,-167.49,-163.795,-161.164,-160.536,-162.599,-168.528,-178.511,165.503,144.469,119.823,95.3897,69.8601,43.349}};

  // create PhaseCal values into matrix
  // PhaseCal Value is e ^ + i*phase	(amplitude should be one)
  // I am not sure about the sign, but probalby it has to be +
  Matrix<DComplex> PhaseCal (161,30);
  for (unsigned int i = 0; i < PhaseCal.ncolumn(); i++)
    for (unsigned int j = 0; j < PhaseCal.nrow(); j++)
      PhaseCal.column(i)(j) = exp ( + DComplex(0.,1.) * (phasecals[i][j]/180*3.1415926) );

  cout << "Writing PhaseCal entries..." << endl;
  // Add the values to the CalTAbles
  for (int i = 0; i < MAX_Antennas; i++)
  {
    cout << "Writing values for antenna: " << antennaIDs[i] << endl;

    if (!writer.AddData(PhaseCal.column(i),antennaIDs[i],"PhaseCal",delay_apr_07_start) )
      cerr << "\nERROR while writing field: PhaseCal" << endl;
    if (!writer.AddData(PhaseCalFreq,antennaIDs[i],"PhaseCalFreq",delay_apr_07_start) )
      cerr << "\nERROR while writing field: PhaseCalFreq" << endl;
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
    //writeRoofRefPhases();

    // Rotate Antennagainfaktors for NS-polarised antennas:
    //rotate_antenna_model();

    // Add the measured dispersion of the LOPES 30 filter boxes
    writePhaseCal();

    // write TV reference phase differences
    writeTVRefPhases();

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
