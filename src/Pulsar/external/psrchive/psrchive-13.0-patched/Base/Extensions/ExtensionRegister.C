




#include <Pulsar/Archive.h>
#include <Pulsar/ObsExtension.h>
#include <Pulsar/FITSHdrExtension.h>
#include <Pulsar/ITRFExtension.h>
#include <Pulsar/Archive.h>
#include <Pulsar/ITRFExtension.h>
#include <Pulsar/Backend.h>
#include <Pulsar/Receiver.h>
#include <Pulsar/WidebandCorrelator.h>
#include <Pulsar/Passband.h>
#include <Pulsar/FluxCalibratorExtension.h>
#include <Pulsar/ProcHistory.h>
#include <Pulsar/PolnCalibratorExtension.h>
#include <Pulsar/DigitiserStatistics.h>
#include <Pulsar/DigitiserCounts.h>
#include <Pulsar/FITSSUBHdrExtension.h>
#include "Pulsar/ExtensionRegister.h"


#include <iostream>



using namespace std;



Pulsar::Extension_register Pulsar::extension_registry;




Pulsar::Extension_register::Extension_register()
{
  push_back( new ObsExtension() );
  push_back( new FITSHdrExtension() );
  push_back( new ITRFExtension() );
  push_back( new Backend() );
  push_back( new Receiver() );
  push_back( new WidebandCorrelator() );
  push_back( new Passband() );
  push_back( new FluxCalibratorExtension() );
  push_back( new ProcHistory() );
  push_back( new PolnCalibratorExtension() );
  push_back( new DigitiserStatistics() );
  push_back( new DigitiserCounts() );
  push_back( new FITSSUBHdrExtension() );
}


