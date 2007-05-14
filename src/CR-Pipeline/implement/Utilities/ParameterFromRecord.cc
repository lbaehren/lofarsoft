/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Lars B&auml;hren (bahren@astron.nl)                                   *
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

/* $Id: ParameterFromRecord.cc,v 1.4 2006/10/31 18:24:08 bahren Exp $*/

#include <Utilities/ParameterFromRecord.h>

namespace CR {
  
  // ============================================================================
  //
  //  Handling of Quantities
  //
  // ============================================================================
  
  bool QuantityFromRecord (Quantity &out,
			   String const &field,
			   casa::GlishRecord const &glishRec)
  {
    bool status (true);
    String error;
    
    // Extract the field from the input record
    
    if (glishRec.exists(field)) {
      // local variables
      casa::GlishRecord record = glishRec.get(field);
      Record rec;
      QuantumHolder x;
      // convert GlishRecord to Record
      record.toRecord (rec);
      // extract quantum from record
      if (!x.fromRecord(error, rec)) {
	std::cerr << error << std::endl;
	status = false;
      } else {
	//
	if (x.isQuantumDouble()) {
	  Quantum<double> quantum = x.asQuantumDouble();
	  out = quantum;
	}
	else {
	  std::cerr << "Illegal quantum type asked" << std::endl;
	  status = false;
	}
      }
    }
    else {
      std::cerr << "No field " << field << " found in input record!" << std::endl;
      status = false;
    }
    
    return status;
  }
  
  // ============================================================================
  //
  //  Handling of Measures
  //
  // ============================================================================
  
  // ----------------------------------------------------------- MEpochFromRecord
  
  bool MEpochFromRecord (MEpoch &out,
			 String const &field,
			 casa::GlishRecord const &glishRec)
  {
    bool status (true);
    
    if (glishRec.exists(field)) {
      casa::GlishRecord record = glishRec.get(field);
      Record rec;
      MeasureHolder x;
      String error;
      //
      record.toRecord (rec);
      if (!x.fromRecord(error, rec)) {
	status = false;
      } else { 
	if (!x.isMEpoch()) {
	  error += String("Illegal Measure type asked");
	  status = false;
	}
	out = x.asMEpoch();
      }
    } else {
      std::cerr << "No field " << field << " found in input record!" << std::endl;
      status = false;
    }
    
    return status;
  }
  
  // ------------------------------------------------------- MDirectionFromRecord
  
  bool MDirectionFromRecord (MDirection &out,
			     String const &field,
			     casa::GlishRecord const &glishRec)
  {
    bool status (true);
    
    if (glishRec.exists(field)) {
      casa::GlishRecord record = glishRec.get(field);
      Record rec;
      MeasureHolder x;
      String error;
      //
      record.toRecord (rec);
      if (!x.fromRecord(error, rec)) {
	status = false;
      } else { 
	if (!x.isMDirection()) {
	  error += String("Illegal Measure type asked");
	  status = false;
	}
	out = x.asMDirection();
      }
    } else {
      std::cerr << "No field " << field << " found in input record!" << std::endl;
      status = false;
    }
    
    return status;
  }
  
  // -------------------------------------------------------- MPositionFromRecord
  
  bool MPositionFromRecord (MPosition &out,
			    String const &field,
			    casa::GlishRecord const &glishRec)
  {
    bool status (true);
    
    if (glishRec.exists(field)) {
      casa::GlishRecord record = glishRec.get(field);
      Record rec;
      MeasureHolder x;
      String error;
      //
      record.toRecord (rec);
      if (!x.fromRecord(error, rec)) {
	status = false;
      } else { 
	if (!x.isMPosition()) {
	  error += String("Illegal Measure type asked");
	  status = false;
	}
	out = x.asMPosition();
      }
    } else {
      std::cerr << "No field " << field << " found in input record!" << std::endl;
      status = false;
    }
    
    return status;
  }
  
} // Namespace CR -- end
