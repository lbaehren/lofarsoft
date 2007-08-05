
#-------------------------------------------------------------------------------
# $Id$
#-------------------------------------------------------------------------------
#
# A set of methods to present a uniform method for adding parameters stored in
# the data object to a record (possibly passed to a Glish CLI).
# <ol>
#   <li><a href="#access">Access to the data structure</a>
#   <li><a href="#diagnostics">Diagnostics tools</a>
# </ol>
#  
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Section: <a name="access">Access to the data structure</a>
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: RecordForDataReader
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Until the LOPES-Tools program kernel is ported to C/C++ communication
#      is required between new tools and the kernel. One of the situations
#      where this is the case, is when using the framework provided by the
#      DataReader (see online documentation for details). Step by step all the
#      the elements in a data file will be made available via a set of classes,
#      such that information can be exchanged directly among objects. Until 
#      this is completed we have to ensure that the required input information
#      for objects is extracted on the Glish side and passed on.<br>
#      The purpose of this function is to extract the parameters required
#      to setup a properly working DataReader object (or an object of one of the
#      derived classes).
#Created: 2006/06/15 (Lars B&auml;hren)
#Par:  rec   - Reference to the data structure.
#Par:  inrec - Record to which the extracted data will be added.
#Ref:  calibrationValues
#-------------------------------------------------------------------------------

RecordForDataReader := function (ref rec,
                                 ref inrec,
                                 frequencyBand=[-1,-1])
{
  nofAntennas := rec.len;

  ## -- Data type ----------------------------------------------------
  #
  # Identification of the type of dataset can be done using a combination
  # of header keywords; in principle this type of parsing also could be 
  # done in the C++ code, but the way of constructing an object there 
  # will typically take place in a much more well defined way.

  observatory := rec.head ("Observatory");
  datatype    := rec.head("DataType");

  if (observatory == "LOPES") {
    if (datatype == "TIM40") {
      inrec.datatype := "LopesEvent";
    } else if (datatype == "TIM40f") {
      inrec.datatype := "LopesProject";
    } else {
      inrec.datatype := "UNDEFINED";
    }
  }
  else if (observatory == "LOFAR-ITS" || observatory == "ITS") {
    if (datatype == "TIM40f") {
      inrec.datatype := "ITSCapture";
    } else if (datatype == "BEAM" || datatype == "Beam") {
      inrec.datatype := "ITSBeam";
    } else {
      inrec.datatype := "UNDEFINED";
    }
  }
  else if (observatory == "NDA") {
    if (datatype == "NANCAY2" || datatype == "BEAM" || datatype == "Beam") {
      inrec.datatype := "NDABeam";
    } else {
      inrec.datatype := "UNDEFINED";
    }
  }
  else {
    inrec.datatype := "UNDEFINED";
  }

  ## -- Basefile of the data set -------------------------------------
  #
  # Even though the "ITSMetadata" class should be able to handle this, 
  # we perform a check on the basefile here as well; since ITS data
  # sets are loaded using a file separate to the original "experiment.meta"
  # file, we already try to fix this here - this way the record passed to
  # a client program should contain the correct filename.

  basefile := rec.head ("Basefile");

  tmp := split (basefile,"/");
  
  if (inrec.datatype != "LopesEvent" && tmp[len(tmp)] != "experiment.meta") {

    basefile := "";

    for (n in [1:(len(tmp)-1)]) {
      basefile := paste (basefile,tmp[n],sep="/");
#      print n,tmp[n],basefile;
    }
    basefile := paste (basefile,'experiment.meta',sep="/");
  }

  inrec.basefile := basefile;

  ## -- Data common to all antennas ----------------------------------

  inrec.blocksize        := as_integer(rec.getmeta('Blocksize'));
  inrec.nyquistZone      := as_integer(rec.head("NyquistZone"));
  inrec.samplingRate     := rec.headf('Samplerate')*rec.headf('SamplerateUnit')
  inrec.antennas         := [[1:rec.len]-1];
  inrec.selectedAntennas := inrec.antennas[rec.select()];
  inrec.antennaSelection := rec.select();

  nofSelectedAntennas := as_integer(sum(inrec.selectedAntennas));

  ## frequency channel selection

  frequencies    := rec.get('Frequency');
  nofFrequencies := len(frequencies);

  if (frequencyBand[1] == -1 && frequencyBand[2] == -1) {
    frequencyBand := [rec.getmeta('FrequencyLow'),rec.getmeta('FrequencyLow')];
  }

  channelFlags := frequencies>=frequencyBand[1] & frequencies<=frequencyBand[2];

  inrec.selectedChannels := [1:nofFrequencies][channelFlags]-1;

  ## data for the individual antennas

  inrec.adc2voltage  := calibrationValues (rec,'Voltage',F,allAntennas=T);
  inrec.fft2calfft   := calibrationValues (rec,'CalFFT',F,allAntennas=T);

}

