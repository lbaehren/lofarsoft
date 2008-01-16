/*-------------------------------------------------------------------------*
 | $Id                                                                   $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2007                                                    *
 *   Frank Schröder                                            *
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

#include <Analysis/analyseLOPESevent.h>

using CR::analyseLOPESevent;

using CR::LopesEventIn;

/*!
  \file tanalyseLOPESevent.cc

  \ingroup apps

  \brief Calls the LOPES analysis pipline in  "analyseLOPESevent"
 
  \author Frank Schröder
 
  \date 2007/04/12
*/



// -----------------------------------------------------------------------------

int main ()
{
  std::cout << "\nStarting Program \"call_pipline\".\n\n" << std::endl;
  
  try {
    analyseLOPESevent eventPipeline;
    Double distance_default = 2000.;
    
    // Define ranges for analysis

    Record obsrec,results;
    obsrec.define("LOPES","/home/horneff/lopescasa/data/LOPES/LOPES-CalTable");

    eventPipeline.initPipeline(obsrec);

/*    
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2006.12.16.16:58:28.978.event", 46.068481, 37.849694, distance_default, -253.7422, -157.5188, True, "1166288308", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2006.12.25.11:39:55.591.event", 295.23156, 45.872063, distance_default, -276.1272, -617.7156, True, "1167046795", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2006.12.27.03:41:26.891.event", 264.83871, 28.021416, distance_default, -605.1727, -422.9571, True, "1167190886", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2006.12.27.06:05:44.608.event", 297.65505, 70.533686, distance_default, -130.4875, -445.2277, True, "1167199544", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.01.04.21:17:08.671.event", 333.68807, 65.201293, distance_default, -267.8873, -543.7399, True, "1167945428", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.01.30.11:10:43.476.event", 135.76542, 36.719426, distance_default, -153.6049, -471.7555, True, "1170155443", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.02.06.22:38:47.920.event", 232.77142, 43.180691, distance_default, -308.7891, -428.9539, True, "1170801527", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.02.22.13:58:40.599.event", 49.042778, 46.421811, distance_default, -46.60105, -359.3270, True, "1172152720", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.03.12.03:10:42.794.event", 75.794883, 27.425511, distance_default, 498.99288, -16.57129, True, "1173669042", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.04.14.18:03:17.670.event", 41.265552, 47.338051, distance_default, -340.6271, -341.0849, True, "1176573797", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.05.03.14:48:37.531.event", 38.445480, 65.825177, distance_default, -275.6237, -396.2462, True, "1178203717", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.05.11.03:20:32.653.event", 345.02893, 46.582676, distance_default, -6.843671, -213.6873, True, "1178853632", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.05.19.08:51:03.744.event", 356.73330, 49.430722, distance_default, -341.4511, -304.2343, True, "1179564663", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.05.20.11:54:02.486.event", 28.040967, 37.852490, distance_default, -353.5362, -499.5422, True, "1179662042", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.06.21.05:09:15.933.event", 330.87347, 50.935879, distance_default, -7.644770, -606.1799, True, "1182402555", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.07.08.22:32:04.957.event", 294.92861, 59.583537, distance_default, -41.45113, -459.4186, True, "1183933924", True, Vector<Int>(), True, True);
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.08.17.23:31:55.678.event", 283.68139, 23.477967, distance_default, -387.2282, -215.3353, True, "1187393515", True, Vector<Int>(), True, True); 
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.09.01.09:22:35.310.event", 354.55215, 63.882182, distance_default, 8.6060886, -368.0933, True, "1188638555", True, Vector<Int>(), True, True); 
    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.09.14.18:24:56.014.event", 327.52468, 39.627625, distance_default, -461.4557, -578.3245, True, "1189794296", True, Vector<Int>(), True, True);*/

    Vector<Int> flagged(25);
    flagged(0)  = 10101; 
    flagged(1)  = 10102;
    flagged(2)  = 10201;
    flagged(3)  = 10202;
    flagged(4)  = 20101;
    flagged(5)  = 20102;
    flagged(6)  = 20201;
    flagged(7)  = 20202;
    flagged(8)  = 30101;
    flagged(9)  = 30102;
    
    flagged(10) = 40101; 
    flagged(11) = 40202;
    flagged(12) = 50101;
    flagged(13) = 50202;
    flagged(14) = 60102;
    
    flagged(15) = 70102;
    flagged(16) = 70201;
    flagged(17) = 70202;
    flagged(18) = 70101;
    flagged(19) = 80101;
    flagged(20) = 80102; 
    flagged(21) = 80201;
    flagged(22) = 80202;
    flagged(23) = 90101;
    flagged(24) = 90102;
    

    results = eventPipeline.ProcessEvent("/home/schroeder/data/lopesstar/2007.09.01.09:22:35.310.event", 354.55215, 63.882182, distance_default, 8.6060886, -368.0933, True, "1188638555", True, flagged, True, False); 
  
  } catch (std::string message) {
    std::cerr << message << std::endl;
  }
 
  return 0;
}
