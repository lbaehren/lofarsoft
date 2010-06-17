c! for a vector and median and std around median, calc number of pts
c! outside and cut them out of the same array

        subroutine get_medianclip_vec(vec,x,n,med,medstd,nsig,nout)
        implicit none
        integer x,n,nout,i,nin
        real*8 vec(x),med,medstd,dvec(x),nsig

        do i=1,n
         dvec(i)=vec(i)
        end do

        nin=0
        do i=1,n
         if (abs(dvec(i)-med).le.nsig*medstd) then
          nin=nin+1
          vec(nin)=dvec(i)          
         end if
        end do
        nout=n-nin

        return
        end



