
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
from Timba.Meq import meq
from numarray import *
import os, re
import random

import Meow
from Meow import PointSource
from Meow import IfrArray
from Meow import Observation
from Meow import Patch
from Meow import CorruptComponent 
from Meow import Bookmarks

from Meow import Jones


import global_model 
import solver_plots
from Timba.LSM.LSM import LSM

TDLRuntimeMenu("MS Selection",
### MS params ####
# MS names - in a text file, line by line
TDLOption('msnames',"MS Names",["filelist.txt"],inline=True),
# read data from
TDLOption('input_column',"Input MS column",["DATA","MODEL_DATA","CORRECTED_DATA"],default=0),
# write data to
TDLOption('output_column',"Output MS column",[None,"MODEL_DATA","CORRECTED_DATA"],default=0),
# number of spectral windows to process 
TDLOption('min_spwid',"Start subband",[1,2,3],more=int),
TDLOption('max_spwid',"End subband",[1,2,3,4,5,6],more=int),
# number of channels per job
TDLOption('channel_step',"channels per job",[8,2,10,20],more=int),
# start channel in subband (from 0)
TDLOption('start_channel',"start channel",[31,99],more=int),
# end channel in subband (from 0)
TDLOption('end_channel',"end channel",[223,41,240],more=int),
);

TDLRuntimeMenu("Solver",
#### solver params ####
# how much to perturb starting values of solvables
TDLOption('perturbation',"Perturb solvables",["random",.1,.2,-.1,-.2]),
# solver debug level
TDLOption('solver_debug_level',"Solver debug level",[0,1,10]),
# solver debug level
TDLOption('solver_lm_factor',"Initial solver LM factor",[1,.1,.01,.001]),
# max number of iterations
TDLOption('solver_maxiter',"Max iterations",[10,3,15],more=int),
# number of timeslots to use at once
TDLOption('tile_size',"Tile size",[6,10,30,48,60,96,480],more=int),
# solve for full J Jones or  diagonal J Jones
TDLOption('full_J',"Full Jones",[False,True]),
);

TDLRuntimeMenu("Calibration",
#### imaging options ####
TDLOption('do_preprocess',"Preprocess",[True,False]),
TDLOption('do_calibrate',"Calibrate",[True,False]),
TDLOption('do_postprocess',"Postprocess",[True,False]),
);

TDLRuntimeMenu("Imager",
#### imaging options ####
#TDLOption('average_channels',"Average corrected data",[True,False],doc="Spectral averaging will speedup imaging"),
#TDLOption('full_images',"Full Images",[True,False]),
### uv distance #####
#TDLOption('min_uv',"Min uv distance squared",[3400,400],more=float)
);



TDLCompileMenu("Parms",
# parm table name
TDLOption('mytable',"parmtable",[None,"peel0.mep"],default=None),
# save nonconverged solutions
TDLOption('mysave',"save all",[False,True],default=False),
# subtile solutions
TDLOption('dosubtile',"subtile",[True,False],default=True),
# solve for real/imag or gain/phase
TDLOption('gain_phase_solution',"Gain/Phase",[True,False],default=False),
# beams
TDLOption('stationbeam',"Station Beam",[True,False],default=True),
);


TDLCompileMenu("Pre-flags",
TDLOption('minclip',"Clip Value",[1e5,10],more=float,doc="Absolute value to clip"),
);

TDLCompileMenu("Post-flags",
TDLOption('mmse_sigma',"MMSE noise var",[2e-5,1e-5],more=float,doc="noise variance for MMSE correction"),
TDLOption('rms_sigmas',"RMS sigmas",[5,10],more=float,doc="How many sigmas away to flag"),
TDLOption('abs_clipval',"Abs Cutoff",[1e4,1e5],more=float,doc="Clipping value"),
TDLOption('max_condnum',"Max condition number",[3,4],more=float,doc="Max condition number"),
);

# correct for all baselines
TDLCompileOption('include_short_base',"All Baselines",[True,False],default=True);


# number of stations
TDLCompileOption('num_stations',"Number of stations",[16,14,15,3]);

