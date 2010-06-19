
c! finds one coeff of cartesian shapelet for an image. hermite poly is input. 
c! arr is image of size n,m
c! nx,ny is order+1 in x and y dir we are decomposing
c! hcx,hcy have the hermite poly coeffs for those nx,ny
c! n2,m2 are centres for function in x and y dir
c! nmax is max num in hc to compute.
c! with mask now.

        subroutine decomp_cart_mask(arr,mask,n,m,hc,beta,n2,m2,
     /             nmax,nhc,cf)
        implicit none
        integer n,m,nmax,i,j,nhc,mask(n,m)
        real*8 arr(n,m),beta,cf(nmax,nmax),hc(nhc,nhc)
        real*8 hcx(nhc),hcy(nhc),B_im(n,m),n2,m2

        do 120 i=1,nmax
         call getcoeffarr(hc,nhc,nhc,i,hcx) 
         do 130 j=1,nmax-i
          call getcoeffarr(hc,nhc,nhc,j,hcy)   
          call getcartim(n,m,beta,i,j,hcx,hcy,n2,m2,nmax,B_im) !  B_im=shapelet image
          call decompcartcoeff_mask(arr,mask,n,m,B_im,cf(i,j))
130      continue
120     continue

        return
        end
        
        subroutine decompcartcoeff_mask(arr,mask,n,m,B_im,coeff)
        implicit none
        integer n,m,i,j,mask(n,m)
        real*8 arr(n,m),coeff,B_im(n,m)
        
c! now to get the coeff
        coeff=0.d0
        do 300 j=1,m
         do 310 i=1,n
          coeff=coeff+B_im(i,j)*arr(i,j)*mask(i,j)
310      continue
300     continue

        return
        end


