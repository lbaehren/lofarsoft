
#% $Id$ 

#
# Copyright (C) 2006
# ASTRON (Netherlands Foundation for Research in Astronomy)
# and The MeqTree Foundation
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
from Meow.Utils import *
from Meow.Direction import *
from Meow.PointSource import *
from Meow.GaussianSource import *
import Meow
#from Meow.Shapelet import *

from Timba.LSM.LSM import LSM


## for observations without phase tracking
def point_and_extended_sources_nophasetrack(ns,lsm,tablename=''):
  """ define two extended sources: positions and flux densities """
  parm_options = record(
      use_previous=False,
      table_name=tablename,
      node_groups='Parm');
  

  x00=3826445.09045;
  y00=460923.318101;
  z00=5064346.19305;

  #x00=3.826318521230000e+06;
  #y00=4.609211927340000e+05;
  #z00=5.064441374160000e+06;


  source_model = []

  #plist=lsm.queryLSM(all=1)
  plist=lsm.queryLSM(count=1000)

  for pu in plist:
     (ra,dec,sI,sQ,sU,sV,SIn,f0,RM)=pu.getEssentialParms(ns)
     (eX,eY,eP)=pu.getExtParms()
     # scale 2 difference
     eX=eX*2
     eY=eY*2
     if f0==0:
       f0=150e6
     if eX!=0 or eY!=0 or eP!=0:
         source_model.append( GaussianSource(ns,name=pu.name,I=sI, Q=sQ, U=sU, V=sV,
                  Iorder=0, direction=AEDirection(ns,pu.name,ra,dec,parm_options=parm_options),
                  spi=SIn,freq0=f0,
                  size=[eX,eY],phi=eP,
                  parm_options=parm_options));
     else:
       # point sources
       if RM==0: 
         source_model.append( PointSource(ns,name=pu.name,I=sI, Q=sQ, U=sU, V=sV,
                  Iorder=0, direction=AEDirection(ns,pu.name,ra,dec,x00,y00,z00, parm_options=parm_options),
                  spi=SIn,freq0=f0,
                  parm_options=parm_options));

       else:
         source_model.append( PointSource(ns,name=pu.name,I=sI, Q=sQ, U=sU, V=sV,
                  Iorder=0, direction=AEDirection(ns,pu.name,ra,dec,x00,y00,z00,parm_options=parm_options),
                  spi=SIn,freq0=f0, RM=RM,
                  parm_options=parm_options));



  return source_model


### for phase tracking
def point_and_extended_sources_abs (ns,lsm,tablename=''):
  """ define two extended sources: positions and flux densities """
  parm_options = record(
      use_previous=False,
      table_name=tablename,
      node_groups='Parm');
  

  source_model = []

  #plist=lsm.queryLSM(all=1)
  plist=lsm.queryLSM(count=1000)

  for pu in plist:
     (ra,dec,sI,sQ,sU,sV,SIn,f0,RM)=pu.getEssentialParms(ns)
     (eX,eY,eP)=pu.getExtParms()
     # scale 2 difference
     eX=eX*2
     eY=eY*2
     if f0==0:
       f0=150e6
     if eX!=0 or eY!=0 or eP!=0:
         source_model.append( GaussianSource(ns,name=pu.name,I=sI, Q=sQ, U=sU, V=sV,
                  Iorder=0, direction=Direction(ns,pu.name,ra,dec,parm_options=parm_options),
                  spi=SIn,freq0=f0,
                  size=[eX,eY],phi=eP,
                  parm_options=parm_options));
     else:
       # point sources
       if RM==0: 
         source_model.append( PointSource(ns,name=pu.name,I=sI, Q=sQ, U=sU, V=sV,
                  direction=Direction(ns,pu.name,ra,dec),
                  spi=SIn,freq0=f0));

       else:
         source_model.append( PointSource(ns,name=pu.name,I=sI, Q=sQ, U=sU, V=sV,
                  direction=Direction(ns,pu.name,ra,dec,parm_options=parm_options),
                  spi=SIn,freq0=f0, RM=RM,
                  parm_options=parm_options));



  return source_model

