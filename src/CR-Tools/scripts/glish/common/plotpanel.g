
#-------------------------------------------------------------------------------
#
# Layout and functionality of the main LOPES Tools GUI.
#
#-------------------------------------------------------------------------------

#plotpanel.g
#Run with
#include 'mathematics.g'; include 'fftserver.g';
#include 'readdata.g'; include 'plotpanel.g'; include 'cursor.g'
#plotgui:=plotguif(data)

#Maximum Number of panels allowed in the plotpanel display
maxnpanels:=globalpar.maxnpanels

dirtyaddedarrays:=[]

gui:=function(){global data,plotgui; if (!is_defined('data') || len(data)<=1) input_event(data,'example.event'); plotgui:=plotguif(data); data.newsagent->message(':o)')}

plot1:=function(ary,aryy=-1,col=-1,lines=T){
global pg
if (!is_defined('pg') || (len(pg)==1)) pg:=pgplotter(foreground='black',background='white')
if (col>=0) pg.sci(col);
if (len(aryy)==1 && aryy==-1) {pg.plotxy1(seq(len(ary)),ary,plotlines=lines)} else {pg.plotxy1(ary,aryy,plotlines=lines)};
}

#-----------------------------------------------------------------------
#Name: plot
#-----------------------------------------------------------------------
#Type: Function
#Doc: A generic tool to quickly produce plots of data
#-----------------------------------------------------------------------
plot:=function(aryx,aryy,err=[],col=-1,lines=F,clear=F,xlabel='',ylabel='',title='',xborder=0.05,yborder=0.05,symbol=2,linestyle=1,psfile=F,filename='lopesplot.ps',xrange=F,yrange=F,errorbarticklength=1,logy=F,logx=F){
  global pg, globalpar
  clr:=clear;

  if (len(err)>0) {
    err1:=aryy+err
    err2:=aryy-err
    if (logy) {
      err1:=log(dezero(abs(err1)));
      err2:=log(dezero(abs(err2)));
    }
  }

  if (logy) {
    aryy:=log(dezero(abs(aryy)));
  }

  if (logx) {
    aryx:=log(dezero(abs(aryx)));
  }

  if (len(xrange)==2) {
    xmin:=xrange[1]
    xmax:=xrange[2]
  } else {
    xmin:=min(aryx)
    xmax:=max(aryx)
  }
  if (len(yrange)==2) {
    ymin:=yrange[1]
    ymax:=yrange[2]
  } else {
    ymin:=min(aryy)
    ymax:=max(aryy)
  }
  
  xmin:=xmin-(xmax-xmin)*xborder
  xmax:=xmax+(xmax-xmin)*xborder
  ymin:=ymin-(ymax-ymin)*yborder
  ymax:=ymax+(ymax-ymin)*yborder

  if (!is_defined('pg') || (len(pg)==1)) {clr:=T; pg:=pgplotter(foreground='black',background='white')};
  if (clr) {
    pg.clear();
}
  if (globalpar.PrintingPlots) {
    pg.sch(globalpar.PrintCharHeight);
    pg.slw(globalpar.PrintLineWidth);
    pg.sci(globalpar.PrintLabelCol);
  }
  if (clr) {
    pg.env(xmin,xmax,ymin,ymax,0,0);
    pg.lab(xlabel,ylabel,title);
  }
  if (globalpar.PrintingPlots) {
    pg.sch(globalpar.PrintCharHeight);
    pg.slw(globalpar.PrintLineWidth);
  }
  if (col>=0) pg.sci(col);
  if (lines) {
    pg.sls(linestyle);
    pg.line(aryx,aryy);
    pg.sls(1);
  } else {
    pg.pt(aryx,aryy,symbol);
  }
  if (len(err)>0) {
    pg.erry(aryx,err1,err2,errorbarticklength)
  } 
  if (psfile) {
    pg.postscript(filename)
    print '=> Writing plot to postscript file ',filename
  }
}

plot2:=plot;

#-----------------------------------------------------------------------
#Name: ad
#-----------------------------------------------------------------------
#Type: Function
#Doc: ad = Add (global) Data records to the standard data record
#
#Par: s = 		- A string of words which contains variable
#			  names of data records that will be added
#			  to the global "data" record. 
#			  "data" is hardwired! 
#     field =		- field name that will be useded to add the data
#			  should be 'f(x)'
#     presync = 769	- value, that will be written into the "presync"
#			  header of the new data record. (Default value
#			  is good for current (30.1.2004) "pipelined" data)
#Exa: agd("a b c") adds the arrays in a,b,& c to  data
#     Note that the first data record is copied first and then overwritten
#Ref: dd
#-----------------------------------------------------------------------
ad:=function(s,field='f(x)',presync=769) {
  global data
  global dirtyaddedarrays
  for (v in s) {
   data.copy(1,data.len+1);
   print 'Added',v,'as record #',data.len,'to data.'
   data.put(eval(v),field=field,index=data.len)
   data.puthead(v,"Filename",data.len)
   data.puthead(presync,"presync",data.len)
   data.putmeta(len(symbol_value(v)),"FileBlocksize",i);
   dirtyaddedarrays:=[dirtyaddedarrays,data.len]
  }
}

#-----------------------------------------------------------------------
#Name: dd
#-----------------------------------------------------------------------
#Type: Function
#Doc: dd = Delete data records that were added with ad
#
#Ref: ad
#-----------------------------------------------------------------------
dd:=function() {
  global data
  global dirtyaddedarrays
  nn:=dirtyaddedarrays[len(dirtyaddedarrays):1]
  print nn
  for (i in nn) {
   print 'Delete',data.head("Filename",i),'as record #',i
   data.del(i);
  }
   dirtyaddedarrays:=[]
}


#-----------------------------------------------------------------------
#Name: corrfiles
#-----------------------------------------------------------------------
#Type: Function
#Doc: Cross-Correlate datasets and averages them
#Ret: T/F
#Par: rec =	  			- Data record
#Par: fields =				- Which coordinate to crosscorrelate
#Par: maxlag =				- Maximum number of lags in Pixel
#Par: maxord =				- Maximum order??
#Ref: readfiles,plotpanels
#-----------------------------------------------------------------------

corrfiles:=function(ref rec,fields,maxlag=100,maxord=100){
    nsum:=0; n:=rec.len+1; corrfile:='';ary:=0; cross:=[];		
#    for (i in [1:rec.len][rec.select()]) {
     print 'Calculating Cross-Correlation'
    for (i in [1:1]) {
#Copy first record with all fields into the new n+1st record
        if (nsum==0) {
	   rec.copy(i,0)
	   rec.fill(fields,0)
           type:=rec.head("DataType",i);
   ID:=rec.head("ID",i);
	 }	   
#Add record to data in n+1st record if type 
# and ID match# && rec.head("ID",i)==ID) {

         if (rec.head("DataType",i)==type) {
	    nsum+:=1; corrfile:=spaste(corrfile,as_string(i),'+');
	    for (j in [1:len(fields)]) {
	       ary1:=rec.get(fields[j],i);
	       ary2:=rec.get(fields[j],i+1);
	       cross:=crosscorr(ary1,ary2,maxlag,maxord);
	       ary:=cross.corr+ary;
	       rec.put(ary,fields[j],n)
##Achtung hier wird gepfuscht
#	       print cross.lag
#	       rec.put(cross.lag,"Lag",n)
	    }
	 }
     }

#Divide by number of files to get average
    for (j in [1:len(fields)]) {
     rec.put(rec.get(fields[j],n)/nsum,fields[j],n)
    }
    rec.puthead("CrossCorr","DataType",n)
    corrfile:=spaste(corrfile,fields[1]);
    rec.puthead(corrfile,"Filename",n)
print 'Done with Cross-Corr!'
}

#-----------------------------------------------------------------------
#Name: avgfiles
#-----------------------------------------------------------------------
#Type: Function
#Doc: Produce an average of the selected files.
#Ret: T/F
#Par: rec =	  			- Data record
#Par: func =				- Function applied to data
#					  before averaging
#Par: fields =				- Which coordinate to average
#Ref: readfiles,plotpanels
#-----------------------------------------------------------------------

avgfiles:=function(ref rec,f,fields) {
    nsum:=0; n:=rec.len+1; avfile:=''		
    for (i in [1:rec.len][rec.select()]) {
#Copy first record with all fields into the new n+1st record
        if (nsum==0) {
	   rec.copy(i,0)
	   rec.fill(fields,0)
           type:=rec.head("DataType",i);
	   ID:=rec.head("ID",i);
	 }
#Add record to data in n+1st record if type
#and ID match # && rec.head("ID",i)==ID
         if (rec.head("DataType",i)==type) {
	    nsum+:=1; avfile:=spaste(avfile,as_string(i),'+');
	    for (j in [1:len(fields)]) {
	    print i,nsum,j,fields[j],n,avfile
	       ary:=f(rec.get(fields[j],i))
	       ary:=rec.get(fields[j],n)+ary
	       rec.put(ary,fields[j],n)
	    }
	 }
     }

#Divide by number of files to get average
    for (j in [1:len(fields)]) {
     rec.put(rec.get(fields[j],n)/nsum,fields[j],n)
    }
    rec.puthead("Average","DataType",n)
    avfile:=spaste(avfile,fields[1]);
    rec.puthead(avfile,"Filename",n)
}

#-----------------------------------------------------------------------
#Name: selectfiles
#-----------------------------------------------------------------------
#Type: Function
#Doc: Opens a window, listing the files available in the buffer and
#     the ones selected by the user.
#Ret: Modifies the selection vector in rec
#-----------------------------------------------------------------------

selectfiles:=function(ref rec){
    tk_hold();
    f  := frame(title='Operations File Selection')
    mf := frame(f,side='left',expand='x')
    m  := message(mf,'Deselected data files in the current data set will be excluded from calculations and the plot window.',
                  width=500)
    h:=min(15,rec.len);
    lbf := frame(f,side='left',expand='both', relief='sunken', borderwidth=1)
    lbfiles := synclistboxes(parent=lbf,
                    nboxes=4, labels=['Nr.','Name','Type','On'],
                    vscrollbarright=T,
                    hscrollbar=F,
                    height=h, width=[4,2,50,10],
                    mode='extended',
                    background=['lightgrey','white','white','white'],
                    relief='flat', borderwidth=0,
                    fill='both');

    local sel := rec.select();
    listf:=function(){
      lbfiles.listbox(1)->delete("start","end")
      lbfiles.listbox(1)->insert(array2string(seq(rec.len)))
      lbfiles.listbox(2)->delete("start","end")
      for (i in 1:rec.len) {
        if (sel[i]) {
          lbfiles.listbox(2)->insert('+')
        } else {
          lbfiles.listbox(2)->insert(' ')
        }
      }
      lbfiles.listbox(3)->delete("start","end")
      lbfiles.listbox(3)->insert(array2string(rec.headarr('Filename')))
      lbfiles.listbox(4)->delete("start","end")
      lbfiles.listbox(4)->insert(array2string(rec.headarr('DataType')))
    }
    listf()

    whenever lbfiles->select do {
	    for (i in $value) sel[i+1] := !sel[i+1] #rec.put(!rec.get("Select",i+1),"Select",i+1)
	    listf()
    }

    ef := frame(f,side='left',expand='x')
    but_allselect   := button(ef,'Select All',
                              borderwidth=1, width=9)
    but_alldeselect := button(ef,'Deselect All',
                              borderwidth=1, width=9)
    but_deletelast  := button(ef,'Delete Last',
                              borderwidth=1, width=9)
    spacer          := frame(ef,width=10,height=1,expand='x')
    but_rescan      := button(ef,'Reset',
                              borderwidth=1, width=9)
    but_apply       := button(ef,'Apply',
                              borderwidth=1, width=9, background='limegreen')
    but_done        := button(ef,'Done',
                              borderwidth=1, width=9, background='red', foreground='white')
    tk_release();

    whenever but_allselect->press do {
	     sel[1:rec.len] := T
	     listf()}
    whenever but_alldeselect->press do {
	     sel[1:rec.len] := F
	     listf()}
    whenever but_deletelast->press do {
	     rec.del(rec.len);
	     listf()}
    whenever but_rescan->press do {
	     sel := rec.select();
	     listf()}
    whenever but_apply->press do {
	     for (i in 1:rec.len) {
	       rec.put(sel[i],"Select",i)
	     }
	     rec.replot()}
    whenever but_done->press do {
	     for (i in 1:rec.len) {
	       rec.put(sel[i],"Select",i)
	     }
	     f:=F;
	     rec.replot()}
}

subsequence datawatchclient(){
  whenever self ->checkdata do {self->newdata(T)}
  whenever self ->message do {self->newmessage($value)}
}

subsequence memwatchclient(){
  whenever self ->update do {self->update(T)}
}


#-----------------------------------------------------------------------
#Name: zoompanel
#-----------------------------------------------------------------------
#Type: Function
#Doc: Sets x1,x2,y1,y2 limits of the box to the values determined
#     with a box cursor.
#Ret: T/F
#Par: plotgui =				- Record describing the GUI
#Ref: plotgui, keypressed
#-----------------------------------------------------------------------

zoompanel:=function(ref plotgui) {
         if (plotgui.select_x1>=plotgui.select_x2 || plotgui.select_y1>=plotgui.select_y2) return
	     plotgui.x1->delete('start','end');
	     plotgui.x2->delete('start','end'); 
	     plotgui.y1->delete('start','end'); 
	     plotgui.y2->delete('start','end'); 
	     plotgui.x1->insert(as_string(plotgui.select_x1));
	     plotgui.x2->insert(as_string(plotgui.select_x2));
	     plotgui.y1->insert(as_string(plotgui.select_y1));
	     plotgui.y2->insert(as_string(plotgui.select_y2));
	     plotgui.fixedscale->state(T);
             plotgui.setscalestatus();
}


#-----------------------------------------------------------------------
#Name: IntegrateData
#-----------------------------------------------------------------------
#Type: Function
#Doc: integrates the data over a certain region and return a record
#     with the sum and mean and background subtracted mean.
#     The background is assumed to be determined by the first
#     set of results in the result record.
#
#Par: ary        - record of arrays with data
#Par: result     - contains a result record that will be modified
#Par: x1,x2      - x-range (in positions) to be integrated
#Par: index=1    - which data set in record (e.g., antenna #)
#Par: ncall=1    - how often did you call this routine, first call
#Par:              will be used for background subtraction
#
#Ret: result record
#Ref: plotgui
#-----------------------------------------------------------------------
IntegrateData:=function(ref ary,ref result,index=1,x1=1,x2=-1,ncall=1) {
          n:=ncall;
          if (x2<0) {x2b:=len(ary[1])} else {x2b:=x2};
          s:=0; width:=(x2b-x1+1);
          asdv:=0; amx:=0; amn:=0; null:=array(0,len(index))
          result[n]:=[=];
          result[n].one:=[=];
          result[n].one.n:=null;
          result[n].one.width:=null;
          result[n].one.sum:=null;
          result[n].one.stddev:=null;
          result[n].one.max:=null;
          result[n].one.min:=null;
          result[n].one.mean:=null;
          result[n].one.sumsubt:=null;
          result[n].one.meansubt:=null;
          result[n].one.sigma_sum:=null;
          result[n].one.sigma_max:=null;
         for (i in index) {
            sn:=sum(ary[i][x1:x2b]);
            sdv:=stddev(ary[i][x1:x2b]);
            mx:=max(ary[i][x1:x2b]);
            mn:=min(ary[i][x1:x2b]);
            result[n].one.n[i]:=n;
            result[n].one.width[i]:=width;
            result[n].one.sum[i]:=sn;
            result[n].one.stddev[i]:=sdv;
            result[n].one.max[i]:=mx;
            result[n].one.min[i]:=mn;
            result[n].one.mean[i]:=sn/width;
            result[n].one.sumsubt[i]:=sn-result[1].one.mean[i]*width;
            result[n].one.meansubt[i]:=result[n].one.mean[i]-result[1].one.mean[i];
            result[n].one.sigma_sum[i]:=result[n].one.sumsubt[i]/(result[1].one.mean[i]*width);
            result[n].one.sigma_max[i]:=result[n].one.max[i]/(result[1].one.stddev[i]);
            s+:=sn; asdv+:=sdv; amx+:=mx; amn+:=mn
          }  
          l:=len(index);
          result[n].all:=[=];
          result[n].all.n:=n;
          result[n].all.nant:=l;
          result[n].all.width:=width;
          result[n].all.gsum:=s;
          result[n].all.avg_stddev:=asdv/l;
          result[n].all.avg_max:=amx/l;
          result[n].all.avg_min:=amn/l;
          result[n].all.avg_sum:=s/l;
          result[n].all.avg_mean:=s/width/l;
          result[n].all.avg_sumsubt:=result[n].all.avg_sum-result[1].all.avg_mean*width
          result[n].all.avg_meansubt:=result[n].all.avg_mean-result[1].all.avg_mean
          result[n].all.sigma_sum:=result[n].all.avg_sumsubt/(result[1].all.avg_mean*width);

          if (len(index)>1) {return result[n].all} else  {return result[n].one}
}

#-----------------------------------------------------------------------
#Name: unflagdata
#-----------------------------------------------------------------------
#Type: Function
#Doc: Fills the flag record rec.filter with "1"s to unflag data
#     for a certain domain (time or frequency)
#
#Par: rec        - data record
#Par: field="T"  - field to define "T" of "F" domain for flagging
#Par: index=1    - which data set in record (e.g., antenna #)
#Par: flag=1     - value to use for flagging
#
#Ret: T/F
#Ref: upcallback, keypressed,flagdata
#-----------------------------------------------------------------------
unflagdata:=function(ref rec,field="FFT",index=1,flag=1) {
    for (i in index) {
         domain:=rec.coordinfo(field,i)["Domain"]
         tfxy:=spaste(domain,"x");
         defvar:=rec.get('DomDefault',i)[tfxy];
         l:=len(rec.get(defvar,i));
         rec.putflag(array(flag,l),field,i)
    }
 }

#-----------------------------------------------------------------------
#Name: flagdata
#-----------------------------------------------------------------------
#Type: Function
#Doc: Fills the flag record rec.filter with 0s to flag data between
#     start and end for a certain domain (time or frequency)
#
#Par: rec        - data record
#Par: field="T"  - field to define "T" of "F" domain for flagging
#Par: index=1    - which data set in record (e.g., antenna #)
#Par: start,end  - flag positions from start to end
#Par: flag=1     - value to use for flagging
#
#
#Ret: T/F
#Ref: upcallback, keypressed, unflagdata
#-----------------------------------------------------------------------

