c! for a vector and median and std around median, calc number of pts
c! outside and cut them out of the same array
c! modify to take one more array with u

        subroutine get_medianclip_vec2(vec,vec2,x,n,med,medstd,
     /             nsig,nout)
        implicit none
        integer x,n,nout,i,nin
        real*8 vec(x),med,medstd,dvec(x),nsig,vec2(x),dvec2(x)

        do i=1,n
         dvec(i)=vec(i)
         dvec2(i)=vec2(i)
        end do

        nin=0
        do i=1,n
         if (abs(dvec(i)-med).le.nsig*medstd) then
          nin=nin+1
          vec(nin)=dvec(i)
          vec2(nin)=dvec2(i)
         end if
        end do
        nout=n-nin

        return
        end



