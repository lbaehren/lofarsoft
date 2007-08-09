#-------------------------------------------------------------------------------
#
# Various functions not directly related to the data, but to the way the
# works.
# <ul>
#   <li><a href="#buttons">Functions changing the display of toolbar buttons</a>
#   <li><a href="#general">Action functions for Toolbox docker buttons</a>
#   <li><a href="#toolbar">Action functions for toolbar buttons</a>
# </ul>
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#
#Section: <a name="general">Functions changing the display of toolbar buttons</a>
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: setButtonStatus
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Set or unset the 'active' status of a button by changing the foreground
#      and background colors.
#Par:  but       - Reference to the button object to apply changes to.
#Par:  state=T   - The status the button is to be set to: T means active, F
#                  means inactive.
#-------------------------------------------------------------------------------
setButtonStatus:=function(ref but, state=T) {
  if(state) {
    buttonActivate(but);
  } else {
    buttonDeactivate(but);
  }
}

#-------------------------------------------------------------------------------
#Name: buttonActivate
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Set 'active' status of a button by changing the foreground
#      and background colors.
#Par:  but       - Reference to the button object to apply changes to.
#-------------------------------------------------------------------------------
buttonActivate:=function(ref but) {
  but->foreground('darkblue');
  but->background('grey93');
}

#-------------------------------------------------------------------------------
#Name: buttonDeactivate
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Set 'inactive' status of a button by changing the foreground
#      and background colors.
#Par:  but       - Reference to the button object to apply changes to.
#-------------------------------------------------------------------------------
buttonDeactivate:=function(ref but) {
  but->foreground('black');
  but->background('lightgrey');
}

#-------------------------------------------------------------------------------
#
#Section: <a name="general">Action functions for Toolbox docker buttons</a>
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: onUseMove
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Sets the current tool to 'move'.
#      Other tools are deselected, their buttons deactivated accordingly, the
#      move subpanel is showed in the toolbox and a message is dispayed in the
#      status bar.
#Par:  plotgui   - Reference to a plotgui interface
#Par:  gui=T     - Set to false if the move tool only needs to be set
#                  internally (when no gui is present).
#-------------------------------------------------------------------------------
onUseMove := function(ref plotgui, gui=T) {
  plotgui.moveactive := T;
  plotgui.selactive  := F;
  plotgui.zoomactive := F;
  plotgui.delactive  := F; 

  if (gui) {
    buttonActivate  (plotgui.movebut);
    buttonDeactivate(plotgui.selectbut);
    buttonDeactivate(plotgui.zoombut);
    buttonDeactivate(plotgui.deletebut);

    plotgui.toolmoveframe->map();
    plotgui.toolselectframe->unmap();
    plotgui.toolzoomframe->unmap();
    plotgui.tooldeleteframe->unmap();

    plotgui.rightmainframe->cursor('left_ptr');
    plotgui.messlabel->text('Switching to move mode. Press button and drag mouse to move or order plots.')
  }
}

#-------------------------------------------------------------------------------
#Name: onUseSelect
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Sets the current tool to 'select'.
#      Other tools are deselected, their buttons deactivated accordingly, the
#      select subpanel is showed in the toolbox and a message is dispayed in the
#      status bar.
#Par:  plotgui   - Reference to a plotgui interface
#Par:  gui=T     - Set to false if the select tool only needs to be set
#                  internally (when no gui is present).
#-------------------------------------------------------------------------------
onUseSelect := function(ref plotgui, gui=T) {
  plotgui.moveactive := F;
  plotgui.selactive  := T;
  plotgui.zoomactive := F;
  plotgui.delactive  := F; 
  plotgui.resultlen:=0; plotgui.result:=[=];

  if (gui) {
    buttonDeactivate(plotgui.movebut);
    buttonActivate  (plotgui.selectbut);
    buttonDeactivate(plotgui.zoombut);
    buttonDeactivate(plotgui.deletebut);

    plotgui.toolmoveframe->unmap();
    plotgui.toolselectframe->map();
    plotgui.toolzoomframe->unmap();
    plotgui.tooldeleteframe->unmap();

    plotgui.rightmainframe->cursor('cross');
    plotgui.messlabel->text('Switching to select mode. Press button and drag mouse to select regions.')
  }
}

