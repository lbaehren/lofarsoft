********************************************************************
      function length(string)
********************************************************************
c     
c     returns the length of 'string' excluding any trailing spaces.
c     
      character string*(*)
c     
c     obtain the location of the last non-space character.
c     
      do 1 i=len(string),1,-1
         if(string(i:i).ne.' ') then
c     
c     length found.
c     
            length=i
            return
         endif
    1 continue
c     
c     string is all spaces or zero length.
c     
      length=0
      return
c     
c     end of integer function length.
c     
      end
