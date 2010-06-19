c! finds one coeff of cartesian shapelet for an image. hermite poly is input. 
c! arr is image of size n,m
c! nx,ny is order+1 in x and y dir we are decomposing
c! hcx,hcy have the hermite poly coeffs for those nx,ny
c! n2,m2 are centres for function in x and y dir
c! nmax is max num in hc to compute

        subroutine decomp_cart(arr,n,m,hc,beta,n2,m2,nmax,nhc,cf)
        implicit none
        integer n,m,nmax,i,j,nhc
        real*8 arr(n,m),beta,cf(nmax,nmax),hc(nhc,nhc)
        real*8 hcx(nhc),hcy(nhc),B_im(n,m),n2,m2
        character f*500,rc*2

        do 120 i=1,nmax
         call getcoeffarr(hc,nhc,nhc,i,hcx) 
         do 130 j=1,nmax-i+1
          call getcoeffarr(hc,nhc,nhc,j,hcy)   
          call getcartim(n,m,beta,i,j,hcx,hcy,n2,m2,nmax,B_im) !  B_im=shapelet image
          call decompcartcoeff(arr,n,m,B_im,cf(i,j))
130      continue
120     continue

        return
        end
        
        subroutine decompcartcoeff(arr,n,m,B_im,coeff)
        implicit none
        integer n,m,i,j
        real*8 arr(n,m),coeff,B_im(n,m)
        
c! now to get the coeff
        coeff=0.d0
        do 300 i=1,n
         do 310 j=1,m
          coeff=coeff+B_im(i,j)*arr(i,j)
310      continue
300     continue

        return
        end


