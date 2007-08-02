###
### This demonstrates the use of FITSImage node.
### For more information, please visit
### http://lofar9.astron.nl/meqwiki/MeqImage
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


Settings.forest_state.cache_policy = 100
Settings.forest_state.bookmarks = []



def _define_forest (ns, **kwargs):
   # just read a FITS file, 
   # cutoff=any value in the range 0 to 1.0
   # filename= any FITS file of an image
   ns.image<<Meq.FITSImage(filename="demo.fits",cutoff=0.2)

   # Make a bookmark of the result node, for easy viewing:
   bm = record(name='result', viewer='Result Plotter',
               udi='/node/image', publish=True)
   Settings.forest_state.bookmarks.append(bm)

   # Finished:
   return True



def _tdl_job_execute (mqs, parent):
    domain = meq.domain(1,10,1,10)                            # (f1,f2,t1,t2)
    cells = meq.cells(domain, num_freq=10, num_time=11)
    request = meq.request(cells, rqtype='ev')
    result = mqs.meq('Node.Execute',record(name='image', request=request))
    return result
