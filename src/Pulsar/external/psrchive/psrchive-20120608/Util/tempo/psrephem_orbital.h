/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __PSREPHEM_ORBITAL_H
#define __PSREPHEM_ORBITAL_H

#include "psrephem.h"

// TDB corresponding to MJD

double get_bcmjd(double mjd, const Legacy::psrephem& eph, double freq, char site);

// Orbital phase of periastron at bcmjd (0.0 - 1.0))

double get_periphs(double bcmjd, const Legacy::psrephem& eph);

// Binary phase wrt periastron (0.0 - 1.0)

double get_binphs_peri(double mjd, const Legacy::psrephem& eph, double freq, char site);

// Binary phase wrt ascending node (0.0 - 1.0)

double get_binphs_asc(double mjd, const Legacy::psrephem& eph, double freq, char site);

// Binary longitude wrt periastron (true anomaly) (deg)

double get_binlng_peri(double mjd, const Legacy::psrephem& eph, double freq, char site);

// Binary longitude wrt ascending node (true anomaly + omega) (deg)

double get_binlng_asc(double mjd, const Legacy::psrephem& eph, double freq, char site);

const Legacy::psrephem* new_psrephem (const Pulsar::Parameters*);

#endif

