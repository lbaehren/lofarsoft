/***************************************************************************
 *
 *   Copyright (C) 2009 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BandpassPlot.h"
#include "Pulsar/Passband.h"

#include <vector>
#include <float.h>
#include <cpgplot.h>

using Pulsar::Archive;
using std::cerr;
using std::endl;
using std::vector;

#define CPG_BLACK 1
#define CPG_RED 2

Pulsar::BandpassPlot::BandpassPlot() : crop(0.0) {}

Pulsar::BandpassPlot::~BandpassPlot() {}

void Pulsar::BandpassPlot::prepare(const Archive* archive)
{
    Reference::To<const Passband> passband = archive->get<Passband>();

    if (!passband)
        throw Error(InvalidState, "Pulsar::BandpassPlot::prepare",
                "Could not get bandpass extension");

    means_minmax.first = FLT_MAX;
    means_minmax.second = -FLT_MAX;

    passband_npol = passband->get_npol();
    passband_nchan = passband->get_nchan();
    passbands.resize(passband->get_npol());

    for (unsigned i = 0; i < passbands.size(); ++i) {
        passbands[i] = passband->get_passband(i);

        vector<float>::iterator ptr = passbands[i].begin();
        ++ptr; // skip the first channel - it tends to be junk

        if (passbands[i].size() > passband_nchan) {
          throw Error(InvalidState, "Pulsar::BandpassPlot::prepare",
              "Passband size (%d) > passband nchan (%d) pol=%d",
              passbands[i].size(), passband_nchan, i);
        }

        for (unsigned j = 0; j < passband_nchan; ++j, ++ptr) {
            if (*ptr < means_minmax.first)
                means_minmax.first = *ptr;
            if (*ptr > means_minmax.second)
                means_minmax.second = *ptr;
        }
    }

    // set a scale buffer of 5% of the max
    means_minmax.second *= 1.05;

    get_attributes()->get_label_above()->set_offset(2.0);
    get_frame()->get_y_axis()->set_label("Amplitude (Arbitrary)");
    get_frame()->get_y_scale()->set_minmax(means_minmax.first,
            means_minmax.second);

    if (crop != 0.0) {
        float min, max;
        get_frame()->get_y_scale()->get_minmax(min, max);

        max *= crop;
        if (min < 0 && max + min < 0)
            min = -max;

        std::pair<float,float> coords;
        coords.first = min;
        coords.second = max;

        get_frame()->get_y_scale()->set_world(coords);
    }

    // remove the top x line of box
    get_frame()->get_x_axis()->rem_opt('C');
}

void Pulsar::BandpassPlot::draw(const Archive* archive)
{
    const unsigned nchan = passband_nchan;
    float xs[nchan];

    const float freq = archive->get_centre_frequency();
    const float bw = archive->get_bandwidth();
    float nextX = freq - bw / 2;
    const float stepX = bw / nchan;

    for (unsigned i = 0; i < nchan; ++i, nextX += stepX)
        xs[i] = nextX;

    for (unsigned ipol = 0; ipol < passband_npol; ++ipol) {
        const int cpgColour = ipol == 0 ? CPG_BLACK : CPG_RED;
        cpgsci(cpgColour);
        cpgline(nchan, xs, &passbands[ipol][0]);
    }

    // draw top x-axis label (0 - nchan)
    float x_min, x_max;
    get_frame()->get_x_scale(true)->get_range_external(x_min, x_max);

    float y_min, y_max;
    get_frame()->get_y_scale(true)->get_range_external(y_min, y_max);

    cpgsci(CPG_BLACK);
    cpgswin(0, nchan, y_min, y_max);
    cpgbox("MSTC", 0, 0, "", 0, 0);

    cpgmtxt("RV", 1, 0.95, 0.0, "AA");
    cpgsci(CPG_RED);
    cpgmtxt("RV", 1, 0.9, 0.0, "BB");
}

TextInterface::Parser *Pulsar::BandpassPlot::get_interface() 
{
    return new Interface(this);
}
