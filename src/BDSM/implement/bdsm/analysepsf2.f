c! try do actual stuff here.
c!

        subroutine analysepsf2(gname,scratch,srldir)
        implicit none
        character gname*500,scratch*500,srldir*500,fname*500,f1*500
        character dumc*500,extn*20,keyword*500,keystrng*500,comment*500
        integer nchar,i,dumi,n,m,ngau,error
        real*8 dumr,cdelt(2),bm_pix(2),cbeam(3)
        logical exists

        do i=nchar(gname),1,-1
         dumi=i
         if (gname(i:i).eq.'.') goto 333
        end do
333     continue
        if (dumi.eq.1) dumi=nchar(gname)+1
        fname=gname(1:dumi-1)
        extn='.img'
        if (.not.exists(fname(1:nchar(fname)),scratch,extn)) goto 444
        extn='.gaul'
        if (.not.exists(gname(1:nchar(gname)),srldir,extn)) goto 444

         write (*,*) 'a1'
        f1=srldir(1:nchar(srldir))//gname(1:nchar(gname))//'.gaul'
        open(unit=22,file=f1(1:nchar(f1)),form='formatted')
        call readhead_srclist(22,12,'Image_size_x',dumc,n,dumr,'i')
        call readhead_srclist(22,12,'Image_size_y',dumc,m,dumr,'i')
        call readhead_srclist(22,19,'Number_of_gaussians',dumc,ngau,
     /       dumr,'i')
        close(22)
         write (*,*) 'a2'

        extn='.header'
        keyword='BMAJ'
        call get_keyword(fname,extn,keyword,keystrng,cbeam(1),
     /       comment,'r',scratch,error)
        keyword='BMIN'
        call get_keyword(fname,extn,keyword,keystrng,cbeam(2),
     /       comment,'r',scratch,error)
        keyword='BPA'
        call get_keyword(fname,extn,keyword,keystrng,cbeam(3),
     /       comment,'r',scratch,error)
        keyword='CDELT1'
        call get_keyword(fname,extn,keyword,keystrng,cdelt(1),
     /       comment,'r',scratch,error)
        keyword='CDELT2'
        call get_keyword(fname,extn,keyword,keystrng,cdelt(2),
     /       comment,'r',scratch,error)
        bm_pix(1)=cbeam(1)/abs(cdelt(1))
        bm_pix(2)=cbeam(2)/abs(cdelt(2))

         write (*,*) 'a3'
        call sub_analysepsf2(gname,fname,scratch,srldir,n,m,ngau,
     /       cbeam,bm_pix,cdelt)

444     continue
        return
        end
c!
c!
c!
        subroutine sub_analysepsf2(gname,fname,scratch,srldir,n,m,ngau,
     /       cbeam,bm_pix,cdelt)
        implicit none
        include "constants.inc"
        character gname*500,scratch*500,srldir*500,fname*500,f1*500
        integer nchar,i,dumi,n,m,ngau,flag(ngau),dumi1,ng,di(ngau)
        integer gauid(ngau),j,nbin,sum1,sum2,sum3,sum4,dumil(ngau)
        real*8 dumr,peak(ngau),bmaj(ngau),ebmaj(ngau),bmin(ngau)
        real*8 ebmin(ngau),bpa(ngau),ebpa(ngau),epeak(ngau)
        real*8 snr(ngau),dumr1,dumr2,dumrl(ngau)
        real*8 cbeam(3),bm_pix(2),cdelt(2),a4,a5
        real*8 sm_snr(100),sm_ebj(100),sm_ebn(100),zeroptj
        real*8 x8(ngau),y8(ngau),zeroptn,usr1,usr2,cfac
        real*4 sm_snr4(100),sm_ebj4(100),sm_m_ebj4(100)
        real*4 sm_ebn4(100),sm_m_ebn4(100)
        real*4 x4(ngau),y4j(ngau),y4n(ngau),mnx,mny,mxx,mxy,dum4
        real*4 x4d(100),y4d(100)
        character code(ngau)*4,names(ngau)*30

c! get gaul parameters
         write (*,*) 'a4 ',ngau
        call readgaul(gname,ngau,ngau,srldir,gauid,di,flag,dumrl,
     /    dumrl,peak,epeak,dumrl,dumrl,dumrl,dumrl,dumrl,dumrl,
     /    dumrl,dumrl,bmaj,ebmaj,bmin,ebmin,bpa,ebpa,dumrl,
     /    dumrl,dumrl,dumrl,dumrl,dumrl,
     /    dumrl,dumrl,dumrl,dumrl,dumrl,dumrl,code,names,
     /    dumil,dumil,dumil,dumil,dumrl,dumrl,dumrl,dumrl,
     /    dumrl,dumrl)
         write (*,*) 'a5'