# which source model to use
TDLCompileOption('source_model',"Source model",[
    global_model.point_and_extended_sources_abs, # absolute
  ],default=0);
  
 
### MS input queue size -- must be at least equal to the no. of ifrs
ms_queue_size = 500


#### short baselines
short_baselines=[(1,2), (1,3), (1,4), (2,3), (2,4), (3,4),
                 (5,6), (5,7), (5,8), (6,7), (6,8), (7,8),
                 (9,10), (11,12), (13,14), (13,15), (13,16),
                 (14,15), (14,16), (15,16)]


def _define_forest(ns, parent=None, **kw):
  # create array model
  stations = range(1,num_stations+1);
  array = IfrArray(ns,stations,mirror_uvw=False);
  observation = Observation(ns);
  
  lsm=LSM()
  #for uv plane include all sources, including the SUN
  lsm.build_from_extlist("global.txt",ns)

  if parent:
    lsm.display()

  if not include_short_base:
    exclude_baselines=[]
  else:
    exclude_baselines=short_baselines



  source_list = source_model(ns,lsm,tablename=mytable);
  
  # add EJones
  beam_parms=[];
  if stationbeam:
    Ej = global_model.EJones_droopy_comp_stat(ns,array,source_list,observation.phase_centre.radec(),meptable='',solvables=beam_parms,solvable=False);
  else:
    Ej = global_model.EJones_droopy_comp(ns,array,source_list,observation.phase_centre.radec(),meptable='',solvables=beam_parms,solvable=False);

  corrupt_list = [
      CorruptComponent(ns,src,label='E',station_jones=Ej(src.direction.name))
      for src in source_list
  ];

  allsky = Patch(ns,'all',observation.phase_centre);
  allsky.add(*corrupt_list);

  # create simulated visibilities for sky
  visibilities = allsky.visibilities(array,observation);


  if dosubtile:
   def_tiling=record(time=1)
  else:
   def_tiling=None

  # Jones
  for station in array.stations():
    ns.Jreal11(station)<<Meq.Parm(1,node_groups='Parm',solvable=1,table_name=mytable, use_previous=2,save_all=mysave, tiling=def_tiling)
    ns.Jimag11(station)<<Meq.Parm(0,node_groups='Parm',solvable=1,table_name=mytable, use_previous=2,save_all=mysave, tiling=def_tiling)
    ns.J11(station)<<Meq.ToComplex(ns.Jreal11(station),ns.Jimag11(station))
    ns.Jreal22(station)<<Meq.Parm(1,node_groups='Parm',solvable=1,table_name=mytable, use_previous=2,save_all=mysave, tiling=def_tiling)
    ns.Jimag22(station)<<Meq.Parm(0,node_groups='Parm',solvable=1,table_name=mytable, use_previous=2,save_all=mysave, tiling=def_tiling)
    ns.J22(station)<<Meq.ToComplex(ns.Jreal22(station),ns.Jimag22(station))
    ns.Jreal12(station)<<Meq.Parm(0,node_groups='Parm',solvable=1,table_name=mytable, use_previous=2,save_all=mysave, tiling=def_tiling)
    ns.Jimag12(station)<<Meq.Parm(0,node_groups='Parm',solvable=1,table_name=mytable, use_previous=2,save_all=mysave, tiling=def_tiling)
    ns.J12(station)<<Meq.ToComplex(ns.Jreal12(station),ns.Jimag12(station))
    ns.Jreal21(station)<<Meq.Parm(0,node_groups='Parm',solvable=1,table_name=mytable, use_previous=2,save_all=mysave, tiling=def_tiling)
    ns.Jimag21(station)<<Meq.Parm(0,node_groups='Parm',solvable=1,table_name=mytable, use_previous=2,save_all=mysave, tiling=def_tiling)
    ns.J21(station)<<Meq.ToComplex(ns.Jreal21(station),ns.Jimag21(station))

    ns.G(station)<<Meq.Matrix22(ns.J11(station), ns.J12(station), ns.J21(station), ns.J22(station))


  allsky = CorruptComponent(ns,allsky,label='G',station_jones=ns.G);

  # create simulated visibilities for sky
  predict = allsky.visibilities(array,observation);

  # now create spigots, condeqs and residuals
  solve_ce=[]
  for sta1,sta2 in array.ifrs():
    spigot = ns.spigot(sta1,sta2) << Meq.Spigot( station_1_index=sta1-1,
                                                 station_2_index=sta2-1,
                                                 flag_bit=4,
                                                 input_col='DATA');
    if (sta1,sta2) not in exclude_baselines:
      pred = predict(sta1,sta2);
      ns.ce(sta1,sta2) << Meq.Condeq(spigot,pred);
      solve_ce+=[ns.ce(sta1,sta2)]

  ### add MMSE equalization
  sigma2=mmse_sigma;
  ns.I0<<Meq.Matrix22(sigma2,0,0,sigma2)
  # threshold for flagging - condition number
  fthreshold=max_condnum;

  for station in array.stations():
    # get min value
    ns.Jmin(station)<<Meq.Min(Meq.Abs(ns.J11(station)+sigma2),Meq.Abs(ns.J22(station)+sigma2))
    ns.Jmax(station)<<Meq.Max(Meq.Abs(ns.J11(station)+sigma2),Meq.Abs(ns.J22(station)+sigma2))
    # catch to ignore division by zero
    ns.Jmin0(station)<<Meq.Max(ns.Jmin(station),1e-9)
    # condition number
    ns.Jcond(station)<<ns.Jmax(station)/ns.Jmin0(station)
    ns.Gc(station)<<ns.G(station)+ns.I0
    # flag
    fc = ns.lim(station) << ns.Jcond(station) - fthreshold;
    ns.Gflagged(station) << Meq.MergeFlags(ns.Gc(station),Meq.ZeroFlagger(fc,flag_bit=2,oper="GE"))



  Jones.apply_correction(ns.corrected,ns.spigot,ns.Gflagged,array.ifrs());

  ## attach a clipper too as final flagging step
  threshold_sigmas=rms_sigmas;
  ## also cutoff
  abs_cutoff=abs_clipval
  for sta1,sta2 in array.ifrs():
    inp = ns.corrected(sta1,sta2);
    a = ns.flagged("abs",sta1,sta2) << Meq.Abs(inp);
    stddev_a = ns.flagged("stddev",sta1,sta2) << Meq.StdDev(a);
    delta = ns.flagged("delta",sta1,sta2) << Meq.Abs(a-Meq.Mean(a));
    fc = ns.flagged("fc",sta1,sta2) << delta - threshold_sigmas*stddev_a;
    inp=ns.flagged0(sta1,sta2) << Meq.MergeFlags(inp,Meq.ZeroFlagger(fc,flag_bit=2,oper="GE"))
    fc = ns.flagged("abscutoff",sta1,sta2) << a - abs_cutoff;
    ns.flagged(sta1,sta2) << Meq.MergeFlags(inp,Meq.ZeroFlagger(fc,flag_bit=2,oper="GE"));
 

  # set up a non-default condeq poll order for efficient parallelization 
  # (i.e. poll child 1:2, 3:4, 5:6, ..., 25:26, then the rest)
  cpo = [];
  #for i in range(array.num_stations()/2):
  #  (sta1,sta2) = array.stations()[i*2:(i+1)*2];
  #  cpo.append(ns.ce(sta1,sta2).name);
  # create solver node
  ns.solver << Meq.Solver(children=solve_ce);
 
  # create sinks and reqseqs 
  for sta1,sta2 in array.ifrs():
    reqseq = Meq.ReqSeq(ns.solver,ns.flagged(sta1,sta2),
                  result_index=1,cache_num_active_parents=1);
    ns.sink(sta1,sta2) << Meq.Sink(station_1_index=sta1-1,
                                   station_2_index=sta2-1,
                                   flag_bit=4,
                                   corr_index=[0,1,2,3],
                                   flag_mask=-1,
                                   output_col='PREDICT',
                                   children=reqseq
                                   );
                                   
  # create visdatamux
  global _vdm;
  _vdm = ns.VisDataMux << Meq.VisDataMux(dep_mask=0xff); #recover from fails in bath mode
  ns.VisDataMux.add_children(*[ns.sink(*ifr) for ifr in array.ifrs()]);
  ns.VisDataMux.add_stepchildren(*[ns.spigot(*ifr) for ifr in array.ifrs()]);
  
  
