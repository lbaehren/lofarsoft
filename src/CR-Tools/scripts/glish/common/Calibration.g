
#-------------------------------------------------------------------------------
# $Id$
#-------------------------------------------------------------------------------

#datasets_base := '/dop71_2/data/ITS/2005.03'
#datasets_base := '/dop71_2/data/ITS/2005.02/monitoring_20050217'
#datasets_base := '/dop71_1/data/ITS/2004.09/deep_integration_01';

#subdirs := shell (spaste('ls ',datasets_base,' | grep -v dat | grep i00'))
#nofSubdirs := len(subdirs);

#datasets := array ("",nofSubdirs)
#for (i in [1:nofSubdirs]) {
#  datasets[i] := spaste(datasets_base,'/',subdirs[i]);
#}

#-------------------------------------------------------------------------------
#Name: extractGaincurve_gui
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Graphical User Interface to 'extractGaincurve'
#-------------------------------------------------------------------------------

extractGaincurve_gui := function () {
}

#-------------------------------------------------------------------------------
#Name: extractGaincurve
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Extract antenna gaincurves form a set of cross-correlation datasets.
#Par:  rec              - Reference to the data structure.
#Par:  nofBands         - Number of subbands, into which the frequency band is
#                         divided.
#Par:  division = 'lin' - Division for the distribution of the subbands:
#                         <ul>
#                           <li><i>lin</i> - linear division
#                           <li><i>log</i> - logarithmic division
#                         </ul>
#Par:  tablename        - Filename for the exported AIPS++ table.
#Par:  plotcurves = -1  - Set of antenna curves to plot; by default all gain
#                         curves are plotted.
#Par:  verboseON = F    - Enable/Disable verbose mode during operation.
#Example: extractGaincurve (data, datasets, nofBands=128);
#Created: 2005/03/09 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

extractGaincurve := function (ref rec,
                              datasets,
                              nofBands=128,
                              division="lin",
                              tablename='gaincurves.tbl',
                              plotcurves=-1,
                              verboseON=F)
{

  # ------------------------------------------------------------------
  # Set location of antenna position file for DataCC

  antennafile := (spaste(globalconst.progDir,'/antpos-its.dat'));

  # ------------------------------------------------------------------
  # Check the list of datasets used for the computation; create a 
  # mask marking wether or not a file in the list exists.

  nofDatasets := len(datasets);
  mask := array(F,nofDatasets);

  for (n in seq(nofDatasets)) {
    if (fexist(datasets[n])) {
      mask[n] := T;
    }
  }

  if (sum(mask) < 1) {
    print "[extractGaincurve] You must at least provide a single datasets!";
    fail;
  }

  # ------------------------------------------------------------------
  # Set up the record passed to the CLI

  local recCLI := [=];

  recCLI.datasets    := datasets[mask];
  recCLI.antennafile := antennafile;
  recCLI.nofBands    := nofBands;
  recCLI.division    := division;
  recCLI.tablename   := tablename;
  recCLI.verboseON   := verboseON;

  FrequencyToRecord (rec,recCLI);

  # ------------------------------------------------------------------
  # Run the CLI to carry out computation

  CLI := client(spaste(globalconst.LOPESBIND,'/ccData2Gaincurve'));

  resultsCLI := CLI->extractGaincurves (recCLI);

  # ------------------------------------------------------------------
  # Display the computation results

  if (len(plotcurves) ==1 && plotcurves[1] == -1) {
    plotcurves := seq(shape(resultsCLI.gainValues)[1]);
  }

  colorcode := 1;

  plot(resultsCLI.gainFrequencies,
       log(resultsCLI.gainValues[plotcurves[1],]),
       col=colorcode, 
       lines=T, 
       clear=T, 
       xlabel='Frequency [Hz]',
       title='ITS antenna gaincurves');

  for (ant in plotcurves[2:len(plotcurves)]) {
    colorcode +:= 1;
    plot(resultsCLI.gainFrequencies,
         log(resultsCLI.gainValues[ant,]),
         col=colorcode,
         lines=T,
         clear=F);
  }

}

#-------------------------------------------------------------------------------
#Name: calibrationValues
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Extract the gain calibration values to convert either
#      <ul>
#        <li><i>f(x)</i> to <i>Voltage</i>, or
#        <li><i>FFT</i> to <i>CalFFT</i>.
#      </ul>
#Par:  rec         - Reference to the data structure.
#Par:  field       - Field for which to extract the calibration values; this can
#                    be either <i>Voltage</i> or <i>CalFFT</i>
#Par:  verboseON   - Be verbose during computation?
#Par:  allAntennas - Get the calibration values for all the antennas in the data
#                    set; if set to <i>F</i> only the values for the currently
#                    selected antennas are returned.
#Ret:  gains      - Array, containing the calibration gain factors.
#Example: calibrationValues (data,verboseON=T)
#Created: 2005/04/04 (Lars B&auml;hren)
#-------------------------------------------------------------------------------

