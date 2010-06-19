c! calls the subroutines to create noise map with correct normalisation
c! and convolves and puts in a file.

        subroutine sim_cr8noise(fn,sens,nchan,imsize,bmsampl,seed)
        implicit none
        include "includes/constants.inc"
        character fn*500
        integer imsize,seed,i,j,wid,nchan
        real*8 sens,bmsampl
        real*8 s1,fac,sig_c(3) 

c! compute wid, fac, s1
        sig_c(1)=bmsampl/fwsig
        sig_c(2)=bmsampl/fwsig
        sig_c(3)=0.d0
        call conv2_get_wid_fac(sig_c,imsize,imsize,s1,fac,wid)

c! call actual prog to do the job (need to know wid for defining g compactly)
        write (*,*) '  Creating noise map ... '
        call do_simcr8noise(fn,sens,nchan,imsize,bmsampl,seed,
     /          fac,s1,wid)
        
        return
        end

c! do the actual work
        subroutine do_simcr8noise(fn,sens,nchan,imsize,bmsampl,seed,
     /             fac,s1,wid)
        implicit none
        include "includes/constants.inc"
        character fn*500,filen*500
        integer imsize,seed,i,j,wid,i1,j1,i2,j2
        integer nchar,nchan,ich
        real*8 sens,bmsampl,a,gasdev,fac,dumr
        real*8 s1,g(wid,wid),image(imsize,imsize)
        real*8 cn(imsize,imsize,nchan),f1

c! first create the convolving gaussian
        do 120 i=1,wid
         do 130 j=1,wid
          i1=(wid+1)/2
          g(i,j)=dexp((-1.d0)*((i-i1)**2.d0+(j-i1)**2.d0)/
     /           (2.d0*bmsampl*bmsampl/fwsig/fwsig))
130      continue
120     continue

c! copy here code of xcreatenoise.f, adjusting rms for 'total' to give correct value
c! make image as 2d for each channel and put in convolved plane into cube.
        write (*,'(a31)') '   Convolving the noise map ... '
        f1=sens/fac
        do 500 ich=1,nchan
         do 100 i=1,imsize
          do 110 j=1,imsize
           a=gasdev(seed)
           image(i,j)=a*f1
110       continue
100      continue

c! now convolve with copied conv2.f here, not yet fft

         do 140 i=1,imsize
          do 150 j=1,imsize
 
           cn(i,j,ich)=0.d0
           do 160 i1=1,wid
            do 170 j1=1,wid
             i2=i-int(wid/2)+(i1-1)
             j2=j-int(wid/2)+(j1-1)
             if (i2.lt.1.or.j2.lt.1.or.i2.gt.imsize.or.j2.gt.
     /                                           imsize) then
              call perbound(image,imsize,imsize,i2,j2,
     /                       imsize,imsize,dumr)  
             else
              dumr=image(i2,j2)
             end if
             cn(i,j,ich)=cn(i,j,ich)+dumr*g(i1,j1)
170         continue
160        continue
           cn(i,j,ich)=cn(i,j,ich)/s1
150       continue
140      continue
500     continue  ! ich

        filen=fn(1:nchar(fn))//'.noi'
        call writearray_bin3D(cn,imsize,imsize,nchan,filen,'mv')

        return
        end

c! put periodic boundary conditions
        subroutine perbound(arr,x,y,i2,j2,n,m,dumr)  ! periodic boundary conditions
        implicit none
        integer i2,j2,n,m,i,j,x,y
        real*8 dumr,arr(x,y)

        i=i2
        j=j2
        if (i2.lt.1) i=n+i2
        if (j2.lt.1) j=m+j2
        if (i2.gt.n) i=i2-n
        if (j2.gt.m) j=j2-m
        dumr=arr(i,j)

        return
        end

