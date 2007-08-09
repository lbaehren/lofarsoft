
#-------------------------------------------------------------------------------
#Name:  tvfilter
#-------------------------------------------------------------------------------
#Type:  Function
#Doc:   filters the data needed for the TV picture
#
#Par:   startpos       - offset for the data
#Par:   antfilter=[]   - indices of antennas which should be flagged out 
#
#Out:   IFcut          - array with filtered data
#-------------------------------------------------------------------------------


tvfilter := function(startpos=197440,antfilter=[]){
  global data;
  starttime := time()
  #data.setblock(-1,blocksize=2097152, offset = 197440 + 20*313*5120) #(i-1)*313*5120)
  data.setblock(-1,blocksize=2097152, offset = startpos)
  #changes blocksize
  #blocksize is set to 2 to the power of 21 (=2097152) in order for a half-picture to fit in
  #this number is chosen so that the FFT can be computed faster
  #the offset depends on the picture you want to look at
  
  #get the frequency and the power values from the data
  freq := data.get('Frequency',1)                                 
  allsel := array(T,len(freq))
  data.putflag(allsel,field='FFT',index=-1)     
  pow := data.get('Power',1)

print 'Setting direction'
  dtime := time();
  #beamforming: setting the direction of the TV station azimuth = 154,784 deg and elevation = 0 deg
  #calculates corrdelays from TV-station
  #only necessary for the first picture, but setting the direction is quick
  phasecmp(data)   
  data.put([154.874,0],'AZEL')
  plotgui.refreshgui() 
  plotgui.setphase()

print 'first spawning'
  fstime := time();
  #Spawn button
  #normally you would have to press the spawn button but here it's done for you automatically
  globalpar.SpawnID+:=1
  data.aux:=[=]
  SpawnRecord(data,data.aux,totype='TIM40rev')
    #plotgui.aux:=plotguif(data.aux, title=spaste('>',title)) 
    #only needed when you want to look at the intermediate results
  data.aux.setblock(-1,blocksize=2097152,offset=0)
  #here the offset should always stay zero!
  #filter out antennas e.g. because they are noisy
  if (len(antfilter)>0) {  data.aux.select(antfilter,F); };

print 'second spawning'
  sstime := time();
  #Beam button: beamforming is really performed
  #it is very important to spawn before and not after the beamforming!
  #beamforming needs reference values from spawn
  data.aux.aux:=[=]
  SpawnRecord(data.aux,data.aux.aux,totype='TIM40beam')
    #plotgui.aux.aux:=plotguif(data.aux.aux,title=spaste('+',title))        
    #only needed when you want to look at the intermediate results
  data.aux.aux.setblock(-1,blocksize=2097152,offset=0)
  #here the offset should always stay zero!

print 'getting tvdata'
  tvtime := time();
  #select data needed for half-picture
  tvsel := freq>62.15e6 & freq<67.5e6
  data.putflag(tvsel,field='FFT',index=-1)     
  #index=-1 means 'for all'
  #creates a mask for datasets                       
  tvdata := data.aux.aux.get('f(x)',1)

print 'getting carrier'
  cartime := time();
  #find carrier as maximum in power-array tvsel
  carrysel := array(F,len(pow))
  c := maxpos(pow*tvsel)                       
  carrysel[c] := T     
print c,sum(carrysel);
  data.putflag(carrysel,field='FFT',index=-1)  
  #another mask is created
  carrier := data.aux.aux.get('f(x)',1)            

print 'combining and filtering'
  filttime := time();
  #combine carrier and tvdata: IF Intermediatefrequency
  IF := tvdata*carrier 

  global_fftserver.complexfft(IF,1)  
  #cutting away the redundant part in the frequency array  
  IF[1:524288] := 0  
  IF[1572864:2097152] := 0 
  #the first and the last quarter of the array are cut away       
  global_fftserver.complexfft(IF,-1)

  IFcut := as_float(IF)

  #looking at the data
  #this shouldn't be used if you want to create more than one picture
  #pg := pgplotter()
  #pg.plotxy1([1:2097152] , IFcut) 

  #saving the data
  ##write_value(IFcut,spaste('intermed',as_string(i),'.sos'))  

  endtime := time()
  print 'Total time:',sprintf('%6.2f',endtime-starttime),'seconds';
  print sprintf('Setup:%6.2f Direction:%6.2f 1st-spawn:%6.2f 2nd-spawn:%6.2f tvdata:%6.2f carrier:%6.2f final:%6.2f',dtime-starttime,fstime-dtime,sstime-fstime,tvtime-sstime,cartime-tvtime,filttime-cartime,endtime-filttime);

  return IFcut;
}


#-------------------------------------------------------------------------------
#Name: gettvpic
#-------------------------------------------------------------------------------
#Type:  Function
#Doc:   creates TV picture out of filtered antenna data
#
#Par:  intermedaux = 	  - filtered antenna data (->IFcut)
#Par:  outfile = ''	  - "file"name to write the image tool
#Par:  beginpic = 247296  - starting point of the TV picture in intermedaux 
#-------------------------------------------------------------------------------