### for solvable fluxes
def point_and_extended_sources_solvable(ns,lsm,tablename=''):
  """ define two extended sources: positions and flux densities """
  parm_options = record(
      use_previous=False,
      table_name=tablename,
      node_groups='Parm');
  

  source_model = []

  #plist=lsm.queryLSM(all=1)
  plist=lsm.queryLSM(count=1000)

  for pu in plist:
     (ra,dec,sI,sQ,sU,sV,SIn,f0,RM)=pu.getEssentialParms(ns)
     (eX,eY,eP)=pu.getExtParms()
     # scale 2 difference
     eX=eX*2
     eY=eY*2
     if f0==0:
       f0=150e6
     if eX!=0 or eY!=0 or eP!=0:
         source_model.append( GaussianSource(ns,name=pu.name,I=Meow.Parm(sI), Q=Meow.Parm(sQ), U=Meow.Parm(sU), V=Meow.Parm(sV),
                  Iorder=0, direction=Direction(ns,pu.name,ra,dec,parm_options=parm_options),
                  spi=SIn,freq0=f0,
                  size=[eX,eY],phi=eP,
                  parm_options=parm_options));
     else:
       # point sources
       if RM==0: 
         source_model.append( PointSource(ns,name=pu.name,I=Meow.Parm(sI), Q=Meow.Parm(sQ), U=Meow.Parm(sU), V=Meow.Parm(sV),
                  direction=Direction(ns,pu.name,ra,dec),
                  spi=SIn,freq0=f0));

       else:
         source_model.append( PointSource(ns,name=pu.name,I=Meow.Parm(sI), Q=Meow.Parm(sQ), U=Meow.Parm(sU), V=Meow.Parm(sV),
                  direction=Direction(ns,pu.name,ra,dec,parm_options=parm_options),
                  spi=SIn,freq0=f0, RM=RM,
                  parm_options=parm_options));



  return source_model




#################### beams beams ...
def makebeam(ns=None,pol='X',scale=1.0, phi0=0, h0=1.4, station=0,solvable=False,solvables=[],meptable=None):
    p_scale=ns.p_scale(pol,station)
    if not p_scale.initialized():
       p_scale<<Meq.Parm(scale,node_groups='Parm', table_name=meptable,auto_save=True);
    p_phi=ns.p_phi(pol,station)
    if pol=='Y':
      # add extra pi/2 
      if not p_phi.initialized():
        p_phi<<Meq.Parm(phi0+math.pi/2,node_groups='Parm', table_name=meptable,auto_save=True);
    else:
      if not p_phi.initialized():
        p_phi<<Meq.Parm(phi0,node_groups='Parm', table_name=meptable,auto_save=True);
    p_h=ns.p_h(pol,station)
    if not p_h.initialized():
      p_h<<Meq.Parm(h0,node_groups='Parm', table_name=meptable,auto_save=True);

    if solvable:
        solvables.append(p_scale.name);
        solvables.append(p_phi.name);
        solvables.append(p_h.name);

    beam = ns.beam(pol,station) << Meq.PrivateFunction(children =(p_scale,p_phi,p_h),
        lib_name="/home/sarod/beams/beam.so",function_name="test");

    return beam;

#################### not a beam, just cuts off source if elevation is negative
def makebeam_cutoff(ns=None,pol='X',scale=1.0, phi0=0, h0=1.4, station=0,solvable=False,solvables=[],meptable=None):
    p_scale=ns.p_scale(pol,station)
    if not p_scale.initialized():
       p_scale<<Meq.Parm(scale,node_groups='Parm', table_name=meptable,auto_save=True);
    p_phi=ns.p_phi(pol,station)
    if pol=='Y':
      # add extra pi/2 
      if not p_phi.initialized():
        p_phi<<Meq.Parm(phi0+math.pi/2,node_groups='Parm', table_name=meptable,auto_save=True);
    else:
      if not p_phi.initialized():
        p_phi<<Meq.Parm(phi0,node_groups='Parm', table_name=meptable,auto_save=True);
    p_h=ns.p_h(pol,station)
    if not p_h.initialized():
      p_h<<Meq.Parm(h0,node_groups='Parm', table_name=meptable,auto_save=True);

    if solvable:
        solvables.append(p_scale.name);
        solvables.append(p_phi.name);
        solvables.append(p_h.name);

    beam = ns.beam_cutoff(pol,station) << Meq.PrivateFunction(children =(p_scale,p_phi,p_h),
        lib_name="/home/sarod/beams/beam_cutoff.so",function_name="test");

    return beam;

