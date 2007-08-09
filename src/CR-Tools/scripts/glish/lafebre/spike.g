#This file deals with the automatic detection of sudden pulses in the antenna
#flux. It includes functions to detect and group these peaks over different
#antennas and to derive a source for the event.

#Section: Functions
#------------------------------------------------------------------------------
#Name: makePeakGUI
#------------------------------------------------------------------------------
#Type: Function
#Doc:  Opens a window with options for detection of spikes in the event file
#      currently opened
#Par:  rec       - a reference to the data structure of the current event file
#Par:  gui       - a reference to the plotgui attached to the current event file
#Ref:  findPeaksInCurFile
#Todo: The functionality of the <i>export</i> button should be expanded, such 
#      that indeed a general export of the results to a datafile is initiated.
#------------------------------------------------------------------------------
makePeakGUI := function(ref rec, ref gui) {
    include 'synclistboxes.g';
    tk_hold();
    w  := frame(title=spaste('[',as_string(globalpar.plotpanelID),'] - Peak detection'), side='left');
    f  := frame(w, side='top', expand='both');

    # Top part: mark peaks in current plot view
    of := frame(f,side='left', expand='x');
      b_markpeaks := button(of,'Show peaks in current view', fill='x', anchor='w', relief='flat',type='check',borderwidth=1, hlthickness=0);
    of_a := frame(f, side='left', height=1, expand='x');
      of_a1  := frame(of_a, expand='none', height=1, width=15);
      b_showtags := button(of_a,'Show peak labels',  fill='x', anchor='w', relief='flat',type='check',borderwidth=1, hlthickness=0);
    of_d := frame(f, side='right', height=1, expand='x');

    mf1 := frame(f, expand='x', height='2', relief='sunken', borderwidth=1);

    f_a := frame(f, side='left', height=1, expand='x');
      t_sens := label(f_a, 'Sensitivity');
      f_a1   := frame(f_a, expand='x', height=1, width=15);
      i_sens := entry(f_a, width=5, borderwidth=1, background='white', fill='none', justify='right');
      if (rec.peaks_sens == -1) i_sens->insert(as_string(plotgui.peaks_sens));
      else i_sens->insert(as_string(rec.peaks_sens));
    f_b := frame(f, side='top', height=1, expand='x', borderwidth=0);
      f_b1           := frame(f_b, side='top', expand='x', height=1);
        b_overallavg := button(f_b1,'Use overall average', relief='flat',type='radio',
                               borderwidth=1, hlthickness=0, fill='x', anchor='w');
        b_runningavg := button(f_b1,'Use running average', relief='flat',type='radio',
                               borderwidth=1, hlthickness=0, fill='x', anchor='w');
      f_b2           := frame(f_b, side='left', expand='x', height=1);
        f_b2a        := frame(f_b2, expand='none', height=1, width=15);
        t_bs         := label(f_b2, 'Block size');
        f_b2b        := frame(f_b2, expand='x', height=1, width=15);
        i_bs         := entry(f_b2, width=5, borderwidth=1, background='white', fill='none', justify='right');
        if (rec.peaks_bs == 0) {
          b_overallavg->state(T);
          f_b2->disable();
          i_bs->insert('1024');
        } else {
          if (rec.peaks_bs == -1) {
            if (plotgui.peaks_bs == 0) {
              b_overallavg->state(T);
              f_b2->disable();
              i_bs->insert('1024');
            } else {
              b_runningavg->state(T);
              i_bs->insert(as_string(plotgui.peaks_bs));
            }
          } else {
            b_runningavg->state(T);
            i_bs->insert(as_string(rec.peaks_bs));
          }
        }

    mf2 := frame(f, expand='x', height='2', relief='sunken', borderwidth=1);

    f_data := frame(f, side='top', height=1, expand='x', borderwidth=0);

      f_data1        := frame(f_data, side='top', expand='x', height=1);
        b_file  := button(f_data1,'Process complete file', relief='flat',type='radio',
                               borderwidth=1, hlthickness=0, fill='x', anchor='w');
        b_block := button(f_data1,'Process loaded datablock', relief='flat',type='radio',
                               borderwidth=1, hlthickness=0, fill='x', anchor='w');
        b_block->state(T);

    mf3 := frame(f, expand='x', height='2', relief='sunken', borderwidth=1);

    rf_tit := message(f, 'Detections', fill='x', anchor='w');

    rf := frame(f,side='top', expand='both', relief='sunken', borderwidth=1);
    rg := frame(rf,side='left', expand='x', relief='flat', borderwidth=0);
    listhead := synclistboxes(parent=rg,
                    nboxes=6, labels=['Nr.','Antennas','t','Az','El','Dist'],
                    vscrollbar=F, hscrollbar=F,
                    height=1, width=[4,12,8,5,5,5],
                    mode='single',
                    background='lightgrey',
                    relief='flat', borderwidth=0,
                    font='fixed',
                    fill='both');
    rga := frame(rg, expand='none', relief='flat', borderwidth=0, width=21, height=1);
    listhead.listbox(1)->insert('Nr.');
    listhead.listbox(2)->insert('Antennas');
    listhead.listbox(3)->insert('Time (s)');
    listhead.listbox(4)->insert('Az.');
    listhead.listbox(5)->insert('El.');
    listhead.listbox(6)->insert('Dist.');
    list := synclistboxes(parent=rf,
                    nboxes=6, labels=['Nr.','Antennas','t','Az','El','Dist'],
                    vscrollbarright=T,
                    hscrollbar=F,
                    height=4, width=[4,12,8,5,5,5],
                    mode='single',
                    background=['lightgrey','white','white','white','white','white'],
                    relief='flat', borderwidth=0,
                    font='fixed',
                    fill='both');
    # Apply and Dismiss buttons
    g := frame(w,side='top', expand='y');
      b_apply   := button(g, 'Refresh', borderwidth=1, width=6);
      f_g1      := frame(g,expand='y', width='1');
      b_find    := button(g, 'Find', borderwidth=1, background='limegreen', width=6);
      b_export  := button(g, 'Export', borderwidth=1, background='yellow', width=6);
      b_done    := button(g, 'Dismiss', borderwidth=1, background='orange', width=6);
    tk_release();

    b_markpeaks->state(gui.peaks_show);
    b_showtags ->state(gui.peaks_tags);

    setoptionstatus := function() {
      if (b_markpeaks->state()) {
        while (b_showtags->disabled()) {
          of_a->enable();
        }
      } else {
        while (! b_showtags->disabled()) {
          of_a->disable();
        }
      }
    }

    addCR := function(i, sig) {
      list.listbox(1)->insert(sprintf("%4d", i));
      list.listbox(2)->insert(((spaste(sig.ant)~s/\s+(?!$)/,/g)~s/^\[//)~s/\].*$//);
      list.listbox(3)->insert(sprintf("%.3f", sig.x0*1e6));
      if (sig.az != -1) {
        list.listbox(4)->insert(sprintf("%3d", sig.az));
      } else {
        list.listbox(4)->insert('---');
      }
      if (sig.az != -1) {
        list.listbox(5)->insert(sprintf("%3d", sig.el));
      } else {
        list.listbox(5)->insert('---');
      }
    }
    clearCRs := function() {
        list.listbox(1)->delete('start','end');
        list.listbox(2)->delete('start','end');
        list.listbox(3)->delete('start','end');
        list.listbox(4)->delete('start','end');
        list.listbox(5)->delete('start','end');
        list.listbox(6)->delete('start','end');
    }

    setoptionstatus();
    for (i in 1:len(rec.events)) {
      addCR(i, rec.events[i]);
    }

    whenever b_markpeaks->press do {
      gui.peaks_show := b_markpeaks->state();
      setoptionstatus();
    }

    whenever b_overallavg->press do {
      f_b2->disable();
    }
    whenever b_runningavg->press do {
      f_b2->enable();
    }

    whenever b_showtags->press do {
      gui.peaks_tags := b_showtags->state();
    }

    whenever b_apply->press do {
      gui.peaks_sens := as_double(i_sens->get());
      if (b_overallavg->state()) gui.peaks_bs := 0;
        else                     gui.peaks_bs := as_integer(i_bs->get());
      w->cursor('watch');
      gui.replot();
      w->cursor('left_ptr');
    }

    whenever b_find->press do {
      # Read settings from the GUI
      gui.peaks_sens := as_double(i_sens->get());
      if (b_overallavg->state()) gui.peaks_bs := 0;
        else                     gui.peaks_bs := as_integer(i_bs->get());
      w->cursor('watch');
      clearCRs();
      if (as_boolean(b_file->state())) {
	# Compute number of datablocks
	Filesize  := rec.getmeta('Filesize');
	Blocksize := rec.getmeta('Blocksize');
	nofBlocks := as_integer(Filesize/Blocksize);
	bar := progress (0,nofBlocks, 'Scanning data for peaks ...')
	bar.activate()
        # Go through all datablocks
	for (block in 1:nofBlocks) {
	  rec.setblock(n=block);
	  gui.replot();
	  ok := findPeaksInCurFile(sigs, rec, gui);
	  rec.events := sigs;
	  for (i in 1:len(sigs)) {
	    addCR(i, sigs[i]);
	  }
	  bar.update(block);
	}
        bar.done();
	rec.setblock(n=1);
      } else {
        ok := findPeaksInCurFile(sigs, rec, gui);
        rec.events := sigs;
        for (i in 1:len(sigs)) {
          addCR(i, sigs[i]);
        }
      }
      gui.replot();
      w->cursor('left_ptr');
    }

    whenever b_export->press do {
      print "List of peaks detected in",rec.data[1].Header.Date;
      print " - Sensitivity .. :",as_double(i_sens->get());
      print " - Block size ... :",as_integer(i_bs->get());
      for (i in 0:len(rec.events)) {
	print list.listbox(1)->get(i),"&",
              list.listbox(2)->get(i),"&",
              list.listbox(3)->get(i),"&",
              list.listbox(4)->get(i),",",list.listbox(5)->get(i);
      }
    }

    whenever b_done->press do {
      gui.peaks_sens := as_double(i_sens->get());
      if (b_overallavg->state()) gui.peaks_bs := 0;
        else                     gui.peaks_bs := as_integer(i_bs->get());
      w := F;
    }
}

#------------------------------------------------------------------------------
#Name: makePeakBatchGUI
#------------------------------------------------------------------------------
#Type: Function
#Doc:  Opens a window with options for detection of spikes in a series of event
#      data sets
#Par:  rec    - a reference to the current plotgui
#Ref:  findPeaksInCurDir
#------------------------------------------------------------------------------
makePeakBatchGUI := function(ref rec) {
    tk_hold();
    f  := frame(title=spaste('[',as_string(globalpar.plotpanelID),'] - Peak detection batch'));

    t_dosearch := label(f,'Search current event dir for spikes');
    f_a := frame(f, side='left', height=1, expand='both');
      t_sens := label(f_a, 'Sensitivity');
      f_a1   := frame(f_a, expand='x', height=1, width=15);
      i_sens := entry(f_a, width=5, borderwidth=1, background='white', justify='right');
      i_sens->insert(as_string(rec.peaks_sens));
    f_b := frame(f, side='left', height=1, expand='both');
      t_bs   := label(f_b, 'Block size');
      f_b1   := frame(f_b, expand='x', height=1, width=15);
      i_bs   := entry(f_b, width=5, borderwidth=1, background='white', justify='right');
      i_bs->insert(as_string(rec.peaks_bs));
    mf1 := frame(f, expand='x', height=2, relief='sunken', borderwidth=1);
    f_e := frame(f, side='left');
      t_dir      := label(f_e, 'Source dir');
      f_e1       := frame(f_e, height=1, width=5, expand='x');
      i_dir      := entry(f_e, width=25, borderwidth=1, background='white', fill='x');
      b_brow     := button(f_e, '...', bitmap=(spaste(globalconst.ImageDir,'browse.xbm')),borderwidth=1, width=2);
      popuphelp(b_brow, "Browse");
      i_dir->insert(spaste(globalpar.cur_event_dir,globalconst.DirSeparator));
    _fi := shell(spaste('ls ',globalpar.cur_event_dir,' | grep "\.event$" | wc -l')) ~ s/^\s*//;
    f_f := frame(f, side='left');
      t_start    := label(f_f, 'File nr.');
      f_f1       := frame(f_f, height=1, width=5, expand='none');
      i_start    := entry(f_f, width=5, borderwidth=1, background='white', justify='right');
      f_f2       := frame(f_f, height=1, width=5, expand='x');
      t_end      := label(f_f, 'to');
      f_f3       := frame(f_f, height=1, width=5, expand='none');
      i_end      := entry(f_f, width=5, borderwidth=1, background='white', justify='right');
      i_start->insert('1');
      i_end  ->insert(as_string(_fi));
    mf2 := frame(f, expand='x', height=2, relief='sunken', borderwidth=1);
    f_c := frame(f, side='left');
      t_file     := label(f_c, 'Output file');
      f_c1       := frame(f_c, height=1, width=5, expand='x');
      i_file     := entry(f_c, width=30, borderwidth=1, background='white', fill='x');
      file := globalpar.cur_event_dir;
      if (last_char(file) == globalconst.DirSeparator) file =~ s/.$//;
      i_file->insert(spaste(file,'.peaks'));
    f_d := frame(f, side='left');
      r_da       := button(f_d, 'Append to file (do not overwrite)', type='check', relief='flat', borderwidth=1,
                           hlthickness=0, fill='x');
      r_da->state(T);
    mf3 := frame(f, expand='x', height=2, relief='sunken', borderwidth=1);

    f_t := message(f,'Press "Start" to search these files', justify='center', padx=10, pady=5);

    # Start and Cancel buttons
    bf := frame(f,side='right', expand='x');
      b_done    := button(bf, 'Cancel', borderwidth=1, background='orange', fill='x', width=10);
      b_go      := button(bf, 'Start', borderwidth=1, background='limegreen',  fill='x', width=10);
    tk_release();

    whenever b_go->press do {
      rec.peaks_sens := as_double(i_sens->get());
      rec.peaks_bs   := as_integer(i_bs->get());
      f->cursor('watch');
      f->disable();
      
      findPeaksInCurDir(i_dir->get(), as_integer(i_bs->get()), as_double(i_sens->get()),
                        label=f_t, file=i_file->get(), append=r_da->state(),
                        start=as_integer(i_start->get()), end=as_integer(i_end->get()));
      f_t->text(spaste('Output written to ',i_file->get(),'.'));
      b_go->background('lightgrey');
      b_go->disable();
      f->enable();
      b_done->text('Dismiss');
      f->cursor('left_ptr');
    }

    insert_dir := function(filename) {
      _fi := shell(spaste('ls ',filename,' | grep "\.event$" | wc -l')) ~ s/^\s*//;
      i_dir  ->delete('start','end');
      i_dir  ->insert(filename);
      i_start->delete('start','end');
      i_start->insert('1');
      i_end  ->delete('start','end');
      i_end  ->insert(as_string(_fi));
      i_file ->delete('start','end');
      if (last_char(filename) == globalconst.DirSeparator) filename =~ s/.$//;
      i_file->insert(spaste(filename,'.peaks'));
    }

    whenever b_brow->press do {
      global dcstarted;
      dc.setselectcallback(insert_dir);
      dc.gui(F,all,F);
      if (!dcstarted){
        dc.show(i_dir->get(),'All',T);
        dcstarted := T;
      }
    }

    whenever b_done->press do {
      rec.peaks_sens := as_double(i_sens->get());
      rec.peaks_bs   := as_integer(i_bs->get());
      f := F;
    }
}

#-------------------------------------------------------------------------------
#Name: groupPeaks
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  This function tries to match peaks from different antennas that could
#      possibly originate from the same event or source. This is done by
#      checking if the time delay between peaks in two different antennas is
#      smaller than the theoretical maximum time delay from light speed and
#      antenna distance. The grouped events are stored in a record object.
#Par:  ret    - reference to a peak group object. This is where the
#               result will be written to. This object is an array, the length
#               of which is the number of grouped events that were found. Each
#               of the array elements is a record containing the fields:
#               <ul><li>
#               <b>ant</b>: array containing the antennas for which a peak was
#               found in the event range.
#               </li><li>
#               <b>x0</b>: the detection time of the first antenna in
#               <b>ant</b>.
#               </li><li>
#               <b>x</b>: array of the detection times of every antenna,
#               relative to the detection time of the first antenna. This means
#               the first element of this array always equals zero.
#               </li><li>
#               <b>y</b>: array of the flux of every antenna for this event.
#               </li><li>
#               <b>s</b>: array of the significance (sigma value above the
#               running average) of every antenna for this event.
#               </li><li>
#               <b>az</b>: the azimuthal angle of the event. This is left blank
#               (set to -1) and can be determined by passing the object to the <a
#               href="spike.doc.html#estimateDir">estimateDir</a>
#               function.
#               </li><li>
#               <b>el</b>: the elevation angle of the event. This is left blank
#               (set to -1) and can be determined by passing the object to the <a
#               href="spike.doc.html#estimateDir">estimateDir</a>
#               function.
#               </li><li>
#               <b>r</b>: the distance to the event in the direction of
#               <b>az</b> and <b>el</b>. This is left blank (set to -1) and can
#               be determined by passing the object to the <a
#               href="spike.doc.html#estimateDir">estimateDir</a>
#               function.
#               </li></ul>
#Par:  px     - twodimensional array with index [antenna number, peak
#               number] giving the time value of a detection.
#Par:  py     - twodimensional array with index [antenna number, peak
#               number] giving the flux (f(x)^2) of a detection.
#Par:  ps     - twodimensional array with index [antenna number, peak
#               number] giving the significance of a detection.
#Par:  _peaks - array with index [antenna number] giving the number of
#               detections per antenna.
#Par:  antpos - array of array, with index [antenna number] giving
#               the position [x, y, z] of an antenna as given by
#               <a href="map.doc.html#CalcAntPos">calcAntPos</a>.
#-------------------------------------------------------------------------------
groupPeaks := function(ref ret, px, py, ps, _peaks, antpos) {
  groups := [=];
  _groups := 0;

  for (_ant in 1:len(_peaks)) {
    if (_peaks[_ant] > 0) for (_p in 1:_peaks[_ant]) {
      if (py[_ant,_p] != 0) {
        # Check confirmed correlations
        cont := 2;
        _g := 1;
        if (_groups > 0) while (cont > 1) {
          if (((px[_ant,_p]-groups[_g].x0)*lightspeed)^2
                < sum((antpos[_ant].position-antpos[groups[_g].ant[1]].position)^2)) {
            n := len(groups[_g].ant)+1;
            for (m in 1:len(groups[_g].ant)) if (_ant == groups[_g].ant[m]) {
              if (ps[_ant,_p] > groups[_g].s[m]) {
                n := m;
              } else {
                n := 0;
              }
            }
            if (n > 0) {
              groups[_g].ant[n] := _ant;
              groups[_g].x[n]   := px[_ant,_p]-groups[_g].x0;
              groups[_g].y[n]   := py[_ant,_p];
              groups[_g].s[n]   := ps[_ant,_p];
              groups[_g].az     := -1;
              groups[_g].el     := -1;
              groups[_g].r      := -1;
            }
            py[_ant,_p] := 0;
            cont := 0;
          }
          _g +:= 1;
          if (_g > _groups) cont := 1;
        }
        # Check other peaks
        if (_ant < len(_peaks) && cont > 0) for (_g in (_ant+1):len(_peaks)) {
          for (m in 1:_peaks[_g]) {
            if (py[_g,m] != 0 && py[_ant,_p] != 0) {
              if (((px[_ant,_p]-px[_g,m])*lightspeed)^2
                    < sum((antpos[_ant].position-antpos[_g].position)^2)) {
                _groups +:= 1;
                groups[_groups] := [x0 =px[_ant,_p],
                                    ant=[_ant,_g],
                                    x  =[0,px[_g,m]-px[_ant,_p]],
                                    y  =[py[_ant,_p],py[_g,m]],
                                    s  =[ps[_ant,_p],ps[_g,m]]];
                py[_ant,_p] := 0; py[_g,m] := 0;
              }
            }
          }
        }
      }
    }
  }
  val ret := groups;
}

#-----------------------------------------------------------------------
#Name: findPeaksInCurFile
#-----------------------------------------------------------------------
#Type: Function
#Doc:  This function finds peaks in the  event file that is currently
#      open, and tries to group 'matching' peaks (i.e. time difference
#      is smaller than light travel time) together. If there are more
#      than 3 antennas in such a group, a direction estimation is also
#      performed.
#Par:  sigs - Reference to an object containing information on events.
#             Results of the function will be written to this variable.
#Par:  dat  - Reference to a data structure.
#Par:  gui  - Reference to a plotgui structure.
#Ref:  data.getary, CalcAntPos, findPeaksInData, groupPeaks, estimateDir
#-----------------------------------------------------------------------
findPeaksInCurFile := function(ref sigs, ref dat, ref gui) {
  px := array(0.,globalpar.maxants,100);
  py := array(0.,globalpar.maxants,100);
  ps := array(0.,globalpar.maxants,100);

  gui.pleasewait();
  local pres := [=];
  for (_ant in 1:len(dat.ant())) {
    rf_tit->text(spaste('Analyzing antenna ', _ant, '/', len(dat.ant()), '...'));
    xary := dat.get('Time', dat.ant()[_ant]);
    yary := as_double(dat.get('f(x)', dat.ant()[_ant]));
    yary *:= yary;
    findPeaksInData(pres[_ant], xary, yary, gui.peaks_bs, gui.peaks_sens);
    xary := dat.get('Time', dat.ant()[_ant]);
    yary := as_double(dat.get('f(x)', dat.ant()[_ant]));
    gui.messlabel->text(spaste('Analyzing antenna ', _ant, '/', len(dat.ant()), '...'));
    yary *:= yary;
    _peaks[_ant]     := pres[_ant].count;
    px[_ant,[1:100]] := pres[_ant].x;
    py[_ant,[1:100]] := pres[_ant].y;
    ps[_ant,[1:100]] := pres[_ant].s;
  }
  dat.peaks_bs   := gui.peaks_bs;
  dat.peaks_sens := gui.peaks_sens;
  # Get delay adjustments
  deladj := dat.getary('Corrdelay');
  for (_p in 1:100) {
    px[1:len(dat.ant()),_p] +:= deladj;
  }

  # Try to group "matching" spikes, i.e. spikes within lightspeed time difference
  # of different antennas
  local groups := [=];
  antpos := CalcAntPos(dat,dat.ant());
  groupPeaks(groups, px, py, ps, _peaks, antpos);
  for (_g in 1:len(groups)) {
    if (len(groups[_g].ant) > 3) {
      estimateDir(groups[_g], antpos);
    }
  }
  val sigs := groups;
  val dat.peaks := pres;
  gui.endwait();

  return F;

}

#------------------------------------------------------------------------------
#Name: findPeaksInCurDir
#------------------------------------------------------------------------------
#Type: Function
#Doc:  As <a href="spike.doc.html#findPeaksInCurFile">findPeaksInCurFile</a>,
#      this function searches for peaks and performs an analysis. Instead of
#      analysing one file, this function acts on a range of files in a certain
#      directory.
#Par:  dir              - The event directory to be searched.
#Par:  peaks_bs         - The block size over which the running average, used
#                         to mark peaks, is to be calculated. Use 0 for a
#                         non-running average.
#Par:  peaks_sens       - The sigma value above which data is to be regarded as
#                         a peak
#Par:  label=''         - Reference to a message object, to which progress
#                         information can be written.
#Par:  file='/dev/null' - The name of the output file for the detected events.
#Par:  start=1          - The number of the first file in <b>dir</b> to be analysed.
#Par:  end=0            - The number of the last file in <b>dir</b> to be analysed.
#                         Set this to 0 to analyse up to the last file in <b>dir</b>.
#Par:  append=T         - If T, appends the results to <b>file</b>. If F, deletes <b>file</b>
#                         and then writes results to it.
#Ref:  CalcAntPos, findPeaksInData, groupPeaks, estimateDir
#------------------------------------------------------------------------------
findPeaksInCurDir := function(dir, peaks_bs, peaks_sens, ref label='', file="/dev/null", start=1, end=0, append=T) {
    if (! append && file != "/dev/null") {
        shell(spaste('if [ -e ',file,' ]; then rm ',file,'; fi'));
        shell(spaste('echo "# Lopes AIPS peak detection output file. This file was generated from event files in ',
                     dir, '.\n',
                     '# filename\tantennas\ttime\tazimuth\televation\tdistance" >> ', file));
    }

    px := array(0.,globalpar.maxants,100);
    py := array(0.,globalpar.maxants,100);
    ps := array(0.,globalpar.maxants,100);

    # Get list of files to be done
    fi := shell(spaste('ls ', dir, ' | grep "\.event"'));
    if (end == 0 || end > len(fi)) end := len(fi);
    if (end < start) end := start;
    for (_fi in start:end) {
      print '---', fi[_fi], ' ---';
      shell(spaste('date +"# %Y-%m-%d, %H:%M --- file ', _fi, '/', len(fi), '" >> ', file));
      label->text(spaste('Processing file ',_fi,' of ',len(fi),'...'));
      # Get an event file
      input_event(locdat, spaste(dir,fi[_fi]));
      #input_event(locdat, "/data/evdata/040201/2004.02.01.06:23:44.691.event");
      #input_event(locdat, "/data/evdata/cr.event");
      px[1:globalpar.maxants,1:100] := 0.;
      py[1:globalpar.maxants,1:100] := 0.;
      ps[1:globalpar.maxants,1:100] := 0.;
      for (_ant in 1:len(locdat.ant())) {
print _ant;
        xary := locdat.get('Time', locdat.ant()[_ant]);
        yary := as_double(locdat.get('f(x)', locdat.ant()[_ant]));
        yary *:= yary;
        # Let's find those spikes!
        findPeaksInData(pres, xary, yary, peaks_bs, peaks_sens);
        _peaks[_ant]     := pres.count;
        px[_ant,[1:100]] := pres.x;
        py[_ant,[1:100]] := pres.y;
        ps[_ant,[1:100]] := pres.s;
      }

      # Get delay adjustments
      deladj := locdat.getary('Corrdelay');
      for (_p in 1:100) {
        px[1:len(locdat.ant()),_p] +:= deladj;
      }
      antpos := CalcAntPos(locdat,locdat.ant());

      # Try to group "matching" spikes, i.e. spikes within lightspeed time difference
      # of different antennas
      local groups := [=];
      groupPeaks(groups, px, py, ps, _peaks, antpos);
      for (_g in 1:len(groups)) {
        if (len(groups[_g].ant) > 3) {
          comm := spaste(' >> ', file);
          estimateDir(groups[_g], antpos);
          print "#",mean(groups[_g].x)+groups[_g].x0,groups[_g].ant;
          shell(spaste('echo "',
                  fi[_fi],'\t',
                  groups[_g].x0,'\t',
                  spaste(groups[_g].ant)~s/\s+(?!$)/,/g,'\t',
                  groups[_g].az,'\t',groups[_g].el,'\t',groups[_g].r,
                '" >> ',file));
        }
      }
      shell(spaste('echo "\n" >> ',file));
    }

}

#------------------------------------------------------------------------------
#Name: findPeaksInData
#------------------------------------------------------------------------------
#Type: Function
#Doc:  Determines the positions of sudden peaks or pulses in an array of
#      data. The running average over the data is calculated, after which
#      'outliers' over a certain sigma value are isolated and marked as
#      possibly interesting.
#Par:  xary    - Array with x values
#Par:  yary    - Array with y values
#Par:  plotgui - Reference to a plotgui
#Ret:  An object with information about detected peaks. This object contains
#      the following fields:
#      <ul><li>
#      <b>x</b>: array with values of <b>xary</b> where peaks were found;
#      </li><li>
#      <b>y</b>: array with values of <b>yary</b> where peaks were found;
#      </li><li>
#      <b>s</b>: array with significance values (the amount of sigma above the
#      running average) of the peak;
#      </li><li>
#      <b>count</b>: the total number of peaks that were found.
#      </li></ul>
#      These values can be plugged right into the <a
#      href="spike.doc.html#groupPeaks">groupPeaks</a> function, which groups
#      matching peaks.
#Ref:  runningavgstd
#------------------------------------------------------------------------------
findPeaksInData:=function(ref obj, xary, yary, bs, sens, sig=0, pwr=0) {
    local reto;
    # x and y array should have the same number of elements
    if (len(xary) != len(yary)) {
      fail "x and y array length mismatch.";
    }

    ispeak := [];

    # Calculate running average. (Set blocksize equal to the number of data points to
    # get a flat average.)
    mea   := as_double(yary);
    var    := as_double(yary);
    runningavgstdnew(yary,mea,var,bs);
    if (pwr!=0) {mea:=pwr};
    stdev := (yary - mea)/var;
    if (pwr==0) {ispeak:=stdev} else {ispeak:=yary; sens:=pwr};

    # determine peaks
    reto := [=];
    _peaks := 0;
    _set   := 0;
    i := 0;
    while (i < len(yary)) {
      i +:= 1;
      if (ispeak[i] > sens) { # peak (group) counting
        # group peaks next to each other
        _set +:= 1;
        if (_set == 1) {_peaks +:= 1}; # peak counter
      };
      if (ispeak[i] < sens | i==len(yary)) { # peak group info
        # when a group is finished, give its data
        if (i==len(yary)) {i+:=1};
        if (_set != 0) {
          x := as_double(i)-as_double(_set)/2.-.5;
          y := as_double(sum(yary[[(i-1):(i-_set)]]))/as_double(_set);
          t := as_double(xary[i-1]+xary[i-_set])/2.;
          l := sum((stdev[(i-1):(i-_set)])^3) #/_set #ispeak[x]*_set
          if (l>=sig) {
            reto.x[_peaks] := t; # array peak indexes
            reto.y[_peaks] := y; # array peak values
            reto.s[_peaks] := ispeak[x]; # array peak values runstddevs
            reto.c[_peaks] := _set; # number of values around peak
            reto.l[_peaks] := l; # significance of peak group (s*c) # sum of peak group stddev
          } else {_peaks-:=1};
          _set := 0;
        };
      };
      if (_peaks >= 100) {
        print "# *** Maximum number of peaks exceeded ***";
        i := len(yary);
      };
    }
    
    # write some statistics of the data set
    reto.count := _peaks;
    val obj := reto;
}

#------------------------------------------------------------------------------
#Name: delaysToDir
#------------------------------------------------------------------------------
#Type: Function
#Doc:  This function attempts to estimate a direction (and in the future also a
#      distance) of an incoming signal. It takes an array <b>dt</b> of time
#      delays as input, which should be of the same length as the number of
#      antenna positions in the <b>antpos</b> parameter. It returns an object
#      with azimuth, elevation and (in the future) also direction.
#      Note that the indices of the <b>dt</b> and <b>antpos</b> parameters
#      should correspond.
#Par:  delays - Array, with index [antenna number] giving the time delay (in
#               seconds) of the signal in that antenna.
#Par:  rec    - Reference to a data structure.
#Ret:  An object with directional information. It contains the following fields:
#      <ul><li>
#      <b>az</b>: the azimuthal angle towards the source;
#      </li><li>
#      <b>el</b>: the elevation angle towards the source;
#      </li><li>
#      <b>r</b>: the distance to the source (<font color="red">not yet
#      implemented&emdash;this value is set to &endash;1.</font>).
#      </li></ul>
#Ref:  estimateDir
#Example: direction:=delaysToDir(data,delays=data.getary('Delay'),layout=T);
#------------------------------------------------------------------------------
delaysToDir:=function(ref rec,delays,layout=F) {
  if (len(delays)!=rec.len) {
    fail "Array length mismatch: 'delays' and 'data' have different sizes.";
  }

  antpos:=CalcAntPos(rec,seq(rec.len),rec.get('RefAnt'),1);

  sig := [=];
  ret := [=];

  #Create signal object
  for (i in 1:len(delays)) {
    sig.ant[i] := i;
    sig.x[i]   := delays[i];
    sig.y[i]   := 500;
    sig.s[i]   := 12;
    sig.az     := -1;
    sig.el     := -1;
    sig.r      := -1;
  }

  estimateDir(sig, antpos);

  if (layout) {
    plotgui.xy:=rec.get('XY');
    if (!has_field(plotgui,'plotantpg')) {plotgui.plotantpg:=[=]};
    plotantf(plotgui.plotantpg,rec,azel=[sig.az,sig.el]*degree,xy=plotgui.xy, observatory=rec.head('Observatory'));
  }

  #extract calculated delays from object
  ret.az := sig.az;
  ret.el := sig.el;
  ret.r  := sig.r;
  return ret;
}
#------------------------------------------------------------------------------
#Name: estimateDir
#------------------------------------------------------------------------------
#Type: Function
#Doc:  This function attempts to estimate a direction (and in the future also a
#      distance) of an incoming signal. It takes an object <b>sig</b> as input,
#      which is a record with a structure as created by <a
#      href="spike.doc.html#groupPeaks">groupPeaks</a>. To this structure, some
#      extra fields are then added.
#Par:  sig    - This is a reference to a record with a structure as created
#               by <a href="spike.doc.html#groupPeaks">groupPeaks</a>. In this
#               record, the following fields are filled in when a successful
#               determination could be made:<br>
#               <ul><li>
#               <b>az</b>: the azimuthal angle towards the source;
#               </li><li>
#               <b>el</b>: the elevation angle towards the source;
#               </li><li>
#               <b>r</b>: the distance to the source (<font color="red">not yet
#               implemented</font>).
#               </li></ul>
#               For an overview of the other fields in this object, see the
#               <b>ret</b> parameter in function <a href="spike.doc.html#groupPeaks">groupPeaks</a>.
#Par:  antpos - array of array, with index [antenna number] giving
#               the position [x, y, z] of an antenna as given by
#               <a href="map.doc.html#CalcAntPos">calcAntPos</a>.
#------------------------------------------------------------------------------
estimateDir:=function(ref sig, antpos) {
  if (len(sig.ant) < 4) {
    fail "Not enough detections to locate this event.";
  }

  beta := array(0,len(sig.ant));
  for (j in 2:len(sig.ant)) {
    if (antpos[sig.ant[j]].position[2] != antpos[sig.ant[1]].position[2]) {
      beta[j] := atan((antpos[sig.ant[j]].position[1]-antpos[sig.ant[1]].position[1])/
                       (antpos[sig.ant[j]].position[2]-antpos[sig.ant[1]].position[2]));
    } else {
      beta[j] := pi/4.*sign(antpos[sig.ant[j]].position[1]-antpos[sig.ant[1]].position[1]);
    }
    if (antpos[sig.ant[j]].position[2] > antpos[sig.ant[1]].position[2]) {
      beta[j] := pi + beta[j];
    }
    if (beta[j] < 0) {
      beta[j] +:= 2*pi;
    }
    r[j] := sqrt(sum(antpos[sig.ant[j]].position^2));
  }
  minres   := 0;
  mintheta := -1;
  minphi   := -1;
  it       := 200;
  res := array(0.,2*it,it/2);
  for (i in 1:(2*it)) {
    phi := i*pi/it;
    rc  := r*cos(phi-beta);
    for (j in 1:(it/2)) {
      theta   := j*pi/it;
      res[i,j] := 0;
      for (k in 2:len(sig.ant)) {
        res[i,j] +:= (sig.x[k]*lightspeed-rc[k]*cos(theta))^2;
      }
      if (res[i,j] < minres || minphi < 0) { mintheta := theta; minphi := phi; minres := res[i,j]; }
    }
  }
  minphi := pi/2-minphi;
  if (minphi < 0) minphi +:= 2*pi;
  val sig.az := minphi/pi*180;
  val sig.el := mintheta/pi*180;
  val sig.r  := -1;
}