calibrationValues := function (ref rec,
                               field='CalFFT',
                               verboseON=F,
                               allAntennas=F)
{
  Blocksize      := as_integer(rec.getmeta('Blocksize'));
  nofFrequencies := len(rec.get("Frequency"));

  if (allAntennas) {
    index := [1:rec.len];
  } else {
    index := [1:rec.len][rec.select()];
  }
  nant := len(index);

  if (verboseON) {
    print '[calibrationValues]';
    print ' - Data field  :',field;
    print ' - Blocksize   :',Blocksize;
    print ' - Frequencies :',nofFrequencies;
    print ' - Antennas    :',index;
    print ' - # Antennas  :',nant;
  }

  antenna := 1;

  if (field == 'Voltage') {
    # Set size of returned array
    gains   := array(0.0,nant);
    # Get data from record and compute gain values
    for (ant in index) {
      fx      := rec.get('f(x)',ant);
      Voltage := rec.get('Voltage',ant);
      gains[antenna] := (Voltage/fx)[1];
      antenna +:= 1;
    }
  } else if (field == 'CalFFT') {
    # Set size of returned array
    gains   := array(0.0+0.0i,nofFrequencies,nant);
    # Get data from record and compute gain values
    for (ant in index) {
      dataFFT    := rec.get("FFT",ant);
      dataCalFFT := rec.get("CalFFT",ant);
      gains[,antenna] := dataCalFFT/dataFFT;
      #
      antenna +:= 1;
    }
  } else {
    print '[calibrationValues] Unsupported data field ',field;
  }

  return gains;
}


#-------------------------------------------------------------------------------
#Name: checkData 
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Compute some basic statistical properties of the data currently loaded.
#      Based on these properties automatic flagging of the data sets can be 
#      applied.
#Par:  rec      - Reference to the data structure.
#Par:  index    - Index of antennae, for which to run the checks; if
#                 <tt>index=-1</tt> all data sets in an observation will be 
#                 inspected, regardless of any previous selection.
#Par:  clipping - Fraction of samples in a data block allowed to be clipped
#                 (e.g. due to saturation of the LNA).
#Par:  autoflag - Apply flagging? If set <i>T</i>, data sets will be deselected
#                 based on the statistical test in the second half of the
#                 routine. If you want to be on the save side, then keep
#                 <tt>flag=F</tt>, inspect the returned record with the results 
#                 and then decide what to do.
#Par:  plot     - Create a plot from the extracted statistics
#Ret:  stats    - Record with the extracted statistics per antenna.
#Created: 2005/06/13 (Lars B&auml;hren)
#Example: rec := checkData (data)
#Ref: plotDataStatistics
#-------------------------------------------------------------------------------

