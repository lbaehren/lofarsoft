/***************************************************************************
 *   Copyright (C) 2006                                                    *
 *   Andreas Horneffer (<mail>)                                            *
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

/* $Id: timportSimulation.cc,v 1.3 2007/03/09 17:59:00 horneff Exp $*/

#include <casa/aips.h>
#include <casa/Exceptions/Error.h>

#include <IO/ImportSimulation.h>
#include <IO/ImportAntenna.h>


// -----------------------------------------------------------------------------

int main ()
{
  
  ImportSimulation Sim;
  bool ok = Sim.openNewSimulation("30deg1e17_shift",
				  "lopes30",
				  "/home/huege/lopes30runs");

  std::cout << "Read ok: " << ok << endl;
  
  double azi, ele;
  Sim.getDirection(azi,ele);
  std::cout << "Azi: " << azi << "\tEle: " << ele << endl;

  long i = 0;
  const ImportAntenna* pAnt;

  Vector<Double> Ezeni, Eazi;
  
  while (Sim.getNextAntenna(pAnt))
  {
    ++i;
    cout << "Antenna " << i << " has ID: " << pAnt->getID() << endl;

    bool negligibleRemainder = pAnt->getTimeSeries(Eazi, Ezeni);    
    std::cout << "Negligible remainder: " << negligibleRemainder << " Eazi: [" << min(Eazi) << " " 
	      << max(Eazi) << "] Ezeni: [" << min(Ezeni) << " " << max(Ezeni) << "]" <<  std::endl;
  }

  Int blocklen=Ezeni.nelements();
  FILE *allout;
  allout = fopen("tImportSimulation_out.tab","w");
  for (i=0; i< blocklen; i++) {
    fprintf(allout,"\n %f ",i);
    fprintf(allout,"\t %e %e ",Ezeni(i), Eazi(i));
  };
  fprintf(allout,"\n");
  fclose(allout);
    
  return 0;
}
