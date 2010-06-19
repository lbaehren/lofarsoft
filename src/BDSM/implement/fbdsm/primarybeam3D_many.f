c! put in effect of primary beam.

        subroutine primarybeam3D_many(f1,imsizex,imsizey,nchan,cdelt1,
     /        freq,dia,npoint,xc,yc,maxpt,scratch)
        implicit none
        include "includes/constants.inc"
        integer imsizex,imsizey,nchan,i,j,k,maxpt,npoint,n,m,z,nchar
        integer xc(maxpt),yc(maxpt),ipt
        real*8 image(imsizex,imsizey,nchan),freq,dia
        real*8 cdelt1,sig,dumr,s1
        character f1*500,f2*500,extn*20,fn*500,scratch*500

        sig=c/freq/dia*rad*3600.d0/cdelt1/fwsig   ! pribeam sigma in pixels

        extn='.img'
        f2=f1(1:nchar(f1))//'.src3D'
        call readarraysize(f2,extn,n,m,z)
        fn=scratch(1:nchar(scratch))//f2(1:nchar(f2))//
     /     extn(1:nchar(extn))
        open(unit=21,file=fn(1:nchar(fn)),
     /       status='old',form='unformatted')
        if (n.ne.imsizex.or.m.ne.imsizey.or.z.ne.nchan) then
         write (*,*) '  ### Sizes dont match. Problem'
         goto 333
        end if
        close(21)
        extn='.img'
        call readarray_bin3D(n,m,z,image,imsizex,imsizey,nchan,f2,extn)

c        do k=1,nchan
c         do i=1,imsizex
c          do j=1,imsizey
c           s1=0.d0
c           do ipt=1,npoint
c            dumr=dexp((-0.5d0)*(((i-xc(ipt))*(i-xc(ipt))+
c     /                 (j-yc(ipt))*(j-yc(ipt)))/sig/sig))
c            s1=s1+dumr
c           end do
c           image(i,j,k)=image(i,j,k)*s1
c          end do
c         end do
c        end do
        
        f2=f1(1:nchar(f1))//'.src3D.pbeam'
        call writearray_bin3D(image,imsizex,imsizey,nchan,f2,'mv')

333     continue
        return
        end


