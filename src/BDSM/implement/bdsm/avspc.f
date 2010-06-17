c! avspc in N dimensions

        subroutine avspc(scratch,filename,imagename,runcode,
     /             n,m,l,bchan,echan,rms)
        implicit none
        integer n,m,l,bchan,echan,nchan,i,j,k
        real*8 image(n,m,l),ch0(n,m),s1,pixval,rms(l),s2,s3
        character scratch*500,filename*500,imagename*500,runcode*2
        character extn*20
        logical blanked

        s2=0.d0
        nchan=0
        do i=1,l
         if (rms(i).ne.0.d0) then
          s2=s2+1.d0/rms(i)/rms(i)
          nchan=nchan+1
         end if
        end do

        extn='.img'
        call readarray_bin3D(n,m,l,image,n,m,l,filename,extn)
        do j=1,m
         do i=1,n
          s1=0.d0
          s3=0.d0
          blanked=.false.
          do k=bchan,echan
           if (rms(k).ne.0.d0) then
            pixval=image(i,j,k) 
            if (pixval.ne.-999.d0) then
             s1=s1+image(i,j,k)/rms(k)/rms(k) 
            else
             blanked=.true.
            end if
           end if ! rms ne 0
          end do
          if (blanked) then
           ch0(i,j)=-999.d0
          else
           ch0(i,j)=s1/s2
          end if
         end do
        end do
        call writearray_bin2D(ch0,n,m,n,m,imagename,runcode)

        return
        end

