#-----------------------------------------------------------------------
#Name: CGpipeline
#-----------------------------------------------------------------------
#Type: Function
#Ret: bool
#Doc: A basic pipeline for LOPES data using the GUI routines
#
#Par: i1 = 1   - Starting event (refers to position in evlist if this is also given)
#Par: i2 = 1   - Ending event (refers to position in evlist if this is also given)
#Par: fileno = 1    - Event table file number to choose appropriate selection
#Par: evlist = []   - List with event numbers, used only if it has non-zero length
#Par: dirname = '.' - directory to put the files in
#Par: genps = T     - generate postscript output files
#Par: filename = 'CGpipeline-results' - name of the logfile
#Par: psfilename = 'CGpipeline-'      - prefix for the ps-images
#Par: verbose = T                     - generate verbose output in the window
#Par: FitPosition = T                 - do a simple fit of position and distance
#
#Example: CGpipeline(1,14,fileno=5) 
#-----------------------------------------------------------------------

CGpipeline := function(i1=1, i2=1, fileno=1, evlist=[], dirname='.', genps=T,
               filename = 'CGpipeline-results', psfilename = 'CGpipeline',
               verbose=T,FitPosition=T){
  starttime := time();

  print ' '
  print '========================================================================'
  print '                        Welcome to the new World!                       '
  print '========================================================================'
  print ' '
  print 'Results will be stored in Directory',dirname,'. Look for files of the form',spaste(psfilename,'*')
  print 'A summary is given in',spaste(filename,'*.g (GLISH file).')
  print ' '

  pipeline_client := client(concat_dir(globalconst.LOPESBIND,'CGuiPipeClient'));
  if (!is_agent(pipeline_client)) {
    print 'Could not create the CGpipeline-client! . . . Aborting.';
    pipeline_client := F;
    return F;
  }
  obsrec := [=]
  for (obs in obspar.observatories) {
    if (has_field(obspar.obs[obs],'CalTablePath') && 
        is_dir(obspar.obs[obs].CalTablePath[1])){
      obsrec[obs] := obspar.obs[obs].CalTablePath[1];
    };
  };
  if (!pipeline_client->initPipeline(obsrec)){
    print 'Could not initialize the CGpipeline-client! . . . Aborting.';
    pipeline_client := F;
    return F;
  };

  evtable := read_value(globalpar.DefTabfiles[fileno]);
  evtable.tabfilename :=  globalpar.DefTabfiles[fileno];

  if (len(evlist) ==0) evlist:=[1:i2]
  nevents:=max(evlist)

  if (!dexist(dirname)) {
    print 'Creating directory ',dirname; 
    cmd.mkdir(dirname)
  }
  logfileprefix :=  concat_dir(dirname,filename);
  logfileheader :=  sprintf('%s-%i-%i-header.g',logfileprefix,evlist[i1],evlist[i2]);
  lfn := open(paste('>',logfileheader));
  write(lfn,'# Output of the CGpipeline for:',evtable.tabfilename, sep=' ');
  write(lfn,'') ;
  write(lfn,sprintf('filename:=array(\'\',%i);date:=array(0,%i);',nevents,nevents))
  write(lfn,sprintf('Az:=array(0.,%i);El:=array(0.,%i);Dist:=array(0.,%i)',nevents,nevents,nevents))
  write(lfn,sprintf('Xconverged:=array(F,%i);CCconverged:=array(F,%i);',nevents,nevents))
  write(lfn,sprintf('Xheight:=array(0.,%i);CCheight:=array(0.,%i);',nevents,nevents))
  write(lfn,sprintf('Xcenter:=array(0.,%i);CCcenter:=array(0.,%i);',nevents,nevents))
  write(lfn,sprintf('Xwidth:=array(0.,%i);CCwidth:=array(0.,%i);',nevents,nevents))
  write(lfn,sprintf('Xheight_error:=array(0.,%i);CCheight_error:=array(0.,%i);',nevents,nevents))
  write(lfn,sprintf('Xwidth_error:=array(0.,%i);CCwidth_error:=array(0.,%i);',nevents,nevents))
  write(lfn,sprintf('Xcenter_error:=array(0.,%i);CCcenter_error:=array(0.,%i);',nevents,nevents))
  write(lfn,sprintf('rmsX:=array(0.,%i);rmsCC:=array(0.,%i);',nevents,nevents))
  write(lfn,sprintf('meandist:=array(0.,%i);',nevents))
  write(lfn,sprintf('Antdistances := [=];'))
  lfn := F;

  psprefix := concat_dir(dirname,psfilename);
  event_dir:= evtable.tabfilename~ globalconst.DirExtractor;

  for (i in evlist[i1:i2]) {#start of the big loop
    initrec := [=];
    initrec.PlotPrefix := sprintf('%s-%04i',psprefix,i)
#spaste(psprefix,'-',as_string(i));
    initrec.generatePlots := as_boolean(genps);
    initrec.verbose := as_boolean(verbose);
    initrec.FitPosition := as_boolean(FitPosition);
    initrec.RotatePos := T; #the XC and YC are in KASCADE coordinates
    initrec.distance := 4000;

    eventname := evtable.filename[i]~ globalconst.FilenameExtractor
    initrec.evname := concat_dir(event_dir,eventname);
    initrec.Az := evtable.AZ[i]/degree;
    initrec.El := 90-(evtable.ZE[i]/degree);
    initrec.XC := evtable.XC[i];
    initrec.YC := evtable.YC[i];
    initrec.FlaggedAntIDs := genCGpipeFlags(evtable.GT[i]);
    
    ergrec := pipeline_client->ProcessEvent(initrec);

    lfn := open(sprintf('>  %s-one-%04i.g',logfileprefix,i));
    write(lfn,'\n # -------------------------------------------\n');
    write(lfn,sprintf('filename[%i] := \'%s\' ',i,evtable.filename[i]));
    write(lfn,sprintf('date[%i] := %i ',i,evtable.GT[i]));
    write(lfn,sprintf('Az[%i] := %e ',i,ergrec.Azimuth));
    write(lfn,sprintf('El[%i] := %e ',i,ergrec.Elevation));
    write(lfn,sprintf('Dist[%i] := %e ',i,ergrec.Distance));
    if (ergrec.Xconverged) {
      write(lfn,sprintf('Xconverged[%i] := T ',i));
    } else {
      write(lfn,sprintf('Xconverged[%i] := F ',i));
    };
    write(lfn,sprintf('Xheight[%i] := %e ',i,ergrec.Xheight*1e6));
    write(lfn,sprintf('Xwidth[%i] := %e ',i,ergrec.Xwidth*1e6));
    write(lfn,sprintf('Xcenter[%i] := %e ',i,ergrec.Xcenter*1e6));
    write(lfn,sprintf('Xheight_error[%i] := %e ',i,ergrec.Xheight_error*1e6));
    write(lfn,sprintf('Xwidth_error[%i] := %e ',i,ergrec.Xwidth_error*1e6));
    write(lfn,sprintf('Xcenter_error[%i] := %e ',i,ergrec.Xcenter_error*1e6));
    if (ergrec.CCconverged) {
      write(lfn,sprintf('CCconverged[%i] := T ',i));
    } else {
      write(lfn,sprintf('CCconverged[%i] := F ',i));
    };
    write(lfn,sprintf('CCheight[%i] := %e ',i,ergrec.CCheight*1e6));
    write(lfn,sprintf('CCwidth[%i] := %e ',i,ergrec.CCwidth*1e6));
    write(lfn,sprintf('CCcenter[%i] := %e ',i,ergrec.CCcenter*1e6));
    write(lfn,sprintf('CCheight_error[%i] := %e ',i,ergrec.CCheight_error*1e6));
    write(lfn,sprintf('CCwidth_error[%i] := %e ',i,ergrec.CCwidth_error*1e6));
    write(lfn,sprintf('CCcenter_error[%i] := %e ',i,ergrec.CCcenter_error*1e6));
    write(lfn,sprintf('rmsCC[%i] := %e ',i,ergrec.rmsC));
    write(lfn,sprintf('rmsX[%i] := %e ',i,ergrec.rmsX));
    write(lfn,sprintf('meandist[%i] := %e ',i,ergrec.meandist));
    write(lfn,sprintf('Antdistances[\'%i\'] := %s ',i,as_inputvec(ergrec.distances)));
    lfn := F
  } #end of the big loop
  gestime := time()-starttime;
  gestime := gestime/60
  print 'Processed',(i2-i1+1),'Events in',as_integer(gestime),'minutes:',gestime/(i2-i1+1),'min/event';
  pipeline_client := F;
  print 'concatting results-files';
  logfileheader :=  sprintf('%s-%i-%i-header.g',logfileprefix,evlist[i1],evlist[i2]);
  logfilessingle :=  sprintf('%s-one-*.g',logfileprefix);
  logfileges :=  sprintf('%s-all.g',logfileprefix,evlist[i1],evlist[i2]);
  shell(paste('cat',logfileheader,logfilessingle,'>',logfileges));
};

genCGpipeFlags := function(date=time()){
  d2sec := function(indate) { return dq.convert(dq.quantity(indate),'s')['value']}
  antIDlist := [10101,10102,10201,10202,20101,20102,20201,20202,30101,30102,  
                40101,40102,40201,40202,50101,50102,50201,50202,60101,60102,
                70101,70102,70201,70202,80101,80102,80201,80202,90101,90102];

  datesec := date+ 3.5067168e+09;
  datadate := '2000/01/01/12:00:00.0';
  datasec := d2sec('2000/01/01/12:00:00.0');
  for (dstring in field_names(obspar.obs['LOPES'].badantennas) ){
    dsec := d2sec(dstring);
    if ( (datesec >= dsec) & (dsec >= datasec) ) {
      datadate := dstring;
      datasec := dsec;
    };
  };
  flaggids := antIDlist[obspar.obs['LOPES'].badantennas[datadate]];
  print 'Flagging IDs for date:',jdr2string(date),' :',flaggids;
  return flaggids
}
