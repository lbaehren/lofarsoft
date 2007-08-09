
#-----------------------------------------------------------------------
#Name: dev2world
#-----------------------------------------------------------------------
#Type: Function
#Doc: Returns panel index and world coordinates of a cursorposition in
#     a multi-panle window.
#Ret: rec.panel, rec.x, rec.y		- Panel number & x/y-coordinates

#Par: rec =				- Record with coordinates,
#					  returned by cursor routines
#Par: plotgui =				- Record describing the GUI
#Ref: plotguif, plotpanels
#-----------------------------------------------------------------------

dev2world:=function(ref plotgui, rec) {
pos:=[=]; pos.panel:=0; pos.x:=0; pos.y:=0
#somehow the device coordinates are offset by 3 pixels in each direction
#the offset changes as a function of zoom - strange ....
pixx:=rec.device[1]-2; pixy:=rec.device[2]-2;
#pixx:=rec.device[1]-3; pixy:=rec.device[2]-3;
#pixx:=rec.device[1]-0; pixy:=rec.device[2]-0;
for (i in seq(plotgui.nplotted)) {
    if ((plotgui.fullviewport[i][1]<=pixx) && 
        (plotgui.fullviewport[i][2]>=pixx) &&
        (plotgui.fullviewport[i][3]<=pixy) &&
        (plotgui.fullviewport[i][4]>=pixy)) {pos.panel:=i}}
if (pos.panel>0) {
   i:=pos.panel
   dx:=(plotgui.boxworld[i][2]-plotgui.boxworld[i][1])/(plotgui.viewport[i][2]-plotgui.viewport[i][1])
   dy:=(plotgui.boxworld[i][4]-plotgui.boxworld[i][3])/(plotgui.viewport[i][4]-plotgui.viewport[i][3])
   pos.x:=(pixx-plotgui.viewport[i][1])*dx+plotgui.boxworld[i][1]
   pos.y:=plotgui.boxworld[i][4]-(pixy-plotgui.viewport[i][3])*dy
#print 'dx/dy',dx,dy
#print 'rec=',rec
#print 'pos=',pos
#print 'vp=',plotgui.viewport
#print 'wrld=',plotgui.boxworld
   }
retpos:=rec; retpos.world:=[pos.x,pos.y]; retpos.panel:=pos.panel
return retpos
};

#-------------------------------------------------------------------------------
#Name: motioncallback
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  (still needs to be added)
#Par:  plotgui - reference tothe main plot GUI
#Par:  rec     - 
#-------------------------------------------------------------------------------

motioncallback:=function(ref plotgui, rec) {
if (plotgui.delactive) return;
if (plotgui.crosscursoron) {
  rec2:=dev2world(plotgui, rec);
  plotgui.cursorx:=rec2.world[1];
#  print rec2;
  if (rec2.panel>=1){
    plotgui.cursory:=plotgui.gety(plotgui.cursorx,rec2.panel)
#    print plotgui.cursorx,plotgui.cursory
  } else {
    plotgui.cursorx:=F; plotgui.cursory:=F
  };
  plotgui.pg.cursor('vline',x=plotgui.cursorx,y=plotgui.cursory);
  plotgui.messlabel->text(spaste('[x=',as_string(plotgui.cursorx),', y=',
                     as_string(plotgui.cursory),']'))
  }
};

#-------------------------------------------------------------------------------
#Name: downcallback
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  (still needs to be added)
#Par:  plotgui - reference tothe main plot GUI
#Par:  rec     - 
#-------------------------------------------------------------------------------

downcallback:=function(ref plotgui, rec) {
if (plotgui.zoomactive || plotgui.selactive) {
  plotgui.start:=dev2world(plotgui, rec).world;
  plotgui.pg.cursor('rect',x=rec.world[1],y=rec.world[2]);
  } else {
   rec2:=dev2world(plotgui, rec);
   plotgui.panelselected:=[rec2.panel];
   if (plotgui.delactive) {
     frompanel:=plotgui.panel[plotgui.panelselected[len(plotgui.panelselected)]];
     wp:=plotgui.whichpanel[frompanel]
     plotgui.messlabel->text(paste('Deselected data sets: ',wp));
     plotgui.rec.select(wp,F);
     plotgui.replot()
   } else {
     plotgui.pg.cursor('cross');
     plotgui.messlabel->text(paste('Panel selected: ',plotgui.panelselected));
   }
 }
};