#-------------------------------------------------------------------------------
#Name: DataToRecord
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc: ../.. 
#Par:  rec   - Reference to the data structure.
#Par:  inrec - Record to which the extracted data will be added.
#Par:  field - Specifier for the set of parameters to be extracted
#Created: 2004/11/16 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

DataToRecord := function (ref rec, ref inrec, field) {

  if (field == 'Frequency') {
    FrequencyToRecord (rec,inrec);
  }

  if (field == 'ObservationInfo') {
    ObservationInfoToRecord (rec,inrec);
  }

}

#-------------------------------------------------------------------------------
#Name: FrequencyToRecord
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Adds new fields to a provided record: FFTSize,FFTlen,FrequencyUnit,
#      SamplerateUnit,Samplerate,NyquistZone,FrequencyLow,FrequencyHigh,
#      FrequencyBandwidth
#Par:  rec   - Reference to the data structure.
#Par:  inrec - Record to which the extracted data will be added.
#Created: 2004/11/16 (Lars Baehren)
#-------------------------------------------------------------------------------

FrequencyToRecord := function (ref rec, ref inrec) {

  inrec.FFTSize            := as_integer(rec.getmeta('FFTSize'));
  inrec.FFTlen             := as_integer(rec.getmeta('FFTlen'));

  inrec.FrequencyUnit      := as_double(rec.headf('FrequencyUnit'));
  inrec.SamplerateUnit     := as_double(rec.headf('SamplerateUnit'));
  inrec.Samplerate         := as_double(rec.headf('Samplerate'));

  inrec.NyquistZone        := as_integer(rec.head('NyquistZone'));

  freqLow  := as_double(rec.getmeta('FrequencyLow'));
  freqHigh := as_double(rec.getmeta('FrequencyHigh'));

  inrec.FrequencyLow       := freqLow;
  inrec.FrequencyHigh      := freqHigh;
  inrec.FrequencyBandwidth := freqHigh-freqLow;

}

#-------------------------------------------------------------------------------
#Name: FilesToRecord
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Adds new fields to a provided record: Files<br>
#      <i>Files</i> contains the names of actual data files of a given data set.
#Par:  rec   - Reference to the data structure.
#Par:  inrec - Record to which the extracted data will be added.
#Created: 2005/05/25 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

FilesToRecord := function (ref rec, ref inrec) 
{
  local files;

  index := [1:rec.len][rec.select()];
  nant  := len(index);

  antenna := 1;
  for (ant in index) {
    files[antenna] := rec.get('Header',ant).Files;
    antenna +:= 1;
  }

  inrec.Files := files;
}


#-------------------------------------------------------------------------------
#Name: ObservationInfoToRecord
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Adds  new fields to a provided record: Date,Observatory
#Par:  rec   - Reference to the data structure.
#Par:  inrec - Record to which the extracted data will be added.
#Created: 2004/12/01 (Lars Baehren)
#-------------------------------------------------------------------------------

ObservationInfoToRecord := function (ref rec, ref inrec) {

  inrec.Date        := qnt.quantity(rec.head('Date',1));
  inrec.Observatory := rec.head('Observatory');

}


#-------------------------------------------------------------------------------
#Name: FilenameFromHeader
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Set up a filename based on the epoch information in the data header; the
#      line
#      <ul>
#       #Date: 2004/07/17/18:50:18.08
#      </ul>
#      will be converted into the filename base <tt>2004.07.17-18:50:18.08</tt>
#-------------------------------------------------------------------------------

FilenameFromHeader := function (ref rec)
{
  filename := rec.get('Header').Date;
  filename =~ s/\//./;
  filename =~ s/\//./;
  filename =~ s/\//-/;
  return filename;
}


#-------------------------------------------------------------------------------
#Name: DataToASCII
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Export the values stored in the data array.
#Par:  rec                - Reference to the data structure.
#Par:  index     = -1     - Set of antennae for which the data are exported.
#Par:  field     = 'f(x)' - Type of data to be exported: f(x)
#Par:  filterRFI = -1     - Make <i>n</i> passed of RFI filtering, before
#                           exporting the data.
#Par:  tar       = F      - Put all created files into a tar archive?
#
#Example: DataToASCII (data, index=[1:10], blocksize=8192, field='f(x)')
#-------------------------------------------------------------------------------

