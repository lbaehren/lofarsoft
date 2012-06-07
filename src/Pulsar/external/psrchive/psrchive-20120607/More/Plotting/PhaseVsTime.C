/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhaseVsTime.h"
#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Telescope.h"
#include "Directional.h"

Pulsar::PhaseVsTime::PhaseVsTime ()
{
  ichan = 0;
  ipol = 0;
  use_hour_angle = false;
}

TextInterface::Parser* Pulsar::PhaseVsTime::get_interface ()
{
  return new Interface (this);
}

void Pulsar::PhaseVsTime::prepare (const Archive* data)
{
  PhaseVsPlot::prepare (data);

  unsigned rows = data->get_nsubint();
 
  const IntegrationOrder* order = data->get<IntegrationOrder>();

  if (order) {
    set_yrange( order->get_Index(0), order->get_Index(rows-1) );
    return;
  }

  double range = (data->end_time() - data->start_time()).in_days();

  const float mjd_hours = 1.0 / 24.0;
  const float mjd_minutes = mjd_hours / 60.0;
  const float mjd_seconds = mjd_minutes / 60.0;

  time_string = "Approximate Elapsed Time ";
  if (range > 1.0) {
    time_string += "(days)";
    set_yrange (0, range);
  }
  else if (range > mjd_hours) {
    time_string += "(hours)";
    set_yrange (0, range/mjd_hours);
  }
  else if (range > mjd_minutes) {
    time_string += "(minutes)";
    set_yrange (0, range/mjd_minutes);
  }
  else {
    time_string += "(seconds)";
    set_yrange (0, range/mjd_seconds);
  }

  // Replace with hour angle if requested
  if (use_hour_angle) {

    const Telescope* tel = data->get<Telescope>();
    if (tel) {

      Reference::To<Directional> dir = tel->get_Directional();
      dir->set_source_coordinates(data->get_coordinates());
      time_string = "Hour Angle (hours)";

      double ha0, ha1;
      dir->set_epoch(data->start_time());
      ha0 = dir->get_hour_angle();
      dir->set_epoch(data->end_time());
      ha1 = dir->get_hour_angle();

      ha0 *= 12.0 / M_PI;
      ha1 *= 12.0 / M_PI;

      // In case we're circumpolar
      if (ha1<ha0) ha1 += 24.0;

      set_yrange(ha0, ha1);
    }

    // TODO : use Pointing extension if available?

  }

}

std::string Pulsar::PhaseVsTime::get_ylabel (const Archive* data)
{
  const IntegrationOrder* order = data->get<IntegrationOrder>();
  if (!order)
    return time_string;

  std::string str = order->get_IndexState();
  if (!order->get_Unit().empty())
    str += " (" + order->get_Unit() + ")";
  return str;
}

unsigned Pulsar::PhaseVsTime::get_nrow (const Archive* data)
{
  return data->get_nsubint();
}

const Pulsar::Profile*
Pulsar::PhaseVsTime::get_Profile (const Archive* data, unsigned isubint)
{
  return Pulsar::get_Profile (data, isubint, ipol, ichan);
}
