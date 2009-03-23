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

#include <time.h>
#include <iostream>
#include <string>

#include <crtools.h>
#include <Calibration/CalTableReader.h>

using CR::CalTableReader;

/*!
  \file readLopesCalTable.cc
  
  \ingroup CR_Applications

  \brief for reading LOPES-CalTables
 
  \author Frank Schröder
 
  \date 2008/03/27
    
  <h3>Motivation</h3>

  This application allows to read values from LOPES-Calibration-Tables
  via the console without programming any new code.
  It is not perfect (some values are implemented as constants) but it is
  flexible enough to allow the read out of different CalTable fields at
  different times.

  <h3>Usage</h3>
  \verbatim
  ./readLopesCalTable FieldName Time [Path]
  \endverbatim
  
  <ul>
    <li> FieldName = Name of the Field you want to read (e.g. Delay)
    <li> Time = Time in seconds (linux format), if time = 0 the current time will be used
    <li> Path = Path to LOPES calibration tables (if the default doesn't work)
  </ul>
   
  <h3>Examples</h3>

  \verbatim
  ./readLopesCalTable Delay 0
  (reads the current value for the delay)
  ./readLopesCalTable Position 978350400 /home/horneff/lopescasa/data/LOPES/LOPES-CalTable
  (reads the position at the start date of LOPES 10, using the table at the specified path)
  \endverbatim  
*/


