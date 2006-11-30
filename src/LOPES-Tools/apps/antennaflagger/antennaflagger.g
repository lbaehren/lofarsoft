#-------------------------------------------------------------------------------
#Name: antennaflagger_GUI
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

antennaflagger_GUI := function (ref rec)
{
  tk_hold();

  guirec := [=];

  guirec.frame := frame(title='Autmated flagging of antennas',
                        width=800,height=350,tpos='c');

  # ------------------------------------------------------------------
  # Information on the loaded data set

  guirec.data := SectionFrame (guirec.frame,
                               'Information on the loaded data set',
                               F);

  guirec.project := SectionEntry (guirec.data,
                                  'Project : ',
                                  rec.head('Project'));

  guirec.observatory := SectionEntry (guirec.data,
                                      'Observatory : ',
                                      rec.head('Observatory'));

  guirec.date := SectionEntry (guirec.data,
                               'Observation date : ',
                               rec.head('Date'));

  # ------------------------------------------------------------------
  # Setup of the processing parameters

  guirec.processing := SectionFrame (guirec.frame,
                                     'Setup of the processing parameters',
                                     T);

  guirec.antennas := SectionEntry (guirec.processing,
                                    'Set of antennas : ',
                                    vector2string(seq(rec.len)));

  whenever guirec.antennas.Entry->return do {
    ant := eval(guirec.antennas.Entry->get());
    guirec.antennas.Entry->delete('start','end');
    guirec.antennas.Entry->insert(vector2string(ant));
  }

  guirec.blocksize := SectionEntry (guirec.processing,
                                     '# Samples / Block : ',
                                     rec.getmeta("Blocksize"));

  
  whenever guirec.blocksize.Entry->return do {
    blocksize := as_integer(eval(guirec.blocksize.Entry->get()));
    #
    rec.setblock(blocksize=blocksize);
    rec.replot();
    #
    guirec.blocksize.Entry->delete('start','end');
    guirec.blocksize.Entry->insert(as_string(blocksize));
  }

  guirec.nofBlocks := SectionEntry (guirec.processing,
                                    '# of blocks : ',
                                    '1');

  guirec.clipping := SectionEntry (guirec.processing,
                                    'Clipping fraction : ',
                                    '0.01');

  guirec.threshold := SectionEntry (guirec.processing,
                                    'Flagging threshold : ',
                                    '2.5');

  guirec.autoflag := SectionButton (guirec.processing,
                                    'Flagging : ',
                                    'check',
                                    'Automatically apply flags?');


  # ------------------------------------------------------------------
  # Control bottons for the GUI

  guirec.control := frame(guirec.frame,
                          side='left',
                          padx='5',
                          pady='1',
                          expand='x',
                          relief='groove');

  guirec.start := button (guirec.control,
                          'Run data flagger',
                          background='green');

  guirec.control.space := frame(guirec.control);

  guirec.dismiss := button (guirec.control,'Dismiss',background='orange');

  whenever guirec.start -> press do {
    # Extract the parameters from the GUI
    recParam := [=];
    recParam.index     := eval(guirec.antennas.Entry->get());
    recParam.blocksize := as_integer(eval(guirec.blocksize.Entry->get()));
    recParam.nofBlocks := as_integer(eval(guirec.nofBlocks.Entry->get()));
    recParam.clipping  := as_float(eval(guirec.clipping.Entry->get()));
    recParam.threshold := as_float(eval(guirec.threshold.Entry->get()));
    recParam.autoflag  := as_boolean(guirec.autoflag.Button->state());
    #
    antennaflagger (rec,
                    index=recParam.index,
                    blocksize=recParam.blocksize,
                    nofBlocks=recParam.nofBlocks,
                    clipping=recParam.clipping,
                    threshold=recParam.threshold,
                    autoflag=recParam.autoflag
                   )
  }

  whenever guirec.dismiss -> press do {
    val guirec.frame:=F;
    val guirec:=F;
  }

  tk_release();

}

#-------------------------------------------------------------------------------
#Name: antennaflagger
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Check a loaded data set for possible bad antennas.
#Par:  rec              - Reference to the data record.
#Par:  index     = -1   - Indicies of the antennae selected for the computation.
#Par:  nofBlocks = 1    - Number of blocks to analyze
#Par:  clipping  = 0.01 - Maximum fraction of samples in a block of data which
#                         are allowed to be clipping at ADC.
#Par:  threshold = 1.2  - Threshold for the deviation between antenna variable
#                         and array variable; if deviation is larger then the 
#                         threshold the antenna is flagged.
#Par:  autoflag  = F    - Automatically apply the flags to the processed data?
#-------------------------------------------------------------------------------

antennaflagger := function (ref rec,
                            index=-1,
                            blocksize=128^2,
                            nofBlocks=1,
                            clipping=0.02,
                            threshold=1.2,
                            autoflag=F)
{
  antennaflaggerClient := client(spaste(globalconst.LOPESBIND,'/antennaflagger'));

  # ------------------------------------------------------------------
  # Process function parameters

  if (index != -1 ) {
    rec.select(1:rec.len,F);
    rec.select(index,T);
  } else {
    rec.select(1:rec.len,T);
  }

  rec.setblock(blocksize=blocksize);
  rec.replot();

  # ------------------------------------------------------------------
  # Extract run-time parameters

  par := [=];

  FilesToRecord (rec,par);
  DataToRecord (rec, par, 'ObservationInfo');

  par.index     := [1:rec.len][rec.select()];
  par.nant      := len(par.index);
  par.blocksize := rec.getmeta('Blocksize');
  par.block     := 1;
  par.nofBlocks := nofBlocks;
  par.clippingLimits   := [as_integer(rec.head('ADCMinChann')),
                           as_integer(rec.head('ADCMaxChann'))-1];
  par.clippingFraction := clipping;
  par.threshold        := threshold;

  RecordForDataReader (rec,par);

  print '[antennaflagger] Parameters and global statistics:';
  print ' - index ............ : ',par.index;
  print ' - nant ............. : ',par.nant;
  print ' - Blocksize ........ : ',par.blocksize;

  # ------------------------------------------------------------------
  # Run the Client

  ok := antennaflaggerClient->init (par);

  if (ok) {
    print "[antennaflagger] Client successfully initialized.";
  } else {
    print "[antennaflagger] Error initializing Client.";
    fail;
  }

  ok := antennaflaggerClient->flag (par);

  if (ok) {
    print "[antennaflagger] Flagging successful.";
  } else {
    print "[antennaflagger] Error while flagging data.";
    fail;
  }

  result := antennaflaggerClient->stats (par);

  flags := as_boolean(result.antennaFlags);

  # ------------------------------------------------------------------
  # Display and apply results

  if (len(par.index) == len(flags)) {
    print '[antennaflagger] Flagged antennas  :',[par.index][!flags];
    print '                 nof times flagged :',[result.nofTimesFlagged][!flags];
  } else {
    print result;
  }

  if (autoflag) {
    print '[antennaflagger] Applying flags to data.';
    rec.select([par.index][flags],T);
    rec.select([par.index][!flags],F);
    rec.replot();
  }
}
