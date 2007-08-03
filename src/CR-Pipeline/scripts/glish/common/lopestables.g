#File: Routines to create a table for lopes. For now only used for
#the event selection table.

#------------------------------------------------------------------------
#Name: makeltab
#------------------------------------------------------------------------
#Type: function
#Ret: Table
#Doc: This will convert Horneffer's event table evtab into an aips++ table.
#     Returns whether a data set was selected or not. You can then view
#     and sort this table with ltab.browse()
#
#Par: evtab  - the event table 
#------------------------------------------------------------------------

makeltab:=function(evtab){
  scd:=[=]
  n:=0
  nrow:=shape(evtab[field_names(evtab)[1]])[1]

  scd0:=tablecreatescalarcoldesc('ID',1)

for (f in field_names(evtab)) {
  s:=shape(evtab[f])
  dim:=len(s)
  if (dim==2) {
  n+:=1;
    scd[n]:=tablecreatearraycoldesc(f,evtab[f][s],shape=s[seq(2,len(s))])
  } else if (dim==1) {
  n+:=1;
    scd[n]:=tablecreatescalarcoldesc(f,evtab[f][1])
  } else {print 'Leaving out',f,'- too many dimensions.'};
}
td:=tablecreatedesc(scd0)
ltab:=table("ltab",tabledesc=td,nrow=nrow,readonly=F)
ltab.putcol('ID',seq(nrow))

for (i in seq(n)) {
  f:=scd[i].name;
  s:=shape(evtab[f]); dim:=len(s); l:=s[1]
  ltab.addcols(scd[i])
  if (dim==2) {
     if (nrow==l) ltab.putcol(f,mx.transpose(evtab[f]))
  } else if (dim==1) {
    if (nrow==l && len(evtab[f])==nrow) ltab.putcol(f,evtab[f])
  }
 }
ltab.addreadmeline('This is a LOPES event table created my makeltab.')
ltab.done()
ltab:=table('ltab')
return ltab
}