#-------------------------------------------------------------------------------
#Name: upcallback
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  (still needs to be added)
#Par:  plotgui - reference tothe main plot GUI
#Par:  rec     - 
#-------------------------------------------------------------------------------

upcallback:=function(ref plotgui, rec) {
if (plotgui.delactive) return;
if (plotgui.zoomactive || plotgui.selactive) {
  rec2:=dev2world(plotgui,rec);
  plotgui.pg.cursor('norm');
  if (plotgui.start[1]<rec2.world[1]) {
    plotgui.select_x1:=plotgui.start[1];
    plotgui.select_x2:=rec2.world[1];
  } else {
    plotgui.select_x2:=plotgui.start[1];
    plotgui.select_x1:=rec2.world[1];
  }
  if (plotgui.start[2]<rec2.world[2]) {
      plotgui.select_y1:=plotgui.start[2];
      plotgui.select_y2:=rec2.world[2];
    } else {
      plotgui.select_y2:=plotgui.start[2];
      plotgui.select_y1:=rec2.world[2];
   }
  plotgui.select_panel:=rec2.panel;
  plotgui.select_abspanel:=rec2.panel+plotgui.startpanel-1;
  plotgui.messlabel->text(paste('Data range selected: [',plotgui.select_x1,
                            ':',plotgui.select_x2,'].'));
  if (plotgui.selactive) {
    plotgui.selected_region_x1:=plotgui.select_x1;
    plotgui.selected_region_x2:=plotgui.select_x2;
    plotgui.selected_region_y1:=plotgui.select_y1;
    plotgui.selected_region_y2:=plotgui.select_y2;
    plotgui.select_xpos:=[plotgui.getxn(plotgui.selected_region_x1,plotgui.select_panel),plotgui.getxn(plotgui.selected_region_x2,plotgui.select_panel)];
  };
  if (plotgui.zoomactive) {plotgui.zoom()}
  else {
   plotgui.refresh(erase=F)
   };
 } else {
   plotgui.pg.cursor('norm');
   rec2:=dev2world(plotgui, rec);
   if (len(plotgui.panelselected)==0 || len(plotgui.panelselected[plotgui.panelselected==rec2.panel]))
     {
      plotgui.panelselected:=[];
      plotgui.messlabel->text('Selection cleared.')}
   else {
   frompanels:=plotgui.panel[plotgui.panelselected[len(plotgui.panelselected)]];
   if (rec2.panel==0) {
       topanel:=plotgui.panel[plotgui.nplotted]+1;insert:=T}
   else {
       topanel:=plotgui.panel[rec2.panel];
       insert:=F;
#print 'xlims=',plotgui.xlims[topanel];
       };
   if (rec2.world[1]<plotgui.xlims[topanel][1]) {insert:=T};
   if (rec2.world[1]>plotgui.xlims[topanel][2]) {insert:=T; topanel+:=1};
   plotgui.messlabel->text(paste('Moving ',frompanels,' to ',topanel));
#print 'world=',rec2.world
#print 'from=',frompanels,'to=',topanel,'insert=',insert
   movepanels(plotgui,frompanels,topanel,insert=insert,moveall=plotgui.moveall)
   plotgui.replot();
  }
 }
 setPlotCursor(plotgui);
};

#-------------------------------------------------------------------------------
#Name: onoff
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  (still needs to be added)
#Par:  b - Boolean variable
#Ret:  s - String with contents 'on' or 'off', depending on wether 'b' was true
#          or false
#-------------------------------------------------------------------------------

onoff:=function(b){if(b) {return "on"} else {return "off"}}

#-------------------------------------------------------------------------------
#Name: setPlotCursor
#-------------------------------------------------------------------------------
#Type: function
#Doc:  Sets the mouse cursor over the PGPlotter window
#Par:  gui               - Reference to a plotgui
#Par:  cursor='left_ptr' - Cursor name
#-------------------------------------------------------------------------------