checkData := function (ref rec,
                       index=-1,
                       clipping=0.01, 
                       autoflag=F,
                       plot=F,
                       which=-1)
{
  if (index != -1 ) {
    rec.select(1:rec.len,F);
    rec.select(index,T);
  } else {
    rec.select(1:rec.len,T);
  }

  index  := [1:rec.len][rec.select()];
  nant   := len(index);

  blocksize := rec.getmeta('Blocksize');
  minADC    := as_integer(rec.head('ADCMinChann'));
  maxADC    := as_integer(rec.head('ADCMaxChann'))-1;

  print '[Check data] Parameters and global statistics:';
  print ' - offset .. : ',rec.getmeta('Offset');
  print ' - index ... : ',index;
  print ' - nant .... : ',nant;
  print ' - clipping  : ',clipping;
  print ' - ADC range : ',minADC,maxADC;
  print ' - Blocksize : ',blocksize;

  # ------------------------------------------------------------------
  # Initialize variables storing the statistics

  stats := [=];

  statsMin  := 0.0;
  statsMax  := 0.0;
  statsMean := 0.0;
  statsSum  := 0.0;

  # ------------------------------------------------------------------
  # Go through all selected data sets and extract statistics

  bar := progress (0, nant, 'Analyzing data sets ...')
  bar.activate()   

  n := 1;
  for (ant in index) {
    stats[n].antenna := ant;
    stats[n].mask := T;
    # load block of data
    dat := rec.get('f(x)',ant);
    # Compute some statistics
    stats[n].min    := min(dat);
    stats[n].max    := max(dat);
    stats[n].sum    := sum(dat);
    stats[n].sqrsum := sum(dat^2);
    stats[n].mean   := stats[n].sum/nant;
    stats[n].stddev := stddev(dat);
    # Check for clipping
    nofMin := 0.0;
    nofMax := 0.0;
    for (i in 1:blocksize) {
      if (dat[i] == maxADC) {
        nofMax +:= 1.0;
      }
      else if (dat[i] == minADC) {
        nofMin +:= 1.0;
      }
    }
    stats[n].fracMax := nofMax/(1.0*blocksize);
    stats[n].fracMin := nofMin/(1.0*blocksize);
    # keep track of some global statistics
    statsMin  +:= stats[n].min;
    statsMax  +:= stats[n].max;
    statsMean +:= stats[n].mean;
    statsSum  +:= stats[n].sum;
    # 
    bar.update(n);
    # increment dataset counter
    n +:= 1;
  }

  # ------------------------------------------------------------------
  # normalize the global statistics
 
  statsMin  /:= nant;
  statsMax  /:= nant;
  statsMean /:= nant;
  statsSum  /:= nant;

  print ' - min .... : ',statsMin;
  print ' - max .... : ',statsMax;
  print ' - mean ... : ',statsMean;
  print ' - sum .... : ',statsSum;
  
  # ------------------------------------------------------------------
  # Check individual data sets against global statistics

  bar := progress (0, nant, 'Auto-flagging data sets ...')
  bar.activate()   

  n := 1;
  for (ant in index) {
    # (a) Minimum value
    diff := abs((statsMin - stats[n].min)/statsMin);
    if (diff > 0.5) {
      stats[n].mask := F;
      print 'Masking dipole',ant,'- deviation in minimum value:',
            statsMin,stats[n].min,diff;
    } 
    # (b) Maximum value
    diff := abs((statsMax - stats[n].max)/statsMax);
    if (diff > 0.5) {
      stats[n].mask := F;
      print 'Masking dipole',ant,'- deviation in maximum value:',
            statsMax,stats[n].max,diff;
    } 
    # (c) Standard deviation
    if (stats[n].stddev < 10) {
      stats[n].mask := F;
      print 'Masking dipole',ant,'- standard deviation of values:',
            stats[n].stddev;
    }
    # (d) Clipping
    if (stats[n].fracMax > clipping || stats[n].fracMin > clipping) {
      stats[n].mask := F;
      print 'Masking dipole',ant,'- clipping:',
            stats[n].fracMin,stats[n].fracMax;
    }
    #
    bar.update(n);
    # increment dataset counter
    n +:= 1;
  }

  if (plot) {
    plotDataStatistics (rec, stats, which=which);
  }

  # ------------------------------------------------------------------
  # If enabled, automatically flag data sets

  if (autoflag) {
    n := 1;
    for (ant in index) {
      if (!stats[n].mask) {
        rec.select(ant,F);
      }
      n +:= 1;
    }
    rec.replot();   
  }

  return stats;
}

#-------------------------------------------------------------------------------
#Name: plotDataStatistics
#-------------------------------------------------------------------------------
#Type: Glish Function
#Doc:  Plot the results from the 'checkData()' routine.
#Par:  rec   - Reference to the data structure.
#Par:  stats - Record with the data set statistics, as returned by checkData
#Par:  which - Which data are plotted:
#              <ul>
#                <li>-1 = all
#                <li>T  = only valid data sets
#                <li>F  - only flagged data sets
#              </ul>
#Created: 2005/06/13 (Lars B&auml;hren)
#Ref: CalcAntPos
#-------------------------------------------------------------------------------

plotDataStatistics := function (ref rec,
                                ref stats,
                                which=-1)
{
  # ------------------------------------------------------------------
  # Get currently selected data sets and the corresponding anntenna
  # positions

  index  := [1:rec.len][rec.select()];
  nant   := len(index);

  antpos := CalcAntPos (rec,
                        index=index,
                        phasecenter=T,
                        verboseON=F);

  # ------------------------------------------------------------------

  filenameBase := FilenameFromHeader(rec);
  offset       := rec.getmeta('Offset');
  filesize     := rec.getmeta('Filesize');

  filename := spaste(filenameBase,'-o:',
                     padLoopIndex(offset,filesize),
                     '.dat');

  # ------------------------------------------------------------------

  n := 1; 
  for (ant in index) {
    output := spaste(antpos[ant].position[1],'  ',  # 1 x-coord
                     antpos[ant].position[2],'  ',  # 2 y-coord
                     antpos[ant].position[3],'  ',  # 3 z-coord
                     stats[n].fracMin,'  ',         # 4 clip-fraction
                     stats[n].fracMax,'  ',         # 5 clip-fraction
                     stats[n].stddev,'  ',          # 6 standard-deviation
                     stats[n].sqrsum);              # 7 Power

    if (which == T && stats[n].mask == T) {
      shell(spaste('echo ',output,' >> ',filename));
    }
    else if (which == F && stats[n].mask == F) {
      shell(spaste('echo ',output,' >> ',filename)); 
    } 
    else if (which == -1) {
      shell(spaste('echo ',output,' >> ',filename)); 
    } 
    n +:= 1;
  }    

}
