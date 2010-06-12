c! create elliptical gaussians

        implicit none   
        character f1*500
        integer i,j,n,m
        real*8 im(32,32)
        real*8 pk,xc,yc,bmaj,bmin,bpa,sx,sy

        write (*,*) ' Enter filename'
        read (*,*) f1
        write (*,*) ' Enter n, m'
        read (*,*) n,m
        write (*,*) ' Enter peak, centres, bmaj bmin bpa'
        read (*,*) pk,xc,yc,bmaj,bmin,bpa
        bpa=bpa*3.14159/180.d0
        sx=bmaj/2.35482d0
        sy=bmin/2.35482d0

        do 100 i=1,n
         do 110 j=1,m
          im(i,j)=pk*dexp(-0.5d0*(
     /    (((i-xc)*dcos(bpa)+(j-yc)*dsin(bpa))/sx)**2.d0+
     /    (((j-yc)*dcos(bpa)-(i-xc)*dsin(bpa))/sy)**2.d0))

c          if (im(i,j).le.pk*1.d-3) im(i,j)=0.d0
110      continue
100     continue

        call writearray_bin(im,32,32,n,m,f1,'mv')
        
        end


        
        
