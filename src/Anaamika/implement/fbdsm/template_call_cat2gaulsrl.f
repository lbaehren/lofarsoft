c! calls write_gaulsrl to write a catalogue to bdsm format.
c! copy read-file to this name.

      subroutine call_cat2gaulsrl(scratch,srldir,runcode)
      implicit none
      character scratch*500,srldir*500,runcode*2

      

      call write_gaulsrl(f1,runcode,scratch,srldir,ngau,
     /   peak,epeak,tot,etot,ra,era,dec,edec,bmaj,ebmaj,bmin,ebmin,bpa,
     /   ebpa,dbmaj,edbmaj,dbmin,edbmin,dbpa,edbpa,rstd,rav,sstd,sav,
     /   spin,espin,imrms,dumr2,dumr3,dumr4,dumr5,dumr6,cbmaj,cbmin,
     /   cbpa,freq,catrms,nam)



      return
      end



