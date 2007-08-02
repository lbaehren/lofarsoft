# MG_SBY_resample.py

# Short description:
#   A template for the generation of MeqGraft (MG) scripts

# Keywords: ....


# Copyright: The MeqTree Foundation

# Full description:

#********************************************************************************
#********************************************************************************
#**************** PART II: Preamble and initialisation **************************
#********************************************************************************
#********************************************************************************


#% $Id: MG_SBY_image.py 5418 2007-07-19 16:49:13Z oms $ 

#
# Copyright (C) 2002-2007
# ASTRON (Netherlands Foundation for Research in Astronomy)
# P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
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
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from Timba.TDL import *
from Timba.Meq import meq
# import JEN code for bookmarks
from Timba.Trees import JEN_bookmarks

import random
Settings.forest_state.cache_policy = 100;

Settings.orphans_are_roots = True;
# from numarray import *
# from string import *
# from copy import deepcopy

# Scripts needed to run a MG_JEN script: 

#-------------------------------------------------------------------------
# Script control record (may be edited here):


#********************************************************************************
#********************************************************************************
#**************** PART III: Required test/demo function *************************
#********************************************************************************
#********************************************************************************

# Tree definition routine (may be executed from the browser):
# To be used as example, for experimentation, and automatic testing.


def _define_forest (ns):
   ns.r<<Meq.FITSImage(filename="3ax.fits",cutoff=0.2)


   JEN_bookmarks.create(ns.r, page='Image', viewer='Result Plotter')


#********************************************************************************
#********************************************************************************
#******************** PART IV: Optional: Importable functions *******************
#********************************************************************************
#********************************************************************************






#********************************************************************************
#********************************************************************************
#*****************  PART V: Forest execution routines ***************************
#********************************************************************************
#********************************************************************************

# The function with the standard name _test_forest(), and any function
# with name _tdl_job_xyz(m), will show up under the 'jobs' button in
# the browser, and can be executed from there.  The 'mqs' argument is
# a meqserver proxy object.
# NB: The function _test_forest() is always put at the end of the menu:

def _test_forest (mqs, parent):

 f0 = 1200
 f1 = 1600
 t0 = 1.0
 t1 = 10.0
 nfreq =20
 ntime =10

 freqtime_domain = meq.domain(startfreq=f0, endfreq=f1, starttime=t0, endtime=t1);
 cells =meq.cells(domain=freqtime_domain, num_freq=nfreq,  num_time=ntime);
 request = meq.request(cells,rqtype='e1');
 b = mqs.meq('Node.Execute',record(name='r',request=request),wait=True);
 #print b
 

#********************************************************************************
#********************************************************************************
#******************** PART VI: Standalone test routines *************************
#********************************************************************************
#********************************************************************************

# These test routines do not require the meqbrowser, or even the meqserver.
# Just run them by enabling the required one (if 1:), and invoking python:
#      > python MG_JEN_template.py

if __name__ == '__main__':
  ns=NodeScope()
  _define_forest(ns)
  print ns.AllNodes()



