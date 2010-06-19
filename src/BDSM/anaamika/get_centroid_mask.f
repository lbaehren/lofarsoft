
        subroutine get_centroid_mask(subim,mask,subn,subm,xcen,ycen)
        implicit none
        integer subn,subm,mask(subn,subm),i,j
        real*8 subim(subn,subm),xcen,ycen,tot

        xcen=0.d0
        ycen=0.d0
        tot=0.d0
        do i=1,subn
         do j=1,subm
          if (mask(i,j).eq.1) then
           tot=tot+subim(i,j)
           xcen=xcen+i*subim(i,j)
           ycen=ycen+j*subim(i,j)
          end if
         end do
        end do 
        xcen=xcen/tot
        ycen=ycen/tot

        return
        end

