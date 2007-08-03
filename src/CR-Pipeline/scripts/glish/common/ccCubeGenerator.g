
#-------------------------------------------------------------------------------
# <ol>
#  <li>Graphical interface
#  <li>Implementation of computation
# </ol>
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
#Name: generate_ccCubeGUI
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  
#Par:  rec          - Reference to the data structure.
#Ret:  status  = F  - Error code; returns 'F' if everything went fine.
#Ref:  generate_ccCube, vector2string
#Created: 2005/02/01 (L. B"ahren)
#-------------------------------------------------------------------------------

generate_ccCubeGUI := function (ref rec)
{
  tk_hold();

  ccgui := [=];

  # creation of outer GUI frame with title and dimensions
  ccgui.frame := frame(title='Data cross-correlation',
                       width=1000, height=350, tpos='c');

  #-------------------------------------------------------------------
  # Data input 

  filename := rec.get('Header').Filename;
  date     := rec.get('Header').Date;

  ccgui.input := SectionFrame (ccgui.frame,'Properties of input data',T);

  ccgui.file := SectionEntry (ccgui.input,'Input dataset : ',filename);

  ccgui.date := SectionEntry (ccgui.input,'Observation date : ',date);

  tk_release();

  #-------------------------------------------------------------------
  # Data processing

  Filesize  := rec.getmeta('Filesize');
  Blocksize := as_integer(rec.getmeta('Blocksize'));
  nofBlocks := as_integer(Filesize/Blocksize);
  antennae  := vector2string(seq(rec.len)[rec.select()]);

  ccgui.processing := SectionFrame (ccgui.frame,'Data processing',T);

  # dataset selection

  ccgui.datasets := SectionEntry (ccgui.processing,
                                  'Dataset selection : ',
                                  antennae);

  whenever ccgui.datasets.Entry->return do {
    antennae := eval(ccgui.antennae.Entry->get());
    ccgui.datasets.Entry->delete('start','end');
    ccgui.datasets.Entry->insert(vector2string(antennae));
  }

  # Input blocksize for the FFT

  ccgui.blocksize := SectionEntry (ccgui.processing,
                                   'FFT input size [samples] : ',
                                   Blocksize);

  ccgui.stride := SectionEntry (ccgui.processing,
                                'Stride size [samples] : ',
                                Blocksize);

  ccgui.StartBlock := SectionEntry (ccgui.processing,
                                    'Start datablock : ',
                                    '1');

  ccgui.nofBlocks := SectionEntry (ccgui.processing,
                                   'Number of blocks : ',
                                   nofBlocks);

  #-------------------------------------------------------------------
  # Control buttons

  ccgui.control := frame(ccgui.frame,side='left',expand='x',relief='groove');

  ccgui.start := button (ccgui.control,
                         'Cross-correlate data',
                         background='green');

  ccgui.dismiss := button (ccgui.control,'Dismiss',background='orange');

  whenever ccgui.dismiss -> press do {
    val ccgui.frame:=F;
    val ccgui:=F;
  }

}


#-------------------------------------------------------------------------------
#Name: generate_ccCube
#-------------------------------------------------------------------------------
#Type: Function
#Doc:  Generate the cross-correlation data cube for the files loaded into the
#      GUI.
#
#Par:  rec                   - Reference to the data structure.
#Par:  index     =           - Index of data sets which are to be included
#Par:  tsize     = 2048      - length of one datablock (i.e. frequency resolution)
#Par:  savedir   = '/data'   - directory where to save the output
#Par:  tinc      = F         - number of samples to go forward for each block
#                              (If tinc&lt;tsize then there is some overlap
#                              between adjacent blocks.)
#Ret:  status  = F  - Error code; returns 'F' if everything went fine.
#
#Ref: 
#
#Created: 2004/11/18 (A. Horneffer)
#-------------------------------------------------------------------------------

generate_ccCube := function(rec, index, tsize=2048, savedir='/data', tinc=F)
{

  if (!tsize) {
    tsize := rec.getmeta('FFTSize');
    nfreq := rec.getmeta('FFTlen');
  }
  else {
    nfreq := as_integer((tsize+2)/2);
  }

  #-------------------------------------------------------------------
  # Set up the record to pass data to the client

  ccmat := [=];
  ccmat.filearray := rec.headarr('Files')
  ccmat.epoch := dm.epoch('utc',qnt.quantity(rec.head('Date',1)));
  ccmat.observatory := rec.head('Observatory')
  ccmat.tsize := tsize;
  ccmat.nfreq := nfreq;

  ccrecord := [=];
  ccrecord.files := ccmat.filearray;
  ccrecord.nant := len(ccmat.filearray);
  ccrecord.tsize := tsize;
  if(tinc) { ccrecord.tinc := tinc; }

  #-------------------------------------------------------------------
  # Perform computation via CLI

  clientstring := concat_dir(globalconst.LOPESBIND,'ccCubeGenerator');
  dsky_client  := client(clientstring);
  ccmat.ccMatrix := dsky_client->generate_ccmat(ccrecord);

  fname := ccmat.filearray[1]~globalconst.FilenameExtractor;
  savename := spaste(savedir,'CC-Matrix-',fname,'.sos');
  write_value(ccmat,savename);

  return F;
}