#-------------------------------------------------------------------------------
#Name: onUseZoom
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Sets the current tool to 'select'.
#      Other tools are deselected, their buttons deactivated accordingly, the
#      zoom subpanel is showed in the toolbox and a message is dispayed in the
#      status bar.
#Par:  plotgui   - Reference to a plotgui interface
#Par:  gui=T     - Set to false if the zoom tool only needs to be set
#                  internally (when no gui is present).
#-------------------------------------------------------------------------------
onUseZoom := function(ref plotgui, gui=T) {
  plotgui.moveactive := F; 
  plotgui.selactive  := F; 
  plotgui.zoomactive := T;
  plotgui.delactive  := F; 

  if (gui) {
    buttonDeactivate(plotgui.movebut);
    buttonDeactivate(plotgui.selectbut);
    buttonActivate  (plotgui.zoombut);
    buttonDeactivate(plotgui.deletebut);

    plotgui.toolmoveframe->unmap();
    plotgui.toolselectframe->unmap();
    plotgui.toolzoomframe->map();
    plotgui.tooldeleteframe->unmap();

    plotgui.rightmainframe->cursor('plus');
    plotgui.messlabel->text('Switching to zoom mode. Press button and drag mouse to select data range.')
  }
}

#-------------------------------------------------------------------------------
#Name: onUseDelete
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Sets the current tool to 'delete'.
#      Other tools are deselected, their buttons deactivated accordingly, the
#      delete subpanel is showed in the toolbox and a message is dispayed in the
#      status bar.
#Par:  plotgui   - Reference to a plotgui interface
#Par:  gui=T     - Set to false if the delete tool only needs to be set
#                  internally (when no gui is present).
#-------------------------------------------------------------------------------
onUseDelete := function(ref plotgui, gui=T) {
  plotgui.moveactive := F; 
  plotgui.selactive  := F; 
  plotgui.zoomactive := F;
  plotgui.delactive  := T; 

  if (gui) {
    buttonDeactivate(plotgui.movebut);
    buttonDeactivate(plotgui.selectbut);
    buttonDeactivate(plotgui.zoombut);
    buttonActivate  (plotgui.deletebut);

    plotgui.toolmoveframe->unmap();
    plotgui.toolselectframe->unmap();
    plotgui.toolzoomframe->unmap();
    plotgui.tooldeleteframe->map();

    plotgui.rightmainframe->cursor('pirate');
    plotgui.messlabel->text('Switching to delete mode. Click a graph panel to remove it.');
  }
}