flagdata:=function(ref rec, field="FFT", index=1,start=-1, end=-1,flag=0) {
    for (i in index) {
         domain:=rec.coordinfo(field,i)["Domain"]
         tfxy:=spaste(domain,"x");
         defvar:=rec.get('DomDefault',i)[tfxy];
         l:=len(rec.get(defvar,i));
         if (len(rec.getflag(field,i))!=l) {unflagdata(rec,field,i,1)}
         if (start>0 && end>=start && end<=l) {
           f:=rec.getflag(field,i)  
           f[start:end]:=flag
           rec.putflag(f,field,i)  
         }
    }
}

#-----------------------------------------------------------------------
#Name: flagdatarange
#-----------------------------------------------------------------------
#Type: Function
#Doc: Fills the flag record rec.filter with 0s to flag datat between
#     startx and endx in world coordinates for a certain domain 
#     (time or frequency)
#
#Par: rec          - data record
#Par: field='FFT' - field on which to perform flagging (x or y-coordinate)
#Par: index=1      - which data set in record (e.g., antenna #)
#Par: range=[0,1]  - limiting values between which to flag
#Par: indside="T"  - If false flag outside the specified range
#Par: flag=1       - value to use for flagging
#
#
#Ret: T/F
#Ref: upcallback, keypressed, unflagdata
#-----------------------------------------------------------------------

flagdatarange:=function(ref rec,field='FFT',index=1,range=[0,1],inside=T,flag=0) {
    for (i in index) {
         domain:=(rec.coordinfo(field,i))["Domain"]
         ary:=rec.get(field,i);
         l:=len(ary);
         if (len(rec.getflag(field,i))!=l) {unflagdata(rec,field,i,1)}
         if (inside) {bl:=((ary >= range[1]) & (ary <= range[2]))}
         else {bl:=(ary < range[1]) | (ary > range[2])}
         fary:=rec.getflag(field,i);
         fary[bl]:=flag;
         rec.putflag(fary,field,i);
    }
}


#-----------------------------------------------------------------------
#Name: zoomoutpanel
#-----------------------------------------------------------------------
#Type: Function
#Doc: Sets x1,x2,y1,y2 limits back to automatic scaling
#Ret: T/F
#Par: plotgui =				- Record describing the GUI
#Ref: plotgui, keypressed
#-----------------------------------------------------------------------

zoomoutpanel:=function(ref plotgui) {
	     plotgui.x1->delete('start','end'); 
	     plotgui.x2->delete('start','end'); 
	     plotgui.y1->delete('start','end'); 
	     plotgui.y2->delete('start','end'); 
	     plotgui.fixedscale->state(F);
}

#-----------------------------------------------------------------------
#Name: setpanel
#-----------------------------------------------------------------------
#Type: Function
#Doc: Resets display to show panel no. n in the center.
#Ret: T/F
#Par: plotgui =				- Record describing the GUI
#Ref: plotgui, keypressed, zoompanel, setnpanels
#-----------------------------------------------------------------------

setpanel:=function(ref plotgui,panel=1) {
  plotgui.startpanel:=min(max(1,as_integer(panel)),plotgui.nselected());
  plotgui.startpanelscale->value(plotgui.startpanel);
}

#-----------------------------------------------------------------------
#Name: setnpanels
#-----------------------------------------------------------------------
#Type: Function
#Doc: Resets display to show nx*ny panels.
#Ret: T/F
#Par: plotgui =				- Record describing the GUI
#Ref: plotgui, keypressed, zoompanel		
#-----------------------------------------------------------------------

setnpanels:=function(ref plotgui,nx=1,ny=1,panel=0) {
	     plotgui.nx := nx;
 	     plotgui.ny := ny;
	     # set negative values to prevent replotting
	     plotgui.nxscale->value(nx);
	     plotgui.nyscale->value(ny);
	     if (panel>0) {
               npanels:=plotgui.nx*plotgui.ny;
	       startpanel:=panel-npanels/2;
	       setpanel(plotgui,startpanel)
	       plotgui.startpanelscale->value(startpanel);
	     } 
}

#-----------------------------------------------------------------------
#Name: deletepanels
#-----------------------------------------------------------------------
#Type: Function
#Doc: Delete selected panels in plotpanel 
#Ret: T/F
#Par: plotgui 				- Record describing the GUI
#Par: frompanels 			- vector containing the ref numbers of panels to 
#                               be deleted
#Ref: plotgui, plotpanel, upcallback, movepanels
#-----------------------------------------------------------------------

deletepanels:=function(ref plotgui,frompanels) {
fp:=sort(frompanels)
nd:=0
for (i in fp) {
    for (j in seq(i-nd,maxnpanels-1))
        plotgui.whichpanel[j]:=plotgui.whichpanel[j+1]
    nd+:=1
    plotgui.whichpanel[maxnpanels]:=[]
  }
}

#-----------------------------------------------------------------------
#Name: movepanels
#-----------------------------------------------------------------------
#Type: Function
#Doc: Move selected panels in plotpanel to another position
#Ret: T/F
#Par: plotgui 				- Record describing the GUI
#Par: frompanels 			- vector containing the ref numbers of panels to 
#                               be moved
#Par: topanel 			    - position where to move the panel
#Par: insert = F		    - insert panels or add them to the target 
#                               panel
#Par: moveall = F		    - move all graphs within a panel or just the
#                             top one
#Ref: plotgui, plotpanel, upcallback
#-----------------------------------------------------------------------

movepanels:=function(ref plotgui,frompanels,topanel,insert=F,moveall=F) {
nins:=len(frompanels);
delp:=[];
if (insert) {
    for (j in seq(maxnpanels-nins,topanel)) { 
        plotgui.whichpanel[j+nins]:=plotgui.whichpanel[j]}
    for (j in seq(nins)) {plotgui.whichpanel[topanel+j-1]:=[]}
  }
for (i in seq(nins)) {
  top:=topanel; if (insert) {top:=top+i-1;}
  from:=frompanels[i]; if (insert && from>=topanel) from:=from+nins
  if (moveall) {
    plotgui.whichpanel[top]:=
       [plotgui.whichpanel[top],plotgui.whichpanel[from]];
  } else {
    plotgui.whichpanel[top]:=
       [plotgui.whichpanel[top],plotgui.whichpanel[from][len(plotgui.whichpanel[from])]];
    }
  if (len(plotgui.whichpanel[from])<=1 || moveall) delp:=[delp,[from]];
  else plotgui.whichpanel[from]:= plotgui.whichpanel[from][seq(len(plotgui.whichpanel[from])-1)];
  }         
if (len(delp)) deletepanels(plotgui,delp)
}

#-----------------------------------------------------------------------
#Name: nselected
#-----------------------------------------------------------------------
#Type: Function
#Doc: Counts the number of selected files in a data record.

#Ret: integer number of selected files
#Par: plotgui =				- Record describing the GUI
#Par: rec =	  			- Data record
#Ref: plotguif, readfiles_gui
#-----------------------------------------------------------------------

nselected:=function(ref plotgui, ref rec){
			n:=0;
			for (i in seq(rec.len)) n+:=1;
			return n
}

#-----------------------------------------------------------------------
#Name: SetCorespAxis
#-----------------------------------------------------------------------
#Type: Function
#Doc: Set the corresponding Axis to the default if appropiate
#Ret: T/F
#Par: plotgui =				- Record describing the GUI
#Par: rec =				- Record describing the dataset
#Par: ncoord =				- Number of changed coordinate
#Par: newval =				- new value of coordinate
#-----------------------------------------------------------------------

SetCorespAxis := function(ref plotgui, ref rec, ncoord, newval){
	index := plotgui.startpanel;
	ci:=rec.get('Coordinfo',index);
	if ( ci[plotgui.Coordselect[ncoord]].Domain != ci[rec.Coord[newval]].Domain) {
	  ddef := rec.get('DomDefault',index);
	  if ( (ncoord%2) == 1) {
	    if (plotgui.Coordselect[ncoord] !=
			ddef[spaste(ci[plotgui.Coordselect[ncoord]].Domain,'x')] ) {
	      plotgui.Coordselect[ncoord+1] := ddef[spaste(ci[rec.Coord[newval]].Domain,'y')];
	      plotgui.Unitsselect[ncoord+1] := ci[plotgui.Coordselect[ncoord+1]].Units;
	    }
	  } else {
	    plotgui.Coordselect[ncoord-1] := ddef[spaste(ci[rec.Coord[newval]].Domain,'x')];
	    plotgui.Unitsselect[ncoord-1] := ci[plotgui.Coordselect[ncoord-1]].Units;
	  };
	};
};

#-----------------------------------------------------------------------
#Name: ChooseAxis
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Produces a menu button for selecting coordinate axes
#Ret:  T/F
#Par:  plotgui =	- Record describing the GUI
#Ref:  plotguif, plotpanels
#-----------------------------------------------------------------------

ChooseAxis:=function(ref plotgui, ref fr, ref rec,label="x",ncoord=1,defcoord=1) {
	plotgui.Coordmenu[ncoord]:=button(fr,spaste(label,'-Axis...'),type='menu', width=10, borderwidth=1, fill='x');
	plotgui.Coord[ncoord]:=[=]
	for (i in seq(len(rec.Coord))) {
	  plotgui.Coord[ncoord][i]:= button(plotgui.Coordmenu[ncoord],rec.Coord[i],
				       type='radio',value=i, borderwidth=1, fill='x');
	  whenever plotgui.Coord[ncoord][i]->press do {
	    SetCorespAxis(plotgui, rec, ncoord, $value);
	    plotgui.Coordselect[ncoord]:=rec.Coord[$value];
	    plotgui.Unitsselect[ncoord]:=rec.Units[$value];
	    plotpanels(rec,plotgui)
          }
	}
	plotgui.Coord[ncoord][defcoord]->state(T);
	plotgui.Coordselect[ncoord]:=rec.Coord[defcoord];
	plotgui.Unitsselect[ncoord]:=rec.Units[defcoord];

#Select Unit Scaling factors: from nano to Tera
	plotgui.Unitsscale_menu[ncoord]:=button(fr,spaste(label,'-Scale...'),type='menu', width=10, borderwidth=1, fill='x')
  	plotgui.Unitsscale[ncoord]:=[=]
	for (u in 1:len(globalconst.UnitPrefixName)) {
  	  plotgui.Unitsscale[ncoord][u]:=button(plotgui.Unitsscale_menu[ncoord],
          text=globalconst.UnitPrefixName[u],type='radio',value=u, borderwidth=1, fill='x');
	  whenever plotgui.Unitsscale[ncoord][u]->press do {
	      plotgui.UnitsscaleFactor[ncoord]:=globalconst.UnitPrefixFactor[$value];
	      plotgui.UnitsscaleSymbol[ncoord]:=globalconst.UnitPrefixSymbol[$value];
	      plotpanels(rec,plotgui)
	    }
	}
      plotgui.UnitsscaleFactor[ncoord]:=globalconst.UnitPrefixFactor[1];
      plotgui.UnitsscaleSymbol[ncoord]:=globalconst.UnitPrefixSymbol[1];
}


#-----------------------------------------------------------------------
#Name: SelectAxis
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Selects Axis variable and scale for plotpanel.
#Par:  plotgui                    - Record describing the GUI
#Par:  rec                        - data record
#Par:  naxis=1                    - which axis to change (1=X, 2=Y, 3=X2, 4=Y2)
#Par:  axis='Time'                - Name of variable to select and display
#Par:  scale='none'               - normalize to which scale, can be:
#                                   'none','nano','micro','milli','Kilo','Mega','Giga','Tera'
#-----------------------------------------------------------------------
SelectAxis:=function(ref plotgui, ref rec, naxis=1,axis='Time',scale='none'){
    nc:=is_where(axis,rec.Coord)[1];
    ns:=is_where(scale,globalconst.UnitPrefixName)[1];
    plotgui.Coord[naxis][nc]->state(T)
    plotgui.Unitsscale[naxis][ns]->state(T)
    plotgui.Coordselect[naxis]:=rec.Coord[nc];
	plotgui.Unitsselect[naxis]:=rec.Units[nc];
    plotgui.UnitsscaleFactor[naxis]:=globalconst.UnitPrefixFactor[ns];
    plotgui.UnitsscaleSymbol[naxis]:=globalconst.UnitPrefixSymbol[ns];
}

#-----------------------------------------------------------------------
#Name: openPrintDialog
#-----------------------------------------------------------------------
#Type: Function
#Doc:  Produces a dialog with settings for printing.
#Par:  plotgui                    - Record describing the GUI
#-----------------------------------------------------------------------
openPrintDialog := function(ref plotgui) {
  local filename := "";
  local command  := "";
  local printers := shell('cat "/etc/printcap" | grep -v "\\s*#" | sed "s/\\(|\\|:\\).*$//"');

  tk_hold();
  w := frame(title='Print - LOPES-tools');
    o1 := button(w, 'Print to command', type='radio', fill='x', anchor='w', relief='flat', borderwidth=1, hlthickness=0);
    o2 := button(w, 'Print to file', type='radio', fill='x', anchor='w', relief='flat', borderwidth=1, hlthickness=0);
    f1 := frame(w, expand='x', side='left');
      l1 := label(f1, 'Command:');
      s1 := frame(f1, width=10, height=1, expand='x');
      i1 := entry(f1, width=25, fill='x', borderwidth=1, background='white');
    s2 := frame(w, expand='x', relief='sunken', height=2);
    o3 := button(w, 'Color print', type='check', fill='x', anchor='w', relief='flat', borderwidth=1, hlthickness=0);
    o4 := button(w, 'Landscape orientation', type='check', fill='x', anchor='w', relief='flat', borderwidth=1, hlthickness=0);
    s3 := frame(w, expand='x', relief='sunken', height=2);
    f2 := frame(w, expand='x', side='left');
      b1 := button(f2, 'Dismiss', width=10, fill='x', background='orange',    borderwidth=1);
      b2 := button(f2, 'Print',   width=10, fill='x', background='limegreen', borderwidth=1);
  tk_release();

  o1->state(T);
  o3->state(T);
  o4->state(T);
  i1->insert(spaste('lpr -P',printers[1]));
  whenever o1->press do {
    filename := i1->get();
    i1->delete('start','end');
    l1->text('Command:');
    if (!len(command)) i1->insert(spaste('lpr -P',printers[1]));
    else  i1->insert(command);
  }
  whenever o2->press do {
    command := i1->get();
    i1->delete('start','end');
    l1->text('Filename:');
    if (!len(filename)) i1->insert(spaste(globalconst.HomeDir,globalconst.DirSeparator,'lopestools.ps'));
    else  i1->insert(filename);
  }
  whenever b1->press do {
    w := F;
  }
  whenever b2->press do {
    w->disable();
    w->cursor('watch');
    if (o1->state()) {
      command  := i1->get();
      filename := spaste('/tmp/lopestools.',environ.USER,'.ps');
      plotgui.pg.postscript(filename, color=o3->state(), landscape=o4->state());
      shell(spaste(command,' ',filename));
      shell(spaste('rm ',filename));
    } else {
      filename := i1->get();
      plotgui.pg.postscript(filename, color=o3->state(), landscape=o4->state());
    }
    w->cursor('left_ptr');
    w->enable();
    w := F;
  }
}

#-----------------------------------------------------------------------
#Name: plotguif
#-----------------------------------------------------------------------
#Type: Function
#Doc: Produces a gui for plotting data. Data record is passed to the
#     function which can contain an arbitrary number of files and an
#     arbitrary number of coordinate axes. Filenames and coordinate
#     axes can be selected from the gui. The number of panels to be
#     displayed simultaneously can be selected by the user.
#Ret: T/F
#Par: rec =	  			- Data record
#Par: plotgui =				- Record describing the GUI
#Ref: readfilesNOEMI, selectfiles, readfiles_gui, plotpanels


#-----------------------------------------------------------------------