def create_solver_defaults(num_iter=solver_maxiter,epsilon=1e-4,convergence_quota=0.9,solvable=[],debug_file=None):
  solver_defaults=record()
  solver_defaults.num_iter      = num_iter
  solver_defaults.epsilon       = epsilon
  solver_defaults.epsilon_deriv = epsilon
  solver_defaults.lm_factor     = solver_lm_factor
  solver_defaults.convergence_quota = convergence_quota
  solver_defaults.balanced_equations = False
  solver_defaults.debug_level = solver_debug_level;
  #solver_defaults.save_funklets= True
  solver_defaults.save_funklets= False
  #solver_defaults.last_update  = True
  solver_defaults.last_update  = False
  solver_defaults.debug_file = debug_file
#See example in TDL/MeqClasses.py
  solver_defaults.solvable     = record(command_by_list=(record(name=solvable,
                                       state=record(solvable=True)),
                                       record(state=record(solvable=False))))
  return solver_defaults
  
def set_node_state (mqs,node,fields_record):
  """helper function to set the state of a node specified by name or
  nodeindex""";
  rec = record(state=fields_record);
  if isinstance(node,str):
    rec.name = node;
  elif isinstance(node,int):
    rec.nodeindex = node;
  else:
    raise TypeError,'illegal node argument';
  # pass command to kernel
  mqs.meq('Node.Set.State',rec,wait=True);
  pass
  

