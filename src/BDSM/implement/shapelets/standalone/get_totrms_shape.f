c! 

        subroutine get_totrms_shape(image,mask,x,y,n,m,total,
     /             rms,sav,fname,nhc,hc,nmax)
        implicit none
        integer x,y,n,m,num,mask(x,y),nmax,i,j,nchar,nhc
        real*8 image(x,y),total,rms,dumr,cf(nmax,nmax)
        real*8 hc(nhc,nhc),a(6),chisq,xcen,ycen,std,av
        real*8 beta,rarr(x,y),resid(x,y),q
        character fcf*500,filec*500,fileo*500,f1*500,sav*1,fname*500
        character dir*500

c! fix centre from findcen, nmax as 8 (adjust later), and beta from gaussian fit
        call fit1gauss(image,mask,x,y,n,m,a,chisq,q)
        beta=sqrt(a(4)*a(5))
        call sigclip(image,x,y,1,1,n,m,std,av,5)
        call s_findcen(image,n,m,hc,nhc,beta,nmax,num,xcen,ycen,
     /       std,av,'mv')
        write (*,*) ' PARAMETERS beta xcen ycen = ',beta,xcen,ycen
        
c! decompose and reconstruct shapelet
        call initialiseimage(cf,nmax,nmax,nmax,nmax,0.d0)
        call decomp_cart(image,n,m,hc,beta,xcen,ycen,nmax,nhc,cf)
        call recons_cart(n,m,hc,beta,xcen,ycen,nmax,cf,rarr,nhc)
        
c! total is computed over entire reconstructed image ans rms in unmasked area
        do 200 i=1,n
         do 210 j=1,m
          resid(i,j)=image(i,j)-rarr(i,j)
210      continue
200     continue
        call arrstatmask(resid,mask,x,y,1,1,n,m,rms,dumr)
        call arrstat(rarr,x,y,1,1,n,m,dumr,av)
        total=av*n*m
        
        if (sav.eq.'y') then
         filec=fname(1:nchar(fname))//'.coeff'
         fileo=fname(1:nchar(fname))//'.recons'
         call writearray_bin2D(cf,nmax,nmax,nmax,nmax,filec,'aq')
         call writearray_bin2D(rarr,n,m,n,m,fileo,'aq')
c         call s_displayshap(fname,fileo,filec)
        end if

        return
        end