setPlotCursor := function(ref gui, cursor='left_ptr') {
  if (len(cursor) > 0) {
    gui->cursor(cursor);
  } else {
    gui->cursor('left_ptr');
  }
}

#-------------------------------------------------------------------------------
#Name: keypressed
#-------------------------------------------------------------------------------
#Type: function
#Doc:  TO BE ADDED
#Par:  plotgui           - Reference to a plotgui
#Par:  rec               - Keyboard event object
#-------------------------------------------------------------------------------
# KEYPRESSED # (BUTTONS)
keypressed:=function(ref plotgui, rec) {
  if (rec.key=='z') onUseZoom(plotgui);
  if (rec.key=='n') onOpenNextFile(plotgui);
  if (rec.key=='N') onOpenPrevFile(plotgui);
  if (rec.key=='D') onUndelete(plotgui);
  if (rec.key=='d') onUseDelete(plotgui);
  if (rec.key=='s') onUseSelect(plotgui);
  if (rec.key=='C') plotgui.crosscorr();
  if (rec.key=='I') onIntegrate(plotgui);
  if (rec.key=='G') onGaussFit(plotgui);
  if (rec.key=='F') onFlagSelection(plotgui);
  if (rec.key=='u') onUnflagSelection(plotgui);
  if (rec.key=='g') onSelectGlobal(plotgui);
  if (rec.key=='x') onSelectConfineX(plotgui);
  if (rec.key=='y') onSelectConfineY(plotgui);
  if (rec.key=='i') onSelectInvert(plotgui);
  if (rec.key=='Z') plotgui.zoomout(plotgui);

  if (rec.key=='a') onMoveLock(plotgui);
  if (rec.key=='r') onStackAll(plotgui);
  if (rec.key=='R') onUnstack(plotgui);

	#m#
  if (rec.key=='m') {
    plotgui.peaks_show:=!plotgui.peaks_show;
    if (! plotgui.peaks_show) {
      plotgui.messlabel->text('Unmarking peaks');
    } else {
      if (len(data.peaks) == 0) findPeaksInCurFile(data, plotgui);
    }
    plotgui.replot();
  };

	#v#
  if (rec.key=='v') {
    plotgui.crosscursoron:=!plotgui.crosscursoron
    if (plotgui.crosscursoron) {
    plotgui.messlabel->text('Toggling value mode on.');
    } else {
    plotgui.messlabel->text('Toggling value mode off.');
    plotgui.pg.cursor('norm')};
  };

  if (rec.key==' ') { plotgui.replot(); };

	#c#1#2#3#4#Left#Right# (panel functions)
	if (rec.key=='c') {
     plotgui.setnpanels(1,data.len,plotgui.select_panel)} #!!!
  if (rec.key=='1') {
     plotgui.setnpanels(1,1,plotgui.select_panel)}
  if (rec.key=='2') {
     plotgui.setnpanels(2,2,plotgui.select_panel)}
  if (rec.key=='3') {
     plotgui.setnpanels(3,3,plotgui.select_panel)}
  if (rec.key=='4') {
     plotgui.setnpanels(4,4,plotgui.select_panel)}
  if (rec.key=='5') {
     plotgui.setnpanels(5,5,plotgui.select_panel)}
  if (rec.key=='6') {
     plotgui.setnpanels(6,6,plotgui.select_panel)}
  if (rec.key=='7') {
     plotgui.setnpanels(7,7,plotgui.select_panel)}
  if (rec.key=='8') {
     plotgui.setnpanels(8,8,plotgui.select_panel)}
  if (rec.key=='9') {
     plotgui.setnpanels(9,9,plotgui.select_panel)}
  if (rec.key=='0') {
     plotgui.setnpanels(10,10,plotgui.select_panel)}
  if (rec.sym=='Left' && !plotgui.zoomactive ) {
     plotgui.setpanel(plotgui.startpanel-1)}
  if (rec.sym=='Right'  && !plotgui.zoomactive) {
     plotgui.setpanel(plotgui.startpanel+1)}
  if (rec.sym=='Prior') {
     plotgui.setpanel(plotgui.startpanel-(plotgui.nx*plotgui.ny))}
  if (rec.sym=='Next') {
     plotgui.setpanel(plotgui.startpanel+(plotgui.nx*plotgui.ny))}

  #Right#Left#<#Up#Down# (zoom functions)
  if ((rec.sym=='Right' && plotgui.zoomactive) || rec.key=='>') {
    onScrollRight(plotgui);
  }
  if ((rec.sym=='Left' && plotgui.zoomactive) || rec.key=='<') {
    onScrollLeft(plotgui);
  }
  if (rec.sym=='Up' && plotgui.zoomactive) { onScrollUp(plotgui); }
  if (rec.sym=='Down' && plotgui.zoomactive) { onScrollDown(plotgui); }

	#f#t#p# (data)
  global data;  if (rec.key=='=') findcrdist(data,plotgui);
  if (rec.key=='!') plotgui.view(1);
  if (rec.key=='"') plotgui.view(2);
  if (rec.key=='f') plotgui.tofrequencydomain();
  if (rec.key=='t') plotgui.totimedomain();
  if (rec.key=='p') plotgui.topower();

  #-#+#^#_# (zoom in/out)
  if (rec.key=='-') onZoomOut(plotgui);
  if (rec.key=='+') onZoomIn(plotgui);
  if (rec.key=='_') onZoomOutY(plotgui);
  if (rec.key=='^') onZoomInY(plotgui);

  #h#
  if (rec.key=='h' || rec.key=='?' || rec.sym=='F1') {
    print 'Help for keys to be used in plotwindow:'
    print '--------------------------------------------------------------'
    print 'n/N:   View next/previous file in current directory'
    print 'd:	  Toggle data select modeon/off, use cursor clicks to deselect panels.'
    print 'D:	  Select all data sets.'
    print 'z:	  Toggle zoom on/off, use cursor to select zoom region'
    print 'Z:	  Zoom out to see all the data'
    print 's:	  Toggle select mode on/off, use cursor to select zoom region'
    print 'x:	  Toggle to select on X-coordinates range for selections'
    print 'y:	  Toggle to select on Y-coordinates range for selections'
    print 'i:	  Toggle to select the inside range for selections'
    print 'g:	  Toggle to select range globally for all panels'
    print 'F:	  Flag selected regions'
    print 'C:	  Cross-Correlate selecteded regions'
    print 'I:	  Integrate selected regions over x'
    print 'G:	  Fit Gaussian to peak in selected region.'
    print 'u:	  UnFlag data set (current domain, i.e. Fourier/Time, only)'
    print 'R:	  Reorder plotpanels to original sequence.'
    print 'r:	  Reorder plotpanels so that all are plotted in one panel.'
    print 'a:	  Toggle whether to move all plots in a panel with the'
    print '       mouse.'
    print 'v:	  Show value of data at cursor position'
    print 't,p,f: Toggle between time-domain, power, and frequency'
    print '!:     Special view optimized for cosmic ray inspection in spawned window.'
    print '":     Special view optimized for cosmic ray inspection in beam window.'
    print 'm:     Mark peaks in the data set (for power domain)'
    print '1-4:   Show multiple panels in the window'
    print 'c:     Show one colum of panels'
    print '>      Move zoom window to the right.'
    print '<      Move zoom window to the left.'
    print 'PgDn:  Previous panel'
    print 'PgUp:  Next panel'
    print 'Left:  If not zoom advance view by one panel'
    print '       If zoom, move zoom window to the left.'
    print 'Right: If not zoom go back by one panel'
    print '       If zoom, move zoom window to the right.'
    print 'Up:    If zoom, move zoom window up.'
    print 'Down:  If zoom, move zoom window down.'
    print '+:     If zoom, zoom in horizontally.'
    print '-:     If zoom, zoom out horizontally.'
    print '^:     If zoom, zoom in vertically.'
    print '_:     If zoom, zoom out vertically.'
    print '<space> replot'
  }
} # end keypressed
