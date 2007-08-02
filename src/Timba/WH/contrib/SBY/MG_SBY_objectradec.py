#
#% $Id$ 
#
#
# Copyright (C) 2002-2007
# The MeqTree Foundation & 
# ASTRON (Netherlands Foundation for Research in Astronomy)
# P.O.Box 2, 7990 AA Dwingeloo, The Netherlands
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>,
# or write to the Free Software Foundation, Inc., 
# 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from Timba.TDL import *
from Timba.Meq import meq
Settings.forest_state.cache_policy = 100;

Settings.orphans_are_roots = True;

def _define_forest (ns):
   ns.cb<<Meq.ObjectRADec(obj_name="MOON")

def _test_forest (mqs, parent):

 f0 = 1200
 f1 = 1600
 t0 = 54126*3600*24 # UTC in days converted to seconds
 t1 = 55126*3600*24
 nfreq =20
 ntime =100

 freqtime_domain = meq.domain(startfreq=f0, endfreq=f1, starttime=t0, endtime=t1);
 cells =meq.cells(domain=freqtime_domain, num_freq=nfreq,  num_time=ntime);
 request = meq.request(cells,rqtype='e1');
 b = mqs.meq('Node.Execute',record(name='cb',request=request),wait=True);
 
if __name__ == '__main__':
  ns=NodeScope()
  _define_forest(ns)
  print ns.AllNodes()



