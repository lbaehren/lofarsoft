
####################################################################################
#                                                                                  #
#                   functions for decoding sound signals                           #
#                                                                                  #
####################################################################################


#------------------------------------------------------------------------------------
#Name:  soundfilter
#------------------------------------------------------------------------------------
#Type:  Function
#Doc:   decodes an AM-modulated transmitter and averages it to a sampling rate of 40kHz
#
#Par:   startfreq      - given frequency of sound carrier (in Hz)
#Par:   bandwidth      - bandwidth of the signal to be decoded (in Hz)
#Par:   maxblocksize   - maximum size that should be used of one dataset
#Par:   antfilter=[]   - antennas that should be flagged out 
#Par:   direction=[]   - array with 2 elements: azimuth and elevation of sound signal
#                        (not included yet)  
#
#Out:   IFmean         - array with filtered and averaged data
#------------------------------------------------------------------------------------

#To do more than one block in a row the offset is increased by 2097152*3/4 for each new block.
#The first and the last eighth of a block are cut away.
#That is why two suceeding blocks have got an overlap of one eighth. 

soundfilter := function(startfreq,bandwidth=2.5e3,maxblocksize=0,antfilter=[],direction=[])
{


  #computing 'numblock' = number of complete blocks of the size 2097152 pixels
if (maxblocksize>0)
{
  numblock := as_integer((maxblocksize-525152)/1572000)
}
else
{
  numblock := as_integer((data.getmeta("FileBlocksize")-525152)/ 1572000)
}

print 'Getting',numblock,'blocks of data.';

IFmean := array(0.,numblock*786)
  #786: size of blocks with averaged data

  #loop over number of blocks
for (i in seq(numblock))
{

  global data;
  starttime := time()
  data.setblock(1,blocksize=2097152,offset=(i-1)*1572000)   #insert data.setblock(-1,...) for more antennas
    #the offset depends on the size of each block where parts are already cut away:
    #2000*786=1572000
    #for further explanation to the numbers see below


##beamforming: setting the direction of the radio station: 
##azimuth = first value in direction[] and elevation = second value in direction[]
##if (len(direction)>0)
##{
##  phasecmp(data)   
##  data.put([firstvalue of direction, second value of direction],'AZEL')
##  plotgui.refreshgui()
##  plotgui.setphase()
##}

##for more antennas:
##for ( j in seq(data.len) ) ....

##for more antennas: 
##if (len(antfilter)>0) 
##  {  data.aux.select(antfilter,F) }

  #the following things need to be performed only once                                        
if (i==1)
{
    #get the frequency and power values from the data
  freq := data.get('Frequency',1)                                 
  allsel := array(T,len(freq))
  data.putflag(allsel,field='FFT',index=1)                   #insert index=-1 for more antennas
  pow := data.get('Power',1)

print 'first spawning'
  #fstime := time();
  globalpar.SpawnID+:=1
  data.aux:=[=]
  SpawnRecord(data,data.aux,totype='TIM40rev')
  data.aux.setblock(1,blocksize=2097152,offset=0)            #insert -1 for more antennas             
    #here the offset should always stay zero!
    #filter out antennas e.g. because they are noisy

  #soundtime := time();
  soundsel := freq > startfreq-bandwidth  &  freq < startfreq+bandwidth
    #symmetric area around the assumed carrier  
    #creates a mask for datasets       
           
  #cartime := time();
    #find carrier as maximum in power-array soundsel
  carrysel := array(F,len(pow))
  c := maxpos(pow*soundsel)                       
  carrysel[c] := T     
print c,sum(carrysel),freq[c]/1e6;                            
    #another mask is created
}  
          
print 'getting sounddata'
  data.putflag(soundsel,field='FFT',index=1)                 #insert index=-1 for more antennas
  sounddata := data.aux.get('f(x)',1)
print 'getting carrier'           
  data.putflag(carrysel,field='FFT',index=1)                 #insert index=-1 for more antennas
  carrier := data.aux.get('f(x)',1)
 
print 'combining and filtering'
  #filttime := time();
  IF := sounddata*carrier

#explanation to the following numbers:
  #overlapping of the averaged blocks:
  #cut away first and last eighth: 2097152/8=262144
  #2097152-262144=1835008
  #lenght of the rest of the array: 1835008-262144=1572864
  #1572864/2000=786.432
  #2000*786=1572000
  #length of array without the outer parts containing the averaged data: 786

    #averaging over every 2000 pixels 
    #provides a sampling rate of 40 kHz
  for (k in seq(786))
  {
      IFmean[k+(i-1)*786] := mean(IF[(262144+(k-1)*2000+1):(262144+k*2000)]);
  }

  endtime := time()

print 'Block',i,'Total time:',sprintf('%6.2f',endtime-starttime),'seconds';

#print sprintf('Setup:%6.2f  1st-spawn:%6.2f sounddata:%6.2f carrier:%6.2f final:%6.2f',fstime-starttime,soundtime-fstime,cartime-soundtime,filttime-cartime,endtime-filttime);  
                                    
}#end of loop over numblock


return IFmean;

}




