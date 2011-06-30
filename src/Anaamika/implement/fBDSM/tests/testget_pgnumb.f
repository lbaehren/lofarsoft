

        integer nchar
        character str*500
        real*4 avx
        integer mm,pp,round4,ii,i,ind

        write (*,*) 'a = ?'
        read (*,*) avx


        if (avx.ge.1) then
         mm=round4(avx*1000.0)
         pp=-3
        else
         write (str,*) avx
         ii=0
333      ii=ii+1
         if (str(ii:ii).ne.'.') goto 333
c!       now ii is posn of decimal point
         do i=ii+1,nchar(str)
          if (str(i:i).ne.'0') then
           ind=i
           goto 444
          end if
         end do
444      continue         
c!       now ind is posn of first non-zero number, so
         mm=round4(avx*(10**(ind-ii+4)))
         pp=-ind+ii-4
        end if


        write (*,*) avx ,' = ',mm,' X 10^',pp

        
        end