#plotguif := function(ref rec,ref plotgui) {
plotguif:=function(ref rec, title='GUI') {

	#global plotgui
	#global proj
	global globalpar;
	plotgui := [=];
    plotgui.rec := ref rec;
    rec.plotgui := ref plotgui

	datawatch:=datawatchclient();
	memwatch:=memwatchclient();
	plotgui.waiting := F;
	plotgui.rightmainframeOldCursor := 'left_ptr';

    plotgui.timerfunc:=function(...){wider plotgui; global globalpar; 
      if (globalpar.timermute) return;
      memwatch->update()
    };

    globalpar.plotpanelID+:=1;
    title2:=spaste(title,'[',as_string(globalpar.plotpanelID),']');

	plotgui.keypressed:=function(...) keypressed(plotgui,...)
	plotgui.upcallback:=function(...) upcallback(plotgui,...)
	plotgui.downcallback:=function(...) downcallback(plotgui,...)
	plotgui.motioncallback:=function(...) motioncallback(plotgui,...)
    plotgui.moveall:=F
    plotgui.peaks_show:=F
    plotgui.crosscursoron:=F
    onUseMove(plotgui, gui=F);
    plotgui.selectglobal:=T
    plotgui.selectinside:=T
    plotgui.selectx:=T
    plotgui.selecty:=F
    plotgui.selected_region_x1:=0; plotgui.selected_region_x2:=0;
    plotgui.selected_region_y1:=0; plotgui.selected_region_y2:=0;
    plotgui.select_x1:=0; plotgui.select_x2:=0;
    plotgui.select_y1:=0; plotgui.select_y2:=0;
    plotgui.select_y1old:=0; plotgui.select_y2old:=0;
    plotgui.select_panel:=1;
    plotgui.panelselected:=[]
#sets the frame for current file
    plotgui.AZEL:=dirtoazel(rec)
    plotgui.xy:=rec.get('XY');
    plotgui.distance:=rec.get('Distance');
    plotgui.refant:=1
    plotgui.result:=[=]


	plotgui.crosscorr:=function() {
	      wider rec; wider plotgui;
          if (!plotgui.selectx || !plotgui.selactive) {print 'No range selected!'; return};
          if (plotgui.selectglobal) npan:=seq(rec.len)
          else npan:=plotgui.panel[plotgui.select_panel]
          print 'x1/2:',plotgui.select_xpos[1],plotgui.select_xpos[2],'Index:',npan,'Refant:',plotgui.refant
          calibratedelayscr(field=plotgui.Coordselect[2],index=npan,x1=plotgui.select_xpos[1],x2=plotgui.select_xpos[2],refant=plotgui.refant)
     }

	plotgui.integrate:=function() {
	      wider rec; wider plotgui;
          if (!plotgui.selectx || !plotgui.selactive) return
          if (!plotgui.selectinside) {print 'Select an "Inside" region!'; return}
          if (plotgui.selectglobal) npan:=seq(min(rec.len,plotgui.nplotted))
          else npan:=plotgui.panel[plotgui.select_panel]
          print IntegrateData(plotgui.yary,plotgui.result,index=npan,x1=plotgui.select_xpos[1],x2=plotgui.select_xpos[2],ncall=plotgui.resultlen)
     }

	plotgui.flag:=function() {
	      wider rec; wider plotgui;
          if (plotgui.selectglobal) npan:=seq(rec.len)
          else npan:=plotgui.panel[plotgui.select_panel]
          for (i in npan) {
            if (plotgui.selectx) flagdatarange(rec,plotgui.Coordselect[1],i,plotgui.flagx,inside=plotgui.selectinside)
            if (plotgui.selecty) flagdatarange(rec,plotgui.Coordselect[2],i,plotgui.flagy,inside=plotgui.selectinside)
          }
	      plotgui.replot()
     }

	plotgui.unflag:=function() {
	      wider rec; wider plotgui;
          if (plotgui.selectglobal) npan:=seq(rec.len)
          else npan:=plotgui.panel[plotgui.select_panel]
          for (i in npan) {unflagdata(rec,plotgui.Coordselect[1],i)}
	      plotgui.replot()
     }

	plotgui.zoom:=function(...) {
	       wider rec;  wider plotgui
	       zoompanel(plotgui);
	       plotpanels(rec,plotgui)}
	plotgui.zoomout:=function(...) {
	       wider rec; wider plotgui
	       zoomoutpanel(plotgui);
	       plotpanels(rec,plotgui)}


	plotgui.pleasewait:=function(message=T) {
	  wider plotgui;
	  if (! plotgui.waiting) {
	    plotgui.waiting := T;
	    plotgui.rightmainframeOldCursor := plotgui.rightmainframe->cursor();
	      if (plotgui.rightmainframeOldCursor == '') {
	        plotgui.rightmainframeOldCursor := 'left_ptr';
	      }
	    plotgui.rightmainframe->cursor('watch');
	    plotgui.frame->cursor('watch');
	    if (message && plotgui.messlabel->text() == 'Ready.') {
	      plotgui.messlabel->text('Please wait...');
	    }
	  }
	}
	plotgui.endwait:=function(message=T) {
	  wider plotgui;
	  if (plotgui.waiting) {
	    plotgui.waiting := F;
	    plotgui.frame->cursor('left_ptr');
	    plotgui.rightmainframe->cursor(plotgui.rightmainframeOldCursor);
	    if (message) {
	      plotgui.messlabel->text('Ready.');
	    }
	  }
	}

#   Array that determines the ordering of plotpanels in the plotpanel window
    plotgui.fitresults:=[=];
    plotgui.fityary:=[=];
    plotgui.fitxary:=[=];
    plotgui.plotfit:=array(F,maxnpanels);
    plotgui.whichpanel:=[=];
    for (i in seq(maxnpanels)) {
     plotgui.fitxary[i]:=[=];
     plotgui.fityary[i]:=[=];
     plotgui.fitresults[i]:=[=];
     plotgui.whichpanel[i]:=[i];
    }

    plotgui.refreshgui:=function(...) {
      wider rec; wider plotgui;
      # OBSERVATORY #
      plotgui.obsent -> text(rec.head('Observatory'));
      # FREQUENCY #
      FrequencyUnit := as_double(rec.head('FrequencyUnit'));
      plotgui.freqstartent->delete('start','end');
      plotgui.freqstartent->insert(as_string(rec.getmeta('FrequencyLow')/FrequencyUnit));
      plotgui.freqstopent->delete('start','end');
      plotgui.freqstopent->insert(as_string(rec.getmeta('FrequencyHigh')/FrequencyUnit));
      # REST #
      plotgui.presyncent->delete('start','end');
      plotgui.presyncent->insert(as_string(rec.head('presync')));
      # BLOCKINFOS #
      plotgui.blockoffsetent->delete('start','end');
      plotgui.blockoffsetent->insert(as_string(rec.getmeta('ZeroOffset')));
      plotgui.filesizeent->delete('start','end');
      plotgui.filesizeent->insert(as_string(rec.getmeta('Filesize')));
      plotgui.blocksizeent->delete('start','end');
      plotgui.blocksizeent->insert(as_string(rec.getmeta('Blocksize')));
      plotgui.blocknumscalevalue:=rec.getmeta('Blocknum',rec.defant());
      plotgui.blocknumscale->start(1);
      plotgui.blocknumscale->value(plotgui.blocknumscalevalue);
      plotgui.blocknumscale->end(as_integer(rec.getmeta('FileBlocksize')/rec.getmeta('Blocksize')));
      plotgui.filesizeentvalue:=rec.getmeta('Filesize');
      plotgui.blocksizeentvalue:=rec.getmeta('Blocksize');
      plotgui.blockoffsetentvalue:=rec.getmeta('ZeroOffset');
      plotgui.AZEL:=rec.get('AZEL');
      plotgui.xy:=rec.get('XY');
      plotgui.distance:=rec.get('Distance');
      plotgui.azent->delete('start','end');
      plotgui.azent->insert(as_string(plotgui.AZEL[1]));
      plotgui.elent->delete('start','end');
      plotgui.elent->insert(as_string(plotgui.AZEL[2]));
      plotgui.offsetxent->delete('start','end');
      plotgui.offsetyent->delete('start','end');
      plotgui.distanceent->delete('start','end');
      plotgui.offsetxent->insert(as_string(plotgui.xy[1]));
      plotgui.offsetyent->insert(as_string(plotgui.xy[2]));
      plotgui.distanceent->insert(as_string(plotgui.distance));
      plotgui.distance:=rec.get('Distance');
      plotgui.dateent->delete('start','end');
      plotgui.dateent->insert(as_string(rec.head('Date')));
      memwatch -> update();
}

	plotgui.replot:=function(allwindows=F,...) {
	  wider rec; wider plotgui;
	  plotgui.pleasewait();
	  #plotgui.refreshgui();
	  plotpanels(rec,plotgui);
	  if (allwindows) {
	  # Also replotting the antenna layout if opened
	    if (has_field(plotgui,'plotantpg') && has_field(plotgui.plotantpg,'pg') && len(plotgui.plotantpg.pg)>1) {
 	    plotantf(plotgui.plotantpg,rec,azel=plotgui.AZEL*degree, xy=plotgui.xy, observatory=rec.head('Observatory'))};
	  }
	  plotgui.endwait();
	}

    plotgui.fitgauss:=function(x1=-1,x2=-1,panel=1){
      wider plotgui,rec
      if (x1==x2) {
        if (plotgui.selactive) {
          xn1:=rec.getxn(plotgui.selected_region_x1*plotgui.UnitsscaleFactor[1],plotgui.Coordselect[1]);
          xn2:=rec.getxn(plotgui.selected_region_x2*plotgui.UnitsscaleFactor[1],plotgui.Coordselect[1]);
        } else {
          xn1:=1;
          xn2:=rec.getlen(plotgui.Coordselect[2]);
        }
      } else {
        xn1:=rec.getxn(x1,plotgui.Coordselect[2]);
        xn2:=rec.getxn(x2,plotgui.Coordselect[2]);
      };
      if (plotgui.selectglobal) {npan:=seq(plotgui.nplotted)}
      else {npan:=plotgui.select_abspanel-plotgui.startpanel+1};
      if (!plotgui.selactive) npan:=panels;
      fitgaussplot(plotgui,xn1,xn2,panels=npan,nfit=1,width=4,maxiter=30)
      plotgui.replot()
    }

	plotgui.setpanel:=function(...) {
	       wider rec;  wider plotgui
	       setpanel(plotgui,...);
	       plotpanels(rec,plotgui)}

	plotgui.setnpanels:=function(...) {
	       wider rec;  wider plotgui
	       setnpanels(plotgui,...);
	       plotpanels(rec,plotgui)}

	plotgui.nselected:=function(...) {
	       wider rec;  wider plotgui
	       nselected(plotgui,rec)}

	plotgui.getxn:=function(x,npan) {
	       wider rec;  wider plotgui
	       l:=len(plotgui.xary[npan])
	       dx:=(plotgui.xary[npan][l]-plotgui.xary[npan][1])/(l-1.)
	       xnmin:=max(min(l-1,as_integer((x-plotgui.xary[npan][1])/dx+1)),1)
           while (xnmin>1 && plotgui.xary[npan][xnmin]>x) xnmin-:=1;
           while (xnmin<(l-1) && plotgui.xary[npan][xnmin+1]<=x) xnmin+:=1;
	       return xnmin;
	       }

	plotgui.gety:=function(x,npan) {
	       wider rec;  wider plotgui
           xnmin:=plotgui.getxn(x,npan);
	       xnmax:=xnmin+1;
           if (plotgui.xary[npan][xnmin]==x) y:=rec.getval(plotgui.Coordselect[2],plotgui.panel[npan],xnmin)
	       else {
	           x1:=plotgui.xary[npan][xnmin]; x2:=plotgui.xary[npan][xnmax];
	           y1:=rec.getval(plotgui.Coordselect[2],plotgui.panel[npan],xnmin);
	           y2:=rec.getval(plotgui.Coordselect[2],plotgui.panel[npan],xnmax);
		   y:=y1+(y2-y1)*(x-x1)/(x2-x1);
		   }
	       return y;
	       }

	plotgui.getx:=function(x,npan) {
	       wider rec;  wider plotgui
           xnmin:=floor(x)
	       xnmax:=ceil(x);
           if (plotgui.xary[npan][xnmin]==x) x0:=rec.getval(plotgui.Coordselect[1],plotgui.panel[npan],xnmin)
	       else {
	           x1:=rec.getval(plotgui.Coordselect[1],plotgui.panel[npan],xnmin);
	           x2:=rec.getval(plotgui.Coordselect[1],plotgui.panel[npan],xnmax);
		   x0:=x1+(x2-x1)*(x-x1)
		   }
	       return x0;
	       }

	plotgui.invfx:=function(x) {
      wider plotgui
      xinv:=x
      logx:=plotgui.logx->state();
      if (logx) {xinv:=10^x};
      xinv:=xinv*plotgui.UnitsscaleFactor[1];
      return (xinv)
    }

	plotgui.invfy:=function(y) {
      wider plotgui
      yinv:=y
      logy:=plotgui.logy->state();
      squared:=plotgui.squared->state();
      if (logy) {yinv:=10^y};
      if (squared) {yinv:=sqrt(y)};
      yinv:=yinv*plotgui.UnitsscaleFactor[2];
      return (yinv)
    }

plotgui.refresh:=function(erase=F){wider plotgui; refreshpanels(plotgui,selection=plotgui.selactive,erase=erase)}

plotgui.highlightselection:=function(){
  wider plotgui;
  if (plotgui.selactive) {
    if (plotgui.selectglobal) npan:=seq(plotgui.nplotted)
    else npan:=plotgui.select_abspanel-plotgui.startpanel+1;
    npan:=npan[npan>0];
    for (i in npan) {
      highlightselection(plotgui,i,inside=plotgui.selectinside,selx=plotgui.selectx,sely=plotgui.selecty)
    }
  }
}

plotgui.setmeanvstate := function() {
  wider plotgui;
  if (plotgui.meanv->state()) {
    plotgui.meanv_npix->disabled(F);
  } else {
    plotgui.meanv_npix->disabled(T);
  }
}

plotgui.setminvstate := function() {
  wider plotgui;
  if (plotgui.minv->state()) {
    plotgui.minv_npix->disabled(F);
  } else {
    plotgui.minv_npix->disabled(T);
  }
}

plotgui.setf2state := function() {
  wider plotgui;
#  if (plotgui.showfreqdomain->state()) {
#    while (plotgui.autoflag->disabled()) plotgui.f2->enable();
#  } else {
# I took this out (HF)
#    while (!plotgui.autoflag->disabled()) plotgui.f2->disable();
    while (plotgui.autoflag->disabled()) plotgui.f2->enable();
#  }
}
   plotgui.view:=function(n=1,xrange=-1){
    wider plotgui,rec
    if (n==1) {
      plotgui.nlines_entry->delete('start','end');
      plotgui.nlines_entry->insert('1');
      plotgui.nlines:=1;
      plotgui.setnpanels(1,1,1);
      plotgui.fixedscale->state(T);
      plotgui.fixedy->state(F);
      plotgui.mean0->state(F);
      plotgui.absy->state(F);
      if (len(xrange)!=2) {
        plotgui.x1->delete('start','end');
        plotgui.x1->insert('-2.');
        plotgui.x2->delete('start','end');
        plotgui.x2->insert('-1.6');
      } else {
        plotgui.x1->delete('start','end');
        plotgui.x1->insert(as_string(xrange[1]/plotgui.UnitsscaleFactor[1]));
        plotgui.x2->delete('start','end');
        plotgui.x2->insert(as_string(xrange[2]/plotgui.UnitsscaleFactor[1]));
      }
      plotgui.whichpanel:=[=];
      for (i in seq(maxnpanels)) plotgui.whichpanel[i]:=[];
      plotgui.whichpanel[1]:=[seq(maxnpanels)];
	   SetCorespAxis(plotgui, rec, 1, 1);
       SelectAxis(plotgui,rec,naxis=1,axis='Time',scale='micro')
       SelectAxis(plotgui,rec,naxis=2,axis='FieldStrength',scale='micro')
#uncheck log button
#       plotgui.showtimedomain->state(T);
       plotgui.showdomainlab->text(plotgui.domainnames[2]);
       plotgui.setf2state();
       plotgui.logy->state(F);
       plotgui.squared->state(F);
       plotgui.mean0->state(F);
       plotgui.meanv->state(F);
       plotgui.setmeanvstate();
       plotgui.minv->state(F);
   	   plotgui.setminvstate();
	   plotgui.replot();
   }
    if (n==2) {
      plotgui.setnpanels(1,1,1);
      plotgui.fixedscale->state(T);
      plotgui.mean0->state(T);
      plotgui.fixedy->state(F);
      if (len(xrange)!=2) {
        plotgui.x1->delete('start','end');
        plotgui.x1->insert('-2.');
        plotgui.x2->delete('start','end');
        plotgui.x2->insert('-1.6');
      } else {
        plotgui.x1->delete('start','end');
        plotgui.x1->insert(as_string(xrange[1]/plotgui.UnitsscaleFactor[1]));
        plotgui.x2->delete('start','end');
        plotgui.x2->insert(as_string(xrange[2]/plotgui.UnitsscaleFactor[1]));
      }
#      plotgui.whichpanel:=[=];
#      for (i in seq(maxnpanels)) plotgui.whichpanel[i]:=[i];
	   SetCorespAxis(plotgui, rec, 1, 1);
       SelectAxis(plotgui,rec,naxis=1,axis='Time',scale='micro')
       SelectAxis(plotgui,rec,naxis=2,axis='X-Beam',scale='micro')
       SelectAxis(plotgui,rec,naxis=3,axis='Time',scale='micro')
       SelectAxis(plotgui,rec,naxis=4,axis='P-Beam',scale='micro')
       plotgui.nlines_entry->delete('start','end');
       plotgui.nlines_entry->insert('2');
       plotgui.nlines:=2;
#uncheck log button
#       plotgui.showtimedomain->state(T);
       plotgui.showdomainlab->text(plotgui.domainnames[2]);
       plotgui.setf2state();
       plotgui.logy->state(F);
       plotgui.squared->state(F);
       plotgui.meanv_npix->delete('start','end');
       plotgui.meanv_npix->insert('3');
       plotgui.meanv->state(T);
       plotgui.absy->state(F);
       plotgui.setmeanvstate();
       plotgui.minv->state(F);
       plotgui.setminvstate();
	   plotgui.replot();
   }
}


   plotgui.tofrequencydomain := function(...) {
#Toggles frequency domain mode for the plot panel
       wider rec;  wider plotgui;
       SelectAxis(plotgui,rec,naxis=1,axis='Frequency',scale='Mega')
       SelectAxis(plotgui,rec,naxis=2,axis='Power',scale='none')
#check log button
       plotgui.logy->state(T);
#       plotgui.showfreqdomain->state(T);
       plotgui.showdomainlab->text(plotgui.domainnames[1]);
       plotgui.setf2state();
       plotgui.squared->state(F);
       plotgui.mean0->state(F);
       plotgui.meanv->state(F);
       plotgui.setmeanvstate();
       plotgui.minv->state(F);
			 plotgui.setminvstate();
       plotgui.peaks_show := F;
       plotgui.fixedscale->state(F);
	   plotgui.replot();
}

    plotgui.totimedomain := function(...) {
#Toggles time domain mode for the plot panel
       wider rec; wider plotgui;
	   SetCorespAxis(plotgui, rec, 1, 1);
       SelectAxis(plotgui,rec,naxis=1,axis='Time',scale='micro')
       SelectAxis(plotgui,rec,naxis=2,axis='f(x)',scale='none')
#uncheck log button
#       plotgui.showtimedomain->state(T);
       plotgui.showdomainlab->text(plotgui.domainnames[2]);
       plotgui.setf2state();
       plotgui.logy->state(F);
       plotgui.squared->state(F);
       plotgui.mean0->state(F);
       plotgui.meanv->state(F);
       plotgui.setmeanvstate();
       plotgui.minv->state(F);
			 plotgui.setminvstate();
	   plotgui.replot()
}

    plotgui.topower := function(...) {
#Toggles power in time domain mode for the plot panel
       wider rec; wider plotgui;
	   SetCorespAxis(plotgui, rec, 1, 1);
       SelectAxis(plotgui,rec,naxis=1,axis='Time',scale='micro')
       SelectAxis(plotgui,rec,naxis=2,axis='f(x)',scale='none')
#uncheck log button
       plotgui.showpowdomain->state(T);
       plotgui.showdomainlab->text(plotgui.domainnames[3]);
       plotgui.setf2state();
       plotgui.logy->state(F);
       plotgui.squared->state(T);
       plotgui.mean0->state(F);
       plotgui.meanv->state(T);
			 plotgui.setmeanvstate();
       plotgui.minv->state(T);
       plotgui.setminvstate();
       plotgui.fixedy->state(F);
	   plotpanels(rec,plotgui)
}

  plotgui.foldOrUnfoldPanel := function(title, flip=F, state=F) {
    wider plotgui;
    if (flip) state := !plotgui.showtools[title];
    if (state) {
      plotgui.showtools[title]  := T;
      plotgui.panelclbut[title]->bitmap(spaste(globalconst.ImageDir,'uparrows.xbm'));
      plotgui.toolpanel[title]->map();
      plotgui.appanel[title]->map();
    } else {
      plotgui.showtools[title]  := F;
      plotgui.panelclbut[title]->bitmap(spaste(globalconst.ImageDir,'downarrows.xbm'));
      plotgui.toolpanel[title]->unmap();
      plotgui.appanel[title]->unmap();
    }
  }

  plotgui.newToolPanel := function(ref parent, title='', show=T, apbut=F) {
    wider plotgui;

    if (!has_field(plotgui,'toolpanel')) {
      plotgui.toolpanelp := [=];
      plotgui.panelhead  := [=];
      plotgui.panellabel := [=];
      plotgui.panelapbut := [=];
      plotgui.panelclbut := [=];
      plotgui.headspace  := [=];
      plotgui.toolpanel  := [=];
      plotgui.showtools  := [=];
    }
    if (len(title)==0) title := as_string(len(toolpanel)+1);

    plotgui.toolpanelp[title] := frame(plotgui.leftmainframe, side='top', relief='raised', expand='x', borderwidth=1);
    plotgui.panelhead[title]  := frame(plotgui.toolpanelp[title], side='left', relief='flat', background='grey70',
                                    expand='x', borderwidth=0);
    plotgui.panellabel[title] := label(plotgui.panelhead[title],  title, background='grey70', anchor='w', fill='x', padx=4, pady=0);
    plotgui.panelclbut[title] := button(plotgui.panelhead[title], 'Fold', bitmap=spaste(globalconst.ImageDir,'uparrows.xbm'),
                                    background='grey70', foreground='grey55', borderwidth=0, hlthickness=0)
    plotgui.headspace[title]  := frame(plotgui.panelhead[title], relief='flat', expand='y', background='grey70', width=1, height=1,
                                    borderwidth=0);
    plotgui.toolpanel[title]  := frame(plotgui.toolpanelp[title], side='top', relief='flat', expand='x', borderwidth=1);
    if (apbut) {
      plotgui.appanel[title]    := frame(plotgui.toolpanelp[title], side='right', relief='flat', expand='x', borderwidth=2);
      plotgui.panelapbut[title] := button(plotgui.appanel[title], 'Apply', bitmap=spaste(globalconst.ImageDir,'apply.xbm'),
                                      borderwidth=0, hlthickness=0)
      popuphelp(plotgui.panelapbut[title], 'Apply changes');
      whenever plotgui.panelapbut[title]->press do {
        plotgui.replot();
        plotgui.refreshgui();
      }
    } else {
      plotgui.appanel[title]    := frame(plotgui.toolpanelp[title], side='right', relief='flat', expand='x', borderwidth=0, height=0);
    }
    popuphelp(plotgui.panelclbut[title], spaste('Fold or unfold panel "',title,'"'));

    plotgui.foldOrUnfoldPanel(title, state=show);
    whenever plotgui.panelclbut[title]->press do {
      wider plotgui;
      plotgui.foldOrUnfoldPanel(title, flip=T);
    }
    return title;
  }

  tk_hold();
	plotgui.frame := frame(title=spaste(title2,' - LOPES-Tools'));

# Initialise menu structure
	plotgui.fm := frame(plotgui.frame,side='left', expand='x',borderwidth=0);
	plotgui.fmfilebut  := button(plotgui.fm, 'File',  type='menu', relief='flat', borderwidth=0);
	plotgui.fmfileopenbut     := button(plotgui.fmfilebut, 'Open', type='menu');
	plotgui.fmfileopenevbut       := button(plotgui.fmfileopenbut, 'Open event file...');
	plotgui.fmfileopenprbut2      := button(plotgui.fmfileopenbut, 'Open project file...');
	plotgui.fmfileopenprbut       := button(plotgui.fmfileopenbut, 'Open project file selectively...');
	plotgui.fmloadynspeclopesbut  := button(plotgui.fmfileopenbut, 'Load LOPES/LORUN Dynamic Spectrum...');
	plotgui.fmloadynspecitsbut    := button(plotgui.fmfileopenbut, 'Load ITS Dynamic Spectrum...');
	plotgui.fmloadimagebut    := button(plotgui.fmfileopenbut, 'Load AIPS++ image tool...');
	plotgui.fmcrguisbut       := button(plotgui.fmfilebut, 'CR-Event GUIs...');
	plotgui.fmfilesubmitits   := button(plotgui.fmfilebut, 'Submit ITS Experiment...');
	plotgui.fmfiletrigits     := button(plotgui.fmfilebut, 'Submit ITS Trigger...');
	plotgui.fmfileopenitbut   := button(plotgui.fmfilebut, 'Get ITS files from Aviary...');
#	plotgui.fmfilereadtimbut  := button(plotgui.fmfilebut, 'Read TIM board', relief='flat', borderwidth=0);
#	plotgui.fmfileconftimbut  := button(plotgui.fmfilebut, 'Configure TIM board...', relief='flat', borderwidth=0);
	plotgui.fmfileprintbut    := button(plotgui.fmfilebut, 'Print...', relief='flat', borderwidth=0);
	plotgui.fmfiledismissbut  := button(plotgui.fmfilebut, 'Dismiss window', background='orange');
	plotgui.fmfiledonebut     := button(plotgui.fmfilebut, 'Exit glish', background='red', foreground='white');

	plotgui.fmviewbut  := button(plotgui.fm, 'View',  type='menu', relief='flat', borderwidth=0);
	plotgui.fmviewshowstbarbut:= button(plotgui.fmviewbut, 'Show status bar', type='check');
	plotgui.fmviewshowtlbarbut:= button(plotgui.fmviewbut, 'Show toolbar', type='check');
	plotgui.fmviewshowdockbut := button(plotgui.fmviewbut, 'Show dockers', type='check');
#	plotgui.fmviewdockersbut  := button(plotgui.fmviewbut, 'Dockers', type='menu');

	plotgui.fmdatabut  := button(plotgui.fm, 'Data set', type='menu', relief='flat', borderwidth=0);
	plotgui.fmdataplotantbut  := button(plotgui.fmdatabut, 'Antenna layout...');
	plotgui.fmdataselectbut   := button(plotgui.fmdatabut, 'Antenna selection...');
	plotgui.fmdataheditbut    := button(plotgui.fmdatabut, 'Edit headers...');
	plotgui.fmdatahsavebut    := button(plotgui.fmdatabut, 'Save headers...');
	plotgui.fmdatacaltablebut := button(plotgui.fmdatabut, 'Browse CalTable...');

	plotgui.fmplotbut  := button(plotgui.fm, 'Plot',  type='menu', relief='flat', borderwidth=0);
	plotgui.logx              := button(plotgui.fmplotbut, 'Logarithmic X axis', type='check');
	plotgui.logy              := button(plotgui.fmplotbut, 'Logarithmic Y axis', type='check');
	plotgui.absy              := button(plotgui.fmplotbut, 'Absolute Value', type='check');
	plotgui.squared           := button(plotgui.fmplotbut, 'Squared (power)', type='check');
	plotgui.squaredsign       := button(plotgui.fmplotbut, 'Squared times sign', type='check');
	plotgui.mean0             := button(plotgui.fmplotbut, 'Zero Offset Baseline', type='check');
	plotgui.normnoise         := button(plotgui.fmplotbut, 'Normalize noise to unity', type='check');
	plotgui.subtractfits          := button(plotgui.fmplotbut, 'Subtract Gaussian fits', type='check');
	plotgui.plotfits          := button(plotgui.fmplotbut, 'Plot Gaussian fits', type='check');
	plotgui.plotfits  ->state(T)
	plotgui.fmplothistobut    := button(plotgui.fmplotbut, 'Histogram', type='check');
	plotgui.histo:=plotgui.fmplothistobut->state();

	plotgui.fmcalcbut  := button(plotgui.fm, 'Calculate', type='menu', relief='flat', borderwidth=0);
	plotgui.fmcalcaverbut     := button(plotgui.fmcalcbut, 'Average files');
	plotgui.fmcalcmpowbut     := button(plotgui.fmcalcbut, 'Mean power');
	plotgui.fmcalcstatbut     := button(plotgui.fmcalcbut, 'Statistics');
	plotgui.fmcalcbaselines   := button(plotgui.fmcalcbut, 'Baselines');

	plotgui.fmtoolsbut := button(plotgui.fm, 'Analyze', type='menu', relief='flat', borderwidth=0);
	plotgui.fmtoolsskymapbut  := button(plotgui.fmtoolsbut, 'Create skymap...');
	plotgui.fmtoolsdynspecbut := button(plotgui.fmtoolsbut, 'Create dynamic spectrum...');
	plotgui.fmtoolsdynspec2but:= button(plotgui.fmtoolsbut, 'Create dynamic spectrum (2)...');
	plotgui.fmtoolspeakbut    := button(plotgui.fmtoolsbut, 'Detect peaks', type='menu');
	plotgui.fmtoolspeakcurbut := button(plotgui.fmtoolspeakbut, 'Current data set...');
	plotgui.fmtoolspeakbatbut := button(plotgui.fmtoolspeakbut, 'Batch event dir...');
	plotgui.fmtoolsimagebut   := button(plotgui.fmtoolsbut, 'Analyze/Manipulate image tool...');
	plotgui.fmtoolsamapbut    := button(plotgui.fmtoolsbut, 'Annotate Skymap (J2000)...');
	plotgui.fmtoolsmapsurveybut   := button(plotgui.fmtoolsbut, 'Map 408 MHz Survey');
	plotgui.fmtoolsdecoderbut   := button(plotgui.fmtoolsbut, 'Decode TV/Radio...');
	plotgui.fmtoolscrbrowser  := button(plotgui.fmtoolsbut, 'Browse CR Events...');
	plotgui.fmtoolscrprocess  := button(plotgui.fmtoolsbut, 'Auto-process CR event');
	plotgui.fmtoolsfindcrdist := button(plotgui.fmtoolsbut, 'Find CR distance');
	plotgui.fmtoolsguipipeline:= button(plotgui.fmtoolsbut, 'GUI-pipeline for current event');

#Reload buttons
	h:=[=];

    whenever plotgui.fmcrguisbut->press do {
       print 'Loading Default Event #68 -- needs to be replaced by CR-event chooser'; 
       if (globalpar.event_loaded>0) {crguis(globalpar.event_loaded)}
       else {crguis(globalpar.DefaultCREventNum)}
    };

    whenever plotgui.fmfileopenprbut->press do
    {
      # Event callback-function for selectively loading a project/ITS file
      loadSelectively := function (dirname) {
        proj:=[=]
        h:=readprojectfiles_gui(proj,dir=dirname,logfile=globalpar.DefLogfile);
        if (len(h)>0) {
	  readfilesH(rec,h,blocksize=globalpar.BlocksizeForRead,
                     offset=globalpar.OffsetForRead);
          datawatch->checkdata();
          plotgui.refreshgui();
        }
      }

      # Start the catalog tool to select the data input source
      global dcstarted;
      dc.setselectcallback(loadSelectively);
      dc.gui(F,all,F);
      if (!dcstarted){
        dc.show(globalpar.DefDataDir,'All',T)
        dcstarted := T;
      }
    }

    whenever plotgui.fmfileopenprbut2->press do {
#      print 'Attention: I am setting globalpar.BlocksizeForRead:=2048 in plotpanel!! (to speed up ITS input)'
#      globalpar.BlocksizeForRead:=2048;
	  readprfiles();
      plotgui.refreshgui();
	}

	whenever plotgui.fmfileopenevbut->press do {
	  readevfiles();
	  datawatch->checkdata();
      plotgui.refreshgui();
	}

	whenever plotgui.fmfilesubmitits->press do {
       itsform.constructor();
    }

	whenever plotgui.fmfiletrigits->press do {
       trigger.gui();
    }

	whenever plotgui.fmfileopenitbut->press do {
      global nameDialog
      nameDialog := single_entry_dialog_box('ITS Experiment Name', getits);
#does not work
      single_entry_dialog_box.delete();
      nameDialog := F;
	  datawatch->checkdata()
      plotgui.refreshgui();
	}
	whenever plotgui.fmdataselectbut->press do {
	  selectfiles(rec);
	}

  # LOAD DYNAMIC SPECTRUM #
	whenever plotgui.fmloadynspeclopesbut->press do {
    loadynspeclopes();
	  datawatch->checkdata();
	}
	whenever plotgui.fmloadynspecitsbut->press do {
    loadynspecits();
	  datawatch->checkdata();
	}

	whenever plotgui.fmloadimagebut->press do {
	  openCatalogGUI (loadimagetool,type=['Image']);
	  datawatch->checkdata();
	}

#	whenever plotgui.fmfilereadtimbut->press do {
#	  newdatasets:=readTIMboard(rec,blocksize=globalpar.BlocksizeForRead,offset=globalpar.OffsetForRead);
#	  for (j in newdatasets) {
#            print '------------------------------------------------------------------------';
#            print 'Statistics for ',rec.head('Filename',j),':'
#	    ##!!Coordinates are hard-wired!! Need to be changed to defaults!
#	    DataStatXY(rec.get('Pos',j),rec.get('f(x)',j),'','');
#	  }
#	  datawatch->checkdata();
#	}
#	whenever plotgui.fmfileconftimbut->press do {
#	  configTIMboardGUI();
#	}
	whenever plotgui.fmfileprintbut->press do {
	  wider plotgui;
	  openPrintDialog(plotgui);
	}
	whenever plotgui.fmfiledismissbut->press do {
	  wider plotgui;
	  plotgui.pg.done();
	  val plotgui:=[=];
	}
	whenever plotgui.fmfiledonebut->press do {
	  exit;
	}
	whenever plotgui.absy->press do {
	  plotpanels(rec,plotgui);
	}
	whenever plotgui.logx->press do {
	  plotpanels(rec,plotgui);
	}
	whenever plotgui.logy->press do {
	  plotpanels(rec,plotgui);
	}
	whenever plotgui.mean0->press do {
	  plotpanels(rec,plotgui);
	}
	whenever plotgui.subtractfits->press do {
      plotgui.replot()
    }
	whenever plotgui.plotfits->press do {
      bool:=plotgui.plotfits->state();
      for (i in ind(plotgui.fitresults)) {if (len(plotgui.fitresults[i])>0) {plotgui.plotfit[i]:=bool}}
      plotgui.replot()
    }
	whenever plotgui.normnoise->press do {
	  plotpanels(rec,plotgui);
	}
	whenever plotgui.squared->press do {
	  plotpanels(rec,plotgui);
	}
	whenever plotgui.squaredsign->press do {
	  plotpanels(rec,plotgui);
	}
	whenever plotgui.fmplothistobut->press do {
	  plotgui.histo:=plotgui.fmviewhistobut->state();
	  plotpanels(rec,plotgui);
	}

	whenever plotgui.fmcalcaverbut->press do {
	  plotgui.pleasewait();
	  if (plotgui.squared->state()) {
	    avgfiles(rec,square,[plotgui.Coordselect[2]]);
	  } else if (plotgui.meanv->state()) {
	    avgfiles(rec,meanv,[plotgui.Coordselect[2]]);
	  } else {
	    avgfiles(rec,ident,[plotgui.Coordselect[2]]);
	  }
	  plotgui.endwait();
	  plotgui.refreshgui()
	}
	whenever plotgui.fmcalcmpowbut->press do {
	  plotgui.pleasewait();
	  ComputeMeanPower(rec);
	  plotgui.endwait();
	}
	whenever plotgui.fmcalcstatbut->press do {
	  wider plotgui;
	  plotgui.pleasewait();
	  for (i in 1:plotgui.nplotted) {
	    print '------------------------------------------------------------------------';
	    print 'Statistics for ',plotgui.filename[i],':'
	    xunit:=spaste(plotgui.UnitsscaleSymbol[1],plotgui.Unitsselect[1]);
	    yunit:=spaste(plotgui.UnitsscaleSymbol[2],plotgui.Unitsselect[2]);
 	    DataStatXY(plotgui.xary[i],plotgui.yary[i],xunit,yunit);
	  }
	  plotgui.endwait();
	}
  
  whenever plotgui.fmcalcbaselines->press do {
	  plotgui.pleasewait()
	  baselines(rec)
    plotgui.endwait()
	}

	whenever plotgui.fmtoolspeakcurbut->press do {
	  wider rec;
	  makePeakGUI(rec, plotgui);
	}
	whenever plotgui.fmtoolspeakbatbut->press do {
	  makePeakBatchGUI(plotgui);
	}
	whenever plotgui.fmtoolsamapbut->press do {
	  print 'Run this command after you have created an image with the Skymapper.'
	  print 'Note that the image needs to be in J2000 coordinates.'
	  annotate_image(imageTool);
	}
	whenever plotgui.fmtoolsmapsurveybut->press do {
      print 'Run this command after you have created an image with the Skymapper.'
	  map_survey(imageTool);
	}
	whenever plotgui.fmtoolsdecoderbut->press do {
	  decoder_gui(rec, plotgui);
	}
	whenever plotgui.fmdatacaltablebut->press do {
      if (has_field(plotgui.CalTableTable,'done')) { plotgui.CalTableTable.done();}
      plotgui.CalTableTable := table(globalpar.CalTablePath);
      plotgui.CalTableTable.browse();
	}	
    whenever plotgui.fmdataplotantbut->press do {
	  plotgui.xy:=rec.get('XY');
	  if (!has_field(plotgui,'plotantpg')) {plotgui.plotantpg:=[=]};
	  plotantf(plotgui.plotantpg,rec,azel=plotgui.AZEL*degree, xy=plotgui.xy, observatory=rec.head('Observatory'));
	}
	whenever plotgui.fmtoolsdynspecbut-> press do {
	  dynspec_gui();
	}
	whenever plotgui.fmtoolsdynspec2but-> press do {
	  DynamicSpectrumGUI(rec);
	}
	whenever plotgui.fmtoolscrbrowser-> press do {
      global evtab;
	  loadcr(rec,evtabin=evtab,i=1,fileno=-1,noload=T);
      crtab.browse();
	}

	whenever plotgui.fmtoolscrprocess -> press do {
      wider plotgui; wider rec;
      if (!is_table(crtab) || !has_field(plotgui,'aux')  || !has_field(plotgui.aux,'aux')) {
        print 'Make sure you have started the CR event GUIs and are in plotpanel #1! (see "File" in the menu bar)'}
#	    crprocess(rec,plotgui,globalpar.event_loaded)
	    crprocess(globalpar.event_loaded)
     }

	whenever plotgui.fmtoolsfindcrdist -> press do {
      wider plotgui; wider rec; local x1,x2
#      if (!is_table(crtab) || !has_field(plotgui,'aux')  || !has_field(plotgui.aux,'aux')) {
        print 'Make sure you have started the CR event GUIs! (see "File" in the menu bar)'
#        } else {
#	  findcrdist(rec,plotgui)
      if (plotgui.selectx & (plotgui.selected_region_x1-plotgui.selected_region_x2!=0)) {
        x1:=rec.getn(plotgui.Coordselect[1],n=rec.getxn(plotgui.selected_region_x1*plotgui.UnitsscaleFactor[1],plotgui.Coordselect[1]))
        x2:=rec.getn(plotgui.Coordselect[1],n=rec.getxn(plotgui.selected_region_x2*plotgui.UnitsscaleFactor[1],plotgui.Coordselect[1]))
        print 'Using selected region for CR distance ',[x1,x2]
	    findcrdist(xrange=[x1,x2])
      } else {
        print 'No selection made. Using default region for CR distance.'
	    findcrdist();
      }
	}

	whenever plotgui.fmtoolsguipipeline -> press do {
      wider plotgui; wider rec;
      if (!dexist('guipipeline')) {cmd.mkdir('guipipeline')};
	  guipipeline(i1=globalpar.event_loaded,i2=globalpar.event_loaded,dirname='guipipeline')
	}

	whenever plotgui.fmtoolsskymapbut-> press do {
      wider plotgui; wider rec;
	  skymapGenerator_gui(rec, plotgui);
	}

	whenever plotgui.fmdataheditbut-> press do {
	  wider rec;
	  EditDataHeader(rec,seq(rec.len)[rec.select()])
	}
	whenever plotgui.fmdatahsavebut-> press do {
	  wider rec; 	    
	  if (EditDataHeader(rec,seq(rec.len))) {
	    SaveData(rec,seq(rec.len)[rec.select()])
	  } else {
	    print 'Not Saved'}
	}

	plotgui.space := frame(plotgui.frame, height=2, expand='x', side='left', borderwidth=1, relief='sunken');

	plotgui.setscalestatus := function() {
	  if (plotgui.fixedscale->state()) {
	    plotgui.x1->disabled(F); plotgui.x2->disabled(F);
	    plotgui.f5b->enable();
	    if (plotgui.fixedy->state()) {
	      plotgui.y1->disabled(F); plotgui.y2->disabled(F);
	    } else {
	      plotgui.y1->disabled(T); plotgui.y2->disabled(T);
	    }
	  } else {
	    plotgui.x1->disabled(T); plotgui.x2->disabled(T);
	    plotgui.f5b->disable();
	  }
	}

	plotgui.toolbarspacer := function(ref parent, ref obj) {
	  val obj := [=];
	  val obj.a := frame(parent, height=24, width=4, expand='none', relief='flat');
	  val obj.b := frame(parent, height=24, width=2, expand='none', relief='sunken');
	  val obj.c := frame(parent, height=24, width=4, expand='none', relief='flat');
	}

	plotgui.f1 := frame(plotgui.frame,side='left', expand='x');
	plotgui.domainnames := ['Spectral Power Density', 'Time Domain Voltage', 'Time Domain Power'];
	plotgui.showhidepanelbut := button(plotgui.f1, '', bitmap=spaste(globalconst.ImageDir,'leftarrows.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.toolbarspacer(plotgui.f1, plotgui.tbsp0);
	plotgui.prevfilebut      := button(plotgui.f1, '', bitmap=spaste(globalconst.ImageDir,'prevfile.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.nextfilebut      := button(plotgui.f1, '', bitmap=spaste(globalconst.ImageDir,'nextfile.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.showpanel := T;
	plotgui.toolbarspacer(plotgui.f1, plotgui.tbsp1);
	plotgui.showdomainlab    := label(plotgui.f1, plotgui.domainnames[2], background='white', borderwidth=1,
	                             width=20, anchor='w', relief='sunken', pady=4);
	plotgui.showdomainbut    := button(plotgui.f1, '', bitmap=spaste(globalconst.ImageDir,'smalldownarrow.xbm'),
	                             width=1, anchor='c', borderwidth=0, hlthickness=0);
	plotgui.showdomainbut->bind('<Button>','click');
	plotgui.showdomainlab->bind('<Button>','click');
	plotgui.toolbarspacer(plotgui.f1, plotgui.tbsp2);
	plotgui.replotbut        := button(plotgui.f1, 'Replot',  bitmap=spaste(globalconst.ImageDir,'replot.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.refreshbut       := button(plotgui.f1,'Refresh', bitmap=spaste(globalconst.ImageDir,'refresh.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.toolbarspacer(plotgui.f1, plotgui.tbsp4);
	plotgui.resizebut        := button(plotgui.f1, '', bitmap=spaste(globalconst.ImageDir,'resize.xbm'),
	                             width=1, anchor='c', borderwidth=0, hlthickness=0);
	plotgui.toolbarspacer(plotgui.f1, plotgui.tbsp5);
	plotgui.overplotbut      := button(plotgui.f1,'Overplot', bitmap=spaste(globalconst.ImageDir,'overplot.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.spawnbut         := button(plotgui.f1,'Spawn', bitmap=spaste(globalconst.ImageDir,'spawn.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.beambut          := button(plotgui.f1,'Beam', bitmap=spaste(globalconst.ImageDir,'beam.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.fbeambut         := button(plotgui.f1,'Full Beam', bitmap=spaste(globalconst.ImageDir,'beam.xbm'),
	                             borderwidth=0, hlthickness=0);
    plotgui.spawnupbut       := button(plotgui.f1,'SpawnUp', bitmap=spaste(globalconst.ImageDir,'spawnup.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.toolbarspacer(plotgui.f1, plotgui.tbsp6);
#	plotgui.lockpgbut := button(plotgui.f1,'LockPlotter', 'check')
	plotgui.dslopesbut := button(plotgui.f1,'dslopes', bitmap=spaste(globalconst.ImageDir,'dslopes.xbm'), foreground='blue3', borderwidth=0, hlthickness=0);
	plotgui.toolbarspacer(plotgui.f1, plotgui.tbsp7);
	plotgui.dismissbut := button(plotgui.f1,'Dismiss', bitmap=spaste(globalconst.ImageDir,'dismiss.xbm'), foreground='orange3', borderwidth=0, hlthickness=0);
	plotgui.donebut := button(plotgui.f1,'Exit', bitmap=spaste(globalconst.ImageDir,'done.xbm'), foreground='red3', borderwidth=0, hlthickness=0);
	plotgui.toolbarspacer(plotgui.f1, plotgui.tbsp8);
	
  popuphelp(plotgui.showhidepanelbut, 'Show or hide tool panels');
	popuphelp(plotgui.prevfilebut,      'Open previous event file');
	popuphelp(plotgui.nextfilebut,      'Open next event file');
	popuphelp(plotgui.showdomainbut,    'Click to select the domain to view the data in');
	popuphelp(plotgui.showdomainlab,    'Click to select the domain to view the data in');
	popuphelp(plotgui.replotbut,        'Apply all changes and refresh the plot');
	popuphelp(plotgui.refreshbut,       'Refresh the plot');
	popuphelp(plotgui.resizebut,        'Set the number of graph panels in x and y dir');
	popuphelp(plotgui.overplotbut,      'Plot spawned data over the current plot');
	popuphelp(plotgui.spawnbut,         'Spawn the data into a new window');
	popuphelp(plotgui.spawnupbut,       'Spawn the data into a new window while upconverting the sample rate. !!! Experimental !!!');
	popuphelp(plotgui.beambut,          'Beam the data into a new window');
	popuphelp(plotgui.fbeambut,         'Beam full TIM40f file in a new window (blocking) !!! UNDER CONSTRUCTION !!!');
	popuphelp(plotgui.dismissbut,       'Close this LOPES-Tools window');
	popuphelp(plotgui.donebut,          'Exit LOPES-Tools');
	popuphelp(plotgui.dslopesbut,       'Load LOPES Daily Dynamic Spectrum');

        #await datawatch->newdata;
	if (!has_field(rec,'len')) {
          newdata(rec);
          rec.newentry(TIM40Header());
         };

	plotgui.peaks_show := F;
	plotgui.peaks_tags := T;
	plotgui.peaks_sens := 12.0;
	plotgui.peaks_bs   := 1024;
	rec.peaks_sens := -1;
	rec.peaks_bs   := -1;


	whenever datawatch->newdata do {
          plotgui.refreshgui();
          plotpanels(rec,plotgui);
        };

	whenever plotgui.showhidepanelbut->press do {
	  if (plotgui.showpanel) {
	    plotgui.leftmainframe->unmap();
	    plotgui.showhidepanelbut->bitmap(spaste(globalconst.ImageDir,'rightarrows.xbm'));
	    plotgui.fmviewshowdockbut->state(F);
	    plotgui.showpanel := F;
	  } else {
	    plotgui.leftmainframe->map();
	    plotgui.showhidepanelbut->bitmap(spaste(globalconst.ImageDir,'leftarrows.xbm'));
	    plotgui.fmviewshowdockbut->state(T);
	    plotgui.showpanel := T;
	  }
	  plotgui.replot();
	}
	whenever plotgui.nextfilebut->press do {
	  onOpenNextFile(plotgui);
	}
	whenever plotgui.prevfilebut->press do {
	  onOpenPrevFile(plotgui);
	}
# This doesn't seem to work. Maybe someone else can make it work.
#	whenever plotgui.lockpgbut->press do {
#	  wider plotgui;
#	  if (plotgui.lockpgbut->state()){
#	    plotgui.rightmainframe->expand('none');
#	print 'lock:',plotgui.rightmainframe->height() , plotgui.rightmainframe->width(), plotgui.pg.size()
#	  } else {
#	    plotgui.rightmainframe->expand('both');
#	print 'unlock:',plotgui.rightmainframe->height() , plotgui.rightmainframe->width(), plotgui.pg.size()
#	  };
#	}

#Main GUI mass: some grouped panels
	plotgui.mainframe      := frame(plotgui.frame, side='right', relief='flat');
	plotgui.rightmainframe := frame(plotgui.mainframe, side='top',expand='none', relief='flat');
	plotgui.rightmainframe->expand('both');
	plotgui.leftmainframe  := frame(plotgui.mainframe, side='top', relief='sunken', expand='y', borderwidth=1, width=212);
	plotgui.newToolPanel(plotgui.leftmainframe, 'Toolbox')
	plotgui.newToolPanel(plotgui.leftmainframe, 'General')
	plotgui.newToolPanel(plotgui.leftmainframe, 'View', show=F, apbut=T)
	plotgui.newToolPanel(plotgui.leftmainframe, 'Frequency', show=F)
	plotgui.newToolPanel(plotgui.leftmainframe, 'Scale', show=F, apbut=T)
	plotgui.newToolPanel(plotgui.leftmainframe, 'Plot', show=F)
	plotgui.newToolPanel(plotgui.leftmainframe, 'Direction', show=F, apbut=T)
	plotgui.newToolPanel(plotgui.leftmainframe, 'Calibration', show=F)
	plotgui.panelfill      := frame(plotgui.leftmainframe, side='top', height=0, relief='raised', width=210, expand='both', borderwidth=1);


	plotgui.pg := pgplotwidget(plotgui.rightmainframe, foreground='black',background='white', havemessages=F,
	                 size=globalpar.PlotterSizeAtStart, padx=0, pady=0);
	plotgui.pg.setcallback('key', plotgui.keypressed);
	plotgui.pg.butdown:=plotgui.pg.setcallback('button',plotgui.downcallback)
	plotgui.pg.butup:=plotgui.pg.setcallback('buttonup', plotgui.upcallback)
	plotgui.pg.motion:=plotgui.pg.setcallback('motion', plotgui.motioncallback)

	# 'Toolbox' panel
	plotgui.f9    := frame(plotgui.toolpanel['Toolbox'], side='top', expand='x', borderwidth=0);
	plotgui.toolbaseframe    := frame(plotgui.f9, side='left', borderwidth=2, expand='x');
	plotgui.movebut          := button(plotgui.toolbaseframe,'Move', bitmap=spaste(globalconst.ImageDir,'move.xbm'),
	                             foreground='darkblue', background='grey93', borderwidth=0, hlthickness=0);
	plotgui.selectbut        := button(plotgui.toolbaseframe,'Select', bitmap=spaste(globalconst.ImageDir,'select.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.zoombut          := button(plotgui.toolbaseframe,'Zoom', bitmap=spaste(globalconst.ImageDir,'zoom.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.deletebut        := button(plotgui.toolbaseframe,'Select', bitmap=spaste(globalconst.ImageDir,'deselect.xbm'),
	                             borderwidth=0, hlthickness=0);
	popuphelp(plotgui.movebut,          'Move and order tool');
	popuphelp(plotgui.selectbut,        'Selection tool');
	popuphelp(plotgui.zoombut,          'Zoom tool');
	popuphelp(plotgui.deletebut,        'Delete tool');

	whenever plotgui.movebut->press do {
	  onUseMove(plotgui);
	}
	whenever plotgui.selectbut->press do {
	  onUseSelect(plotgui);
	}
	whenever plotgui.zoombut->press do {
	  onUseZoom(plotgui);
	}
	whenever plotgui.deletebut->press do {
	  onUseDelete(plotgui);
	}

	plotgui.f9sp1           := frame(plotgui.f9, relief='sunken', height=2, expand='x', borderwidth=1);
	plotgui.toolmoveframe   := frame(plotgui.f9, side='top', borderwidth=2, expand='x');
	plotgui.f9a             := frame(plotgui.toolmoveframe, side='left', expand='x', borderwidth=0);
	plotgui.moveunsticonbut := button(plotgui.f9a,'', bitmap=spaste(globalconst.ImageDir,'reset.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9asp1          := frame(plotgui.f9a, relief='flat', height=1, width=12, expand='none');
	plotgui.movesticonbut   := button(plotgui.f9a,'', bitmap=spaste(globalconst.ImageDir,'stack.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9asp2          := frame(plotgui.f9a, relief='flat', height=1, width=12, expand='none');
	plotgui.movelockiconbut := button(plotgui.f9a,'', bitmap=spaste(globalconst.ImageDir,'movelock.xbm'),
	                             borderwidth=0, hlthickness=0);
	popuphelp(plotgui.moveunsticonbut, 'Restore original sequence');
	popuphelp(plotgui.movelockiconbut, 'Move all plots in a panel together');
	popuphelp(plotgui.movesticonbut,   'Stack all plots in one panel');
	setButtonStatus(plotgui.movelockiconbut,  plotgui.moveall);

	whenever plotgui.movelockiconbut->press do { onMoveLock(plotgui); }
	whenever plotgui.movesticonbut  ->press do { onStackAll(plotgui); }
	whenever plotgui.moveunsticonbut->press do { onUnstack(plotgui); }

	plotgui.toolselectframe := frame(plotgui.f9, side='top', borderwidth=2, expand='x');
	plotgui.f9f             := frame(plotgui.toolselectframe, side='left', expand='x', borderwidth=0);
	plotgui.seldeseliconbut := button(plotgui.f9f,'', bitmap=spaste(globalconst.ImageDir,'reset.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9fsp1          := frame(plotgui.f9f, relief='flat', height=1, width=12, expand='none');
	plotgui.selglobaliconbut:= button(plotgui.f9f,'', bitmap=spaste(globalconst.ImageDir,'global.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.selinviconbut   := button(plotgui.f9f,'', bitmap=spaste(globalconst.ImageDir,'invert.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9fsp2          := frame(plotgui.f9f, relief='flat', height=1, width=12, expand='none');
	plotgui.selflagiconbut  := button(plotgui.f9f,'', bitmap=spaste(globalconst.ImageDir,'flag.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.selunflagiconbut:= button(plotgui.f9f,'', bitmap=spaste(globalconst.ImageDir,'unflag.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9g             := frame(plotgui.toolselectframe, side='left', expand='x', borderwidth=0);
	plotgui.f9gsp1          := frame(plotgui.f9g, relief='flat', height=1, width=36, expand='none');
	plotgui.selconfxiconbut := button(plotgui.f9g,'', bitmap=spaste(globalconst.ImageDir,'confinex.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.selconfyiconbut := button(plotgui.f9g,'', bitmap=spaste(globalconst.ImageDir,'confiney.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9gsp2          := frame(plotgui.f9g, relief='flat', height=1, width=12, expand='none');
	plotgui.selccorriconbut := button(plotgui.f9g,'', bitmap=spaste(globalconst.ImageDir,'crosscorr.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.selinticonbut   := button(plotgui.f9g,'', bitmap=spaste(globalconst.ImageDir,'integrate.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.selgaussiconbut := button(plotgui.f9g,'', bitmap=spaste(globalconst.ImageDir,'gauss.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.toolselectframe->unmap();
	popuphelp(plotgui.seldeseliconbut,  'Deselect');
	popuphelp(plotgui.selglobaliconbut, 'Make selection global');
	popuphelp(plotgui.selinviconbut,    'Invert selection');
	popuphelp(plotgui.selconfxiconbut,  'Confine selection in x dir');
	popuphelp(plotgui.selconfyiconbut,  'Confine selection in y dir');
	popuphelp(plotgui.selflagiconbut,   'Flag selection');
	popuphelp(plotgui.selunflagiconbut, 'Unflag all flagged areas');
	popuphelp(plotgui.selccorriconbut,  'Cross-correlate selection');
	popuphelp(plotgui.selinticonbut,    'Integrate selection over x dir');
	popuphelp(plotgui.selgaussiconbut,  'Apply a Gaussian fit to selection');
	setButtonStatus(plotgui.selconfxiconbut,  plotgui.selectx);
	setButtonStatus(plotgui.selconfyiconbut,  plotgui.selecty);
	setButtonStatus(plotgui.selglobaliconbut, plotgui.selectglobal);
	setButtonStatus(plotgui.selinviconbut,   !plotgui.selectinside);

	whenever plotgui.seldeseliconbut ->press do { onDeselect(plotgui); }
	whenever plotgui.selglobaliconbut->press do { onSelectGlobal(plotgui); }
	whenever plotgui.selinviconbut   ->press do { onSelectInvert(plotgui); }
	whenever plotgui.selconfxiconbut ->press do { onSelectConfineX(plotgui); }
	whenever plotgui.selconfyiconbut ->press do { onSelectConfineY(plotgui); }
	whenever plotgui.selflagiconbut  ->press do { onFlagSelection(plotgui); }
	whenever plotgui.selunflagiconbut->press do { onUnflagSelection(plotgui); }
	whenever plotgui.selccorriconbut ->press do { plotgui.crosscorr(); }
	whenever plotgui.selinticonbut   ->press do { onIntegrate(plotgui); }
	whenever plotgui.selgaussiconbut ->press do { onGaussFit(plotgui); }

	plotgui.toolzoomframe   := frame(plotgui.f9, side='top', borderwidth=2, expand='x');
	plotgui.f9k             := frame(plotgui.toolzoomframe, side='left', expand='x', borderwidth=0);
	plotgui.zoomalliconbut  := button(plotgui.f9k,'', bitmap=spaste(globalconst.ImageDir,'reset.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9ksp1          := frame(plotgui.f9k, relief='flat', height=1, width=12, expand='none');
	plotgui.zoominiconbut   := button(plotgui.f9k,'', bitmap=spaste(globalconst.ImageDir,'zoomin.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.zoominxiconbut  := button(plotgui.f9k,'', bitmap=spaste(globalconst.ImageDir,'zoominx.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.zoominyiconbut  := button(plotgui.f9k,'', bitmap=spaste(globalconst.ImageDir,'zoominy.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9ksp2          := frame(plotgui.f9k, relief='flat', height=1, width=8, expand='x');
	plotgui.moveupiconbut   := button(plotgui.f9k,'', bitmap=spaste(globalconst.ImageDir,'moveup.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9ksp3          := frame(plotgui.f9k, relief='flat', height=1, width=24, expand='none');
	plotgui.f9l             := frame(plotgui.toolzoomframe, side='left', expand='x', borderwidth=0);
	plotgui.zoomseliconbut  := button(plotgui.f9l,'', bitmap=spaste(globalconst.ImageDir,'zoomsel.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9lsp1          := frame(plotgui.f9l, relief='flat', height=1, width=12, expand='none');
	plotgui.zoomouticonbut  := button(plotgui.f9l,'', bitmap=spaste(globalconst.ImageDir,'zoomout.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.zoomoutxiconbut := button(plotgui.f9l,'', bitmap=spaste(globalconst.ImageDir,'zoomoutx.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.zoomoutyiconbut := button(plotgui.f9l,'', bitmap=spaste(globalconst.ImageDir,'zoomouty.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.f9lsp2          := frame(plotgui.f9l, relief='flat', height=1, width=8, expand='x');
	plotgui.movelefticonbut := button(plotgui.f9l,'', bitmap=spaste(globalconst.ImageDir,'moveleft.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.movedowniconbut := button(plotgui.f9l,'', bitmap=spaste(globalconst.ImageDir,'movedown.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.moverighticonbut:= button(plotgui.f9l,'', bitmap=spaste(globalconst.ImageDir,'moveright.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.toolzoomframe->unmap();

	popuphelp(plotgui.zoomalliconbut,  'Zoom out to view all data');
	popuphelp(plotgui.zoomseliconbut,  'Zoom to selection');
	popuphelp(plotgui.zoominiconbut,   'Zoom in');
	popuphelp(plotgui.zoominxiconbut,  'Zoom in horizontally');
	popuphelp(plotgui.zoominyiconbut,  'Zoom in vertically');
	popuphelp(plotgui.zoomouticonbut,  'Zoom out');
	popuphelp(plotgui.zoomoutxiconbut, 'Zoom out horizontally');
	popuphelp(plotgui.zoomoutyiconbut, 'Zoom out vertically');
	popuphelp(plotgui.movelefticonbut, 'Scroll left');
	popuphelp(plotgui.movedowniconbut, 'Scroll down');
	popuphelp(plotgui.moveupiconbut,   'Scroll up');
	popuphelp(plotgui.moverighticonbut,'Scroll right');

	whenever plotgui.zoomalliconbut  ->press do { plotgui.zoomout(plotgui); }
	whenever plotgui.zoomseliconbut  ->press do { onZoomSel(plotgui); }
	whenever plotgui.zoominiconbut   ->press do { onZoomIn(plotgui); }
	whenever plotgui.zoominxiconbut  ->press do { onZoomInX(plotgui); }
	whenever plotgui.zoominyiconbut  ->press do { onZoomInY(plotgui); }
	whenever plotgui.zoomouticonbut  ->press do { onZoomOut(plotgui); }
	whenever plotgui.zoomoutxiconbut ->press do { onZoomOutX(plotgui); }
	whenever plotgui.zoomoutyiconbut ->press do { onZoomOutY(plotgui); }
	whenever plotgui.movelefticonbut ->press do { onScrollLeft(plotgui); }
	whenever plotgui.movedowniconbut ->press do { onScrollDown(plotgui); }
	whenever plotgui.moveupiconbut   ->press do { onScrollUp(plotgui); }
	whenever plotgui.moverighticonbut->press do { onScrollRight(plotgui); }

	plotgui.tooldeleteframe := frame(plotgui.f9, side='top', borderwidth=2, expand='x');
	plotgui.f9p             := frame(plotgui.tooldeleteframe, side='left', expand='x', borderwidth=0);
	plotgui.delreseticonbut := button(plotgui.f9p,'', bitmap=spaste(globalconst.ImageDir,'reset.xbm'),
	                             borderwidth=0, hlthickness=0);
	plotgui.tooldeleteframe->unmap();

	popuphelp(plotgui.delreseticonbut,  'Reset deleted panels');

	whenever plotgui.delreseticonbut->press do { onUndelete(plotgui) }
	# 'General' panel
	plotgui.f0    := frame(plotgui.toolpanel['General'], side='top', expand='x');
	plotgui.datelabel := label(plotgui.f0,'Observation date:', anchor='w', fill='x');
	plotgui.dateent   := entry(plotgui.f0, width=20, borderwidth=1, background='white', fill='x');
	plotgui.dateent->insert(as_string(rec.head('Date')));

	plotgui.f0_a    := frame(plotgui.f0, side='left', expand='x');
	plotgui.obslabel  := label(plotgui.f0_a,'Observatory: ', anchor='w', fill='x');
	plotgui.obsent    := label(plotgui.f0_a, as_string(rec.head('Observatory')), anchor='w',fill='x', foreground='darkblue');

	# 'View' panel
	plotgui.f7_x    := frame(plotgui.toolpanel['View'], side='left', expand='x');
	plotgui.filesizelabel  := label(plotgui.f7_x,'File size');
	plotgui.f7_xsp              := frame(plotgui.f7_x, width=1, height=1, expand='x');
	plotgui.filesizeent    := entry(plotgui.f7_x, width=6, borderwidth=1, background='white', fill='none', justify='right');
	plotgui.filesizeent->insert(as_string(rec.getmeta('Filesize')));
	plotgui.filesizeentvalue:=rec.getmeta('Filesize');

	plotgui.f7    := frame(plotgui.toolpanel['View'], side='left', expand='x');
	plotgui.blocksizelabel  := label(plotgui.f7,'Block size');
	plotgui.f7sp            := frame(plotgui.f7, width=1, height=1, expand='x');
	plotgui.blocksizeent    := entry(plotgui.f7, width=6, borderwidth=1, background='white', fill='none', justify='right');
	plotgui.blocksizeent->insert(as_string(rec.getmeta('Blocksize')));
	plotgui.blocksizeentvalue:=rec.getmeta('Blocksize');

	plotgui.f7_00    := frame(plotgui.toolpanel['View'], side='left', expand='x');
	plotgui.blockoffsetlabel  := label(plotgui.f7_00,'Offset from Start');
	plotgui.f7_00sp            := frame(plotgui.f7_00, width=1, height=1, expand='x');
	plotgui.blockoffsetent    := entry(plotgui.f7_00, width=6, borderwidth=1, background='white', fill='none', justify='right');
	plotgui.blockoffsetent->insert(as_string(rec.getmeta('ZeroOffset')));
	plotgui.blockoffsetentvalue:=rec.getmeta('ZeroOffset');

    plotgui.blocknumscalevalue:=rec.getmeta('Blocknum',rec.defant());

	plotgui.f7_1  := frame(plotgui.toolpanel['View'], side='left', expand='x');
	plotgui.blocknumscalelabel   := label(plotgui.f7_1,'Block #');
	plotgui.f7_1sp          := frame(plotgui.f7_1, width=1, height=1, expand='x');
	plotgui.blocknumscale   := scale(plotgui.f7_1,1,as_integer(rec.getmeta('FileBlocksize')/rec.getmeta('Blocksize')),1,
	                                 borderwidth=1, hlthickness=0, width=6, fill='none')

	plotgui.f7_000    := frame(plotgui.toolpanel['View'], side='left', expand='x');
	plotgui.presynclabel  := label(plotgui.f7_000,'Location of t=0');
	plotgui.f7_000sp            := frame(plotgui.f7_000, width=1, height=1, expand='x');
	plotgui.presyncent    := entry(plotgui.f7_000, width=6, borderwidth=1, background='white', fill='none', justify='right');
	plotgui.presyncent->insert(as_string(rec.head('presync')));

	plotgui.f7_2   := frame(plotgui.toolpanel['View'],side="left", expand='x');
	plotgui.colorlabel      := label(plotgui.f7_2,'Data color');
	plotgui.f7_2sp          := frame(plotgui.f7_2,width=1, height=1, expand='x');
	plotgui.color           := entry(plotgui.f7_2, width=6, borderwidth=1, background='white', fill='none', justify='right');
	plotgui.color->insert('4');
	plotgui.f7_3   := frame(plotgui.toolpanel['View'],side="left", expand='x');
	plotgui.meanv           := button(plotgui.f7_3,'Block avg.',type='check', borderwidth=1, relief='flat',
	                                 hlthickness=0, fill='x', anchor='w')
	plotgui.meanv_npix      := entry(plotgui.f7_3,width=6, borderwidth=1, background='white', fill='none', justify='right');
	plotgui.meanv_npix -> insert("3");
	plotgui.f7_4   := frame(plotgui.toolpanel['View'],side="left", expand='x');
	plotgui.minv           := button(plotgui.f7_4,'Block min.',type='check', borderwidth=1, relief='flat',
	                                 hlthickness=0, fill='x', anchor='w')
	plotgui.minv_npix      := entry(plotgui.f7_4,width=6, borderwidth=1, background='white', fill='none', justify='right');
	plotgui.minv_npix -> insert("3");
	plotgui.f7a1   := frame(plotgui.toolpanel['View'],side="left", expand='x');
	plotgui.nxlabel         := label(plotgui.f7a1,'# Panels (x)');
	plotgui.f7asp1          := frame(plotgui.f7a1,width=1, height=1, expand='x');
	plotgui.nxscale         := scale(plotgui.f7a1,1,rec.len,1, text='', borderwidth=1, hlthickness=0, width=6, fill='none')
	plotgui.f7a2   := frame(plotgui.toolpanel['View'],side="left", expand='x');
	plotgui.nylabel         := label(plotgui.f7a2,'# Panels (y)');
	plotgui.f7asp2          := frame(plotgui.f7a2,width=1, height=1, expand='x');
	plotgui.nyscale         := scale(plotgui.f7a2,1,rec.len,1, text='', borderwidth=1, hlthickness=0, width=6, fill='none')
	plotgui.f7a3   := frame(plotgui.toolpanel['View'],side="left", expand='x');
	plotgui.splabel         := label(plotgui.f7a3,'Start at');
	plotgui.f7asp3          := frame(plotgui.f7a3,width=1, height=1, expand='x');
	plotgui.startpanelscale := scale(plotgui.f7a3,1,rec.len,1, text='',  borderwidth=1, hlthickness=0, width=6, fill='none')

	plotgui.meanv_npix->disabled(T);
	whenever plotgui.meanv->press do {
	  plotgui.setmeanvstate();
          plotgui.refreshgui();
	}
	plotgui.minv_npix->disabled(T);
	whenever plotgui.minv->press do {
	  plotgui.setminvstate();
          plotgui.refreshgui();
	}
	whenever plotgui.frame->resize do {
	  if (globalpar.PlotterResize) {
	    plotgui.pg.refresh();
	  }
	}

	# Middle left frame: frequency
	plotgui.f2_2 := frame(plotgui.toolpanel['Frequency'], side='left', expand='x');
	plotgui.freqstartent   := entry(plotgui.f2_2,width=4, borderwidth=1, background='white', justify='right');
	plotgui.freqstartent->insert(as_string(rec.getmeta('FrequencyLow')/as_double(rec.head('FrequencyUnit'))));
	plotgui.freqstartlabel := label(plotgui.f2_2,'to');
	plotgui.freqstopent    := entry(plotgui.f2_2,width=4, borderwidth=1, background='white', justify='right');
	plotgui.freqstopent->insert(as_string(rec.getmeta('FrequencyHigh')/as_double(rec.head('FrequencyUnit'))));
	plotgui.freqstoplabel  := label(plotgui.f2_2,'MHz');
	plotgui.f2space2       := frame(plotgui.f2_2, width=15, height=1, expand='none');
	plotgui.freqbutt       := button(plotgui.f2_2,'Apply', borderwidth=1);
	plotgui.f2_3 := frame(plotgui.toolpanel['Frequency'], side='left', expand='x');
	plotgui.autoflag       := button(plotgui.f2_3,'Autoflag', borderwidth=1, fill='x');
    plotgui.unflag         := button(plotgui.f2_3,'Unflag', borderwidth=1, fill='x');

	whenever plotgui.showdomainlab->click do {
	  plotgui.makeDomainComboBox();
	}

	whenever plotgui.showdomainbut->click do {
	  plotgui.makeDomainComboBox();
	}

	plotgui.makeDomainComboBox := function () {
  	  wider plotgui;
	  plotgui.showdomainframe := frame(tlead=plotgui.showdomainlab, tpos='sw', relief='ridge', width=150, expand='y')
	  plotgui.showfreqdomain  := button(plotgui.showdomainframe, plotgui.domainnames[1], borderwidth=0, relief='flat',
	                               hlthickness=0, background='white', fill='x', anchor='w')
	  plotgui.showtimedomain  := button(plotgui.showdomainframe, plotgui.domainnames[2], borderwidth=0, relief='flat',
	                               hlthickness=0, background='white', fill='x', anchor='w')
	  plotgui.showpowdomain   := button(plotgui.showdomainframe, plotgui.domainnames[3], borderwidth=0, relief='flat',
	                               hlthickness=0, background='white', fill='x', anchor='w')
	  plotgui.showdomainframe->bind('<Leave>','kill');

	  whenever plotgui.showtimedomain->press do {
	    plotgui.totimedomain();
	    plotgui.showdomainframe := F;
	  }
	  whenever plotgui.showpowdomain->press do {
	    plotgui.topower();
	    plotgui.showdomainframe := F;
	  }
	  whenever plotgui.showfreqdomain->press do {
	    plotgui.tofrequencydomain();
	    plotgui.showdomainframe := F;
	  }
	  whenever plotgui.showdomainframe->kill do {
	    plotgui.showdomainframe := F;
	  }
	}

	whenever plotgui.resizebut->press do {
	  wider plotgui;
	  x:= 0; y:= 0;

	  maxant := len(data.selant());
	  nant := maxant;
	  if (maxant > 10) maxant := 10;
	  tk_hold();
	  plotgui.resizeframe := frame(tlead=plotgui.resizebut, tpos='sw', relief='raised', borderwidth=1, expand='y')
	  plotgui.resizecanvas   := canvas(plotgui.resizeframe, borderwidth=0, hlthickness=0, width=(10*maxant+20),
	                                  height=(10*maxant+2), relief='flat', fill='none')
	  plotgui.resizelabel    := label(plotgui.resizeframe, spaste(plotgui.nx,' x ',plotgui.ny),
	                                  anchor='w', fill='x', relief='sunken', borderwidth=1);
	  tk_release();
	  
	  plotgui.resizecanvas->bind('<Motion>','move');
	  plotgui.resizecanvas->bind('<ButtonRelease>','apply');
	  plotgui.resizelabel ->bind('<ButtonRelease>','apply');
	  plotgui.resizeframe ->bind('<Leave>','kill');
	  whenever plotgui.resizecanvas->move do {
	    wider x;
	    wider y;

	    plotgui.resizecanvas->delete('rect');
	    x := as_integer(($value.device[1]-3)/8)+1;
	    y := as_integer(($value.device[2]-3)/8)+1;
	    dox := T;
	    while ((x*(y-1) >= nant) || (y*(x-1) >= nant)) {
	      dox := !dox;
	      if (dox) x -:= 1; else y -:= 1;
	    }
	    tk_hold();
	    plotgui.resizelabel->text(spaste(x,' x ',y));
	    plotgui.resizecanvas->rectangle(1,1,x*8+1,y*8+1,width='1',fill='white', tag='rect')
	    for (i in 1:x-1) {
	      plotgui.resizecanvas->line(i*8+1,1,i*8+1,y*8+1, tag='rect');
	    }
	    for (i in 1:y-1) {
	      plotgui.resizecanvas->line(1,i*8+1,x*8+1,i*8+1, tag='rect');
	    }
	    tk_release();
	  }
	  whenever plotgui.resizecanvas->apply do {
	    plotgui.messlabel->text(spaste('New graph panel geometry: ',x,' x ',y));
	    plotgui.resizeframe := F;
	    plotgui.setnpanels(x,y);
	  }
	  whenever plotgui.resizelabel ->apply do {
	    plotgui.messlabel->text(spaste('New graph panel geometry: ',x,' x ',y));
	    plotgui.resizeframe := F;
	    plotgui.setnpanels(x,y);
	  }
	  whenever plotgui.resizeframe->kill do {
	    plotgui.resizeframe := F;
	  }
	}

	whenever plotgui.unflag -> press do {
	  wider plotgui; wider rec
	  plotgui.pleasewait();
          rec.unflag();
          rec.unflag('f(x)');
	  plotgui.endwait();
	  plotgui.replot()
        }

	whenever plotgui.autoflag -> press do {
	  wider plotgui;
	  plotgui.pleasewait();
          autoflag(rec);
	  plotgui.endwait();
	  plotgui.replot();
	}

	# Bottom left frame: scale
	plotgui.f5   := frame(plotgui.toolpanel['Scale'], side='top', expand='x');
	plotgui.fixedscale:=button(plotgui.f5, 'Fix scales',type='check', borderwidth=1, relief='flat',
	                           hlthickness=0, fill='x', anchor='w')
	plotgui.f5a  := frame(plotgui.f5, side='left', expand='x', borderwidth=0);
	plotgui.x1        := entry(plotgui.f5a, width=10, borderwidth=1, background='white', fill='x', justify='right');
	plotgui.fixxlabel := label(plotgui.f5a, 'to');
	plotgui.x2        := entry(plotgui.f5a, width=10, borderwidth=1, background='white', fill='x', justify='right');
	plotgui.f5b  := frame(plotgui.toolpanel['Scale'],side='top', expand='x');
	plotgui.fixedy    :=button(plotgui.f5b, 'Fix y scale',type='check', borderwidth=1, relief='flat',
	                           hlthickness=0, fill='x', anchor='w')
	plotgui.f5c  := frame(plotgui.f5b,side='left', expand='x', borderwidth=0);
	plotgui.y1        := entry(plotgui.f5c, width=10, borderwidth=1, background='white', fill='x', justify='right');
	plotgui.fixylabel := label(plotgui.f5c, 'to');
	plotgui.y2        := entry(plotgui.f5c, width=10, borderwidth=1, background='white', fill='x', justify='right');
	plotgui.setscalestatus();

	#Top right frame: plot
	plotgui.Coord:=[=]; plotgui.Coordmenu:=[=];
	plotgui.Unitsscale_menu:=[=]; plotgui.Unitsscale:=[=];
	plotgui.nlines:=1;
	plotgui.f3 := frame(plotgui.toolpanel['Plot'], side='left', expand='x');
	ChooseAxis(plotgui, plotgui.f3, rec,"x",ncoord=1,defcoord=1) ;
	plotgui.f3a := frame(plotgui.toolpanel['Plot'], side='left', expand='x');
	ChooseAxis(plotgui, plotgui.f3a, rec,"y",ncoord=2,defcoord=3);
	plotgui.f3b := frame(plotgui.toolpanel['Plot'], side='left', expand='x');
	plotgui.nlines_label := label(plotgui.f3b,'Number of Lines  ');
	plotgui.nlines_entry := entry(plotgui.f3b,width=2, borderwidth=1, background='white');
	plotgui.f3c := frame(plotgui.toolpanel['Plot'], side='left', expand='x');
	ChooseAxis(plotgui, plotgui.f3c, rec,"x(2)",ncoord=3,defcoord=5);
	plotgui.f3d := frame(plotgui.toolpanel['Plot'], side='left', expand='x');
	ChooseAxis(plotgui, plotgui.f3d, rec,"y(2)",ncoord=4,defcoord=11);

	whenever plotgui.nlines_entry -> return do {
	  plotgui.nlines:=as_integer(eval(plotgui.nlines_entry->get()));
	  plotgui.nlines:=min(2,max(1,plotgui.nlines));
	}

	# Bottom right frame: direction
	plotgui.f4a   := frame(plotgui.toolpanel['Direction'],side="left", expand='x');
        plotgui.offsetxlabel  := label(plotgui.f4a,'X value', fill='x', anchor='w');
	plotgui.offsetxent    := entry(plotgui.f4a, width=7, borderwidth=1, background='white', justify='right', fill='none');
	plotgui.offsetxent->insert(as_string(plotgui.xy[1]));
	plotgui.f4b   := frame(plotgui.toolpanel['Direction'],side="left", expand='x');
        plotgui.offsetylabel  := label(plotgui.f4b,'Y value', fill='x', anchor='w');
	plotgui.offsetyent    := entry(plotgui.f4b, width=7, borderwidth=1, background='white', justify='right', fill='none');
	plotgui.offsetyent->insert(as_string(plotgui.xy[2]));
	plotgui.f4c   := frame(plotgui.toolpanel['Direction'],side="left", expand='x');
        plotgui.azlabel       := label(plotgui.f4c,'Azimuth', fill='x', anchor='w');
	plotgui.azent         := entry(plotgui.f4c, width=7, borderwidth=1, background='white', justify='right', fill='none');
	plotgui.azent->insert(as_string(plotgui.AZEL[1]));
	plotgui.f4d   := frame(plotgui.toolpanel['Direction'],side="left", expand='x');
        plotgui.ellabel       := label(plotgui.f4d,'Elevation', fill='x', anchor='w');
	plotgui.elent         := entry(plotgui.f4d, width=7, borderwidth=1, background='white', justify='right', fill='none');
	plotgui.elent->insert(as_string(plotgui.AZEL[2]));
	plotgui.f4e   := frame(plotgui.toolpanel['Direction'],side="left", expand='x');
        plotgui.distancelabel := label(plotgui.f4e, 'Distance', fill='x', anchor='w');
	plotgui.distanceent   := entry(plotgui.f4e, width=7, borderwidth=1, background='white', justify='right', fill='none');
	plotgui.distanceent->insert(as_string(plotgui.distance));
	plotgui.f4f := frame(plotgui.toolpanel['Direction'],side="left", expand='x');
	plotgui.directionguibut   := button(plotgui.f4f,'Set...', width=6, borderwidth=1, fill='x');
	plotgui.pastedirectionbut := button(plotgui.f4f,'Paste',  width=6, borderwidth=1, fill='x');
	plotgui.resetdirectionbut := button(plotgui.f4f,'Reset',  width=6, borderwidth=1, fill='x');

	plotgui.f4f2 := frame(plotgui.toolpanel['Direction'],side="left", expand='x');
	plotgui.savedirectionbut := button(plotgui.f4f2,'Save Dir',  width=6, borderwidth=1, fill='x');
	plotgui.restoredirectionbut := button(plotgui.f4f2,'Restore',  width=6, borderwidth=1, fill='x');
	plotgui.defaultdirectionbut := button(plotgui.f4f2,'Default',  width=6, borderwidth=1, fill='x');

	plotgui.f4gsp1            := frame(plotgui.appanel['Direction'],side="left", expand='x', height=1, width=12);
	plotgui.upsourcebut       := button(plotgui.appanel['Direction'],'Possible Sources', borderwidth=1, fill='x');
	plotgui.f4gsp2            := frame(plotgui.appanel['Direction'],side="left", expand='none', height=1, width=1);
	whenever plotgui.directionguibut -> press do {
      rec.doframe();
	  plotgui.messlabel->text(paste('Press Convert and Copy in Direction GUI and then "Paste" button in this GUI'));
	  dm.directiongui();
	}
	whenever plotgui.upsourcebut -> press do {
      list_visible_sources(rec)
	}


	# 'Calibration' panel
	plotgui.f8a := frame(plotgui.toolpanel['Calibration'],side="left", expand='x');
	plotgui.f8b := frame(plotgui.toolpanel['Calibration'],side="left", expand='x');
	plotgui.refantabel      := label(plotgui.f8b,'Reference antenna', anchor='w', fill='x');
	plotgui.refantent       := entry(plotgui.f8b, width=3, borderwidth=1, background='white', justify='right', fill='none');
	plotgui.refantent->insert(as_string(plotgui.refant));
	plotgui.f8c := frame(plotgui.toolpanel['Calibration'],side="top", expand='x');
	plotgui.callabel        := label(plotgui.f8c,'Calibrate on:', fill='x', anchor='w');
	plotgui.calibratebutdir := button(plotgui.f8c,'Direction panel', type='radio', borderwidth=1, hlthickness=0,
	                                    relief='flat', fill='x', anchor='w');
	plotgui.calibratebutsun := button(plotgui.f8c,'Sun', type='radio', borderwidth=1, hlthickness=0, relief='flat',
	                                    fill='x', anchor='w');
	plotgui.calibratebuttv  := button(plotgui.f8c,'TV station', type='radio', borderwidth=1, hlthickness=0,
	                                    relief='flat', fill='x', anchor='w');
	plotgui.calibratebutcr  := button(plotgui.f8c,'Cosmic ray', type='radio', borderwidth=1, hlthickness=0,
	                                    relief='flat', fill='x', anchor='w');
	plotgui.f8d := frame(plotgui.toolpanel['Calibration'],side="left", expand='x');
	plotgui.calibratebut    := button(plotgui.f8d,'Calibrate', borderwidth=1, width=8, fill='x');
	plotgui.uncalibratebut  := button(plotgui.f8d,'Uncalibrate', borderwidth=1, width=8, fill='x');
	plotgui.calibratebutdir->state(T);


	whenever plotgui.calibratebut->press do {
	  plotgui.pleasewait();
	  if (plotgui.calibratebutdir->state()) {
print 'Dir calibration'
	    #This function calibrates the delays using a crosscorrelation of that data
	    #assuming the noise is dominated by a single point source (e.g. during a 
	    #solar burst).
	    if (!plotgui.selactive) {
	      plotgui.messlabel->text('Please select a frequency range with the cursor first (press "s")');
	    } else {
	      plotgui.messlabel->text(paste('Calibration on Point Source at AZEL=',plotgui.azel));
	      freqs:=[plotgui.selected_region_x1*plotgui.UnitsscaleFactor[1],plotgui.selected_region_x1*plotgui.UnitsscaleFactor[1]];
	      calibrate_phases_on_pointsource(rec,index=-1,frequency=freqs);
	      plotgui.replot();
	    }
	  } else if (plotgui.calibratebutsun->state()) {
print 'Sun calibration'
	    plotgui.messlabel->text(paste('Start calibration on the Sun - make sure you have "spawned" the data first!'));
	    print 'Phase CALIBRATION on Sun';
	    print 'please wait...';
	    calibratedelays(rec,index=seq(rec.len));
	    rec.setphase(index=-1);
	  } else if (plotgui.calibratebutcr->state()) {
print 'CR calibration'
	    plotgui.messlabel->text(paste('Start calibration on CR - make sure you have selected a region around the CR!'));
	    print 'Phase CALIBRATION on Cosmic ray pulse';
	    plotgui.crosscorr();
	  } else if (plotgui.calibratebuttv->state()) {
print 'TV calibration'
	    #This function calibrates the delays using a TV station and a reference
	    #datum where the array was well calibrated
	    plotgui.messlabel->text(paste('Start calibration on TV station.'));
	    print 'Phase CALIBRATION on TV';
	    print 'please wait...';
	    phasecmp(rec);
	    rec.setphase(index=-1);
	  }
	  plotgui.endwait();
	}

	plotgui.setphase := function() {
	  wider rec; wider plotgui
	  plotgui.AZEL:=[as_float(eval(plotgui.azent->get())),as_float(eval(plotgui.elent->get()))]
	  plotgui.refant:=as_float(eval(plotgui.refantent->get()))
	  plotgui.distance:=as_float(eval(plotgui.distanceent->get()))
	  plotgui.xy:=[as_float(eval(plotgui.offsetxent->get())),as_float(eval(plotgui.offsetyent->get()))];
	  rec.put(plotgui.refant,'RefAnt',-1)
	  rec.put(plotgui.AZEL,'AZEL',-1)
	  rec.put(plotgui.distance,'Distance',-1)
	  rec.put(plotgui.xy,'XY',-1)
	  rec.put(T,'Doshift',-1)
	  rec.setphase()
	}

	whenever plotgui.uncalibratebut -> press do {
	  plotgui.pleasewait(message=F);
	  plotgui.messlabel->text(paste('Resetting calibration information...'));
      FixDelays(rec); rec.put(0,'Caldelay',-1);
	  rec.setphase(index=-1);
	  plotgui.endwait();
	  plotgui.replot();
	}

	whenever plotgui.offsetxent -> return do {plotgui.setphase()}
	whenever plotgui.offsetyent -> return do {plotgui.setphase()}
	whenever plotgui.elent -> return do {plotgui.setphase()}
	whenever plotgui.azent -> return do {plotgui.setphase()}
	whenever plotgui.distanceent -> return do {plotgui.setphase()}
	whenever plotgui.refantent -> return do {plotgui.refant:=as_float(eval(plotgui.refantent->get()))}

        whenever plotgui.dateent->return do {
          wider rec;
          rec.setdate(as_string(plotgui.dateent->get())); 
          plotgui.dateent->delete('start','end');
          plotgui.dateent->insert(as_string(rec.head('Date')));
          plotgui.refreshgui();
        };


	whenever plotgui.resetdirectionbut -> press do {rec.resetdir(); plotgui.replot()}
	whenever plotgui.savedirectionbut -> press do {
        rec.savedirection(storagearea='0'); 
        rec.saveblockinfo(storagearea='0'); 
       };
	whenever plotgui.restoredirectionbut -> press do {
        rec.restoredirection(storagearea='0',refresh=F); 
        rec.restoreblockinfo(storagearea='0'); 
        plotgui.replot()};
	whenever plotgui.defaultdirectionbut -> press do {
        rec.restoredirection(refresh=F); 
        rec.restoreblockinfo(); 
        plotgui.replot()};

	whenever plotgui.pastedirectionbut -> press do {
	  azel:=dcb.paste()
	  meas:=[=]
	  meas.type:=dm.gettype(azel)
	  meas.refer:=dm.getref(azel)
	  meas.m0:=dm.getvalue(azel)[1]
	  meas.m1:=dm.getvalue(azel)[2]
	  azel:=meastoazel(rec,meas)
	  plotgui.azent->delete('start','end');
	  plotgui.azent->insert(as_string(azel[1]));
	  plotgui.elent->delete('start','end');
	  plotgui.elent->insert(as_string(azel[2]));
	  plotgui.setphase()
	}


	plotgui.f6 := frame(plotgui.frame,side="left", expand='x', relief='sunken', borderwidth=1);
	plotgui.messlabel := label(plotgui.f6, '', justify='left', relief='flat', borderwidth=0);
	plotgui.messlabelspace := frame(plotgui.f6, width=1, height=1, expand='x');
	plotgui.memlabel := label(plotgui.f6, spaste('Memory Usage: ',roundn(alloc_info().used/(1024*1024),2),'MB'), justify='right', relief='flat', borderwidth=0);
	tk_release();
	rec.newsagent := datawatch;
	whenever plotgui.fixedy->press do {
	  plotgui.setscalestatus();
	};
	whenever plotgui.fixedscale->press do {
	  plotgui.setscalestatus();
	};
    whenever memwatch -> update do {
      plotgui.memlabel->text(spaste('Memory Usage: ',roundn(alloc_info().used/(1024*1024),1),'MB'));
    }
	whenever datawatch->newmessage do {
		plotgui.messlabel->text(paste($value));
	};

	plotgui.fmviewshowtlbarbut->state(T)
	whenever plotgui.fmviewshowtlbarbut->press do {
	  tk_hold();
	  if (plotgui.fmviewshowtlbarbut->state()) {
	    plotgui.mainframe->unmap();
	    plotgui.f6->unmap();
	    plotgui.space->map();
	    plotgui.f1->map();
	    plotgui.mainframe->map();
	    if (plotgui.fmviewshowstbarbut->state()) {
	      plotgui.f6->map();
	    }
	  } else {
	    plotgui.space->unmap();
	    plotgui.f1->unmap();
	  }
	  tk_release();
	}
	plotgui.fmviewshowstbarbut->state(T)
	whenever plotgui.fmviewshowstbarbut->press do {
	  if (plotgui.fmviewshowstbarbut->state()) {
	    plotgui.f6->map();
	  } else {
	    plotgui.f6->unmap();
	  }
	}
	plotgui.fmviewshowdockbut->state(T)
	whenever plotgui.fmviewshowdockbut->press do {
	  if (plotgui.showpanel) {
	    plotgui.leftmainframe->unmap();
	    plotgui.showhidepanelbut->bitmap(spaste(globalconst.ImageDir,'rightarrows.xbm'));
	    plotgui.showpanel := F;
	  } else {
	    plotgui.leftmainframe->map();
	    plotgui.showhidepanelbut->bitmap(spaste(globalconst.ImageDir,'leftarrows.xbm'));
	    plotgui.showpanel := T;
	  }
	  plotgui.replot();
	}


        plotgui.startpanel := 1
	plotgui.select_panel:=1
        plotgui.nx := 1
        plotgui.ny := 1

	FRegionRecalc := function() {
      wider plotgui;
	  wider rec;
	  startf := as_float(eval(plotgui.freqstartent->get()));
	  stopf := as_float(eval(plotgui.freqstopent->get()));

	  for (i in [1:rec.len][rec.select()]) {
	    startfreq:=startf*rec.headf("FrequencyUnit",i);
	    stopfreq:=stopf*rec.headf("FrequencyUnit",i);
	    freqarr := rec.get('Frequency',i,start=1,end=1e6);
	    size := len(freqarr)
	    start := max([1:size][freqarr<startfreq],1);
	    stop := min([1:size][freqarr>stopfreq],size);
	    if ((start<1) || (start>size)) start :=1;
	    if ((stop<1) || (stop>size)) stop := size;
	print 'setting region for',i,'to',[start,stop];
	    rec.setregion([start,stop],'F',i);
	  };

	};


#User interaction: an automatic replot is initiated when 
#parameters are changed.

#	whenever plotgui.datasizeent -> return do {
#	    globalpar.DataSize := min(globalpar.MaxDataSize,as_integer(eval(plotgui.datasizeent->get())));
#	plotgui.datasizeent->delete('start','end');
#	plotgui.datasizeent->insert(as_string(globalpar.DataSize));
#	}

	whenever plotgui.blocksizeent -> return do {
	    blocksize := as_integer(eval(plotgui.blocksizeent->get()));
	    if (blocksize<0) blocksize := 2000000000;
	    for (i in [1:rec.len]) {
		t:= rec.getmeta("FileBlocksize",i);
		if ( rec.select()[i] && (t >0) && (t<blocksize) ) blocksize := t;
	    };
print 'plotgui.blocksizeent:',blocksize;
        rec.setblock(1,blocksize)
        plotgui.blocksizeentvalue:=blocksize;
        plotgui.refreshgui();
	};

	whenever plotgui.blockoffsetent -> return do {
	    blockoffset := as_integer(eval(plotgui.blockoffsetent->get()));
	    if (blockoffset<0) blockoffset := 10000000000;
	    for (i in [1:rec.len]) { 
		t1:= rec.getmeta("FileBlocksize",i); 
		t2:= rec.getmeta("Blocksize",i); 
		if ( rec.select()[i] && (t1 >0) && (t1<blockoffset+t2) ) blockoffset := t1;
	    };
        rec.setblock(offset=blockoffset)
        plotgui.blockoffsetentvalue:=blockoffset;
        plotgui.refreshgui();
	};

	whenever plotgui.presyncent -> return do {
	    presync := as_string(as_float(eval(plotgui.presyncent->get())));
        rec.puthead(presync,'presync',-1)
        plotgui.refreshgui();
	};

	whenever plotgui.blocknumscale -> value do {
        plotgui.blocknumscalevalue:=as_integer($value);
	    rec.setblock(plotgui.blocknumscalevalue);
    }


#Select frequency region 
	whenever plotgui.freqbutt->press do {
		FRegionRecalc(); plotpanels(rec,plotgui);}


#Select starting panel by the user
	whenever plotgui.startpanelscale -> value do {
	  if (plotgui.startpanel != $value) {
	    plotgui.startpanel:=$value;
	  }
        plotgui.refreshgui();
	}

#Describe Nx,Ny layout of the panels
	whenever plotgui.nxscale -> value do {
	  if (plotgui.nx != $value) {
	    plotgui.nx:=$value;
	  }
        plotgui.refreshgui();
	}

	whenever plotgui.nyscale -> value do {
	  if (plotgui.ny != $value) {
	    plotgui.ny:=$value;
	  }
        plotgui.refreshgui();
	}

#Dismiss button
	whenever plotgui.dismissbut->press do
           {wider plotgui; global globalpar;
            if (globalpar.timerstart) timer.remove(plotgui.timer); 
            plotgui.pg.done(); 
            val plotgui:=[=];
           }

#Done button
	whenever plotgui.donebut->press do
           {exit;}

#Spawn button
	whenever plotgui.spawnbut->press do
           {wider plotgui; 
            globalpar.SpawnID+:=1; 
            rec.aux:=[=];
            SpawnRecord(rec,rec.aux,totype='TIM40rev');
            plotgui.aux:=plotguif(rec.aux, title=spaste('>',title));
            }

#SpawnUp button
	whenever plotgui.spawnupbut->press do
           {wider plotgui; 
            if (!is_function(TIM40rev_up_modifyheader)) {
              linclude('data-TIM40rev_upconv.g');
            }
            globalpar.SpawnID+:=1; 
            rec.aux:=[=];
            SpawnRecord(rec,rec.aux,totype='TIM40rev_up');
            plotgui.aux:=plotguif(rec.aux, title=spaste('^',title));
            }

#Beam button
	whenever plotgui.beambut->press do
           {wider plotgui; 
            rec.aux:=[=];
            SpawnRecord(rec,rec.aux,totype='TIM40beam');
            plotgui.aux:=plotguif(rec.aux,title=spaste('+',title));
            }
            
#Full Beam button
	whenever plotgui.fbeambut->press do
           {wider plotgui; 
             datawatch->message('Under construction :o)');          
           }

#Replot button
	whenever plotgui.replotbut->press do
	{
      plotgui.replot()
	}

#Refresh button
	whenever plotgui.refreshbut->press do {
	  plotgui.pg.refresh();
	  #refreshpanels(plotgui,selection=plotgui.selactive,erase=T)
	}

#Overplot button
	whenever plotgui.overplotbut->press do
           {refreshpanels(plotgui,selection=plotgui.selactive,erase=F,overplotaux=T)}

	plotpanels(rec,plotgui);
	datawatch->message("Ready.");
	
#Dslopes button
	whenever plotgui.dslopesbut->press do
           {loadynspeclopes();}


  if (globalpar.timerstart) plotgui.timer:=timer.execute(plotgui.timerfunc,globalpar.timerinterval,F);

  return ref plotgui;	

}
###END plotguif

#-----------------------------------------------------------------------
#Name: plotpanels
#-----------------------------------------------------------------------
#Type: Function
#Doc: Plots mulitple panels in the PG-Plotter window. Called by
#     plotguif events
#     Constructs x/y-axis labels with correct units, scale factor symbols,
#     logs etc.
#
#Ret: T/F
#Par: rec =	  			- Data record
#Par: plotgui =				- Record describing the GUI
#Ref: readfiles_gui, selectfiles, plotguif
#-----------------------------------------------------------------------

plotpanels:=function(ref rec,ref plotgui) {
    plotgui.pleasewait();
    plotgui.pg.clear()
    plotgui.fullscreen:=plotgui.pg.qvsz(3)
    plotgui.viewport:=[=];  plotgui.fullviewport:=[=];  plotgui.boxworld:=[=]
    plotgui.xary:=[=]; plotgui.yary:=[=]; plotgui.panel:=[=]
    plotgui.sci:=[=]; plotgui.panelix:=[=]; plotgui.paneliy:=[=];
    plotgui.xlims:=[=]; plotgui.ylims:=[=]
    logx:=plotgui.logx->state(); logy:=plotgui.logy->state();
    absy:=plotgui.absy->state();
    normnoise:=plotgui.normnoise->state();
    mean0:=plotgui.mean0->state();
    subfits:=plotgui.subtractfits->state();
    squared:=plotgui.squared->state();
    squaredsign:=plotgui.squaredsign->state();
    axis:=0; if (logx) axis+:=10; if (logy) axis+:=20;
    plotgui.axisstyle:=axis;
    if (squared) {sqrtxt:='\\u2\\d'} else {sqrtxt:=''};
    plotgui.pg.subp(plotgui.nx,plotgui.ny);
    i0:=plotgui.startpanel-1; plotgui.nplotted:=0
    plotgui.npanels:=(plotgui.nx*plotgui.ny);
    while (i0<rec.len && i0<maxnpanels && plotgui.nplotted<plotgui.npanels){
    i0+:=1; 
    wp:=plotgui.whichpanel[i0]; wp:=wp[wp>=1 & wp<=rec.len];
    selected:=rec.select()[wp];
    wp:=wp[selected];
    if (len(wp)>0) {
       plotgui.nplotted+:=1; 
       plotgui.panel[plotgui.nplotted]:=i0
       iy:=as_integer((plotgui.nplotted-1)/plotgui.nx)+1
       ix:=plotgui.nplotted-(iy-1)*plotgui.nx
       plotgui.panelix[plotgui.nplotted]:=ix;
       plotgui.paneliy[plotgui.nplotted]:=iy;
       nsubplots:=0;
       for (i in wp) {
         plotgui.pg.bbuf();
         nsubplots+:=1;
         for (ncoord in seq(plotgui.nlines)) {
          xaxis:=(ncoord-1)*2+1; yaxis:=xaxis+1;
	      plotgui.pg.sci(1);
	      xary:=rec.get(plotgui.Coordselect[xaxis],i)
	      yary:=rec.get(plotgui.Coordselect[yaxis],i)
          ly:=len(yary); lx:=len(xary);
 	       if (lx<ly) {yary:=yary[1:lx]; ly:=lx};
  	       if (ly<lx) {xary:=xary[1:ly]; lx:=ly};
           l1:=as_integer(ly*globalpar.xbeam_l1); l2:=l1+ly/4;
           if (plotgui.UnitsscaleFactor[xaxis]!=1) {
              xary:=xary/plotgui.UnitsscaleFactor[xaxis]}
           if (plotgui.UnitsscaleFactor[yaxis]!=1) {
              yary:=yary/plotgui.UnitsscaleFactor[yaxis]}
	   if (squaredsign) {yary:=sqr_sign(yary)} 
       else if (squared) yary:=yary^2;
       if (mean0) {yary:=yary-mean(yary[l1:l2])}
       if (normnoise) {yary:=yary/stddev(yary[l1:l2])}
       if (absy) yary:=abs(yary);
	   if (plotgui.meanv->state()) {
           meanv_npix:=as_integer(eval(plotgui.meanv_npix->get()))
           if (meanv_npix>3 && meanv_npix<10000) {yary:=blockaverage(yary,meanv_npix)}
           else {yary:=meanv(yary)}
       }
 	   if (plotgui.minv->state()) {
           minv_npix:=as_integer(eval(plotgui.minv_npix->get()))
           if (minv_npix>3 && minv_npix<10000) {yary:=runminc(yary,minv_npix)}
           else {yary:=meanv(yary)}
       }
        
	   if (logx) xary:=log(dezero(abs(xary)));
	   if (logy) yary:=log(dezero(abs(yary)));

#Determine plot limits and write them into GUI if automatic.
      if (nsubplots==1 && ncoord==1) {
       if (subfits && len(plotgui.fitresults[i])>0) yary:=yary-plotgui.fityary[wp[1]];
	   plotgui.xary[plotgui.nplotted]:=xary
	   plotgui.yary[plotgui.nplotted]:=yary
       plotgui.filename[plotgui.nplotted]:=rec.head('Filename',i)

	   if (plotgui.fixedscale->state()) {
 	     x1:=as_float(eval(plotgui.x1->get()));
 	     x2:=as_float(eval(plotgui.x2->get()));
         if (! plotgui.fixedy->state()) {
            y1:=min(yary[xary>=x1 & xary<=x2 & yary>neginf]);                               
            y2:=max(yary[xary>=x1 & xary<=x2]);                               
	        y1:=y1-(y2-y1)*0.05; y2:=y2+(y2-y1)*0.05;
 	        plotgui.y1->delete('start','end'); 
	        plotgui.y2->delete('start','end'); 
	        plotgui.y1->insert(as_string(y1));
	        plotgui.y2->insert(as_string(y2));
         } else {
            y1:=as_float(eval(plotgui.y1->get()));
 	        y2:=as_float(eval(plotgui.y2->get()));
            }
	   } else {
	     x1:=min(xary); x2:=max(xary);
	     y1:=min(yary[yary>neginf]); y2:=max(yary);
	     y1:=y1-(y2-y1)*0.05; y2:=y2+(y2-y1)*0.05;
	     plotgui.x1->delete('start','end'); 
	     plotgui.x2->delete('start','end'); 
	     plotgui.y1->delete('start','end');
	     plotgui.y2->delete('start','end'); 
	     plotgui.x1->insert(as_string(x1));
	     plotgui.x2->insert(as_string(x2));
	     plotgui.y1->insert(as_string(y1));
	     plotgui.y2->insert(as_string(y2));
	   };
           if (x2==x1) {x1:=x1-1; x2:=x1+1}
           if (y2==y1) {y1:=y1-1; y2:=y1+1}
	   
	      plotgui.xlims[plotgui.nplotted]:=[x1,x2]
	      plotgui.ylims[plotgui.nplotted]:=[y1,y2]
	   }
 	   plotgui.pg.sci(color:=as_integer(eval(plotgui.color->get())));
	   if (plotgui.histo) {
	      plotgui.pg.hist(yary,y1,y2,100,0)
	   } else {
	  lplim:=0;
	  rplim:=0;
#         plotgui.pg.panl(ix,iy);  
#         Note: pg.evn includes a page command and moves to the next panel

      if (nsubplots==1 && ncoord==1) {
 	     plotgui.pg.sci(color:=1);
	     if (lx>0) {
               if (globalpar.PrintingPlots) {
	         plotgui.pg.sch(globalpar.PrintCharHeight);
	         plotgui.pg.slw(globalpar.PrintLineWidth);
	       };
               plotgui.pg.env(x1,x2,y1,y2,0,axis);
             };
	     if (!globalpar.PrintingPlots){
 	       plotgui.pg.sci(color:=as_integer(eval(plotgui.color->get())));
	     } else {
	       plotgui.pg.sci(globalpar.PrintLabelCol);
	     };
             if (!globalpar.PrintingPlots || globalpar.PrintToplbl){
	       plotgui.pg.lab(
                spaste(plotgui.Coordselect[xaxis]," [",plotgui.UnitsscaleSymbol[xaxis],
					  plotgui.Unitsselect[xaxis],"]"),
                spaste(plotgui.Coordselect[yaxis],sqrtxt," [",
					  plotgui.UnitsscaleSymbol[yaxis],
					  plotgui.Unitsselect[yaxis],sqrtxt,"]"),
	        spaste("[",as_string(i),"] ",rec.head('Filename',i)));
             } else {
	       plotgui.pg.lab(
                spaste(plotgui.Coordselect[xaxis]," [",plotgui.UnitsscaleSymbol[xaxis],
					  plotgui.Unitsselect[xaxis],"]"),
                spaste(plotgui.Coordselect[yaxis],sqrtxt," [",
					  plotgui.UnitsscaleSymbol[yaxis],
					  plotgui.Unitsselect[yaxis],sqrtxt,"]"),
	        '');
             };
	   }
	     off:=nsubplots*ncoord;
	     if (plotgui.peaks_show) {
	       if (len(rec.peaks) < 1) peaksearch(sigs, rec, plotgui);
	       plotgui.pg.sci(8);
	       if (rec.peaks[i].count > 0) for (_p in 1:rec.peaks[i].count) {
	         if (rec.peaks[i].x[_p] > plotgui.xlims[plotgui.nplotted][1] &&
	             rec.peaks[i].x[_p] < plotgui.xlims[plotgui.nplotted][2])
	           plotgui.pg.line([rec.peaks[i].x[_p]/plotgui.UnitsscaleFactor[1],
	                            rec.peaks[i].x[_p]/plotgui.UnitsscaleFactor[1]],
	                           [plotgui.ylims[plotgui.nplotted][1]/plotgui.UnitsscaleFactor[2],
	                            plotgui.ylims[plotgui.nplotted][2]/plotgui.UnitsscaleFactor[2]]);
	       }
	     }

 	     plotgui.pg.sci(color:=as_integer(eval(plotgui.color->get()))+off-1);
	     plotgui.panelsci[plotgui.nplotted]:=color;
	     plotgui.pg.line(xary,yary)
             if (!globalpar.PrintingPlots || globalpar.PrintPlotind){
	       plotgui.pg.mtxt('T', -off*1.5-1, 1, 0.0, spaste(" - [",as_string(i),"]"));
             };

	     if (plotgui.peaks_show && plotgui.peaks_tags) {
	       plotgui.pg.sci(0);
	       plotgui.pg.stbg(8);
	       if (rec.peaks[i].count > 0) for (_p in 1:rec.peaks[i].count) {
	         if (rec.peaks[i].x[_p] > plotgui.xlims[plotgui.nplotted][1] &&
	             rec.peaks[i].x[_p] < plotgui.xlims[plotgui.nplotted][2])
	           plotgui.pg.ptxt(rec.peaks[i].x[_p]/plotgui.UnitsscaleFactor[1],
	                                    (plotgui.ylims[plotgui.nplotted][1])/plotgui.UnitsscaleFactor[2],
	                           90., 0.,
	                           as_string(rec.peaks[i].x[_p]/plotgui.UnitsscaleFactor[1]));
	       }
	       plotgui.pg.stbg(-1);
	     }
	   }
	   plotgui.splitscreen:=plotgui.pg.qvsz(3)
	   plotgui.viewport[plotgui.nplotted]:=plotgui.pg.qvp(3)+
	      [plotgui.splitscreen[2]*(ix-1),plotgui.splitscreen[2]*(ix-1),
	       plotgui.splitscreen[4]*(iy-1),plotgui.splitscreen[4]*(iy-1)]
	   plotgui.fullviewport[plotgui.nplotted]:=plotgui.splitscreen+
	      [plotgui.splitscreen[2]*(ix-1),plotgui.splitscreen[2]*(ix-1),
	       plotgui.splitscreen[4]*(iy-1),plotgui.splitscreen[4]*(iy-1)]
	   plotgui.boxworld[plotgui.nplotted]:=[x1,x2,y1,y2]

	}
         plotgui.pg.ebuf();
#End for i in whichpanel
    }
    if (plotgui.plotfit[wp[1]]) { 
   	     plotgui.pg.sci(6);
plotgui.pg.sls(2)
	     plotgui.pg.line(plotgui.fitxary[wp[1]],plotgui.fityary[wp[1]])
plotgui.pg.sls(1)
    }
    }}
    plotgui.highlightselection();
    plotgui.endwait();
}

#Name: highlightselection
#-----------------------------------------------------------------------
#Type: Function
#Doc: Overplots the selected region(s) in red on existing panels
#
#Ret: T/F
#Par: plotgui =				- Record describing the GUI
#Par: npan    =             - which panel to highlight selection
#Par: inside  =             - highlight region inside or outside limits
#Par: selx    =             - use xlimits for selection 
#Par: sely    =             - use ylimits for selection 
#Ref: plotpanels, refresh, upcallback
#-----------------------------------------------------------------------
highlightselection:=function(ref plotgui,npan=1,inside=T,selx=T,sely=F){
        if (!selx && !sely) return
        if (npan > plotgui.nplotted) return
        plotgui.select_y1old:=plotgui.selected_region_y1; plotgui.select_y2old:=plotgui.selected_region_y2; 
 	    plotgui.pg.sci(2);
        xary:=plotgui.xary[npan]
        yary1:=plotgui.yary[npan]
        sel1:=seq(len(xary)); sel2:=sel1
        plotgui.pg.panl(plotgui.panelix[npan],plotgui.paneliy[npan])
        plotgui.pg.swin(plotgui.xlims[npan][1],plotgui.xlims[npan][2],plotgui.ylims[npan][1],plotgui.ylims[npan][2])
        if (inside) {
          if (selx) sel1:=((xary >=plotgui.selected_region_x1) & (xary <= plotgui.selected_region_x2))
          if (sely) {
                    yary1[yary1 <  plotgui.selected_region_y1]:=plotgui.selected_region_y1
                    yary1[yary1 >  plotgui.selected_region_y2]:=plotgui.selected_region_y2
                    }
	    plotgui.pg.line(xary[sel1],yary1[sel1])
        } else {
          yary2:=yary1; yary0:=yary1;
          if (selx) {
                sel1:=(xary < plotgui.selected_region_x1)
                sel2:=(xary > plotgui.selected_region_x2)
             }
          if (sely) {
                yary1[(yary1 >=  plotgui.selected_region_y1)]:=plotgui.selected_region_y1
                yary2[(yary2 <=  plotgui.selected_region_y2)]:=plotgui.selected_region_y2
           }
	    if (selx){plotgui.pg.line(xary[sel1],yary0[sel1]);plotgui.pg.line(xary[sel2],yary0[sel2])}
        if (sely){plotgui.pg.line(xary,yary1); plotgui.pg.line(xary,yary2)}
        }
 	    plotgui.pg.sci(1);
}

#Name: refreshpanels
#-----------------------------------------------------------------------
#Type: Function
#Doc: Replots the stored data calculated for each panel in plotpanels.
#     Does not use rec.get!
#
#Ret: T/F
#Par: plotgui =				- Record describing the GUI
#Ref: plotpanels
#-----------------------------------------------------------------------
refreshpanels:=function(ref plotgui,selection=F,erase=F,overplotaux=F){
  plotgui.pleasewait();
  if (erase) plotgui.pg.eras()
  for (i in seq(plotgui.nplotted)) {
    plotgui.pg.bbuf();
    if (erase) {
        plotgui.pg.sci(1)
        if (globalpar.PrintingPlots) {
          plotgui.pg.sch(globalpar.PrintCharHeight);
          plotgui.pg.slw(globalpar.PrintLineWidth);
        };
  plotgui.pg.env(plotgui.xlims[i][1],plotgui.xlims[i][2],plotgui.ylims[i][1],plotgui.ylims[i][2],0,plotgui.axisstyle)
    } else {
    plotgui.pg.panl(plotgui.panelix[i],plotgui.paneliy[i])
    plotgui.pg.swin(plotgui.xlims[i][1],plotgui.xlims[i][2],plotgui.ylims[i][1],plotgui.ylims[i][2])
    plotgui.pg.sci(0)
	plotgui.pg.line(plotgui.xlims[i],[plotgui.select_y1old,plotgui.select_y1old])
	plotgui.pg.line(plotgui.xlims[i],[plotgui.select_y2old,plotgui.select_y2old])
    }
    plotgui.pg.sci(plotgui.panelsci[i])
	plotgui.pg.line(plotgui.xary[i],plotgui.yary[i])
    if (has_field(plotgui,"aux") && globalpar.SpawnID>0 && overplotaux) {
        if (i<=plotgui.aux.nplotted) {
          plotgui.pg.sci(plotgui.panelsci[i]+1)
      	  plotgui.pg.line(plotgui.aux.xary[i],plotgui.aux.yary[i])
        }
    }
    plotgui.pg.ebuf();
  }
  if (selection) plotgui.highlightselection()
  plotgui.endwait();
}