gettvpic := function(intermedaux,outfile='',beginpic=247296) 
{

  #the next two lines are only needed when you want to look at the plotted data
  #data.put(intermedaux,'Dummy',1)
  #data.putmeta(2097152,'FileBlocksize',1)
  #the blocksize is set to 2 to the power of 21 (=2097152) so that the FFT can be computed faster
  #this number is also chosen for a half-picture to fit in

#  beginpic := 225990  

  #There have to be 313 rows for half a picture as mentioned before. In each row we have got 5120 pixels. That's why we need an array of the following size:

 ary1 := array(0.,5120,313)

   #Gaussfitting:
  #beginning at the first nicely visible sync-signal at the end of the black-level : beginpic

  #the following is only needed when you want to look at the first Gaussfit:
  #pg := pgplotter() 
  #pg.plotxy1([(beginpic - 0.5*5120):(beginpic + 0.5*5120)],intermedaux[(beginpic - 0.5*5120):(beginpic + 0.5*5120)])
  #This shouldn't be used in the normal picture creating process because it will take quite some time. 
  #Yet it might be helpful if something goes wrong.


  fitter := gauss1dfitter()
  guess.height := 1500
  guess.width := 100
  guess.center := beginpic 
  fitter.setstate(guess)
  fitter.fit([(beginpic - 0.06*5120):(beginpic + 0.1*5120)],intermedaux[(beginpic - 0.06*5120):(beginpic + 0.1*5120)])

 fiterg1 := fitter.getstate()

  print fiterg1

  fiterg := fiterg1

  #ary1[,313] := intermedaux[ fiterg.center : (fiterg.center + 5120) ]               

  #fitting gauss at the other sync-signals:
  oldcenter := fiterg1.center
  fitter := gauss1dfitter()
  fitlines := []
  fitpositions :=[]
  num := 0
  for (j in seq(286))
  { 
    #fiterg.center := beginpic + (j-1)*5120 
    guess.height := 1500
    guess.width := 100
    guess.center := oldcenter + 5120      
    fitter.setstate(guess)
    fitter.fit([(oldcenter+0.95*5120):(oldcenter+1.1*5120)],intermedaux[(oldcenter+0.95*5120):(oldcenter+1.1*5120)] - mean(intermedaux[(oldcenter+1.05*5120):(oldcenter+1.1*5120)]))

    fiterg := fitter.getstate() 
  
      #making sure that gauss fit doesn't fail unnoticed
    if (fiterg.converged && (fiterg.width>260) && (fiterg.width<400)) 
    {
      oldcenter := fiterg.center
      num:=num+1
      fitlines[num]:=(305-j)
      fitpositions[num]:=oldcenter
        #print j,fiterg.center,fiterg.converged,fiterg.width,'O.K.'
    } 
    else 
    {
      oldcenter := oldcenter + 5120
        print j,(305-j),fiterg.center,fiterg.converged,fiterg.width,'skipped'
    }

  }


  #fit line through calculated starting points of rows in order to get an improved picture
  linefitter := polyfitter()
  ok := linefitter.fit(coeff, coefferrs, chisq, fitlines, fitpositions, order=1)
#global coeff
#coeff[2] := -5120
  print 'Linefitter:',coeff,coefferrs

  for (j in [1:313])   
  {
    pos := coeff[1]+j*coeff[2]
    pos := as_integer(pos)
    ary1[,j] := intermedaux[pos:(pos+5120)]
  }


  #array for averaged data: ary2
  ary2 := array(0.,512,313)

  #taking the mean value of ten pixels from ary1, getting an array with only 512 pixels in one row
  for (i in seq(len(ary2[1,])))
  {
    for (j in seq(len(ary2[,1])))
    {
      ary2[j,i] := mean(ary1[ [((j-1)*10+1):(j*10)] ,i ]);
        #invert intensity, necessary because black level is high and not low in our data
    }
  }
  ary2 := max(ary2) - ary2;


  if (outfile!='') {  
    im := imagefromarray(outfile,pixels=ary2)
  } else {
    #display the TV picture
    im := imagefromarray(pixels=ary2)
    im.view()
  };

} #end of function
  #no explicit return given, therefore the return is F


#------------------------------------------------------------------------------------------
#Name: tvpicture
#------------------------------------------------------------------------------------------
#Type: Function
#Doc:  combines tvfilter and gettvpic
#
#Par:  syncpos    - center of the syncpeak before the last sync before the first blacklevel
#Par:  antfilter  - antennas that should be flagged out
#------------------------------------------------------------------------------------------


tvpicture := function(syncpos,antfilter=[])
{

beginpic := 247296   
 
startpos := syncpos - beginpic + 10*5120

IFcut := tvfilter(startpos,antfilter=antfilter)

gettvpic(IFcut,'',beginpic) 

}
