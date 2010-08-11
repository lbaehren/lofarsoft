
        subroutine show_shap(scratch,fitsd,fitsdir,plotd,saveplots,
     /        fullname,isln,
     /        bet,xc,yc,std,av,blc1,blc2,trc1,trc2,srcim_s,image,n,m,
     /        subn,subm,fitsname,nisl,maxnmax,nmax,coef)
        implicit none
        integer n,m,isln,blc1,blc2,trc1,trc2,subn,subm,i,j,l,nisl,nchar
        integer nmax,maxnmax,delx,dely
        real*8 image(n,m),srcim_s(n,m),bet,xc,yc,std,av
        character scratch*500,plotd*500,saveplots*1,fullname*500
        character fitsd*500,fitsname*500,str10*10,f1*500,fname*500
        character fitsdir*500
        real*8 subsrc(subn,subm),subim(subn,subm),resid(subn,subm)
        real*8 coef(maxnmax,maxnmax,nisl),cf(nmax,nmax)

        do j=blc2,trc2
         do i=blc1,trc1
          subsrc(i-blc1+1,j-blc2+1)=srcim_s(i,j)
          subim(i-blc1+1,j-blc2+1)=image(i,j)
          resid(i-blc1+1,j-blc2+1)=image(i,j)-srcim_s(i,j)
         end do
        end do

        do j=1,nmax
         do i=1,nmax
          cf(i,j)=coef(i,j,isln)
         end do
        end do

        call num2str_fn(isln,nisl,str10)
        f1=fullname(1:nchar(fullname))//'.'//str10(1:nchar(str10))
        delx=blc1-1
        dely=blc2-1
        if (saveplots.eq.'y') then
         fname=f1(1:nchar(f1))//'.shap'
         call writefits(subsrc,subn,subm,subn,subm,fname,fitsd,
     /              fitsdir,scratch,fitsname)
         fname=f1(1:nchar(f1))//'.subim'
         call writefits(subim,subn,subm,subn,subm,fname,fitsd,
     /              fitsdir,scratch,fitsname)
         fname=f1(1:nchar(f1))//'.resid.shap'
         call writefits(resid,subn,subm,subn,subm,fname,fitsd,
     /              fitsdir,scratch,fitsname)
         call writefitsshapcoef(f1,fitsdir,fitsd,nmax,cf,
     /        bet,xc,yc,delx,dely)
        else
         call displayshap(subim,subsrc,resid,cf,subn,subm,nmax,
     /              bet,xc,yc,delx,dely,f1)
        end if
        
        return
        end


