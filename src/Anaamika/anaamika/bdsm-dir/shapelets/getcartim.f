c! returns the image of an cartesian shapelet from vectors hcx, hcy with
c! centres n2,m2, and beta and order nx,ny
        subroutine getcartim(n,m,beta,nx,ny,hcx,hcy,n2,m2,
     /             nmax,B_im)
        implicit none
        integer n,m,i,j,nx,ny,nmax
        real*8 beta,hcx(50),hcy(50),rfact
        real*8 B_im(n,m),pi,fac,dumr3,B_nx(n),B_ny(m)
        real*8 dumr1,dumr2,x,y,n2,m2
        character f1*500

        pi=3.14159d0

c! create the B_n image
c!                              first the x-axis
        fac=rfact(nx-1)
        dumr1=sqrt((2.d0**(nx-1))*sqrt(pi)*fac)
        do 100 i=1,n
         x=(i*1.d0-n2)/beta
         dumr3=0.d0
         do 110 j=1,nmax  ! doesnt actually depend on nmax of course -- its all zero 
          dumr3=dumr3+hcx(j)*(x**(j-1))
110      continue
         B_nx(i)=dexp((-0.5d0)*x*x)*dumr3/dumr1/sqrt(beta)
100     continue

c!                              then the y-axis
        fac=rfact(ny-1)
        dumr2=sqrt((2.d0**(ny-1))*sqrt(pi)*fac)
        do 130 i=1,m
         y=(i*1.d0-m2)/beta
         dumr3=0.d0
         do 140 j=1,nmax
          dumr3=dumr3+hcy(j)*(y**(j-1))
140      continue
         B_ny(i)=dexp((-0.5d0)*y*y)*dumr3/dumr2/sqrt(beta)
130     continue

c!                              then the tensor product
        do 200 i=1,n
         do 210 j=1,m
          B_im(i,j)=B_nx(i)*B_ny(j)
210      continue
200     continue

        return
        end