#################### beams for droopy dipole
def makebeam_droopy(ns=None,pol='X',L=0.9758, phi0=0, alpha=math.pi/4.001, h=1.706, station=0,solvable=False,solvables=[],meptable=None):
    p_L=ns.p_L(pol,station)
    if not p_L.initialized():
       p_L<<Meq.Parm(L,node_groups='Parm', table_name=meptable,auto_save=True);
    p_phi=ns.p_phi(pol,station)
    if pol=='Y':
      # add extra pi/2 
      if not p_phi.initialized():
        p_phi<<Meq.Parm(phi0+math.pi/2,node_groups='Parm', table_name=meptable,auto_save=True);
    else:
      if not p_phi.initialized():
        p_phi<<Meq.Parm(phi0,node_groups='Parm', table_name=meptable,auto_save=True);
    p_h=ns.p_h(pol,station)
    if not p_h.initialized():
      p_h<<Meq.Parm(h,node_groups='Parm', table_name=meptable,auto_save=True);
    p_alpha=ns.p_alpha(pol,station)
    if not p_alpha.initialized():
      p_alpha<<Meq.Parm(alpha,node_groups='Parm', table_name=meptable,auto_save=True);

    if solvable:
        solvables.append(p_L.name);
        solvables.append(p_phi.name);
        solvables.append(p_h.name);
        solvables.append(p_alpha.name);

    beam = ns.beam_droopy(pol,station) << Meq.PrivateFunction(children =(p_h,p_L,p_alpha,p_phi),
        lib_name="/home/sarod/beams/beam_dr.so",function_name="test");
 
    return beam;

def makebeam_droopy_phi(ns=None,pol='X',L=0.9758, phi0=0, alpha=math.pi/4.001, h=1.706, station=0,solvable=False,solvables=[],meptable=None):
    p_L=ns.p_L(pol,station)
    if not p_L.initialized():
       p_L<<Meq.ToComplex(Meq.Parm(L,node_groups='Parm', table_name=meptable,auto_save=True),0);
    p_phi=ns.p_phi(pol,station)
    if pol=='Y':
      # add extra pi/2 
      if not p_phi.initialized():
        p_phi<<Meq.ToComplex(Meq.Parm(phi0-math.pi/2,node_groups='Parm', table_name=meptable,auto_save=True),0);
    else:
      if not p_phi.initialized():
        p_phi<<Meq.ToComplex(Meq.Parm(phi0,node_groups='Parm', table_name=meptable,auto_save=True),0);
    p_h=ns.p_h(pol,station)
    if not p_h.initialized():
      p_h<<Meq.ToComplex(Meq.Parm(h,node_groups='Parm', table_name=meptable,auto_save=True),0);
    p_alpha=ns.p_alpha(pol,station)
    if not p_alpha.initialized():
      p_alpha<<Meq.ToComplex(Meq.Parm(alpha,node_groups='Parm', table_name=meptable,auto_save=True),0);

    if solvable:
        solvables.append(p_L.name);
        solvables.append(p_phi.name);
        solvables.append(p_h.name);
        solvables.append(p_alpha.name);
    beam =  ns.beam_phi(pol,station)<< Meq.PrivateFunction(children =(p_h,p_L,p_alpha,p_phi),
        lib_name="/home/sarod/beams/beam_dr_phi.so",function_name="test");

    return beam;

def makebeam_droopy_theta(ns=None,pol='X',L=0.9758, phi0=0, alpha=math.pi/4.001, h=1.706, station=0,solvable=False,solvables=[],meptable=None):
    p_L=ns.p_L(pol,station)
    if not p_L.initialized():
       p_L<<Meq.ToComplex(Meq.Parm(L,node_groups='Parm', table_name=meptable,auto_save=True));
    p_phi=ns.p_phi(pol,station)
    if pol=='Y':
      # add extra pi/2 
      if not p_phi.initialized():
        p_phi<<Meq.ToComplex(Meq.Parm(phi0-math.pi/2,node_groups='Parm', table_name=meptable,auto_save=True));
    else:
      if not p_phi.initialized():
        p_phi<<Meq.ToComplex(Meq.Parm(phi0,node_groups='Parm', table_name=meptable,auto_save=True));
    p_h=ns.p_h(pol,station)
    if not p_h.initialized():
      p_h<<Meq.ToComplex(Meq.Parm(h,node_groups='Parm', table_name=meptable,auto_save=True));
    p_alpha=ns.p_alpha(pol,station)
    if not p_alpha.initialized():
      p_alpha<<Meq.ToComplex(Meq.Parm(alpha,node_groups='Parm', table_name=meptable,auto_save=True));

    if solvable:
        solvables.append(p_L.name);
        solvables.append(p_phi.name);
        solvables.append(p_h.name);
        solvables.append(p_alpha.name);
    beam = ns.beam_theta(pol,station) << Meq.PrivateFunction(children =(p_h,p_L,p_alpha,p_phi),
        lib_name="/home/sarod/beams/beam_dr_theta.so",function_name="test");
    return beam;