def _perturb_solvables (mqs,solvables,rng=[0.2,0.3]):
  global perturbation;
  for name in solvables:
    polc = mqs.getnodestate(name).real_polc;
    if perturbation == "random":
      polc.coeff[0,0] *= 1 + random.uniform(*rng)*random.choice([-1,1]);
    else:
      polc.coeff[0,0] *= 1+perturbation;
    set_node_state(mqs,name,record(init_funklet=polc));
  return solvables;
    
def _reset_solvables (mqs,solvables,value=None):
  for name in solvables:
    polc = mqs.getnodestate(name).real_polc;
    if value is not None:
      polc.coeff[0,0] = value;
    set_node_state(mqs,name,record(init_funklet=polc));
  return solvables;


def create_inputrec (_ms_selection,msname=None):
  rec = record();
  rec.ms_name          = msname
  rec.data_column_name = input_column;
  rec.tile_size        = tile_size
  rec.selection = _ms_selection or record();
  rec = record(ms=rec);
  rec.python_init = 'Meow.ReadVisHeader';
  rec.mt_queue_size = ms_queue_size;
  return rec;


def create_outputrec (outcol):
  rec=record()
  rec.mt_queue_size = ms_queue_size;
  rec.write_flags=True
  rec.predict_column=outcol;
  return record(ms=rec);


def _run_solve_job (mqs,solvables,_ms_selection,msname=None,wait=False,debug_file=None):
  """common helper method to run a solution with a bunch of solvables""";
  req = meq.request();
  req.input  = create_inputrec(_ms_selection,msname);
  if output_column is not None:
    req.output = create_outputrec(output_column);

  # set solvables list in solver
  solver_defaults = create_solver_defaults(solvable=solvables,debug_file=debug_file)
  set_node_state(mqs,'solver',solver_defaults)

  mqs.execute('VisDataMux',req,wait=wait);
  pass


def _tdl_job_0_run_pipeline(mqs,parent,**kw):
  ########### read in the MS file list ########
  infile=open(msnames,'r')
  filelist=[]
  for line in infile:
    filelist += line.strip().split()
  infile.close()
  
  ### run each file through the pipeline
  if do_preprocess:
    for fname in filelist:
      _do_preprocess(fname,mqs);
  if do_calibrate:
    for fname in filelist:
      _do_calibrate(fname,mqs);
  if do_postprocess:
    for fname in filelist:
      _do_postprocess(fname,mqs);

def _do_preprocess(fname,mqs):
  if fname==None: return
  # add additional dummy 'arg' for glish to work properly
  os.spawnvp(os.P_WAIT,'glish',['glish','-l','preprocess.g','args','ms='+fname,'minuv=1','minclip='+str(minclip)]);


