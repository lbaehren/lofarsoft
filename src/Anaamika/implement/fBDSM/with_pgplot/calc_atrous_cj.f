c! here i calculate the c_j image defined as
c! c_j (k) = Sum_l H(l) c_(j-1) (k+ 2^(j-1)l)
c! where spacing is 2^(j-1)

        subroutine calc_atrous_cj(image,n,m,cj,hsize,hmat,j,std,
     /             opt,meds)
        implicit none
        integer n,m,hsize,j,space,ii,jj,cen,edge,edge1,edge2
        integer ih,jh,opt,meds
        real*8 image(n,m),cj(n,m),hmat(hsize,hsize),std,val
        real*8 vec(meds*meds),median

        if (opt.eq.1) then    ! a trous multiscale transform
         cen=(hsize+1)/2
         space=2**(j-1) * 1
         do jj=1,m
          do ii=1,n
           cj(ii,jj)=0.d0
           edge=space*(hsize-cen)
           if (ii-edge.lt.1.or.ii+edge.gt.n.or.
     /         jj-edge.lt.1.or.jj+edge.gt.m) then
            do jh=1,hsize
             do ih=1,hsize
              edge1=ii+(ih-cen)*space
              edge2=jj+(jh-cen)*space
              if (edge1.lt.1.or.edge1.gt.n.or.edge2.lt.1.or.edge2.gt.m) 
     /        then    
               val=std 
              else
               val=image(ii+(ih-cen)*space,jj+(jh-cen)*space)
              end if
              cj(ii,jj)=cj(ii,jj)+hmat(ih,jh)*val
             end do
            end do
           else
            do jh=1,hsize
             do ih=1,hsize
              cj(ii,jj)=cj(ii,jj)+hmat(ih,jh)*
     /                  image(ii+(ih-cen)*space,jj+(jh-cen)*space)
             end do
            end do
           end if   ! over the edge 
          end do
         end do
        end if

c! meds is 2s+1 = size of kernel. so 2d kernel is medsXmeds. meds odd.
        if (opt.eq.2) then    ! median multiscale transform
         do jj=1,m
          do ii=1,n
           cj(ii,jj)=0.d0
           do jh=1,meds
            do ih=1,meds
             edge1=ii+ih-(meds+1)/2
             edge2=jj+jh-(meds+1)/2
             if (edge1.lt.1.or.edge1.gt.n.or.edge2.lt.1.or.edge2.gt.m) 
     /       then
              vec(ih+meds*(jh-1))=std 
             else
              vec(ih+meds*(jh-1))=image(edge1,edge2)
             end if      
            end do
           end do
           call c_calcmedian(vec,meds*meds,median)
           cj(ii,jj)=median
          end do
         end do
        end if

        return
        end


