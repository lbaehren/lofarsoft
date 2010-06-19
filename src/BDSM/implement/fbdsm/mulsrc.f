
        subroutine mulsrc(f1)
        implicit none
        character f1*(*),extn*20
        integer n,m,l

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_mulsrc(f1,n,m)
        
        return
        end


        subroutine sub_mulsrc(f1,n,m)
        implicit none
        character f1*(*),extn*20
        integer n,m,i,j,coord,xsurr(8),ysurr(8),fac,i1
        real*8 image(n,m),std,av,nsig
        logical okay,pixinimage

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        call sigclip(image,n,m,1,1,n,m,std,av,3.d0)

        write (*,'(a,$)') '  What n-sigma for identifying peaks ? '
        read (*,*) nsig

        call pgbegin(0,'/xs',1,1)
        call pgvport(0.1,0.9,0.1,0.9)
        call plotimage(image,n,m,n,m)
        call pgsci(2)
        fac=1000
        do 100 j=1,m
         do 110 i=1,n
          if (image(i,j)-av.ge.nsig*std) then
           coord=j*fac+i
           call getsurr_8pix(coord,xsurr,ysurr,fac) 
           okay=.true.
           do i1=1,8
            if (pixinimage(xsurr(i1),ysurr(i1),1,n,1,m)) then
             okay=okay.and.(image(i,j).gt.image(xsurr(i1),ysurr(i1)))
            end if
           end do
           if (okay) then
            call pgpoint(1,i*1.0,j*1.0,2)
           end if
          end if
110      continue
100     continue
        call pgsci(1)
        call pgend
        
        return
        end