#------------------------------------------------------------------------------------
#Name:  int2shortsBE
#------------------------------------------------------------------------------------
#Type:  Function
#Doc:   convert one integer value to two shorts using big-endian coding
#
#Par:   inp	- input value (should be integer)
#
#Out:   array of two shorts
#------------------------------------------------------------------------------------
int2shortsBE := function(inp){
  inp := as_integer(inp[1])
  out := as_short([0,0]);
  out[1] := as_short(inp/65536);
  out[2] := as_short(inp%65536);
  return out;
};


#------------------------------------------------------------------------------------
#Name:  soundformat
#------------------------------------------------------------------------------------
#Type:  Function
#Doc:   writes filtered sound data into a SUN-style audio file
#
#Par:   IFmean         - array with filtered and averaged sound data
#Par:   samplerate     - sample rate (e.g. 40 kHz)
#Par:   outname        - name of the output soundfile, e.g. 'soundfile.au' 
#
#Out:   soundfile       
#------------------------------------------------------------------------------------
soundformat := function(IFmean, outname='soundfile.au', samplerate=40000)
{

outfile := as_short(array(0,(len(IFmean)+12)));
  #this size includes space for header and actual data from IFmean

  #making the range of values for the data larger
IFmeanbig := array(0.,(len(IFmean)))
IFmeanbig := IFmean*2^14/max(abs(IFmean))
 
 
IFmeanint := as_short(IFmeanbig) 
  #sounddata should consist of integers



outfile[1:2] := int2shortsBE(779316836);
  #magic number, first part of the header of a soundfile
  #0x2e736e64

outfile[3:4] := int2shortsBE(24);
  #data offset, 24 is the minimum valid number

outfile[5:6] := int2shortsBE(2147483647)
  #data size in octects
  #here just a very large number is chosen
  #0x7fffffff

outfile[7:8] := int2shortsBE(3);
  #data encoding format
  #'5' stands for 32-bit linear PCM

outfile[9:10] := int2shortsBE(samplerate)
  #sample rate is set to 40 kHz if nothing else is inserted

outfile[11:12] := int2shortsBE(1);
  #number of interleaved channels

outfile[13:(len(IFmean)+12)] := IFmeanint[1:(len(IFmean))]
  #filling in the actual data


wrec.filename := outname
wrec.type := 'int16be'
wrec.data := outfile

return globalpar.readdat_client->writefile(wrec)

}



#------------------------------------------------------------------------------------
#Name:  soundfile
#------------------------------------------------------------------------------------
#Type:  Function
#Doc:   combines soundfilter and soundformat
#
#input for soundfilter:
#Par:   startfreq      - given frequency of sound carrier
#Par:   antfilter=[]   - antennas that should be flagged out 
#Par:   maxblocksize   - maximum size that should be used of one dataset
#Par:   bandwidth          - area around the carrier 
#Par:   direction=[]   - array with 2 elements: azimuth and elevation of sound signal
#                        (not included yet) 
#input for soundformat:
#Par:   samplerate     - sampling rate, e.g. 40000
#Par:   outname        - name of the outputfile 
#
#Out:   outfile        - array with sound data in the correct format 
#------------------------------------------------------------------------------------

soundfile := function(startfreq,antfilter=[],maxblocksize=0,bandwidth=2.5e3,direction=[],samplerate=40000,outname='soundfile.au')
{

IFmean := soundfilter(startfreq,antfilter=antfilter,maxblocksize=maxblocksize,bandwidth=bandwidth,direction=direction)

soundformat(IFmean, outname=outname, samplerate=samplerate)

}







#useful stuff: 

#pg := pgplotter()
#pg.plotxy1([1:(786*64)] , IFmeanint[1:(786*64)])
#pg.plotxy1([1:(786*64)] , IFmeanbig[1:(786*64)])
#pg.plotxy1([1:(786*64+6)] , outfile[1:(786*64+6)])
#outfile2 := read_value('sounddata3.sos')
#write_value(outfile,'sounddata4.sos')

