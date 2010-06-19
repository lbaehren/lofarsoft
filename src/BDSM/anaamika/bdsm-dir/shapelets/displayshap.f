c! for plotting 4 images, passed as arrays, with text for shapelets
c! they are the image, shapelet reconstruction, resid, and coeff matrix.

        subroutine displayshap(im1,im2,im3,cf,subn,subm,nmax,
     /             beta,xc,yc,delx,dely,fname)
        implicit none
        integer subn,subm,nmax,nc,delx,dely
        real*8 im1(subn,subm),im2(subn,subm),im3(subn,subm)
        real*8 cf(nmax,nmax),dumr,beta,xc,yc
        character strdev*5,lab*500
        character extn*20,fname*500
        integer n,m,n1,m1,nchar,i,j,n2,m2,round
        real*4 low1,up1,low2,up2,lowd,upd,low3,up3

        call arr2dnz(im1,subn,subm,subn,subm,low1,up1)
        call arr2dnz(im2,subn,subm,subn,subm,low2,up2)
        low2=low1
        up2=up1

        call arr2dnz(im3,subn,subm,subn,subm,lowd,upd)
        
        low3=cf(nmax,1)
        up3=cf(nmax,1)
        do 400 i=1,nmax
         do 410 j=1,nmax-i
          if (i*j.ne.1) then
           if (cf(i,j).gt.up3) up3=cf(i,j)
           if (cf(i,j).lt.low3) low3=cf(i,j)
          end if
410      continue
400     continue
        up3=sqrt(up3*cf(1,1))

        strdev='/xs'
        call pgbegin(0,strdev,2,2)

        call pgpanl(1,1)
        lab='Image'
        call plotimage2(subn,subm,subn,subm,im1,up1,low1,lab,'LG')

        call sub_displayshap_head(subn,subm,fname,beta,nmax,xc,yc,
     /       delx,dely)

        call pgpanl(2,1)
        lab='Shapelet reconstruction'
        call plotimage2(subn,subm,subn,subm,im2,up2,low2,lab,'RG')

        call pgpanl(1,2)
        lab='Residual image'
        call plotimage2(subn,subm,subn,subm,im3,upd,lowd,lab,'LG')

        call pgpanl(2,2)
        lab='Shapelet coefficient matrix'
        call plotimage2(nmax,nmax,nmax,nmax,cf,up3,low3,lab,'RG')

        call pgend

        return
        end
c!
c!
c!
        subroutine plotimage2(x,y,n,m,arr,up,low,lab,str2)
        implicit none
        integer n,m,i,j,x,y
        character lab*500,str2*2
        real*8 arr(x,y)
        real*4 up,low,array(n,m),tr(6)

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/

        call array8to4(arr,x,y,array,n,m)
        if (up/low.gt.10.0) then   
         call converttolog(array,n,m,low,up)
        end if
        call pgsch(1.3)
        call pgvport(0.15,0.85,0.15,0.85)
        call pgwindow(0.5,n*1.0+0.5,0.5,m*1.0+0.5)
        call pgbox('BCINST',0.0,0,'BCINST',0.0,0)
        call pggray(array,n,m,1,n,1,m,up,low,tr)
        call pglabel(' ',' ',lab)
        call pgwedg(str2,2.0,3.0,up,low,' ')
        call pgsch(1.0)

        return
        end
c!
c!
c!
        subroutine sub_displayshap_head(subn,subm,fname,beta,nmax,xc,yc,
     /       delx,dely)
        implicit none
        integer subn,subm,nmax,delx,dely,round,nchar,nc
        real*8 xc,yc,beta
        character fname*500,dumc*500,dumc1*500,dumc2*500

        call pgsch(1.3)
        call pgtext(subn*0.85,-0.1*subm,'File       :  '//
     /       fname(1:nchar(fname)))
        call pgnumb(abs(round(beta*1.d4)),-4,1,dumc1,nc)
        call pgnumb(abs(round(nmax*1.d4)),-4,1,dumc2,nc)
        call pgtext(subn*0.85,-0.17*subm,'Beta      :  '//
     /       dumc1(1:nchar(dumc1))//';      n_max   :  '//
     /       dumc2(1:nchar(dumc2)))
        call pgnumb(abs(round(xc*1.d4)),-4,1,dumc1,nc)
        call pgnumb(abs(round(yc*1.d4)),-4,1,dumc2,nc)
        dumc='Xcen   :  '//dumc1(1:nchar(dumc1))//
     /       ';   Ycen   :  '//dumc2(1:nchar(dumc2))
        call pgnumb(abs(round(delx*1.d2)),-2,1,dumc1,nc)
        call pgnumb(abs(round(dely*1.d2)),-2,1,dumc2,nc)
        call pgtext(subn*0.7,-0.24*subm,dumc(1:nchar(dumc))//
     /      ';  Del_X   :  '//dumc1(1:nchar(dumc1))//
     /      ';  Del_Y   :  '//dumc2(1:nchar(dumc2)))
        call pgsch(1.0)

        return
        end