int main (int argc, char *argv[])
{
  // define constants for the start date of LOPES 10 and LOPES 30
  const unsigned int LOPES10_start = 978350400;
  const unsigned int LOPES30_start = 1104580800;
  
  try
  {
    // Check correct number of arguments (2 or 3 + program name = 3 or 4)
    if ((argc < 3) || (argc > 4))
    {
      std::cerr << "Wrong number of arguments in call of \"readLopesCalTable\". The correct format is:\n";
      std::cerr << "readLopesCalTable FieldName time [Path to Caltables]\n";
      std::cerr << "for example:\n";
      std::cerr << "readLopesCalTable Delay 1104580800\n" << std::endl;
      std::cerr << "time = 0 (or nonsense) means that the current time is used." << std::endl;
      std::cerr << "If no CalTable-path is given, the default will be used.\n" << std::endl;
      return 1;				// Exit the program
    }
       
    // assign the arguments (fieldname and time)
    string fieldName(argv[1]);
    
    // if time argument is not an positive integer, use the time of today
    int temp = -1;
    string timeString(argv[2]);
    stringstream(timeString) >> temp; 
    unsigned int date = time(NULL);	// default time: now
    if (temp > 0) date = temp;
    
    // Set maximum Antenna Numbers: (print warning if time before start of LOPES 10 is requested.)
    int maxAntennas = 30;		// default: LOPES 30
    if (date < LOPES30_start) maxAntennas = 10;
    if (date < LOPES10_start) 
    {
      std::cerr << "\nWarining: Requested time seems to be earlier than the start of LOPES 10!" << endl;
    }
   
    // Default CalTable-Path
    string CalTablePath=caltable_lopes; 	
    // Check if there are 4 arguments: The last one is the path to the Caltables
    if (argc == 4) CalTablePath.assign(argv[3]);
       
    // Create Vector of antenna IDs
    vector<int> antennaIDs;
    
    // First Data for Antenna 1-10 at time 978350400
    antennaIDs.push_back(10101);
    antennaIDs.push_back(10102);
    antennaIDs.push_back(10201);
    antennaIDs.push_back(10202);
    antennaIDs.push_back(20101);
    antennaIDs.push_back(20102);
    antennaIDs.push_back(20201);
    antennaIDs.push_back(20202);
    antennaIDs.push_back(30101);
    antennaIDs.push_back(30102);
    
    // First Data for Antenna 11-30 at time 1104580800
    antennaIDs.push_back(40101);
    antennaIDs.push_back(40102);
    antennaIDs.push_back(40201);
    antennaIDs.push_back(40202);
    antennaIDs.push_back(50101);
    antennaIDs.push_back(50102);
    antennaIDs.push_back(50201);
    antennaIDs.push_back(50202);
    antennaIDs.push_back(60101);
    antennaIDs.push_back(60102);
    
    antennaIDs.push_back(70101);
    antennaIDs.push_back(70102);
    antennaIDs.push_back(70201);
    antennaIDs.push_back(70202);
    antennaIDs.push_back(80101);
    antennaIDs.push_back(80102);
    antennaIDs.push_back(80201);
    antennaIDs.push_back(80202);
    antennaIDs.push_back(90101);
    antennaIDs.push_back(90102);
    
    // Initialise CalTableReader    
    CalTableReader table;
      
    // open the table
    table.AttachTable(CalTablePath);
    
    // general Keyword-Check (observatory should be LOPES, user can check it.)
    String observatory;
    
    if (!table.GetKeyword("Observatory", &observatory)) 
    {
      cerr << "Error while retrieving data" << endl;
      return 1; // exit program
    }
    cout << "Data are valid for the following observatory: "
         << observatory << "\n";
   
    // Find the typ of the field
    string fieldType = table.GetFieldType(fieldName);
    if ( fieldType == "" ) 
    {
      cerr << "\nError: Field \"" << fieldName << "\" does not exist.\n" << endl;
      return 1; // exit program
    }

    cout << "Type of the requested field is: "
         << fieldType << "\n";
	 
    // create read out variables:
    String string_value;
    Double double_value;
    DComplex DComplex_value;
    Array<Double> Darray_value;
    Array<DComplex> Carray_value; 
 
    // Reading the values  
    cout << "The value of the field \"" << fieldName << "\" for the time " << date << " is: " << endl;          
      
    // Loop through antennas and get the data (depending of the type)
    for (int j = 0; j < maxAntennas ; j++)
    {
      if (fieldType == "String")
      {
         if (!table.GetData(date, antennaIDs[j], fieldName, &string_value)) 
         {
           cerr << "Error while retrieving data" << endl;
         } else
	 {
           cout << "for antenna " << antennaIDs[j] << ": " << string_value << endl;          
	 }
      } else
      if (fieldType == "Double")
      {
         if (!table.GetData(date, antennaIDs[j], fieldName, &double_value)) 
         {
           cerr << "Error while retrieving data" << endl;
         } else
	 {
           cout << "for antenna " << antennaIDs[j] << ": " << double_value << endl;          
	 }
      } else
      if (fieldType == "DComplex")
      {
         if (!table.GetData(date, antennaIDs[j], fieldName, &DComplex_value)) 
         {
           cerr << "Error while retrieving data" << endl;
         } else
	 {
           cout << "for antenna " << antennaIDs[j] << ": " << DComplex_value << endl;          
	 }
      } else
      if (fieldType == "Array<Double>")
      {
         if (!table.GetData(date, antennaIDs[j], fieldName, &Darray_value)) 
         {
           cerr << "Error while retrieving data" << endl;
         } else
	 {
           cout << "for antenna " << antennaIDs[j] << ": " << Darray_value << endl;          
	 }
      } else
      if (fieldType == "Array<DComplex>")
      {
         if (!table.GetData(date, antennaIDs[j], fieldName, &Carray_value)) 
         {
           cerr << "Error while retrieving data" << endl;
         } else
	 {
           cout << "for antenna " << antennaIDs[j] << ": " << Carray_value << endl;          
	 }
      } else // if the type was not found, exit the program
      {	
          cerr << "\nThe field type '" << fieldType << "' is currently not supported.\n" << endl;
	  return 1;
      }
    }
  } 
  catch (AipsError x) 
  {
      cerr << "readLopesCalTable: " << x.getMesg() << endl;
      return 1;
  };
       
  return 0;     
}       
