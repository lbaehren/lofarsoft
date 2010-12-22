/**************************************************************************
 *  IO module for the CR Pipeline Python bindings.                        *
 *                                                                        *
 *  Copyright (c) 2010                                                    *
 *                                                                        *
 *  Martin van den Akker <martinva@astro.ru.nl>                           *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/


// ========================================================================
//
//  Wrapper Preprocessor Definitions
//
// ========================================================================

//-----------------------------------------------------------------------
//Some definitions needed for the wrapper preprosessor:
//-----------------------------------------------------------------------
//$FILENAME: HFILE=mIO_AERA.def.h
//-----------------------------------------------------------------------

// ========================================================================
//
//  Header files
//
// ========================================================================

#include "core.h"
#include "mModule.h"
#include "mIO_AERA.h"

#include "AERA/Datareader.h"
#include "AERA/Data/Header.h"
#include "AERA/Data/Event.h"
#include "AERA/Data/LocalStation.h"

// ========================================================================
//
//  Implementation
//
// ========================================================================

using namespace std;

#undef HFPP_FILETYPE
//--------------------
#define HFPP_FILETYPE CC
//--------------------


// ========================================================================
//$SECTION: IO AERA functions
// ========================================================================

//$DOCSTRING: Function to open a file based on a filename and returning an AERA datareader object.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFileOpen
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (AERADatareader)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (HString)(Filename)()("Filename of file to open including full directory name")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
AERA::Datareader& HFPP_FUNC_NAME (const HString Filename)
{
  AERA::Datareader* dr_ptr = NULL;

  dr_ptr = new AERA::Datareader();

  if (dr_ptr != NULL) {
    dr_ptr->open(Filename);
    dr_ptr->read();
  }

  return *dr_ptr;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Function to close an AERA data file.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFileClose
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;

  status = dr.close();

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"



//$DOCSTRING: Get attribute of an AERA data file.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAGetAttribute
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HPyObject)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HString)(key)()("Key name of parameter")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
HPyObject HFPP_FUNC_NAME (AERA::Datareader& dr, const std::string keyValue)
{
  std::string key = keyValue;
  HPyObject pyObj = HPyObject(NULL);
  AERA::Data::Header* header_ptr = dr.header();
  stringToLower(key);

  // ___________________________________________________________________
  //                                                                File

  if ( "filename" == key ) { //                            Filename
    std::string result = dr.getFileName();
    return HPyObject(result);
  } else if ( "nevents" == key ) { //              Number of events
    int result = dr.nEvents();
    return HPyObject(result);
  } else if ( NULL != header_ptr ) {
    if ( "runnumber" == key ) { //                       Run number
      int result = header_ptr->getRunNumber();
      return HPyObject(result);
    } else if ( "runmode" == key ) { //                    Run mode
      int result = header_ptr->getRunMode();
      return HPyObject(result);
    } else if ( "fileserialnr" == key ) { //         File serial nr
      int result = header_ptr->getFileSerialNumber();
      return HPyObject(result);
    } else if ( "firsteventnr" == key ) { //     First event number
      int result = header_ptr->getFirstEvent();
      return HPyObject(result);
    } else if ( "firsteventtime" == key ) { //     Time first event
      std::string result = header_ptr->getFirstEventTime();
      return HPyObject(result);
    } else if ( "lasteventnr" == key ) {  //      Last event number
      int result = header_ptr->getLastEvent();
      return HPyObject(result);
    } else if ( "lasteventtime" == key ) {  //      Time last event
      std::string result = header_ptr->getLastEventTime();
      return HPyObject(result);
    }
  }

  // ___________________________________________________________________
  //                                                               Event

  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::EventHeader* eventhdr_ptr = NULL;

  if ( NULL != event_ptr ) {
    if ( "nlocalstations" == key ) { //    Number of local stations
      AERA::Data::Event* event_ptr = dr.currentEvent();
      int result = event_ptr->nLocalStations();
      return HPyObject(result);
    }
    eventhdr_ptr = event_ptr->eventHeader();
    if ( NULL != eventhdr_ptr ) {
      if ( "eventlength" == key ) { //                 Event length
        int result = eventhdr_ptr->getEventLength();
        return HPyObject(result);
      } else if ( "eventid" == key ) { //                  Event ID
        int result = eventhdr_ptr->getEventID();
        return HPyObject(result);
      } else if ( "t3" == key ) { //                    T3 event ID
        int result = eventhdr_ptr->getT3eventID();
        return HPyObject(result);
      } else if ( "timestampsec" == key ) { //        Timestamp (s)
        unsigned int result = eventhdr_ptr->getSeconds();
        return HPyObject(result);
      } else if ( "timestampnanosec" == key ) { //   Timestamp (ns)
        unsigned int result = eventhdr_ptr->getNanoSeconds();
        return HPyObject(result);
      } else if ( "eventtype" == key ) {//        Type of the event
        int result = eventhdr_ptr->getEventType();
        return HPyObject(result);
      }
    }
  } else {
    throw PyCR::ValueError("Event pointer is NULL.");
    return pyObj;
  }

  // ___________________________________________________________________
  //                                                       Local station

  if ( NULL != event_ptr ) {
    AERA::Data::LocalStation* ls_ptr = event_ptr->currentLocalStation();
    if ( NULL != ls_ptr ) {
      if ( "bodylength" == key ) { //          Length of event body
        UINT16 result = ls_ptr->getLength();
        return HPyObject(result);
      } else if ( "localstationid" == key ) { //   Local station ID
        UINT16 result = ls_ptr->getLocalStationID();
        return HPyObject(result);
      } else if ( "hardware" == key ) { //     Hardware description
        std::string result = ls_ptr->getHardware();
        return HPyObject(result);
      } else if ( "triggerflag" == key ) { //          Trigger flag
        UINT16 result = ls_ptr->getTriggerFlag();
        return HPyObject(result);
      } else if ( "triggerposition" == key ) { //  Trigger position
        UINT16 result = ls_ptr->getTriggerPosition();
        return HPyObject(result);
      } else if ( "samplingfreq" == key ) { //   Sampling frequency
        UINT16 result = ls_ptr->getSamplingFrequency();
        return HPyObject(result);
      } else if ( "channelmask" == key ) { //          Channel mask
        UINT16 result = ls_ptr->getChannelMask();
        return HPyObject(result);
      } else if ( "adcresolution" == key ) { //      ADC resolution
        UINT16 result = ls_ptr->getADCResolution();
        return HPyObject(result);
      } else if ( "tracelength" == key ) { //          Trace length
        UINT16 result = ls_ptr->getTraceLength();
        return HPyObject(result);
      }
    } else {
      throw PyCR::ValueError("LocalStation pointer is NULL.");
      return pyObj;
    }
  }

  // ___________________________________________________________________
  //                                                                Test

  if ( "tvectori" == key ) { //                  Test vector integer
    std::vector<HInteger> result(1024,hfnull<HInteger>());
    return HPyObject(result);
  } else   if ( "tvectorf" == key ) { //          Test vector number
    std::vector<HNumber> result(1024,hfnull<HNumber>());
    return HPyObject(result);
  } else   if ( "tvectorc" == key ) { //         Test vector complex
    std::vector<HComplex> result(1024,hfnull<HComplex>());
    return HPyObject(result);
  }

  // ___________________________________________________________________
  //                                                          Other keys

  throw PyCR::KeyError("Key is not defined.");

  if ( "default" == key ) { //                              Default
    cout << "Using the default value (0)" << endl;
    pyObj = HPyObject(hfnull<HInteger>());
  }

  return pyObj;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Time series data for alle channels of current localstation.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAGetADCData
//-----------------------------------------------------------------------
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_UNMUTABLE_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME (AERA::Datareader& dr,
                     const Iter vec, const Iter vec_end)
{
  Iter v_begin = vec;
  Iter v_end   = vec_end;
  int tracelength = 0;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* station_ptr = NULL;

  if ( NULL != event_ptr ) {
    station_ptr = event_ptr->currentLocalStation();
    if ( NULL != station_ptr ) {
      tracelength = station_ptr->getTraceLength();
      for (int channel = 0; channel < N_CHANNELS; ++channel) {
        v_begin = vec + tracelength*channel;
        v_end = v_begin + tracelength;
        if ( (v_begin < vec_end) && (v_end <= vec_end) ) {
          hAERAGetADCData(dr, v_begin, v_end, channel);
        }
      }
    } else {
      cerr << "ERROR: [hAERAGetADCData] LocalStation pointer is NULL." << endl;
    }
  } else {
    cerr << "ERROR: [hAERAGetADCData] Event pointer is NULL." << endl;
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Time series data for specified channel of the current localstation.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAGetADCData
//-----------------------------------------------------------------------
#define HFPP_FUNC_MASTER_ARRAY_PARAMETER 1
#define HFPP_FUNCDEF (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_UNMUTABLE_SCALAR)()(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_1 (HFPP_TEMPLATED_TYPE)(vec)()("Output vector")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_2 (HInteger)(Channel)()("Channel number to read out")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
//$COPY_TO END ----------------------------------------------------------
/*!
  \brief $DOCSTRING
  $PARDOCSTRING
*/
template <class Iter>
void HFPP_FUNC_NAME (AERA::Datareader& dr,
                     const Iter vec, const Iter vec_end,
                     const HInteger channel)
{
  vector<int> ADCChannelData;
  vector<int>::iterator ADC_it;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* station_ptr = NULL;
  Iter vec_it = vec;

  if ( ( 0 <= channel ) && ( N_CHANNELS > channel ) ) {
    if ( NULL != event_ptr ) {
      station_ptr = event_ptr->currentLocalStation();
      if ( NULL != station_ptr ) {
        ADCChannelData = station_ptr->getADCData((unsigned short) channel);
        ADC_it = ADCChannelData.begin();
        while ( ( ADC_it != ADCChannelData.end() ) &&
                ( vec_it != vec_end ) ) {
          *vec_it = (IterValueType)(*ADC_it);
          ++ADC_it; ++vec_it;
        }
      } else {
        cerr << "ERROR [hAERAGetADCData]: Local station pointer is NULL." << endl;
      }
    } else {
      cerr << "ERROR [hAERAGetADCData]: Event pointer is NULL." << endl;
    }
  } else {
    throw PyCR::ValueError("Invalid value of channel number.");
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Set the event pointer to the first event of the file.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFirstEvent
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  Return true if the event pointer was successfully changed, false otherwise.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = NULL;

  event_ptr = dr.firstEvent();
  if ( NULL != event_ptr ) {
    status = hAERAFirstLocalStation(dr);
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the event pointer to the previous event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAPrevEvent
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  Return true if the event pointer was successfully changed, false otherwise.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = NULL;

  if ( ! dr.isFirstEvent() ) {
    event_ptr = dr.prevEvent();

    if ( event_ptr != NULL ) {
      status = true;
    }
  }
  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the event pointer to the next event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERANextEvent
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  Return true if the event pointer was successfully changed, false otherwise.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = NULL;

  if ( ! dr.isLastEvent() ) {
    event_ptr = dr.nextEvent();

    if ( event_ptr != NULL ) {
      status = true;
    }
  }
  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the event pointer to the last event of this file.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERALastEvent
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  Return true if the event pointer was successfully changed, false otherwise.

  \brief $DOCSTRING
  $PARDOCSTRING
*/

HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = NULL;

  event_ptr = dr.lastEvent();
  if ( NULL != event_ptr ) {
    status = hAERAFirstLocalStation(dr);
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the Local Station pointer to the first Local station of this event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFirstLocalStation
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  Return true if the local station pointer was successfully changed, false otherwise.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* localstation_ptr = NULL;

  if ( NULL != event_ptr ) {
    localstation_ptr = event_ptr->firstLocalStation();

    if ( NULL != localstation_ptr ) {
      status = true;
    }
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the local station pointer to the previous local station of the current event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAPrevLocalStation
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  Return true if the local station pointer was successfully changed, false otherwise.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* localstation_ptr = NULL;

  if ( NULL != event_ptr ) {
    if ( ! event_ptr->isFirstLocalStation() ) {
      localstation_ptr = event_ptr->prevLocalStation();

      if ( NULL != localstation_ptr ) {
        status = true;
      }
    }
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the local station pointer to the next local station of the current event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERANextLocalStation
//-----------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END ----------------------------------------------------------
/*!
  Return true if the local station pointer was successfully changed, false otherwise.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* localstation_ptr = NULL;

  if ( NULL !=  event_ptr ) {
    if ( ! event_ptr->isLastLocalStation() ) {
      localstation_ptr = event_ptr->nextLocalStation();

      if ( NULL != localstation_ptr ) {
        status = true;
      }
    }
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Set the local station pointer to the last local station of the current event.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERALastLocalStation
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HBool)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
  Return true if the local station pointer was successfully changed, false otherwise.

  \brief $DOCSTRING
  $PARDOCSTRING
*/
HBool HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  HBool status = false;
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* localstation_ptr = NULL;

  if ( NULL != event_ptr ) {
    localstation_ptr = event_ptr->lastLocalStation();

    if ( NULL != localstation_ptr ) {
      status = true;
    }
  }

  return status;
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


//$DOCSTRING: Print a brief summary of the AERA data file contents and current settings.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAFileSummary
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
void HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  dr.summary();
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Print a brief summary of the AERA data file contents and current settings.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERAEventSummary
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
void HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  AERA::Data::Event* event_ptr = dr.currentEvent();

  if ( NULL != event_ptr ) {
    event_ptr->summary();
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"

//$DOCSTRING: Print a brief summary of the AERA data file contents and current settings.
//$COPY_TO HFILE START --------------------------------------------------
#define HFPP_FUNC_NAME hAERALocalStationSummary
//------------------------------------------------------------------------
#define HFPP_BUILD_ADDITIONAL_Cpp_WRAPPERS HFPP_NONE
#define HFPP_FUNCDEF  (HFPP_VOID)(HFPP_FUNC_NAME)("$DOCSTRING")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)
#define HFPP_PARDEF_0 (AERADatareader)(dr)()("AERA Datareader object")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_REFERENCE)
//$COPY_TO END --------------------------------------------------
/*!
 \brief $DOCSTRING
 $PARDOCSTRING
*/
void HFPP_FUNC_NAME (AERA::Datareader& dr)
{
  AERA::Data::Event* event_ptr = dr.currentEvent();
  AERA::Data::LocalStation* localstation_ptr = NULL;

  if ( NULL != event_ptr ) {
    localstation_ptr = event_ptr->currentLocalStation();

    if ( NULL != localstation_ptr ) {
      localstation_ptr->summary();
    }
  }
}
//$COPY_TO HFILE: #include "hfppnew-generatewrappers.def"


