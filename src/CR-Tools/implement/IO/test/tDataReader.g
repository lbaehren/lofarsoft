
# -------------------------------------------------------------------------------
# $Id$
# -------------------------------------------------------------------------------


#--------------------------------------------------------------------------------
#Name: tdatareader
#--------------------------------------------------------------------------------
#Type: tdatareader
#Doc:  Main routine for exporting data to ASCII file.
#Par:  rec        -- Data record, passed by reference.
#Par:  infile     -- Name of the input file; if left black, the currently loaded
#                    data file will be used.
#Par:  index      -- The set of antennas to be included.
#Par:  blocksize  -- Blocksize, [samples].
#Par:  blocks     -- The number of blocks to process.
#Example: tdatareader (data, infile='', index=[1:10], blocksize=1024, blocks=5)
#--------------------------------------------------------------------------------

tdatareader := function (ref rec,
                         infile='',
                         index=-1,
                         blocksize=128^2,
                         blocks=1)
{
 local dataArray;
 fields := ["f(x)","Voltage","FFT","CalFFT"];

  ## if a filename if given by the user, we load this data file first; otherwise
  ## we assume that the data already have been loaded

  if (infile != '') {
    print '[tDataReader] Loading data file',infile,'...';
    readprfiles_cbfunc (infile);
  }

  ## check set of antennae

  if (index==-1) {
    index := [1:rec.len];
  }
  nant := len(index);

  ## adjust the blocksize

  rec.setblock(blocksize=blocksize);
  rec.replot();

  ## main loop over the number of blocks

  bar := progress (0,blocks, 'Scanning data ...')
  bar.activate()

  for (block in [1:blocks]) {
    
    for (field in fields) {

      dataArray := adjustDataArray (rec,field,blocksize,nant);

      for (antenna in [1:nant]) {
    
        f := rec.get(field=as_string(field),
                     index=index[antenna]);

        dataArray[,antenna] := f;

      } # end: antenna

      data2disk (block,field,dataArray);

    } # end: field

    bar.update(block);
    rec.nextblock();

  } # end : block

  ## revert to the start of the data volume

  rec.setblock(n=1);
  rec.replot();
}

#--------------------------------------------------------------------------------
#Name: adjustDataArray
#--------------------------------------------------------------------------------
#Type: 
#Doc:  Adjust the shape of the data array
#Par:  
#--------------------------------------------------------------------------------

adjustDataArray := function (ref rec,
                             field='f(x)',
                             blocksize,
                             nant)
{
  local tmp;

  if (field == 'f(x)') {
    tmp := array (0,blocksize,nant);
  }
  else if (field == 'Voltage') {
    tmp := array (0,blocksize,nant);
  }
  else if (field == 'FFT') {
    tmp := array (0,rec.getmeta('FFTlen'),nant);
  } 
  else if (field == 'CalFFT') {
    tmp := array (0,rec.getmeta('FFTlen'),nant);
  } 
  else {
    print 'Unknown field name!';
  }

  return tmp;  
}

#--------------------------------------------------------------------------------
#Name: data2disk
#--------------------------------------------------------------------------------
#Type: 
#Doc:  Write extracted data vector to disk
#Par:  
#Doc: Write extracted data vector to disk
#--------------------------------------------------------------------------------

data2disk := function (block,
                       field,
                       dataArray)
{
  ## adjust the field name identifier before using it for the filename

  if (field == 'f(x)') {
    field := 'fx';
  }

  filename := spaste ('tDataReader-',block,'-',field,'.data');

  bar := progress (0,shape(dataArray)[1], 'Exporting data to file ...')
  bar.activate()

  for (sample in [1:shape(dataArray)[1]]) { 
    output := "";
    for (antenna in [1:shape(dataArray)[2]]) {
      output := spaste (output,'  ',dataArray[sample,antenna]);
    }
    shell(spaste('echo ',output,' >> ',filename)); 
    #
    bar.update(sample);
  }

}

#--------------------------------------------------------------------------------
#Name: runClient
#--------------------------------------------------------------------------------
#Type: 
#Doc:  
#Par:  
#--------------------------------------------------------------------------------
 
runClient := function ()
{
}