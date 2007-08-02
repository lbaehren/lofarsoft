###
### This scripts demonstrates the use of Compounder.
### You need to have some knowledge of MeqParm node to use this.
### For more information please visit
### http://lofar9.astron.nl/meqwiki/MeqCompounder


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

# for pi
import math

Settings.forest_state.cache_policy = 100
Settings.forest_state.bookmarks = []



def _define_forest (ns, **kwargs):
  """ We create two horizontal dipoles, perpendicular to one another
      and multiply their Voltage (not Power) beam shapes. The voltage
      beam is taken to be the positive square root of the power beam.
  """;

  par=['p0+p1*x0+p2*x1+p3*x0*x1','p4+p5*x0+p6*x1+p7*x0*x1',\
       'p8+p9*x0+p10*x1+p11*x0*x1','p12+p13*x0+p14*x1+p15*x0*x1']
  # polynomial coefficients
  coeff=[[1,0,0,0],[1,0,0,0],[1,0,0,0],[1,0,0,0]]
  # choose this to see some time variation
  # the third polynomial should model variation of dipole height
  # with freqency                        \/ -- this models height
  #coeff=[[1,0.1,0,0],[1,0.2,0,0],[1,0.10,0.5,0],[1,0.30,0,0]]
  # value of pi
  pi=str(math.pi)
  # dipole height from ground plane in wavelengths
  # make dipole height above ground=lambda/4 to get peak at top
  h=0.25

  print par
  print coeff

  # create XX beam
  x_node=_create_dipole_beam_h(par,coeff,h)
  ns.x<<x_node

  # L,M coordinates
  l=[[0.1, 0.001],[0.2,-0.002]]
  m=[[0.2, 0.002],[0.1,-0.001]]
  ns.l0<< Meq.Parm(l,node_groups='Parm')*0.01
  ns.m0<< Meq.Parm(m,node_groups='Parm')*0.01

  # this will generate the LM grid
  ns.lm<<Meq.Composer(ns.l0, ns.m0)


  ns.y<<Meq.Compounder(children=[ns.lm,ns.x],common_axes=[hiid('l'),hiid('m')])

  bm = record(name='Beam', viewer='Result Plotter',
               udi='/node/x', publish=True)
  Settings.forest_state.bookmarks.append(bm)
  bm = record(name='Grid', viewer='Result Plotter',
               udi='/node/lm', publish=True)
  Settings.forest_state.bookmarks.append(bm)
  bm = record(name='Compounder', viewer='Result Plotter',
               udi='/node/y', publish=True)
  Settings.forest_state.bookmarks.append(bm)


  return True



def _tdl_job_execute (mqs, parent):
    """Execute the forest, starting at the named node"""
    domain = meq.domain(1200,1600,1,10)                            # (f1,f2,t1,t2)
    cells = meq.cells(domain, num_freq=10, num_time=10)
    request = meq.request(cells, rqtype='ev')
    result = mqs.meq('Node.Execute',record(name='y', request=request))
    return result





###
### function for creation of a dipole beam
### this is a 4D funklet using MeqParm
def _create_dipole_beam_h(tfpoly=['1','1','1','1'],coeff=[[1],[1],[1],[1]],h=0.25,x='x2',y='x3'):
  """ Create Horizontal Dipole beam:
      The theoretical (power) beam shape is:
      z=(1-sin(x)^2 sin(y)^2)*sin(2*pi*h*cos(y)))^2;
      where x:azimuth angle (phi)
            y:elevation angle (theta) (both in radians)
            h: dipole height from ground
      we create a voltage beam, using square root of power as the r.m.s.
      voltage, and add polynomials for time,freq as given by {TF}
      z=(1-sin({TF0}x)^2 sin({TF1}y)^2)*sin(2*pi*h*{TF2}cos({TF3}y))^2
      so we need 4 polynomials, which must be given as tfpoly array.
      The coefficients for these polynomials should be given by coeff array.
      x,y should be given as polynomials of x2 and x3 respectively.
  """
  if len(tfpoly)<4:
   print "Invalid No. of Polynomials, should be 4"
   return None

  h_str=str(h)
  pi=str(math.pi)
  # voltage beam, so do not take ^2
  beamshape='sqrt((1-(sin(('+tfpoly[0]+')*('+x+'))*sin(('+tfpoly[1]+')*('+y+')))^2)*(sin(2*'+pi+'*'+h_str+'*('+tfpoly[2]+')*cos(('+tfpoly[3]+')*('+y+'))))^2)'
  polc = meq.polc(coeff=coeff,subclass=meq._funklet_type)
  print beamshape
  print coeff
  polc.function = beamshape;

  root=Meq.Parm(polc,node_groups='Parm')

  return root
