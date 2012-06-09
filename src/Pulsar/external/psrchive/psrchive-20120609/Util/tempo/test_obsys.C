/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "tempo++.h"
#include "tempo_impl.h"

using namespace std;

int main () try
{
  Tempo::load_obsys ();

  for (unsigned i=0; i<Tempo::antennae.size(); i++)
  {
    Tempo::Observatory* obs = Tempo::antennae[i];

    cout << "obs name=" << obs->get_name() << " code=" << obs->get_code()
	 << " itoa=" << obs->get_itoa_code() << endl;
  }

  const Tempo::Observatory* obs = Tempo::observatory ("pks");

  double lat, lon, rad;
  obs->get_sph (lat, lon, rad);

  cout << "lat=" << lat << " lon=" << lon << endl;

  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}