def _do_calibrate(fname,mqs):
  if fname==None: return

  ### setup solvables #####
  solvables = ['Jreal11:'+str(station) for station in range(1,num_stations+1)];
  solvables += ['Jimag11:'+str(station) for station in range(1,num_stations+1)];
  if full_J:
    solvables += ['Jreal12:'+str(station) for station in range(1,num_stations+1)];
    solvables += ['Jimag12:'+str(station) for station in range(1,num_stations+1)];
    solvables += ['Jreal21:'+str(station) for station in range(1,num_stations+1)];
    solvables += ['Jimag21:'+str(station) for station in range(1,num_stations+1)];
  solvables += ['Jreal22:'+str(station) for station in range(1,num_stations+1)];
  solvables += ['Jimag22:'+str(station) for station in range(1,num_stations+1)];

    
  for spwid in range(min_spwid-1,max_spwid):
    for schan in range(start_channel,end_channel,channel_step):
      ms_selection=record(channel_start_index=schan,
          channel_end_index=schan+channel_step-1,
          channel_increment=1,
          ddid_index=spwid,
          selection_string='sumsqr(UVW[1:2]) > 10')
      parmtablename=fname+"_"+str(schan)+"_"+str(spwid)+".mep";
      if mytable !=None:
        for sname in solvables:
          set_node_state(mqs,sname,record(table_name=parmtablename))
      debug_filename=fname+"_"+str(schan)+"_"+str(spwid)+".log"
      _run_solve_job(mqs,solvables,ms_selection,wait=True,msname=fname,debug_file=debug_filename);
      #try:
      #  solver_plots.create_residual_plots(debug_filename)
      #except: pass


def _do_postprocess(fname,mqs):
  if fname==None: return
  # post processing will make images, and calculate mean image - dont wait here 
  # because we will go on to the next MS
  os.spawnvp(os.P_WAIT,'glish',['glish','-l','postprocess_uv.g','args','ms='+fname,'spwids='+str(max_spwid),'startch='+str(start_channel+1),'endch='+str(end_channel+1),'step='+str(channel_step)]);
  # residual plots
  for spwid in range(0,max_spwid):
    for schan in range(start_channel,end_channel,channel_step):
      debug_filename=fname+"_"+str(schan)+"_"+str(spwid)+".log"
      solver_plots.create_residual_plots(debug_filename)

  # cleanup any crumbs left from glish
  # to be done



Settings.forest_state.cache_policy = 1  # -1 for minimal, 1 for smart caching, 100 for full caching
Settings.orphans_are_roots = False

def _test_compilation ():
  ns = NodeScope();
  _define_forest(ns);
  ns.Resolve();

if __name__ == '__main__':
  # -run option causes us to run in batch mode
  if '-run' in sys.argv:
    from Timba.Apps import meqserver
    from Timba.TDL import Compile
    from Timba.TDL import TDLOptions

    # this starts a kernel. Note how we pass the "-mt 7" option
    mqs = meqserver.default_mqs(wait_init=10,extra=["-mt","7"]);

    # this loads a tdl.conf file, and selects a section based on our filename
    # note that it is probably better to use an explicit config file, rather
    # than the default .tdl.conf that the browser creates
    TDLOptions.config.read(".tdl.conf");
    # this points TDL at the appropriate section of the config file. It's
    # up to you to make sure this section exists.
    print "reading config section",__file__;
    TDLOptions.init_options(__file__);

    # this compiles a script as a TDL module. Any errors will be thrown as
    # and exception, so this always returns successfully
    (mod,ns,msg) = Compile.compile_file(mqs,__file__);

    # this runs the appropriate solve job. wait=True is needed to wait
    # until the job is finished before exiting
    mod._tdl_job_0_run_pipeline(mqs,None,wait=True);


  # -prof option profiles TDL compilation
  elif '-prof' in sys.argv:
    import profile
    profile.run('_test_compilation()','pipeling.prof');
  # default is simply to test compilation
  else:
#    Timba.TDL._dbg.set_verbose(5);
    _test_compilation();
