/*
 *  Fitters.h
 *
 *  Created by Oleksandr Usov on 30/10/2007.
 *
 */

#ifndef _FITTERS_H_INCLUDED
#define _FITTERS_H_INCLUDED

#include "MGFunction.h"

bool lmder_fit(MGFunction &fcn, bool final, int verbose);
bool dn2g_fit(MGFunction &fcn, bool final, int verbose);
bool dnsg_fit(MGFunction &fcn, bool final, int verbose);

#endif // _FITTERS_H_INCLUDED