DataToASCII := function (ref rec, 
                         index=-1,
                         field='f(x)',
                         blocksize=128^2,
                         filterRFI=-1, 
                         tar=F)
{
  fieldname := "UNDEFINED";
  local dataArray;

  # check set of antennae
  if (index==-1) {
    index := [1:rec.len];
  }
  nant := len(index);

  rec.setblock(blocksize=blocksize);
  rec.replot();

  # Set up the base for the created data files
  filenameBase := FilenameFromHeader (rec);

  ## data field dependent settings ---------------------------------------------

  if (field == 'f(x)') {
    fieldname := 'fx';
    dataArray := array (0,blocksize,nant);
  }
  else if (field == 'Voltage') {
    fieldname := 'voltage';
    dataArray := array (0,blocksize,nant);
  }
  else if (field == 'RawFFT') {
    fieldname := 'rawfft';
    dataArray := array (0,rec.getmeta('FFTlen'),nant);
  } 
  else if (field == 'FFT') {
    fieldname := 'fft';
    dataArray := array (0,rec.getmeta('FFTlen'),nant);
  } 
  else if (field == 'CalFFT') {
    fieldname := 'calfft';
    dataArray := array (0,rec.getmeta('FFTlen'),nant);
  } 
  else {
    print fieldname := "UNIDENTIFIED";
  }

  print 'field     :',field;
  print 'fieldname :',fieldname;

  ## export of the data --------------------------------------------------------

  export := T;

  for (n in index) {
    filenames[n] := filename;
    dat := rec.get(field=as_string(field),index=n);
    dataArray[,n] := dat;
  }

  if (export) {
    bar := progress (0,shape(dataArray)[1], 'Exporting data to file ...')
    bar.activate()
    # 
    filename := spaste (filenameBase,'-',blocksize,'samples','-',fieldname,'.data');
    shape    := shape(dataArray);
    #
    for (sample in [1:shape[1]]) { 
      output := "";
      for (antenna in [1:shape[2]]) {
        output := spaste (output,
	                  '  ',
			  real(dataArray[sample,antenna]),
			  '  ',
			  imag(dataArray[sample,antenna]));
      }
      shell(spaste('echo ',output,' >> ',filename)); 
      #
      bar.update(sample);
    }
  }

  if (tar) {
    tarfile := spaste(filenameBase,'.tgz');
    shell (spaste('tar -cvzf ',tarfile),' ',filenames)
  }

}


#-------------------------------------------------------------------------------
#Section: <a name="diagnostics">Diagnostics tools</a>
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: DataPrintDelays
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Print the currently set values for the various types of delays.
#Par:  rec   - Reference to the data structure.
#Ref: data.getary, data.get
#Created: 2004/11/24 (Lars Baehren)
#-------------------------------------------------------------------------------

DataPrintDelays := function (ref rec)
{
  print '======================================================================'
  print '[DataRecordIO::DataPrintDelays]'
  print ' - Correlation delay .... :',rec.getary('Corrdelay');
  print ' - Calibration delay .... :',rec.getary('Caldelay');
  print ' - Delay ................ :',rec.getary('Delay');
  print ' - z-correction ......... :',rec.getary('DoZ');
  print ' - Shift by trigger delay :',rec.getary('Doshift');
  print ' - Sky phase center ..... :',rec.get('AZEL');
  print ' - Source distance ...... :',rec.get('Distance');
  print ' - Shift in xy-plane .... :',rec.get('XY');
  print '======================================================================'
}

#-------------------------------------------------------------------------------
#Name: DataPrintBlockinfo
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Print information concerning blocksize and buffered blocks
#Par:  rec   - Reference to the data structure.
#Ref: data.getary, data.get
#Created: 2004/12/21 (Lars Baehren)
#-------------------------------------------------------------------------------

DataPrintBlockinfo := function (ref rec)
{
  print '======================================================================'
  print '[DataRecordIO::DataPrintBlockinfo]'
  print ' - Total file size ... [Samples] :',rec.getmeta('Filesize');
  print ' - File block size ... [Samples] :',rec.getmeta('FileBlocksize');
  print ' - FFT input size .... [Samples] :',rec.getmeta('FFTSize');
  print ' - FFT output length ........... :',rec.getmeta('FFTlen');
  print ' - Blocksize ......... [Samples] :',rec.getmeta('Blocksize');
  print ' - Size of datablock   [Samples] :',len(rec.get('f(x)'));
  print ' - Size of FFTed datablock ..... :',len(rec.get('FFT'));
  print ' - Size of CalFFTed datablock .. :',len(rec.get('CalFFT'));
  print ' - Size of phase gradients array :',len(rec.get('PhaseGrad'));
  print ' - Stored phase gradient ....... :',rec.getmeta('PhaseGradstoredregion');
  print ' - Stored raw FFT .............. :',rec.getmeta('RawFFTstoredregion');
  print ' - Current block number ........ :',rec.getmeta('Blocknum');
  print ' - Zero Offset ....... [Samples] :',rec.getmeta('ZeroOffset');
  print ' - Offset ............ [Samples] :',rec.getmeta('Offset');
  print '======================================================================'
}