#-------------------------------------------------------------------------------
#Name: onScrollLeft
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Scrolls the PGPlotter viewport to the left by one fourth of the viewport
#      width.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onScrollLeft:=function(ref plotgui) {
    plotgui.select_x1:=as_float(plotgui.x1->get());
    plotgui.select_x2:=as_float(plotgui.x2->get())
    plotgui.select_y1:=as_float(plotgui.y1->get());
    plotgui.select_y2:=as_float(plotgui.y2->get())
    dx:=plotgui.select_x2-plotgui.select_x1;
    plotgui.select_x1:=plotgui.select_x1-dx/4;
    plotgui.select_x2:=plotgui.select_x2-dx/4;
    plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onScrollRight
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Scrolls the PGPLotter viewport to the right by one fourth of the viewport
#      width.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onScrollRight:=function(ref plotgui) {
  plotgui.select_x1:=as_float(plotgui.x1->get());
  plotgui.select_x2:=as_float(plotgui.x2->get())
  plotgui.select_y1:=as_float(plotgui.y1->get());
  plotgui.select_y2:=as_float(plotgui.y2->get())
  dx:=plotgui.select_x2-plotgui.select_x1;
  plotgui.select_x1:=plotgui.select_x1+dx/4;
  plotgui.select_x2:=plotgui.select_x2+dx/4;
  plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onScrollUp
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Scrolls the PGPLotter viewport up by one fourth of the viewport height.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onScrollUp:=function(ref plotgui) {
    plotgui.select_y1:=as_float(plotgui.y1->get());
    plotgui.select_y2:=as_float(plotgui.y2->get())
    plotgui.select_x1:=as_float(plotgui.x1->get());
    plotgui.select_x2:=as_float(plotgui.x2->get())
    dy:=plotgui.select_y2-plotgui.select_y1;
    plotgui.select_y1:=plotgui.select_y1+dy/4;
    plotgui.select_y2:=plotgui.select_y2+dy/4;
    plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onScrollDown
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Scrolls the PGPLotter viewport down by one fourth of the viewport height.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onScrollDown:=function(ref plotgui) {
    plotgui.select_y1:=as_float(plotgui.y1->get());
    plotgui.select_y2:=as_float(plotgui.y2->get())
    plotgui.select_x1:=as_float(plotgui.x1->get());
    plotgui.select_x2:=as_float(plotgui.x2->get())
    dy:=plotgui.select_y2-plotgui.select_y1;
    plotgui.select_y1:=plotgui.select_y1-dy/4;
    plotgui.select_y2:=plotgui.select_y2-dy/4;
    plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onZoomSel
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Zooms in to the selected area, both in x and y direction.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onZoomSel:=function(ref plotgui) {
    plotgui.fixedscale->state(T);
    plotgui.select_x1:=as_float(plotgui.selected_region_x1);
    plotgui.select_x2:=as_float(plotgui.selected_region_x2)
    plotgui.select_y1:=as_float(plotgui.selected_region_y1);
    plotgui.select_y2:=as_float(plotgui.selected_region_y2)
    dx:=plotgui.select_x2-plotgui.select_x1;
    dy:=plotgui.select_y2-plotgui.select_y1;
    plotgui.select_x2:=plotgui.select_x2+dx/18.;
    plotgui.select_x1:=plotgui.select_x1-dx/18.;
    plotgui.select_y2:=plotgui.select_y2+dy/18.;
    plotgui.select_y1:=plotgui.select_y1-dy/18.;
    plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onZoomIn
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Zooms in to three fifths of the PGPLotter viewport, both in x and y
#      direction.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onZoomIn:=function(ref plotgui) {
    plotgui.fixedscale->state(T);
    plotgui.select_x1:=as_float(plotgui.x1->get());
    plotgui.select_x2:=as_float(plotgui.x2->get())
    plotgui.select_y1:=as_float(plotgui.y1->get());
    plotgui.select_y2:=as_float(plotgui.y2->get())
    dx:=plotgui.select_x2-plotgui.select_x1;
    dy:=plotgui.select_y2-plotgui.select_y1;
    plotgui.select_x2:=plotgui.select_x2-dx/5.;
    plotgui.select_x1:=plotgui.select_x1+dx/5.;
    plotgui.select_y2:=plotgui.select_y2-dy/5.;
    plotgui.select_y1:=plotgui.select_y1+dy/5.;
    plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onZoomInX
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Zooms in to three fifths of the PGPLotter viewport, in x direction only.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onZoomInX:=function(ref plotgui) {
    plotgui.fixedscale->state(T);
    plotgui.select_x1:=as_float(plotgui.x1->get());
    plotgui.select_x2:=as_float(plotgui.x2->get())
    plotgui.select_y1:=as_float(plotgui.y1->get());
    plotgui.select_y2:=as_float(plotgui.y2->get())
    dx:=plotgui.select_x2-plotgui.select_x1;
    plotgui.select_x2:=plotgui.select_x2-dx/5.;
    plotgui.select_x1:=plotgui.select_x1+dx/5.;
    plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onZoomInY
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Zooms in to three fifths of the PGPLotter viewport, in y direction only.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onZoomInY:=function(ref plotgui) {
    plotgui.fixedy->state(T);
    plotgui.select_x1:=as_float(plotgui.x1->get());
    plotgui.select_x2:=as_float(plotgui.x2->get())
    plotgui.select_y1:=as_float(plotgui.y1->get());
    plotgui.select_y2:=as_float(plotgui.y2->get())
    dy:=plotgui.select_y2-plotgui.select_y1;
    plotgui.select_y2:=plotgui.select_y2-dy/5.;
    plotgui.select_y1:=plotgui.select_y1+dy/5.;
    plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onZoomOut
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Zooms out to five thirds of the PGPLotter viewport, both in x and y
#      direction.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onZoomOut:=function(ref plotgui) {
    plotgui.select_x1:=as_float(plotgui.x1->get());
    plotgui.select_x2:=as_float(plotgui.x2->get())
    plotgui.select_y1:=as_float(plotgui.y1->get());
    plotgui.select_y2:=as_float(plotgui.y2->get())
    dx:=plotgui.select_x2-plotgui.select_x1;
    dy:=plotgui.select_y2-plotgui.select_y1;
    plotgui.select_x2:=plotgui.select_x2+dx/3.;
    plotgui.select_x1:=plotgui.select_x1-dx/3.;
    plotgui.select_y2:=plotgui.select_y2+dy/3.;
    plotgui.select_y1:=plotgui.select_y1-dy/3.;
    plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onZoomOutX
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Zooms out to five thirds of the PGPLotter viewport, in x direction only.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onZoomOutX:=function(ref plotgui) {
    plotgui.select_x1:=as_float(plotgui.x1->get());
    plotgui.select_x2:=as_float(plotgui.x2->get())
    plotgui.select_y1:=as_float(plotgui.y1->get());
    plotgui.select_y2:=as_float(plotgui.y2->get())
    dx:=plotgui.select_x2-plotgui.select_x1;
    plotgui.select_x2:=plotgui.select_x2+dx/3.;
    plotgui.select_x1:=plotgui.select_x1-dx/3.;
    plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onZoomOutY
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Zooms out to five thirds of the PGPLotter viewport, in y direction only.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onZoomOutY:=function(ref plotgui) {
    plotgui.fixedy->state(T);
    plotgui.select_x1:=as_float(plotgui.x1->get());
    plotgui.select_x2:=as_float(plotgui.x2->get())
    plotgui.select_y1:=as_float(plotgui.y1->get());
    plotgui.select_y2:=as_float(plotgui.y2->get())
    dy:=plotgui.select_y2-plotgui.select_y1;
    plotgui.select_y2:=plotgui.select_y2+dy/3.;
    plotgui.select_y1:=plotgui.select_y1-dy/3.;
    plotgui.zoom();
}

#-------------------------------------------------------------------------------
#Name: onDeselect
#-------------------------------------------------------------------------------
#Type: function
#Doc:  If in select mode, deselects the selected areas.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onDeselect:=function(ref plotgui) {
  if (plotgui.selactive) {
print "Deselecting"
    plotgui.selected_region_x1:=0;
    plotgui.selected_region_x2:=0;
    plotgui.selected_region_y1:=0;
    plotgui.selected_region_y2:=0;
    plotgui.refresh();
  }
}

#-------------------------------------------------------------------------------
#Name: onFlagSelection
#-------------------------------------------------------------------------------
#Type: function
#Doc:  If in select mode, flags the selected areas
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
onFlagSelection:=function(ref plotgui) {
  if (plotgui.selactive) {
    plotgui.pleasewait();
    plotgui.flagx := [plotgui.invfx(plotgui.selected_region_x1),
                      plotgui.invfx(plotgui.selected_region_x2)]
    plotgui.flagy := [plotgui.invfy(plotgui.selected_region_y1),
                      plotgui.invfy(plotgui.selected_region_y2)];   
    plotgui.flag();
    plotgui.endwait();
  }
}

onUnflagSelection:=function(ref plotgui) {
  if (plotgui.selactive) {
    plotgui.unflag();
  }
}

onSelectConfineX:=function(ref plotgui) {
  if (plotgui.selactive) {
    plotgui.selectx := !plotgui.selectx;
    setButtonStatus(plotgui.selconfxiconbut, plotgui.selectx);
    plotgui.refresh(erase=F);
  }
}

onSelectConfineY:=function(ref plotgui) {
  if (plotgui.selactive) {
    plotgui.selecty := !plotgui.selecty;
    setButtonStatus(plotgui.selconfyiconbut, plotgui.selecty);
    plotgui.refresh(erase=F);
  }
}

onSelectGlobal:=function(ref plotgui) {
  if (plotgui.selactive) {
    plotgui.selectglobal := !plotgui.selectglobal;
    plotgui.resultlen := 0;
    plotgui.result := [=];
    setButtonStatus(plotgui.selglobaliconbut, plotgui.selectglobal);
    plotgui.refresh(erase=F);
  }
}

onSelectInvert:=function(ref plotgui) {
  if (plotgui.selactive) {
    plotgui.selectinside := !plotgui.selectinside;
    setButtonStatus(plotgui.selinviconbut, !plotgui.selectinside);
    plotgui.refresh(erase=F);
  }
}

onIntegrate:=function(ref plotgui) {
    plotgui.resultlen +:= 1;
    plotgui.integrate();
}

onGaussFit:=function(ref plotgui) {
  if (plotgui.selactive) {
    plotgui.fitgauss()
  } else {
    plotgui.plotfit:=array(F,maxnpanels);
    plotgui.fitresults[i]:=[=];
    plotgui.replot();
  }
}

onMoveLock:=function(ref plotgui) {
    plotgui.moveall := !plotgui.moveall;
    setButtonStatus(plotgui.movelockiconbut, plotgui.moveall);
}

onUnstack:=function(ref plotgui) {
    plotgui.whichpanel := [=];
    for (i in seq(maxnpanels))
      plotgui.whichpanel[i] := [i];
    plotgui.replot();
}

onStackAll:=function(ref plotgui) {
    plotgui.whichpanel := [=];
    for (i in seq(maxnpanels))
      plotgui.whichpanel[i] := [];
    plotgui.whichpanel[1] := [seq(maxnpanels)];
    plotgui.replot();
}

onUndelete:=function(ref plotgui) {
    plotgui.rec.select(-1,T);
    plotgui.replot();
  }

#-------------------------------------------------------------------------------
#
#Section: <a name="toolbar">Action functions for toolbar buttons</a>
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: onOpenNextFile
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Loads the next event file in a directory, if present.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
# updated @ 050603 (an)
#-------------------------------------------------------------------------------
onOpenNextFile:=function(ref plotgui) {
  global globalpar
print 'onOpenNextFile: cur_event_dir:',globalpar.cur_event_dir
print 'onOpenNextFile: cur_event_num:',globalpar.cur_event_file
print 'onOpenNextFile: cur_event_num:',globalpar.cur_event_num
#  files := shell(spaste('ls ',globalpar.cur_event_dir))
#  files:=filesel(globalpar.cur_event_dir,ext='*.event',first=1,last=-1,disp=F)
#print 'onOpenNextFile: files:',len(files),files[1]
#  if (globalpar.cur_event_num<len(files)&&(globalpar.cur_event_dir!='')) {
  nfiles:=as_integer(shell(spaste('ls ',globalpar.cur_event_dir,'|wc -l')))
#print 'onOpenNextFile: number of files in directory:',nfiles
  globalpar.cur_event_num:=as_integer(shell(spaste('s=',globalpar.cur_event_file,
    '; a=`ls ',globalpar.cur_event_dir,'`; c=1; for i in $a; do c=$(($c+1));',
    ' if [ "$i" == "$s" ]; then echo $c; fi; done')))-1
print 'onOpenNextFile: found number:',globalpar.cur_event_num,'out of',nfiles
  if (globalpar.cur_event_num<nfiles&&(globalpar.cur_event_dir!='')) {
    globalpar.cur_event_num+:=1
    fnamelen:=30
    globalpar.cur_event_file:=shell(spaste('a=`ls ',globalpar.cur_event_dir,
      '`; echo ${a:$((',globalpar.cur_event_num-1,'*',fnamelen,')):',fnamelen,'}'))
print 'onOpenNextFile: loading file:',globalpar.cur_event_num,globalpar.cur_event_file
    input_event(data,spaste(globalpar.cur_event_dir,'/',globalpar.cur_event_file))
#    input_event(data,globalpar.cur_event_file)
    data.newsagent->newdata()
    data.newsagent->checkdata()
    plotgui.messlabel->text(spaste('Event directory ', globalpar.cur_event_dir,
      ' - file ', globalpar.cur_event_num,'/',len(files),
      ' - ', globalpar.cur_event_file))
    print 'Opened file',  globalpar.cur_event_num, 'of', len(files),
      '-',  globalpar.cur_event_file
  }
}

#-------------------------------------------------------------------------------
#Name: onOpenPrevFile
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Loads the next event file in a directory, if present.
#Par:  plotgui   - Reference to a plotgui interface
#-------------------------------------------------------------------------------
# updated @ 050603 (an)
#-------------------------------------------------------------------------------
onOpenPrevFile:=function(ref plotgui) {
  global globalpar
print 'onOpenPrevFile: cur_event_dir:',globalpar.cur_event_dir
print 'onOpenPrevFile: cur_event_num:',globalpar.cur_event_num
#   files := shell(spaste('ls ',globalpar.cur_event_dir))
#   files:=filesel(globalpar.cur_event_dir,ext='*.event',first=1,last=-1,disp=F)
#print 'onOpenPrevFile: files:',len(files),files[1]
#    if (globalpar.cur_event_num>1&&globalpar.cur_event_dir!='') {
  nfiles:=as_integer(shell(spaste('ls ',globalpar.cur_event_dir,'|wc -l')))
#print 'onOpenNextFile: number of files in directory:',nfiles
  globalpar.cur_event_num:=as_integer(shell(spaste('s=',globalpar.cur_event_file,
    '; a=`ls ',globalpar.cur_event_dir,'`; c=1; for i in $a; do c=$(($c+1));',
    ' if [ "$i" == "$s" ]; then echo $c; fi; done')))-1
print 'onOpenPrevFile: found number:',globalpar.cur_event_num,'out of',nfiles  
  if (globalpar.cur_event_num>1&&(globalpar.cur_event_dir!='')) {
    globalpar.cur_event_num-:=1
    fnamelen:=30
    globalpar.cur_event_file:=shell(spaste('a=`ls ',globalpar.cur_event_dir,
      '`; echo ${a:$((',globalpar.cur_event_num-1,'*',fnamelen,')):',fnamelen,'}'))
print 'onOpenPrevFile: loading file:',globalpar.cur_event_num,globalpar.cur_event_file
    input_event(data,spaste(globalpar.cur_event_dir,'/',globalpar.cur_event_file))   
#    input_event(data,globalpar.cur_event_file)
    data.newsagent->newdata()
    data.newsagent->checkdata()
    plotgui.messlabel->text(spaste('Event directory ',globalpar.cur_event_dir,
      ' - file ',globalpar.cur_event_num,'/',len(files),
      ' - ',globalpar.cur_event_file))
    print 'Opened file', globalpar.cur_event_num, 'of', len(files),
      '-', globalpar.cur_event_file
  }
}
