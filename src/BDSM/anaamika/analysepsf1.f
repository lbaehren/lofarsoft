c! take input psf variation (from simulation --> .bmajor and .bminor) and compare with
c! .gaul o/p of bdsm and do clever things. Assume all sources are unresolved, so 
c! all measured sizes are psf.

c! first check if variation is significant within errors. 

c! if significant then fit a function (predefined, from setvarypsf) and output parameters
c! to compare with inputs.
c! no, that will work anyway -- this was just to show that i can reproduce bmaj, bmin
c! properly. will now start on real data. look at analysepsf2.

        subroutine analysepsf1(f1,scratch,srldir)
        implicit none
        character f1*500,f2*500,f3*500,scratch*500,extn*20,srldir*500
        integer nchar,i,dumi,n,m,ngau,l
        real*8 dumr
        logical exists

        do i=nchar(f1),1,-1
         dumi=i
         if (f1(i:i).eq.'.') goto 333
        end do
333     continue
        if (dumi.eq.1) dumi=nchar(f1)
        f2=f1(1:dumi-1)
        
        if (.not.exists(f2(1:nchar(f2))//'.bminor',
     /      scratch,'.img')) goto 444
        if (.not.exists(f2(1:nchar(f2))//'.bmajor',
     /      scratch,'.img')) goto 444

        extn='.img'
        call readarraysize(f2,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        
        f3=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.gaul'
        open(unit=22,file=f3(1:nchar(f3)),form='formatted')
        call readhead_srclist(22,19,'Number_of_gaussians',f3,ngau,
     /       dumr,'i')
        close(22)
        call sub_analysepsf1(f1,f2,scratch,srldir,n,m,ngau)


444     continue
        return
        end

        subroutine sub_analysepsf1(f1,f2,scratch,srldir,n,m,ngau)
        implicit none
        character f1*500,f2*500,scratch*500,extn*20,srldir*500,f3*500
        integer nchar,i,dumi,n,m,ngau,dumi1,flag(ngau),round
        integer iwksp(ngau),subn,subm,xind(ngau),yind(ngau)
        real*8 dumr,peak(ngau),bmaj(ngau),ebmaj(ngau),bmin(ngau)
        real*8 ebmin(ngau),bpa(ngau),ebpa(ngau),ndigit
        real*8 imbmajor(n,m),imbminor(n,m),imbpa(n,m)
        real*8 xpix(ngau),ypix(ngau),wksp(ngau),ctr(ngau)

c! read in stuff and do a ypix*10^n+xpix to sort the whole 
c! gaul blc to brc, and then to trc later
        ndigit=10.d0**(int(log10(n*1.0))+1)
        f3=srldir(1:nchar(srldir))//f1(1:nchar(f1))//'.gaul.bin'   
        open(unit=22,file=f3(1:nchar(f3)),form='unformatted') 
        do i=1,ngau
         read (22) dumi1,dumi,flag(i),dumr,dumr,peak(i),dumr,
     /    dumr,dumr,dumr,dumr,xpix(i),dumr,ypix(i),dumr,bmaj(i),
     /    ebmaj(i),bmin(i),ebmin(i),bpa(i),ebpa(i),dumr,dumr,dumr,
     /    dumr,dumr,dumr,dumr,dumr,dumr,dumr,dumr,dumi,dumi
         if (dumi1.ne.i) write (*,*) ' @@@ ERRORRRR !!! ',i,dumi1
         ctr(i)=(round(ypix(i))*ndigit+round(xpix(i)))*1.d0
        end do
        close(22)

        extn='.img'
        f3=f2(1:nchar(f2))//'.bmajor'
        call readarray_bin(n,m,imbmajor,n,m,f3(1:nchar(f3)),extn)
        f3=f2(1:nchar(f2))//'.bminor'
        call readarray_bin(n,m,imbminor,n,m,f3(1:nchar(f3)),extn)

        write (*,*) ' Assuming that the gaussians are on a grid !! '

c! now sort them
        call sort3_analysepsf1(ngau,ctr,flag,peak,xpix,ypix,bmaj,
     /       ebmaj,bmin,ebmin,bpa,ebpa,wksp,iwksp)
        do i=2,ngau
         if (xpix(i)+3.lt.xpix(i-1)) then    ! in case centre is a bit shifted. hack !
          subn=i-1
          goto 444
         end if 
        end do
444     continue
        subm=ngau/subn
        if (subm.ne.int(subm)) write (*,*) ' !! Not GRIDDED !!'
c! assign subn,subm indices
        do i=1,ngau
         xind(i)=mod(i-1,subn)+1
         yind(i)=((i-mod(i-1,subn)-1)/subn)+1
        end do

        call calc_beam_e_im(subn,subm,ngau,xind,yind,bmaj,bmin,ebmaj,
     /       ebmin,xpix,ypix,imbmajor,imbminor,n,m,f1)

        return
        end


        subroutine calc_beam_e_im(subn,subm,ngau,xind,yind,bmaj,
     /    bmin,ebmaj,ebmin,xpix,ypix,imbmajor,imbminor,n,m,f1)
        implicit none
        integer ngau,subn,subm,xind(ngau),yind(ngau),i,j,round,nchar
        integer n,m,mask(subn,subm),ia(3)
        real*8 bmaj(ngau),bmin(ngau),ebmaj(ngau),ebmin(ngau)
        real*8 xpix(ngau),ypix(ngau),imbmajor(n,m),imbminor(n,m)
        real*8 bmaj_e_im(subn,subm),bmin_e_im(subn,subm)
        real*8 std,av,stdclip1,avclip1,mx,mn,mxv,mnv,blankv
        real*8 stdclip2,avclip2,bmaj_v(subn*subm),bmin_v(subn*subm)
        real*8 d,prob,ks_gaus,kappa,a_maj(3)
        real*8 bmaj_o_im(subn,subm),bmin_o_im(subn,subm)
        real*8 bmaj_o_e_im(subn,subm),bmin_o_e_im(subn,subm)
        real*8 covarmaj(3,3),alpha(3,3),chisq,alamda
        real*8 covarmin(3,3),a_min(3)
        character f1*500,f2*500,ch1*1
        logical blanky,bmaj_sl,bmin_sl
        external ks_gaus

        data ia/1,1,1/

c! calc (ob-real)/error images
        do i=1,ngau
         bmaj_e_im(xind(i),yind(i))=(bmaj(i)-imbmajor(round(xpix(i)),
     /     round(ypix(i))))/ebmaj(i)
         bmin_e_im(xind(i),yind(i))=(bmin(i)-imbminor(round(xpix(i)),
     /     round(ypix(i))))/ebmin(i)
         bmaj_o_im(xind(i),yind(i))=bmaj(i)
         bmin_o_im(xind(i),yind(i))=bmin(i)
         bmaj_o_e_im(xind(i),yind(i))=ebmaj(i)
         bmin_o_e_im(xind(i),yind(i))=ebmin(i)
         mask(xind(i),yind(i))=1
        end do        

        kappa=3.d0
        f2=f1(1:nchar(f1))//'.e_bmaj'
        call writearray_bin2D(bmaj_e_im,subn,subm,subn,subm,f2,'aq')
        call get_imagestats2D(f2,kappa,subn,subm,std,av,stdclip1,
     /       avclip1,mx,mn,mxv,mnv,blanky,blankv)

        f2=f1(1:nchar(f1))//'.e_bmin'
        call writearray_bin2D(bmin_e_im,subn,subm,subn,subm,f2,'aq')
        call get_imagestats2D(f2,kappa,subn,subm,std,av,stdclip2,
     /       avclip2,mx,mn,mxv,mnv,blanky,blankv)

        f2=f1(1:nchar(f1))//'.o_bmaj'
        call writearray_bin2D(bmaj_o_im,subn,subm,subn,subm,f2,'aq')
        f2=f1(1:nchar(f1))//'.o_bmin'
        call writearray_bin2D(bmin_o_im,subn,subm,subn,subm,f2,'aq')
        
c! do a KS test. I am a moron.
        call arr8tovec8(bmaj_e_im,subn,subm,subn,subm,bmaj_v,subn*subm)
        call arr8tovec8(bmin_e_im,subn,subm,subn,subm,bmin_v,subn*subm)
        do i=1,subn*subm                                ! normalise to mean 0, std 1
         bmaj_v(i)=(bmaj_v(i)-avclip1)/stdclip1
         bmin_v(i)=(bmin_v(i)-avclip2)/stdclip2
        end do

        call ksone(bmaj_v,subn*subm,ks_gaus,d,prob)
        write (*,*) d,prob,' WRONG !!'
        call ksone(bmin_v,subn*subm,ks_gaus,d,prob)
        write (*,*) d,prob,' WRONG !!'

        write (*,*) '  Has BDSM done a good job (y)/n ? '
        read (*,'(a1)') ch1
        if (ichar(ch1).eq.32) ch1='y'
        if (ch1.eq.'n') goto 334

c! if good, then see if there is a slope first
        a_min(1)=0.d-3
        a_min(2)=0.d-3
        a_min(3)=bmin_o_im(1,1)
        alamda=-1.d0
        do i=1,10
         call mrqmin2dm(bmin_o_im,mask,bmin_o_e_im,subn,subm,a_min,ia,3,
     /        covarmin,alpha,3,chisq,alamda,3)
        end do
        alamda=0.d0
        call mrqmin2dm(bmin_o_im,mask,bmin_o_e_im,subn,subm,a_min,ia,3,
     /        covarmin,alpha,3,chisq,alamda,3)
        do i=1,subn
         do j=1,subm
          bmin_e_im(i,j)=(bmin_o_im(i,j)-
     /                       (a_min(1)*i+a_min(2)*j+a_min(3)))/ebmin(i)
         end do
        end do
        f2=f1(1:nchar(f1))//'.bmin_fit'
        call writearray_bin2D(bmin_e_im,subn,subm,subn,subm,f2,'aq')
        
        a_maj(1)=0.d-3
        a_maj(2)=0.d-3
        a_maj(3)=bmaj_o_im(1,1)
        alamda=-1.d0
        do i=1,10
         call mrqmin2dm(bmaj_o_im,mask,bmaj_o_e_im,subn,subm,a_maj,ia,3,
     /        covarmaj,alpha,3,chisq,alamda,3)
        end do
        alamda=0.d0
        call mrqmin2dm(bmaj_o_im,mask,bmaj_o_e_im,subn,subm,a_maj,ia,3,
     /        covarmaj,alpha,3,chisq,alamda,3)
        do i=1,subn
         do j=1,subm
          bmaj_e_im(i,j)=(bmaj_o_im(i,j)-
     /                       (a_maj(1)*i+a_maj(2)*j+a_maj(3)))/ebmaj(i)
         end do
        end do
        f2=f1(1:nchar(f1))//'.bmaj_fit'
        call writearray_bin2D(bmaj_e_im,subn,subm,subn,subm,f2,'aq')
        
        if (abs(a_maj(1)/sqrt(covarmaj(1,1))).gt.1.d0.or.
     /      abs(a_maj(2)/sqrt(covarmaj(2,2))).gt.1.d0) then
         write (*,*) '  Bmajor has a nonzero slope, ',
     /     a_maj(1)/sqrt(covarmaj(1,1)),a_maj(2)/sqrt(covarmaj(2,2))
         bmaj_sl=.true.
        else
         bmaj_sl=.false.
        end if
        if (abs(a_min(1)/sqrt(covarmin(1,1))).gt.1.d0.or.
     /      abs(a_min(2)/sqrt(covarmin(2,2))).gt.1.d0) then
         write (*,*) '  Bminor has a nonzero slope, ',
     /     a_min(1)/sqrt(covarmin(1,1)),a_min(2)/sqrt(covarmin(2,2))
         bmin_sl=.true.
        else
         bmin_sl=.false.
        end if
        
334     continue
        return
        end


