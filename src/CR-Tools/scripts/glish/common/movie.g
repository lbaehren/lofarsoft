#pg:=pgplotter()
#i:=pg.open('plot.ps/ps')
#pg.env(-1,1,-1,1,1,0)
#pg.clos()
#pg.end()
#pg.slct(i)

movie:=function(ref rec,ref plotgui,fromazel=[0,90],toazel=F,nsteps=10,file='crevent',refant=1,leadout=0){
  padding:=['000','00','0','']
  if (len(toazel)!=2) {azel2:=rec.get('AZEL',refant)} else {azel2:=toazel}
  azel0:=rec.get('AZEL',refant);
  azel1:=fromazel;
  steps:=(azel2-azel1)/(nsteps-1);
  rec.put(refant,'RefAnt',-1);
  xy:=plotgui.pg.qwin()
  xd:=(xy[2]-xy[1]);
  yd:=(xy[4]-xy[3]);
  x1:=xy[1]; y1:=xy[3];
  x0:=x1+0.8*xd;  y0:=y1+0.8*yd;
  xsize:=0.25*xd;
  ysize:=0.25*yd;
  for (n in seq(nsteps+leadout)) {
    azel:=azel1+(n-1)*steps;
    rec.put(azel,'AZEL',-1);
    rec.setphase();
    plotgui.replot();

    plotgui.pg.sci(3)
    az:=toazel[1]*degree; el:=toazel[2]*degree;
    x:=x0+sin(az)*xsize*cos(el)
    y:=y0+cos(az)*ysize*cos(el)
    plotgui.pg.arro(x0,y0,x,y)
#print 1,x0,y0,x,y

    plotgui.pg.sci(2)
    az:=azel[1]*degree; el:=azel[2]*degree;
    x:=x0+sin(az)*xsize*cos(el)
    y:=y0+cos(az)*ysize*cos(el)
    plotgui.pg.arro(x0,y0,x,y)
##print 2,x0,y0,x,y


#    print 'setphase 2'
#    rec.setphase();
#    plotgui.replot();
    filename:=spaste('../moviedata/',file,padding[ceil(log(n+1))],as_string(n));
    print n,azel,filename; 
    plotgui.pg.psprinttofile(spaste(filename,'.ps'));
    shell(spaste('cd ../moviedata/; pstopnm ',filename,'.ps'));
    shell(spaste('pnmrotate -90 ',filename,'001.ppm > ',filename,'.ppm'));
    shell(spaste('rm ',filename,'001.ppm'));
    shell(spaste('rm ',filename,'.ps'));
  }
  rec.put(azel0,'AZEL',-1);
  shell('mpeg_encode movie.par')
  print 'Created movie.mpg!'
}


#select't' and fixed scale in x and y
scroll_movie:=function(ref rec,ref plotgui,xstart=0,xwidth=2,xstep=0.025,nsteps=200,file='scroll_movie',makemovie=T){
  padding:=['000','00','0','']
  for (n in seq(nsteps)) {
    x1:=xstart+(n-1)*xstep;
    x2:=x1+xwidth;
    plotgui.x1->delete('start','end');
    plotgui.x1->insert(as_string(x1));
    plotgui.x2->delete('start','end');
    plotgui.x2->insert(as_string(x2));
    plotgui.replot();

if (makemovie) {
    filename:=spaste('../moviedata/',file,padding[ceil(log(n+1))],as_string(n));
    print n,x1,filename; 
    plotgui.pg.psprinttofile(spaste(filename,'.ps'));
    shell(spaste('cd ../moviedata/; pstopnm ',filename,'.ps'));
    shell(spaste('pnmrotate -90 ',filename,'001.ppm > ',filename,'.ppm'));
    shell(spaste('rm ',filename,'001.ppm'));
    shell(spaste('rm ',filename,'.ps'));
   }
  }
if (makemovie) {
  shell('mpeg_encode scroll_movie.par')
  print 'Created movie.mpg!'
}
}

#globalpar.delays[1:8] := [0,0.15-2,-0.2,-0.4,-2,-2,1.15,0.75-1.75]
#FixDelays(data); data.setphase()

# 
#movie(data.aux,plotgui.aux.aux,nsteps=5*24,fromazel=[0,90],toazel=[41.9898208,64.7054428],leadout=2*24)
#movie(data.aux,plotgui.aux,nsteps=5*24,fromazel=[0,90],toazel=[41.9898208,64.7054428],leadout=2*24)
#movie(data.aux,plotgui.aux,nsteps=5,fromazel=[0,90],toazel=[41.9898208,64.7054428])
#ranges: -2.5,-1  und -1000 7e4  oder 1e5