### BEAM
def EJones (ns,array,sources,radec0,meptable=None,solvables=[],solvable=False, name="E"):
  Bx={}
  By={}
  for station in array.stations():
   Bx[station] = makebeam(ns,station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   By[station] = makebeam(ns,pol='Y',station=station,meptable=meptable,solvable=solvable,solvables=solvables);

  Ej0 = ns[name];

  # get array xyz
  xyz=array.xyz();

  # create per-direction, per-station E Jones matrices
  for src in sources:
    dirname = src.direction.name;
    radec=src.direction.radec()
    Ej = Ej0(dirname);

    for station in array.stations():
        # create Az,El per station, per source
        azelnode=ns.azel(dirname,station)<<Meq.AzEl(radec=src.direction.radec(),xyz=xyz(station))
        Xediag = ns.Xediag(dirname,station) << Meq.Compounder(children=[azelnode,Bx[station]],common_axes=[hiid('l'),hiid('m')])
        Yediag = ns.Yediag(dirname,station) << Meq.Compounder(children=[azelnode,By[station]],common_axes=[hiid('l'),hiid('m')])
        # create E matrix, normalize for zenith at 60MHz
        Ej(station) << Meq.Matrix22(Xediag,0,0,Yediag)/88.00;
  return Ej0;

### BEAM with projection only
def EJones_P_only (ns,array,sources,radec0,meptable=None,solvables=[],solvable=False, name="E"):
  Bx={}
  By={}
  for station in array.stations():
   Bx[station] = makebeam_cutoff(ns,station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   By[station] = makebeam_cutoff(ns,pol='Y',station=station,meptable=meptable,solvable=solvable,solvables=solvables);

  Ej0 = ns[name];

  # get array xyz
  xyz=array.xyz();

  # create per-direction, per-station E Jones matrices
  for src in sources:
    dirname = src.direction.name;
    radec=src.direction.radec()
    Ej = Ej0(dirname);

    for station in array.stations():
        # create Az,El per station, per source
        azelnode=ns.azel(dirname,station)<<Meq.AzEl(radec=src.direction.radec(),xyz=xyz(station))
        # projection
        az=ns.az(dirname,station)<<Meq.Selector(azelnode,multi=True,index=[0])-math.pi/4
        el=ns.el(dirname,station)<<Meq.Selector(azelnode,multi=True,index=[1])
        # sin,cosines
        az_C=ns.az_C(dirname,station)<<Meq.Cos(az);
        az_S=ns.az_S(dirname,station)<<Meq.Sin(az);
        el_S=ns.el_S(dirname,station)<<Meq.Sin(el);
        proj=ns.proj(dirname,station)<<Meq.Matrix22(el_S*az_C,az_S,Meq.Negate(el_S*az_S),az_C);
        #proj=ns.proj(dirname,station)<<Meq.Matrix22(el_S*az_C,az_S,el_S*az_C,Meq.Negate(az_S));

        Xediag = ns.Xediag(dirname,station) << Meq.Compounder(children=[azelnode,Bx[station]],common_axes=[hiid('l'),hiid('m')])
        Yediag = ns.Yediag(dirname,station) << Meq.Compounder(children=[azelnode,By[station]],common_axes=[hiid('l'),hiid('m')])
        # create E matrix
        EE=ns.E0(dirname,station)<<Meq.Matrix22(Xediag,0,0,Yediag);
        Ej(station) <<Meq.MatrixMultiply(EE,proj)
  return Ej0;


### BEAM with projection  and horizontal dipole
def EJones_P (ns,array,sources,radec0,meptable=None,solvables=[],solvable=False, name="E"):
  Bx={}
  By={}
  for station in array.stations():
   Bx[station] = makebeam(ns,station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   By[station] = makebeam(ns,pol='Y',station=station,meptable=meptable,solvable=solvable,solvables=solvables);

  Ej0 = ns[name];

  # get array xyz
  xyz=array.xyz();

  # create per-direction, per-station E Jones matrices
  for src in sources:
    dirname = src.direction.name;
    radec=src.direction.radec()
    Ej = Ej0(dirname);

    for station in array.stations():
        # create Az,El per station, per source
        azelnode=ns.azel(dirname,station)<<Meq.AzEl(radec=src.direction.radec(),xyz=xyz(station))
        # projection
        az=ns.az(dirname,station)<<Meq.Selector(azelnode,multi=True,index=[0])-math.pi/4
        el=ns.el(dirname,station)<<Meq.Selector(azelnode,multi=True,index=[1])
        # sin,cosines
        az_C=ns.az_C(dirname,station)<<Meq.Cos(az);
        az_S=ns.az_S(dirname,station)<<Meq.Sin(az);
        el_S=ns.el_S(dirname,station)<<Meq.Sin(el);
        proj=ns.proj(dirname,station)<<Meq.Matrix22(el_S*az_C,az_S,Meq.Negate(el_S*az_S),az_C);

        Xediag = ns.Xediag(dirname,station) << Meq.Compounder(children=[azelnode,Bx[station]],common_axes=[hiid('l'),hiid('m')])
        Yediag = ns.Yediag(dirname,station) << Meq.Compounder(children=[azelnode,By[station]],common_axes=[hiid('l'),hiid('m')])
        # create E matrix
        EE=ns.E0(dirname,station)<<Meq.Matrix22(Xediag,0,0,Yediag);
        Ej(station) <<Meq.MatrixMultiply(EE,proj)
  return Ej0;

### Minimal BEAM - just to attenuate sources near or below  horizon
def EJones_minimal(ns,array,sources,radec0,meptable=None,solvables=[],solvable=False, name="E"):
  Bx={}
  By={}
  for station in array.stations():
   Bx[station] = makebeam_cutoff(ns,station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   By[station] = makebeam_cutoff(ns,pol='Y',station=station,meptable=meptable,solvable=solvable,solvables=solvables);

  Ej0 = ns[name];

  # get array xyz
  xyz=array.xyz();

  # create per-direction, per-station E Jones matrices
  for src in sources:
    dirname = src.direction.name;
    radec=src.direction.radec()
    Ej = Ej0(dirname);

    for station in array.stations():
        # create Az,El per station, per source
        azelnode=ns.azel(dirname,station)<<Meq.AzEl(radec=src.direction.radec(),xyz=xyz(station))
        Xediag = ns.Xediag(dirname,station) << Meq.Compounder(children=[azelnode,Bx[station]],common_axes=[hiid('l'),hiid('m')])
        Yediag = ns.Yediag(dirname,station) << Meq.Compounder(children=[azelnode,By[station]],common_axes=[hiid('l'),hiid('m')])
        # create E matrix
        Ej(station) << Meq.Matrix22(Xediag,0,0,Yediag);
  return Ej0;



### Complex BEAM - droopy dipole see writeup
def EJones_my(ns,array,sources,radec0,meptable=None,solvables=[],solvable=False, name="E"):
  Bx={}
  By={}
  
  for station in array.stations():
   Bx[station] = makebeam_droopy(ns,station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   By[station] = makebeam_droopy(ns,pol='Y',station=station,meptable=meptable,solvable=solvable,solvables=solvables);

  Ej0 = ns[name];

  # get array xyz
  xyz=array.xyz();

  # create per-direction, per-station E Jones matrices
  for src in sources:
    dirname = src.direction.name;
    radec=src.direction.radec()
    Ej = Ej0(dirname);

    for station in array.stations():
        # create Az,El per station, per source
        azelnode=ns.azel(dirname,station)<<Meq.AzEl(radec=src.direction.radec(),xyz=xyz(station))
        # projection 
        # azimuth: take orientation into account
        az=ns.az(dirname,station)<<Meq.Selector(azelnode,multi=True,index=[0])-math.pi/4
        el=ns.el(dirname,station)<<Meq.Selector(azelnode,multi=True,index=[1])
        # sin,cosines
        az_C=ns.az_C(dirname,station)<<Meq.Cos(az+math.pi/2);
        az_S=ns.az_S(dirname,station)<<Meq.Sin(az+math.pi/2);
        el_S=ns.el_S(dirname,station)<<Meq.Sin(el);

        proj=ns.proj(dirname,station)<<Meq.Matrix22(el_S*az_C,az_S,Meq.Negate(el_S*az_S),az_C);

        Xediag = ns.Xediag(dirname,station) << Meq.Compounder(children=[Meq.Composer(az,el),Bx[station]],common_axes=[hiid('l'),hiid('m')])
        Yediag = ns.Yediag(dirname,station) << Meq.Compounder(children=[Meq.Composer(az,el),By[station]],common_axes=[hiid('l'),hiid('m')])
        # create E matrix
        EE=ns.E0(dirname,station)<<Meq.Matrix22(Xediag,0,0,Yediag);
        Ej(station) <<Meq.MatrixMultiply(EE,proj)

  return Ej0;



### Complex BEAM - droopy dipole without projection, only XX and YY
def EJones_droopy(ns,array,sources,radec0,meptable=None,solvables=[],solvable=False, name="E"):
  Bx={}
  By={}
  
  for station in array.stations():
   Bx[station] = makebeam_droopy(ns,station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   By[station] = makebeam_droopy(ns,pol='Y',station=station,meptable=meptable,solvable=solvable,solvables=solvables);

  Ej0 = ns[name];

  # get array xyz
  xyz=array.xyz();

  # create per-direction, per-station E Jones matrices
  for src in sources:
    dirname = src.direction.name;
    radec=src.direction.radec()
    Ej = Ej0(dirname);
    # create Az,El per source, using station 1
    azelnode=ns.azel(dirname)<<Meq.AzEl(radec=src.direction.radec(),xyz=xyz(1))
 
    for station in array.stations():
        Xediag = ns.Xediag(dirname,station) << Meq.Compounder(children=[azelnode,Bx[station]],common_axes=[hiid('l'),hiid('m')])
        Yediag = ns.Yediag(dirname,station) << Meq.Compounder(children=[azelnode,By[station]],common_axes=[hiid('l'),hiid('m')])
        # create E matrix, no projection
        Ej(station) <<Meq.Matrix22(Xediag,0,0,Yediag)

  return Ej0;

#### even more complex droopy dipole
def EJones_droopy_comp(ns,array,sources,radec0,meptable=None,solvables=[],solvable=False, name="E"):
  Bx_phi={}
  Bx_theta={}
  By_phi={}
  By_theta={}
  
  for station in array.stations():
   Bx_phi[station] = makebeam_droopy_phi(ns,station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   Bx_theta[station] = makebeam_droopy_theta(ns,station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   By_phi[station] = makebeam_droopy_phi(ns,pol='Y',station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   By_theta[station] = makebeam_droopy_theta(ns,pol='Y',station=station,meptable=meptable,solvable=solvable,solvables=solvables);

  Ej0 = ns[name];

  # get array xyz
  xyz=array.xyz();

  # create per-direction, per-station E Jones matrices
  for src in sources:
    dirname = src.direction.name;
    radec=src.direction.radec()
    Ej = Ej0(dirname);

    # create Az,El per source, using station 1
    azelnode=ns.azel(dirname)<<Meq.AzEl(radec=src.direction.radec(),xyz=xyz(1))
    # make shifts
    az=ns.az(dirname)<<Meq.Selector(azelnode,multi=True,index=[0])
    azX=ns.azX(dirname)<<az-math.pi/4
    azY=ns.azY(dirname)<<az-math.pi/4
    el=ns.el(dirname)<<Meq.Selector(azelnode,multi=True,index=[1])
   
    # sin,cosines
    #az_CX=ns.az_CX(dirname)<<Meq.Cos(az);
    #az_SX=ns.az_SX(dirname)<<Meq.Sin(az);
    #az_CY=ns.az_CY(dirname)<<Meq.Cos(az);
    #az_SY=ns.az_SY(dirname)<<Meq.Sin(az);
    #el_S=ns.el_S(dirname)<<Meq.Sin(el);

    for station in array.stations():
        Xediag_phi = ns.Xediag_phi(dirname,station) << Meq.Compounder(children=[Meq.Composer(azX,el),Bx_phi[station]],common_axes=[hiid('l'),hiid('m')])
        Xediag_theta= ns.Xediag_theta(dirname,station) << Meq.Compounder(children=[Meq.Composer(azX,el),Bx_theta[station]],common_axes=[hiid('l'),hiid('m')])
        Yediag_phi = ns.Yediag_phi(dirname,station) << Meq.Compounder(children=[Meq.Composer(azY,el),By_phi[station]],common_axes=[hiid('l'),hiid('m')])
        Yediag_theta = ns.Yediag_theta(dirname,station) << Meq.Compounder(children=[Meq.Composer(azY,el),By_theta[station]],common_axes=[hiid('l'),hiid('m')])
        # create E matrix
        Ej(station) <<Meq.Matrix22(Xediag_theta,Xediag_phi,Yediag_theta,Yediag_phi)/88.0

  return Ej0;


#### even more complex droopy dipole - with station beam
def EJones_droopy_comp_stat(ns,array,sources,radec0,meptable=None,solvables=[],solvable=False, name="E"):
  Bx_phi={}
  Bx_theta={}
  By_phi={}
  By_theta={}
  
  for station in array.stations():
   Bx_phi[station] = makebeam_droopy_phi(ns,station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   Bx_theta[station] = makebeam_droopy_theta(ns,station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   By_phi[station] = makebeam_droopy_phi(ns,pol='Y',station=station,meptable=meptable,solvable=solvable,solvables=solvables);
   By_theta[station] = makebeam_droopy_theta(ns,pol='Y',station=station,meptable=meptable,solvable=solvable,solvables=solvables);

  Ej0 = ns[name];

  # get array xyz
  xyz=array.xyz();

  # station beam
  if not ns.freq0.initialized():
    ns.freq0<<Meq.Constant(59e6)
  if not ns.freq1.initialized():
    ns.freq1<<Meq.Constant(60e6)

  if not ns.Xstatbeam.initialized():
    Xstatbeam=ns.Xstatbeam<<Meq.StationBeam(filename="AntennaCoords",radec=radec0,xyz=array.xyz0(),phi0=Meq.Constant(-math.pi/4),ref_freq=(ns.freq0+ns.freq1)/2)
  else:
    Xstatbeam=ns.Xstatbeam;

  if not ns.Ystatbeam.initialized():
   Ystatbeam=ns.Ystatbeam<<Meq.StationBeam(filename="AntennaCoords",radec=radec0,xyz=array.xyz0(),phi0=Meq.Constant(-math.pi/4),ref_freq=(ns.freq0+ns.freq1)/2)
  else:
    Ystatbeam=ns.Ystatbeam;

  # create per-direction, per-station E Jones matrices
  for src in sources:
    dirname = src.direction.name;
    radec=src.direction.radec()
    Ej = Ej0(dirname);

    # create Az,El per source, using station 1
    azelnode=ns.azel(dirname)<<Meq.AzEl(radec=src.direction.radec(),xyz=xyz(1))
    # make shifts
    az=ns.az(dirname)<<Meq.Selector(azelnode,multi=True,index=[0])
    azX=ns.azX(dirname)<<az-math.pi/4
    azY=ns.azY(dirname)<<az-math.pi/4
    el=ns.el(dirname)<<Meq.Selector(azelnode,multi=True,index=[1])
   

    for station in array.stations():
        azelX =ns.azelX(dirname,station)<<Meq.Composer(azX,el)
        azelY =ns.azelY(dirname,station)<<Meq.Composer(azY,el)
        Xediag_phi = ns.Xediag_phi(dirname,station) << Meq.Compounder(children=[azelX,Bx_phi[station]],common_axes=[hiid('l'),hiid('m')])
        Xediag_theta= ns.Xediag_theta(dirname,station) << Meq.Compounder(children=[azelX,Bx_theta[station]],common_axes=[hiid('l'),hiid('m')])
        Yediag_phi = ns.Yediag_phi(dirname,station) << Meq.Compounder(children=[azelY,By_phi[station]],common_axes=[hiid('l'),hiid('m')])
        Yediag_theta = ns.Yediag_theta(dirname,station) << Meq.Compounder(children=[azelY,By_theta[station]],common_axes=[hiid('l'),hiid('m')])
        # create E matrix, normalize for zenith at 60MHz
        #Ej(station) <<Meq.Matrix22(el_S*az_CX*Xediag_theta,az_SX*Xediag_phi,Meq.Negate(el_S*az_SY)*Yediag_theta,az_CY*Yediag_phi)
        if station==1:
          Xstatgain=ns.Xstatgain(dirname,station)<<Meq.Compounder(children=[azelX,Xstatbeam],common_axes=[hiid('l'),hiid('m')])
          Ystatgain=ns.Ystatgain(dirname,station)<<Meq.Compounder(children=[azelY,Ystatbeam],common_axes=[hiid('l'),hiid('m')])
          Ej(station) <<Meq.Matrix22(Xstatgain*Xediag_theta,Xstatgain*Xediag_phi,Ystatgain*Yediag_theta,Ystatgain*Yediag_phi)/88.00
          #Ej(station) <<Meq.Matrix22(Xediag_theta,Xediag_phi,Yediag_theta,Yediag_phi)
        else:
          Ej(station) <<Meq.Matrix22(Xediag_theta,Xediag_phi,Yediag_theta,Yediag_phi)/88.00
        #Ej(station) <<Meq.Matrix22(el_S*az_S*Xediag_theta,az_C*Xediag_phi,Meq.Negate(el_S*az_S)*Yediag_theta,az_C*Yediag_phi)

  return Ej0;




##### beam using FITS file
def EJones_fits(ns,array,sources,radec0,meptable=None,solvables=[],solvable=False, name="E",infile_name=None):
  ns.image << Meq.FITSImage(filename=infile_name,cutoff=1.0,mode=2)
  ns.resampler << Meq.Resampler(ns.image, dep_mask=0xff)

  Ej0 = ns[name];

  # get array xyz
  xyz=array.xyz();

  # create per-direction, per-station E Jones matrices
  for src in sources:
    dirname = src.direction.name;
    radec=src.direction.radec()
    Ej = Ej0(dirname);
    # create Az,El per source, using station 1
    azelnode=ns.azel(dirname)<<Meq.AzEl(radec=src.direction.radec(),xyz=xyz(1))
    # make shifts
    az=ns.az(dirname)<<Meq.Selector(azelnode,multi=True,index=[0])-math.pi/4
    az_y=ns.az_y(dirname)<<az+math.pi/2
    el0=ns.el0(dirname)<<Meq.Selector(azelnode,multi=True,index=[1])
    theta=ns.theta(dirname)<<-el0+math.pi/2
    # find modulo values
    az_X=ns.az_X(dirname)<<az-Meq.Floor(az/(2*math.pi))*2*math.pi
    az_Y=ns.az_Y(dirname)<<az_y-Meq.Floor(az_y/(2*math.pi))*2*math.pi
    
 
    for station in array.stations():
        Xediag = ns.Xediag(dirname,station) << Meq.Compounder(children=[Meq.Composer(az_X,theta),ns.resampler],common_axes=[hiid('l'),hiid('m')])
        Yediag = ns.Yediag(dirname,station) << Meq.Compounder(children=[Meq.Composer(az_Y,theta),ns.resampler],common_axes=[hiid('l'),hiid('m')])
        # create E matrix, no projection
        Ej(station) <<Meq.Matrix22(Xediag,0,0,Yediag)

  return Ej0;


def EJones_fits_P(ns,array,sources,radec0,meptable=None,solvables=[],solvable=False, name="E",infile_name=None):
  ns.theta_real<< Meq.FITSImage(filename=infile_name+'_theta_real.fits',cutoff=1.0,mode=2)
  ns.theta_imag<< Meq.FITSImage(filename=infile_name+'_theta_imag.fits',cutoff=1.0,mode=2)
  ns.phi_real<< Meq.FITSImage(filename=infile_name+'_phi_real.fits',cutoff=1.0,mode=2)
  ns.phi_imag<< Meq.FITSImage(filename=infile_name+'_phi_imag.fits',cutoff=1.0,mode=2)
  beam_phi=ns.resampler_phi << Meq.Resampler(Meq.ToComplex(ns.phi_real,ns.phi_imag), dep_mask=0xff)
  beam_theta=ns.resampler_theta<< Meq.Resampler(Meq.ToComplex(ns.theta_real,ns.theta_imag), dep_mask=0xff)

  Ej0 = ns[name];

  # get array xyz
  xyz=array.xyz();

  # create per-direction, per-station E Jones matrices
  for src in sources:
    dirname = src.direction.name;
    radec=src.direction.radec()
    Ej = Ej0(dirname);
    # create Az,El per source, using station 1
    azelnode=ns.azel(dirname)<<Meq.AzEl(radec=src.direction.radec(),xyz=xyz(1))
    # make shifts
    az=ns.az(dirname)<<Meq.Selector(azelnode,multi=True,index=[0])-math.pi/4
    el0=ns.el0(dirname)<<Meq.Selector(azelnode,multi=True,index=[1])
    theta=ns.theta(dirname)<<-el0+math.pi/2
    # find modulo values
    az_X=ns.az_X(dirname)<<az-Meq.Floor(az/(2*math.pi))*2*math.pi
    az_y=ns.az_y(dirname)<<az_X-math.pi/2
    az_Y=ns.az_Y(dirname)<<az_y-Meq.Floor(az_y/(2*math.pi))*2*math.pi
    
    # sin,cosines
    #az_C=ns.az_C(dirname)<<Meq.Cos(az+math.pi/2);
    #az_S=ns.az_S(dirname)<<Meq.Sin(az+math.pi/2);
    #el_S=ns.el_S(dirname)<<Meq.Sin(el0);

    #proj=ns.proj(dirname)<<Meq.Matrix22(el_S*az_C,az_S,Meq.Negate(el_S*az_S),az_C);

    for station in array.stations():
        X_theta = ns.X_theta(dirname,station) << Meq.Compounder(children=[Meq.Composer(az_X,theta),beam_theta],common_axes=[hiid('l'),hiid('m')])
        X_phi= ns.X_phi(dirname,station) << Meq.Compounder(children=[Meq.Composer(az_X,theta),beam_phi],common_axes=[hiid('l'),hiid('m')])
        Y_theta = ns.Y_theta(dirname,station) << Meq.Compounder(children=[Meq.Composer(az_Y,theta),beam_theta],common_axes=[hiid('l'),hiid('m')])
        Y_phi= ns.Y_phi(dirname,station) << Meq.Compounder(children=[Meq.Composer(az_Y,theta),beam_phi],common_axes=[hiid('l'),hiid('m')])
        # create E matrix, no projection
        #EE=ns.E0(dirname,station)<<Meq.Matrix22(Xediag,0,0,Yediag);
        # flip X-Y polarizations
        #EE=ns.E0(dirname,station)<<Meq.Matrix22(0,Yediag,Xediag,0);
        Ej(station) <<Meq.Matrix22(X_theta,X_phi,Y_theta,Y_phi)

  return Ej0;