c! for good gaussians which are not exactly bmaj, store SNR, BMAJ in arrays
        ng=0
        do i=1,ngau
         if (gauid(i).ne.i) write (*,*) ' @@@ ERRORRRR !!! ',i,gauid(i)
         if (flag(i).eq.0.and.peak(i).gt.0.d0.and.code(i).eq.'S'
     /       .and.abs(bmaj(i)-cbeam(1)*3600.d0).gt.1.d-5) then
          if (i.lt.ngau) ng=ng+1
          snr(ng)=peak(i)/epeak(i)
          x4(ng)=log10(snr(ng))  
          y4j(ng)=bmaj(i)
          y4n(ng)=bmaj(i)
          x8(ng)=log10(snr(ng))
          y8(ng)=bmaj(i)
         end if
        end do
c! x=log(SNR); y=bmaj
        call range_vec4mxmn(x4,ngau,ng,mnx,mxx)
        call range_vec4mxmn(y4j,ngau,ng,mny,mxy)

c! calc median vs snr
        call sub_analysepsf2_med(ngau,ng,x8,y8,x4d,y4d,100,nbin)
        zeroptj=(y4d(1)+y4d(2))*0.5d0
        if (abs((zeroptj-cbeam(1)*3600.d0)/zeroptj).gt.0.2) 
     /      write (*,*) '  BAD zero pt !!!!!!!!!!!!!!'

c! calc theoretical envelope numbers
        a4=cbeam(1)/(fwsig*abs(cdelt(1)))*1.5
        a5=cbeam(2)/(fwsig*abs(cdelt(2)))*1.5
        dumr=sqrt(a4*a5/(4.d0*bm_pix(1)/fwsig*bm_pix(2)/fwsig))
        dumr1=1.d0+bm_pix(1)/fwsig*bm_pix(2)/fwsig/(a4*a4)
        dumr2=1.d0+bm_pix(1)/fwsig*bm_pix(2)/fwsig/(a5*a5)
        usr1=a4*sq2/(dumr*(dumr1**1.25d0)*
     /         (dumr2**0.25d0))*fwsig*abs(cdelt(1))*3600.d0
        usr2=a5*sq2/(dumr*(dumr1**0.25d0)*
     /         (dumr2**1.25d0))*fwsig*abs(cdelt(2))*3600.d0

c! now calculate better envelope (see notes). cfac is factor to multiply usr1 for env
        sum1=0    
        sum2=0    
        sum3=0    
        sum4=0    
        do i=1,ng
         if (y8(i).le.zeroptj) sum1=sum1+1
         if (y8(i).lt.(zeroptj-usr1*3.d0/(10.d0**x8(i)))) sum2=sum2+1
         if (y8(i).lt.(zeroptj-usr1*1.d0/(10.d0**x8(i)))) sum3=sum3+1
        end do
        if ((sum3-sum2)*1.d0/sum1.le.0.05) then
         cfac=1.d0
        else
         dumr=3.d0**0.1d0
         do i=2,10
          dumr1=dumr**i
          sum4=0
          do j=1,ng
           if (y8(j).lt.(zeroptj-usr1*dumr1/(10.d0**x8(j)))) sum4=sum4+1
          end do
          cfac=dumr1
          if ((sum4-sum2)*1.d0/sum1.le.0.05) goto 445
         end do
        end if
445     continue

c! full envelope
        sm_snr(1)=mnx*1.d0
        do i=1,100
         if (i.ne.1) sm_snr(i)=sm_snr(i-1)+(mxx-mnx)*1.d-2
         sm_ebj(i)=usr1/(10.d0**sm_snr(i))
         sm_ebn(i)=usr2/(10.d0**sm_snr(i))
         sm_ebj(i)=zeroptj-sm_ebj(i)
         sm_ebn(i)=zeroptn-sm_ebn(i)
         sm_snr4(i)=sm_snr(i)
         sm_ebj4(i)=sm_ebj(i)
        end do


c! plot
        call pgbegin(0,'/xs',1,1)
        call pgvport(0.15,0.9,0.15,0.9)
        call pgwindow(mnx,mxx,mny,mxy)
        call pgbox('BCNST',0.0,0,'BCNST',0.0,0)
        do i=1,ng
         dum4=ebmaj(i)
         call pgpoint(1,x4(i),y4j(i),3)
         call pgerry(1,x4(i),y4j(i)+dum4,y4j(i)-dum4,1.0)
        end do
        call pglabel('log10(SNR)','B_major','')
        call pgsci(2)
        call pgline(100,sm_snr4,sm_ebj4)
        call pgsci(3)
        call pgline(nbin,x4d,y4d)
        call pgsci(4)
        call pgpoint(nbin,x4d,y4d,17)
        call pgsci(1)

        call pgend

        return
        end



