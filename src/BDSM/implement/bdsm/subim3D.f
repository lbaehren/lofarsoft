c! subim for 3d

        subroutine subim3D(f1,nax1,nax2,nax3,sub_,ptsizex,
     /             ptsizey,nchan,blc,trc)
        implicit none
        integer nax1,nax2,nax3,ptsizey,nchan,blc(3),trc(3)
        integer ptsizex
        real*8 sub_(ptsizex,ptsizey,nchan)
        real*8 array(nax1,nax2,nax3)
        character f1*500,extn*20
        integer nchar,i,j,k,n,m,z

        extn='.img'
        call readarraysize(f1,extn,n,m,z)
        if (n.ne.nax1.or.m.ne.nax2.or.z.ne.nax3) then
         write (*,*) '  ### Sizes dont match. Problem'
        write (*,*) n,m,z,nax1,nax2,nax3
         goto 333
        end if
        call readarray_bin3D(n,m,z,array,nax1,nax2,nax3,f1,extn)

        do 100 i=blc(1),trc(1)
         do 120 j=blc(2),trc(2)
          do 130 k=blc(3),trc(3)
           sub_(i-blc(1)+1,j-blc(2)+1,k-blc(3)+1)=array(i,j,k)
130       continue
120      continue
100     continue

333     continue
        return
        end


