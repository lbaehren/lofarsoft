#-------------------------------------------------------------------------------
#Name: decoder_gui
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Provides the graphical front-end to the routines decoding the tv-picture.
#-------------------------------------------------------------------------------

decoder_gui :=function(ref rec, ref plotgui)
{
  global decgui:=[=];

  decgui.selectPanel := function(index='nopanel'){
    wider decgui;
    #unfold selected panel
    if (index != 'nopanel') { decgui.toolpanel[index]->map(); };
    decgui.showtool := index;
    #fold other panels
    for (ind in field_names(decgui.toolpanel)) {
      if (ind != index) { decgui.toolpanel[ind]->unmap();};
    };
  };

  decgui.newToolPanel := function(ref parent, title='',index='', show=T) {
    wider decgui;

    if (len(title)==0) title := as_string(len(toolpanel)+1);
    if (index=='') { index := title; };
 
    if (!has_field(decgui,'toolpanel')) {
      decgui.toolpanelp := [=];
      decgui.panelhead  := [=];
      decgui.panellabel := [=];
      decgui.panelapbut := [=];
      decgui.panelclbut := [=];
      decgui.headspace  := [=];
      decgui.toolpanel  := [=];
      decgui.showtool  := index;
    }

    decgui.toolpanelp[index] := frame(parent, side='top', relief='raised', expand='x', borderwidth=1);
    decgui.panelhead[index]  := frame(decgui.toolpanelp[index], side='left', relief='flat', background='grey70',
                                    expand='x', borderwidth=0);
    decgui.panellabel[index] := label(decgui.panelhead[index],  title, background='grey70', anchor='w', fill='x');
    decgui.panelclbut[index] := button(decgui.panelhead[index], 'Fold', bitmap=spaste(globalconst.ImageDir,'apply.xbm'),
                                    background='grey70', borderwidth=0, hlthickness=0)
    decgui.headspace[index]  := frame(decgui.panelhead[index], relief='flat', expand='y', background='grey70', width=2, height=1,
                                    borderwidth=0);
    decgui.toolpanel[index]  := frame(decgui.toolpanelp[index], side='top', relief='flat', expand='x', borderwidth=1);

    popuphelp(decgui.panelclbut[index], spaste('Fold or unfold panel "',title,'"'));

    whenever decgui.panelclbut[index]->press do {
      wider decgui;
       decgui.selectPanel(index);
    }
    return title;
  }





  widthFrameLeft  := 30;
  widthFrameRight := 30;

  tk_hold();
 
  decgui.frame:=frame(title='TV Decoder',width=1000,height=350,tpos='c');

  decgui.newToolPanel(decgui.frame, 'Decode a TV picture',index='tvdec')
  decgui.newToolPanel(decgui.frame, 'Decode AM Radio',index='amdec')

  # ---------------- TV-decoding  ----------------
  decgui.TVcontents := frame(decgui.toolpanel['tvdec'],side='top',expand='x');
  #
  decgui.TVstartpos := frame(decgui.TVcontents,side='left',expand='x');
  decgui.TVstartpos_left  := frame(decgui.TVstartpos,side='top',expand='x');
  decgui.TVstartpos_right := frame(decgui.TVstartpos,side='top',expand='x');
  decgui.TVstartpos_label := label(decgui.TVstartpos_left,'Position of reference H-Sync : ',
                                  anchor='ne',width=widthFrameLeft);
  decgui.TVstartpos_entry := entry(decgui.TVstartpos_right,width=widthFrameRight,
                                  fill='x',background='LemonChiffon');
  decgui.TVstartpos_entry->insert('372265'); 
  #
  decgui.TVAntSel := frame(decgui.TVcontents,side='left',expand='x');
  decgui.TVAntSel_left  := frame(decgui.TVAntSel,side='top',expand='x');
  decgui.TVAntSel_right := frame(decgui.TVAntSel,side='top',expand='x');
  decgui.TVAntSel_label := label(decgui.TVAntSel_left,'Exclude Antennas : ',
                                  anchor='ne',width=widthFrameLeft);
  decgui.TVAntSel_entry := entry(decgui.TVAntSel_right,width=widthFrameRight,
                                  fill='x',background='LemonChiffon');
  decgui.TVAntSel_entry->insert(''); 

  # ---------------- AM-decoding  ----------------
  decgui.AMcontents := frame(decgui.toolpanel['amdec'],side='top',expand='x');
  #
  decgui.AMfreq := frame(decgui.AMcontents,side='left',expand='x');
  decgui.AMfreq_left  := frame(decgui.AMfreq,side='top',expand='x');
  decgui.AMfreq_right := frame(decgui.AMfreq,side='top',expand='x');
  decgui.AMfreq_label := label(decgui.AMfreq_left,'Transmitter Frequency (in MHz) : ',
                                  anchor='ne',width=widthFrameLeft);
  decgui.AMfreq_entry := entry(decgui.AMfreq_right,width=widthFrameRight,
                                  fill='x',background='LemonChiffon');
  decgui.AMfreq_entry->insert('13.78'); 
  #
  decgui.AMbandwidth := frame(decgui.AMcontents,side='left',expand='x');
  decgui.AMbandwidth_left  := frame(decgui.AMbandwidth,side='top',expand='x');
  decgui.AMbandwidth_right := frame(decgui.AMbandwidth,side='top',expand='x');
  decgui.AMbandwidth_label := label(decgui.AMbandwidth_left,'Bandwidth (in kHz) : ',
                                  anchor='ne',width=widthFrameLeft);
  decgui.AMbandwidth_entry := entry(decgui.AMbandwidth_right,width=widthFrameRight,
                                  fill='x',background='LemonChiffon');
  decgui.AMbandwidth_entry->insert('2.5'); 
  #
  decgui.AMAntSel := frame(decgui.AMcontents,side='left',expand='x');
  decgui.AMAntSel_left  := frame(decgui.AMAntSel,side='top',expand='x');
  decgui.AMAntSel_right := frame(decgui.AMAntSel,side='top',expand='x');
  decgui.AMAntSel_label := label(decgui.AMAntSel_left,'Exclude Antennas : ',
                                  anchor='ne',width=widthFrameLeft);
  decgui.AMAntSel_entry := entry(decgui.AMAntSel_right,width=widthFrameRight,
                                  fill='x',background='LemonChiffon');
  decgui.AMAntSel_entry->insert(''); 
  #
  decgui.AMfilename := frame(decgui.AMcontents,side='left',expand='x');
  decgui.AMfilename_left  := frame(decgui.AMfilename,side='top',expand='x');
  decgui.AMfilename_right := frame(decgui.AMfilename,side='top',expand='x');
  decgui.AMfilename_label := label(decgui.AMfilename_left,'Filename : ',
                                  anchor='ne',width=widthFrameLeft);
  decgui.AMfilename_entry := entry(decgui.AMfilename_right,width=widthFrameRight,
                                  fill='x',background='LemonChiffon');
  decgui.AMfilename_entry->insert('lopessound.au'); 


  # ---------------- Button for starting  ----------------
  decgui.fstart:=frame(decgui.frame,side="left");
  decgui.startbut:=button(decgui.fstart,'Start',background='green');
  decgui.startspace:=frame(decgui.fstart);
  # Button for dismissing the GUI; no map will be generated
  decgui.endbut:=button(decgui.fstart,'Dismiss',background='orange');


  # ---------------- Select a panel  ----------------
  if (rec.head('Observatory') == 'LOPES') {
    decgui.selectPanel('tvdec');
  } else if (rec.head('Observatory') == 'LOFAR-ITS') {
    decgui.selectPanel('amdec');
  } else {
    decgui.selectPanel();
  };

  tk_release();

  # ----------------------------------------------------------------------------
  #  Functionality of the GUI buttons
  # ----------------------------------------------------------------------------

  whenever decgui.startbut->press do {
    if (decgui.showtool == 'tvdec') {
      startpos := as_integer(decgui.TVstartpos_entry->get());
      selstring := decgui.TVAntSel_entry->get();
      selstring := split(selstring,' ,;');
      selary := [];
      j := 1;
      if (len(selstring)>0) {
        for (ind in selstring){
          i := as_integer(ind);
          if ((i>0)&(i<=rec.len)){ selary[j] := i; j := j+1;};
        };
      };
      print 'Starting TV decoder with parameters:';
      print 'Startpos:',startpos;
      print 'DeselectArray: [',as_string(selary),']';
      tvpicture(startpos,selary);
    } else if (decgui.showtool == 'amdec') {
      startfreq := as_double(decgui.AMfreq_entry->get())*1e6;
      bandwidth := as_double(decgui.AMbandwidth_entry->get())*1e3;
      filename := decgui.AMfilename_entry->get();
      selstring := decgui.TVAntSel_entry->get();
      selstring := split(selstring,' ,;');
      selary := [];
      j := 1;
      if (len(selstring)>0) {
        for (ind in selstring){
          i := as_integer(ind);
          if ((i>0)&(i<=rec.len)){ selary[j] := i; j := j+1;};
        };
      };
      print 'Starting AM radio decoder with parameters:';
      print 'Startfreq:',startfreq/1e6,'MHz';
      print 'Bandwidth:',bandwidth/1e3,'kHz';
      print 'Filename:',filename;
      print 'DeselectArray: [',as_string(selary),']';
      soundfile(startfreq,selary,bandwidth=bandwidth,outname=filename);
    } else {
      print 'No panel selected!';
    };
  };

  #
  # Dismiss the GUI 
  #
  whenever decgui.endbut->press do
  {
    val decgui.frame := F;
    val decgui := F;
  };


};
