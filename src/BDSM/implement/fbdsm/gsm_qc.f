c! checks gsm outputs and makes plot

        
        subroutine gsm_qc(nassoc,sumngaul,spin_arr,espin_arr,avdec,
     /      plotdir,gsmsolnname,freq,nf,scratch,filename,assoc_arr,
     /      tot,maxngaul,etot,plotpairs)
        implicit none
        integer nassoc,i,sumngaul,dumi1,dumi2,nf,nchar
        integer assoc_arr(sumngaul,nf),maxngaul
        real*8 spin00(nassoc),avsp00,stdsp00,medcsp00
        real*8 espin00(nassoc),avdec(sumngaul),dec(nassoc)
        real*8 avclsp00,stdclsp00,medclsp00,freq(nf)
        real*8 spin_arr(sumngaul,5),espin_arr(sumngaul,5)
        real*8 spin10(nassoc),spin11(nassoc),I00(nassoc),I10(nassoc)
        real*8 I00_1(nassoc),spin00_1(nassoc)
        character gsmsolnname*500,plotdir*500,scratch*500
        character filename(nf)*500,plotpairs*500
        real*8 tot(nf,maxngaul),etot(nf,maxngaul)

c! set up variables
        dumi1=0
        dumi2=0
        do i=1,nassoc
         if (spin_arr(i,1).gt.-990.d0) then
          dumi1=dumi1+1
          I00(dumi1)=spin_arr(i,1)
          spin00(dumi1)=spin_arr(i,2)
          espin00(dumi1)=espin_arr(i,2)
          dec(dumi1)=avdec(i)
         end if
           
         if (spin_arr(i,3).gt.-990.d0) then
          dumi2=dumi2+1
          I10(dumi2)=spin_arr(i,3)
          spin10(dumi2)=spin_arr(i,4)
          spin11(dumi2)=spin_arr(i,5)
          I00_1(dumi2)=spin_arr(i,1)
          spin00_1(dumi2)=spin_arr(i,2)
         end if
        end do
        if (dumi2.eq.0) dumi2=1

c!  write out values
        call sub_gsm_qc_1(nassoc,dumi1,dumi2,spin00,I00,spin10,
     /       I10,spin11,spin00_1,I00_1,dec,plotdir,gsmsolnname,medcsp00,
     /       espin00)

c! plot for each pair of catalogues
        if (plotpairs.eq.'true') 
     /  call sub_gsm_qc_3(nf,freq,scratch,gsmsolnname,plotdir,nassoc,
     /       assoc_arr,sumngaul,filename,tot,maxngaul,etot)

c! plot spectral indices for each association and write table also.
        call sub_gsm_qc_2(nf,freq,scratch,gsmsolnname,plotdir,medcsp00,
     /       filename,nassoc,sumngaul,assoc_arr)

        return
        end
c!
c!
c!
        subroutine sub_gsm_qc_1(nassoc,dumi1,dumi2,spin00,I00,spin10,
     /       I10,spin11,spin00_1,I00_1,dec,plotdir,gsmsolnname,medcsp00,
     /       espin00)
        implicit none
        integer nassoc,dumi1,dumi2,nchar,i
        real*8 spin00(nassoc),I00(nassoc),spin10(nassoc),I10(nassoc)
        real*8 espin00(nassoc),dec(nassoc)
        real*8 spin11(nassoc),spin00_1(nassoc),I00_1(nassoc),medcsp00
        real*4 sp00(dumi1),i0(dumi1),sp10(dumi2),i1(dumi2),sp11(dumi2)
        real*4 id1(dumi1),id2(dumi2),sp00_1(dumi2),i0_1(dumi2),nsig4
        real*4 sp00_10(dumi2),i01(dumi2),sp001011(dumi2),dum4
        real*4 esp00(dumi1),dec4(dumi1)
        character label1*30,label2*30,chr1*1,xl*6,yl*6,fname*500,nam*500
        character psname*500,plotdir*500,rcode*2,scrat*500
        character gsmsolnname*500

        do i=1,dumi1
         sp00(i)=spin00(i)
         dec4(i)=dec(i)
         i0(i)=log10(I00(i))
         id1(i)=i*1.0
         esp00(i)=espin00(i)
        end do
        if (dumi2.gt.1) then
         do i=1,dumi2
          sp10(i)=spin10(i)
          sp11(i)=spin11(i)
          i1(i)=log10(I10(i))
          id2(i)=i*1.0
          sp00_1(i)=spin00_1(i)
          i0_1(i)=log10(I00_1(i))
          i01(i)=log10(I00_1(i)/I10(i))
          sp00_10(i)=spin00_1(i)-spin10(i)
          sp001011(i)=spin00_1(i)-spin10(i)-spin11(i)
         end do
        end if

        rcode='aq'
        chr1='d'
        xl='BCNST '
        yl='BCNST '
        scrat='gsm_'//gsmsolnname(1:nchar(gsmsolnname))
        fname=plotdir(1:nchar(plotdir))//
     /     gsmsolnname(1:nchar(gsmsolnname))//'/gsm_'//
     /     gsmsolnname(1:nchar(gsmsolnname))//'_gsmqc_'

        if (dumi1.gt.1) then

        nsig4=3.0
        call calcmedian4clip(sp00,dumi1,1,dumi1,dum4,nsig4)
        medcsp00=dum4

c!      number vs spin00
         label1='id'
         label2='Spectral Index sp00'
         nam='id_sp00'
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(id1,sp00,dumi1,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')
         
c!      dec vs spin00
         label1='Declination'
         label2='Spectral Index sp00'
         nam='dec_sp00'
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(dec4,sp00,dumi1,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')
         
c!      spin00 vs I00
         label1='I00 (Jy)'
         label2='sp00 Spectral Index'  ! first sp00 else plots equal axes
         nam='I0_sp00'
         xl='BCNSTL'
         yl='BCNST '
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(i0,sp00,dumi1,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')
         
c!      spin00 vs espin00
         label1='sp00 Spectral Index'  
         label2='Error in sp00 Spectral Index'
         nam='sp00_esp00'
         xl='BCNST '
         yl='BCNST '
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(sp00,esp00,dumi1,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')

        end if
        
        if (dumi2.gt.1) then
c!      I00 vs spin10
         label1='I00 (Jy)'
         label2='sp10 Spectral Index'
         nam='I0_sp10'
         xl='BCNSTL'
         yl='BCNST '
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(i0_1,sp10,dumi2,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')
         
c!      spin00 vs spin10
         label1='sp00 Spectral index'
         label2='sp10 Spectral Index'
         nam='sp00_sp10'
         xl='BCNST'
         yl='BCNST '
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(sp00_1,sp10,dumi2,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')
        
c!      spin00-spin10 vs spin11
         label1='sp00-sp10 Spectral index'
         label2='sp11 Spectral Index'
         nam='sp00s10_sp11'
         xl='BCNST'
         yl='BCNST '
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(sp00_10,sp11,dumi2,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')
        
c!       I10 vs spin10
         label1='I10 (Jy)'
         label2='sp10 Spectral Index'
         nam='I10_sp10'
         xl='BCNSTL'
         yl='BCNST '
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(i1,sp10,dumi2,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')
         
c!       I00/I10 vs sp00-sp10-sp11
         label1='I00/I11 '
         label2='sp00-sp10-sp11'
         nam='I00I11_sp001011'
         xl='BCNSTL'
         yl='BCNST'
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(i01,sp001011,dumi2,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')
         
c!       I00 vs I10
         label1='I00 (Jy)'
         label2='I10 (Jy)'
         nam='I00_I10'
         xl='BCNSTL'
         yl='BCNSTL'
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(i0_1,i1,dumi2,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')
         
c!       I10 vs spin11
         label1='I10 (Jy)'
         label2='sp11 Spectral Index'
         nam='I10_sp11'
         xl='BCNSTL'
         yl='BCNST '
         psname=fname(1:nchar(fname))//nam(1:nchar(nam))//'.png/PNG'
         call plot_menu(i1,sp11,dumi2,label1,label2,chr1,xl,yl,
     /              psname,rcode,scrat,'n')
        end if
        
        return
        end
c!
c!
c! plot around the fiducial spectral index for whole gsm.
c! 1 is peak and 2 is total
        subroutine sub_gsm_qc_2(nf,freq,scratch,gsmsolnname,plotdir,
     /             medcsp00,filename,nassoc,sumngaul,assoc_arr)
        implicit none
        integer nf,nchar,i,j,nn,k,low,hi,dumi,nums(nf,nf),n
        integer sumngaul,assoc_arr(sumngaul,nf),nassoc
        real*8 freq(nf),av(nf,nf,2),std(nf,nf,2),med(nf,nf,2),medcsp00
        real*8 stdc(nf,nf,2),avc(nf,nf,2),medc(nf,nf,2),d1,d2
        real*4 mnx,mxx,mny,mxy,x(2),y(2),fflux(nf)
        character gsmsolnname*500,scratch*500,f1*500,plotdir*500,f2*500
        character f3*500,f4*500,filename(nf)*500
        logical ex,op

        f1=scratch(1:nchar(scratch))//'/gsm_workf_'//
     /     gsmsolnname(1:nchar(gsmsolnname))
        inquire(file=f1,exist=ex,opened=op,number=n)
        if (op) close(n)
        open(unit=31,file=f1)
110      read (31,*,END=100) i,j,nn
         if (nn.eq.35.or.nn.eq.36) then
          backspace(31)
          read (31,*) i,j,nn,av(i,j,nn-34),std(i,j,nn-34),
     /     med(i,j,nn-34),avc(i,j,nn-34),stdc(i,j,nn-34),medc(i,j,nn-34)
         end if
         goto 110
100     continue
        close(31)
        
        call rangevec8(freq,nf,nf,d1,d2)
        mnx=d1
        mxx=d2
        mny=0.5d0                     ! min flux is 1 Jy for highest freq
        mxy=mny*((mnx/mxx)**(-1.2d0))  ! just in case
        do i=1,nf
         fflux(i)=(freq(i)/mxx)**medcsp00
        end do
        mnx=log10(mnx)-6.0
        mxx=log10(mxx)-6.0
        mny=log10(mny)
        mxy=log10(mxy)

c! get number of pair associations
        call initialisemask(nums,nf,nf,nf,nf,0)
        do i=1,nassoc
         do j=1,nf
          do k=j+1,nf
           if (assoc_arr(i,j).ne.0.and.assoc_arr(i,k).ne.0) 
     /      nums(j,k)=nums(j,k)+1
          end do
         end do
        end do
        f4=scratch(1:nchar(scratch))//'gsm_number_'//
     /     gsmsolnname(1:nchar(gsmsolnname))
        open(unit=22,file=f4)
        write (22,*) ' Number of pair associations '
        write (22,'(a20,$)') '          '
        do j=1,nf
         write (22,'(a20,$)') filename(j)(1:nchar(filename(j)))
        end do
        write (22,*)
        do j=1,nf
         write (22,'(a18,a2,$)') filename(j)(1:nchar(filename(j))),'  '
         do k=1,j
          write (22,'(a20,$)') '          '
         end do
         write (22,'(a10,$)') '          '
         do k=j+1,nf
          write (22,'(4x,i6,a10,$)') nums(j,k),'          '
         end do 
         write (22,*)
        end do
        write (22,*)
        close(22)

c! plot the median spins
        f1=plotdir(1:nchar(plotdir))//gsmsolnname(1:nchar(gsmsolnname))
     /      //'/gsm_peak_allspin.png/PNG'
        f2=plotdir(1:nchar(plotdir))//gsmsolnname(1:nchar(gsmsolnname))
     /      //'/gsm_total_allspin.png/PNG'
        f3=scratch(1:nchar(scratch))//'gsm_spins_'//
     /     gsmsolnname(1:nchar(gsmsolnname))
        open(unit=21,file=f3)
        do i=1,2
         if (i.eq.1) call pgbegin(0,f1,1,1)
         if (i.eq.2) call pgbegin(0,f2,1,1)
         if (i.eq.1) write (21,*) 
     /       ' Clipped median spectral indices for peak flux'
         if (i.eq.2) write (21,*) 
     /       ' Clipped median spectral indices for total flux'
         write (21,'(a20,$)') '          '
         do j=1,nf
          write (21,'(a20,$)') filename(j)(1:nchar(filename(j)))
         end do
         write (21,*)
         call pgvport(0.1,0.9,0.1,0.9)
         call pgwindow(mnx-0.1*(mxx-mnx),mxx+0.1*(mxx-mnx),mny,mxy)
         call pgbox('BCNLST',0.0,0,'BCNLST',0.0,0)
         if (i.eq.1) call pglabel('Frequency (MHz)','Peak flux ',' ')
         if (i.eq.2) call pglabel('Frequency (MHz)','Total flux ',' ')
         dumi=1
         do j=1,nf
          write (21,'(a18,a2,$)') filename(j)(1:nchar(filename(j))),'  '
          do k=1,j
           write (21,'(a20,$)') '                    '
          end do
          write (21,'(a10,$)') '          '
          do k=j+1,nf
           if (freq(j).lt.freq(k)) then
            low=j
            hi=k
           else
            low=k
            hi=j
           end if
           x(1)=log10(freq(low))-6.0
           x(2)=log10(freq(hi))-6.0
           y(2)=log10(fflux(hi))
           y(1)=y(2)+medc(j,k,i)*log10(freq(low)/freq(hi))
           call pgsci(dumi)
           call pgpoint(2,x,y,15)
           call pgline(2,x,y)
           dumi=dumi+1
           write (21,'(4x,f6.3,a10,$)') medc(j,k,i),'          '
          end do 
          write (21,*)
         end do
         write (21,*)
         call pgend
        end do
        close(21)

        return
        end
c!
c!
c! spin vs flux for every pair. Also median spin. 
        subroutine sub_gsm_qc_3(nf,freq,scratch,gsmsolnname,plotdir,
     /       nassoc,assoc_arr,sumngaul,filename,tot,maxngaul,etot)
        implicit none
        integer nassoc,sumngaul,nf,nchar,i,j,k
        integer assoc_arr(sumngaul,nf),nct,maxngaul
        character gsmsolnname*500,plotdir*500,scratch*500,f1*500
        character filename(nf)*500,s1*10,s2*10,scrat*500,fname*500
        character f3*500,f4*500,f2*500,ff(nf)*500,dumc*10
        real*4 x4(maxngaul),y4(maxngaul),x41(maxngaul)
        character label1*30,label2*30,chr1*1,xl*6,yl*6,nam*500,rcode*2
        real*8 freq(nf),tot(nf,maxngaul),dumr1,dumr2,dumr3,dumr4
        real*4 mny,mxy
        real*8 etot(nf,maxngaul),df1,df2

c! names cos pgplot doesnt like long names > 61 chars
        do i=1,nf
         call int2str(i,dumc)
         ff(i)='C'//dumc(1:nchar(dumc))
        end do

c! spin vs flux for every pair
        scrat='gsm_'//gsmsolnname(1:nchar(gsmsolnname))
        fname=plotdir(1:nchar(plotdir))//
     /     gsmsolnname(1:nchar(gsmsolnname))//'/gsm_'//
     /     gsmsolnname(1:nchar(gsmsolnname))
        rcode='aq'
        chr1='d'
        do j=1,nf-1
         do k=j+1,nf
          dumr3=dlog10(freq(j)/freq(k))
          nct=0
          do i=1,nassoc
           if (assoc_arr(i,j).ne.0.and.assoc_arr(i,k).ne.0) then
            nct=nct+1
            dumr1=tot(j,assoc_arr(i,j))
            dumr2=tot(k,assoc_arr(i,k))
            y4(nct)=dlog10(dumr1/dumr2)/dumr3
            x4(nct)=dlog10(dumr1)
            x41(nct)=dlog10(dumr2)
           end if
          end do
          label2='Spin '//filename(j)(1:nchar(filename(j)))//
     /           ' '//filename(k)(1:nchar(filename(k)))
          xl='BCNSTL'
          yl='BCNST '

c! flux vs spin, median spin
          label1='Flux '//filename(j)(1:nchar(filename(j)))//' (Jy)'
          f1=fname(1:nchar(fname))//'_flux'//
     /      ff(j)(1:nchar(ff(j)))//'_vs_spin_'
          f2=f1(1:nchar(f1))//ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_plot_menu(x4,y4,maxngaul,nct,label1,label2,
     /         chr1,xl,yl,f2,rcode,scrat)

          label1='Flux '//filename(k)(1:nchar(filename(k)))//' (Jy)'
          f1=fname(1:nchar(fname))//'_flux'//
     /      ff(k)(1:nchar(ff(k)))//'_vs_spin_'
          f2=f1(1:nchar(f1))//ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_plot_menu(x41,y4,maxngaul,nct,label1,label2,
     /         chr1,xl,yl,f2,rcode,scrat)

          label2='Median spin '
          label1='Flux '//filename(j)(1:nchar(filename(j)))//' (Jy)'
          f3=filename(j)(1:nchar(filename(j)))//
     /           ' '//filename(k)(1:nchar(filename(k)))
          f2=fname(1:nchar(fname))//'_flux'//
     /       ff(j)(1:nchar(ff(j)))//'_vs_medspin'//
     /       ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_calc_plot_median(x4,y4,maxngaul,nct,label1,label2,
     /         xl,yl,f2,rcode,scrat,f3,-1.2,-0.3)
          label1='Flux '//filename(k)(1:nchar(filename(k)))//' (Jy)'
          f2=fname(1:nchar(fname))//'_flux'//
     /       ff(k)(1:nchar(ff(k)))//'_vs_medspin'//
     /       ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_calc_plot_median(x41,y4,maxngaul,nct,label1,label2,
     /         xl,yl,f2,rcode,scrat,f3,-1.2,-0.3)

          label2='rms spin '
          label1='Flux '//filename(j)(1:nchar(filename(j)))//' (Jy)'
          f2=fname(1:nchar(fname))//'_flux'//
     /       ff(j)(1:nchar(ff(j)))//'_vs_rmsspin'//
     /       ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_calc_plot_std(x4,y4,maxngaul,nct,label1,label2,
     /         xl,yl,f2,rcode,scrat,f3)
          label1='Flux '//filename(k)(1:nchar(filename(k)))//' (Jy)'
          f2=fname(1:nchar(fname))//'_flux'//
     /       ff(k)(1:nchar(ff(k)))//'_vs_rmsspin'//
     /       ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_calc_plot_std(x41,y4,maxngaul,nct,label1,label2,
     /         xl,yl,f2,rcode,scrat,f3)

! spin vs snr
          nct=0
          label2='Spin '//filename(j)(1:nchar(filename(j)))// 
     /        ' '//filename(k)(1:nchar(filename(k)))
          do i=1,nassoc
           if (assoc_arr(i,j).ne.0.and.assoc_arr(i,k).ne.0) then
            nct=nct+1
            dumr1=tot(j,assoc_arr(i,j))/etot(j,assoc_arr(i,j))
            dumr2=tot(k,assoc_arr(i,k))/etot(k,assoc_arr(i,k))
            y4(nct)=dlog10(tot(j,assoc_arr(i,j))/
     /              tot(k,assoc_arr(i,k)))/dumr3
            x4(nct)=dlog10(dumr1)
            x41(nct)=dlog10(dumr2)
           end if
          end do
          label1='SNR '//filename(j)(1:nchar(filename(j)))
          f1=fname(1:nchar(fname))//'_snr'//
     /      ff(j)(1:nchar(ff(j)))//'_vs_spin_'
          f2=f1(1:nchar(f1))//ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_plot_menu(x4,y4,maxngaul,nct,label1,label2,
     /         chr1,xl,yl,f2,rcode,scrat)

          label1='SNR '//filename(k)(1:nchar(filename(k)))//' (Jy)'
          f1=fname(1:nchar(fname))//'_snr'//
     /      ff(k)(1:nchar(ff(k)))//'_vs_spin_'
          f2=f1(1:nchar(f1))//ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_plot_menu(x41,y4,maxngaul,nct,label1,label2,
     /         chr1,xl,yl,f2,rcode,scrat)

! medspin and rmsspin vs SNR
          label2='Median spin '
          label1='SNR '//filename(j)(1:nchar(filename(j)))
          f3=filename(j)(1:nchar(filename(j)))//
     /           ' '//filename(k)(1:nchar(filename(k)))
          f2=fname(1:nchar(fname))//'_snr'//
     /       ff(j)(1:nchar(ff(j)))//'_vs_medspin'//
     /       ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_calc_plot_median(x4,y4,maxngaul,nct,label1,label2,
     /         xl,yl,f2,rcode,scrat,f3,-1.2,-0.3)
          label1='SNR '//filename(k)(1:nchar(filename(k)))
          f2=fname(1:nchar(fname))//'_snr'//
     /       ff(k)(1:nchar(ff(k)))//'_vs_medspin'//
     /       ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_calc_plot_median(x41,y4,maxngaul,nct,label1,label2,
     /         xl,yl,f2,rcode,scrat,f3,-1.2,-0.3)

          label2='rms spin '
          label1='SNR '//filename(j)(1:nchar(filename(j)))
          f2=fname(1:nchar(fname))//'_snr'//
     /       ff(j)(1:nchar(ff(j)))//'_vs_rmsspin'//
     /       ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_calc_plot_std(x4,y4,maxngaul,nct,label1,label2,
     /         xl,yl,f2,rcode,scrat,f3)
          label1='SNR '//filename(k)(1:nchar(filename(k)))
          f2=fname(1:nchar(fname))//'_snr'//
     /       ff(k)(1:nchar(ff(k)))//'_vs_rmsspin'//
     /       ff(j)(1:nchar(ff(j)))//'_'//
     /       ff(k)(1:nchar(ff(k)))//'.png/PNG'
          call call_calc_plot_std(x41,y4,maxngaul,nct,label1,label2,
     /         xl,yl,f2,rcode,scrat,f3)

         end do
        end do

! flux vs snr
        xl='BCNSTL'
        yl='BCNSTL'
        do j=1,nf
         nct=0
         do i=1,nassoc
          if (assoc_arr(i,j).ne.0) then
           nct=nct+1
           x4(nct)=dlog10(tot(j,assoc_arr(i,j)))
           y4(nct)=dlog10(tot(j,assoc_arr(i,j))/etot(j,assoc_arr(i,j)))
          end if
         end do
         label1='Flux (Jy) '//filename(j)(1:nchar(filename(j)))
         label2='SNR '//filename(j)(1:nchar(filename(j)))
         f1=fname(1:nchar(fname))//'_snr'//
     /     ff(j)(1:nchar(ff(j)))//'_vs_flux'
         f2=f1(1:nchar(f1))//ff(j)(1:nchar(ff(j)))//
     /      '.png/PNG'
         call call_plot_menu(x4,y4,maxngaul,nct,label1,label2,
     /        chr1,xl,yl,f2,rcode,scrat)
        end do

c! plot C1C2 spin - C2C3 spin vs flux/SNR and median also.
        do j=1,nf-2
         df1=dlog10(freq(j)/freq(j+1))
         df2=dlog10(freq(j+1)/freq(j+2))
         nct=0
         do i=1,nassoc
          if (assoc_arr(i,j).ne.0.and.assoc_arr(i,j+1).ne.0.and.
     /        assoc_arr(i,j+2).ne.0) then
           nct=nct+1
           dumr1=tot(j,assoc_arr(i,j))
           dumr2=tot(j+1,assoc_arr(i,j+1))
           dumr3=tot(j+2,assoc_arr(i,j+2))
           y4(nct)=dlog10(dumr1/dumr2)/df1-dlog10(dumr2/dumr3)/df2
           x4(nct)=dlog10(dumr2)
           x41(nct)=dlog10(dumr2/etot(j+1,assoc_arr(i,j+1)))
          end if
         end do
         xl='BCNSTL'
         yl='BCNST '
         label2='Diff spin '//
     /    ff(j)(1:nchar(ff(j)))//'_'//ff(j+1)(1:nchar(ff(j+1)))//' - '//
     /    ff(j+1)(1:nchar(ff(j+1)))//'_'//ff(j+2)(1:nchar(ff(j+2)))
c! vs flux
         label1='Flux '//filename(j+1)(1:nchar(filename(j+1)))//' (Jy)'
         f1=fname(1:nchar(fname))//'_flux'//
     /     ff(j)(1:nchar(ff(j)))//'_vs_diffspin_'
         f2=f1(1:nchar(f1))//ff(j)(1:nchar(ff(j)))//'_'//
     /      ff(j+1)(1:nchar(ff(j+1)))//'-'//ff(j+1)(1:nchar(ff(j+1)))//
     /      '_'//ff(j+2)(1:nchar(ff(j+2)))//'.png/PNG'
         call call_plot_menu(x4,y4,maxngaul,nct,label1,label2,
     /        chr1,xl,yl,f2,rcode,scrat)
c! vs snr
         label1='SNR '//filename(j+1)(1:nchar(filename(j+1)))//' (Jy)'
         f1=fname(1:nchar(fname))//'_snr'//
     /     ff(j)(1:nchar(ff(j)))//'_vs_diffspin_'
         f2=f1(1:nchar(f1))//ff(j)(1:nchar(ff(j)))//'_'//
     /      ff(j+1)(1:nchar(ff(j+1)))//'-'//ff(j+1)(1:nchar(ff(j+1)))//
     /      '_'//ff(j+2)(1:nchar(ff(j+2)))//'.png/PNG'
         call call_plot_menu(x41,y4,maxngaul,nct,label1,label2,
     /        chr1,xl,yl,f2,rcode,scrat)

c! now do median of diff in spin in bins vs flux and snr
         label2='Median diff spin '
         label1='Flux '//filename(j+1)(1:nchar(filename(j+1)))//' (Jy)'
         f2=fname(1:nchar(fname))//'_flux'//
     /     ff(j+1)(1:nchar(ff(j+1)))//'_vs_meddiffspin'//
     /     ff(j)(1:nchar(ff(j)))//'_'//ff(j+1)(1:nchar(ff(j+1)))//'-'//
     /     ff(j+1)(1:nchar(ff(j+1)))//'_'//ff(j+2)(1:nchar(ff(j+2)))//
     /     '.png/PNG'
         call call_calc_plot_median(x4,y4,maxngaul,nct,label1,label2,
     /        xl,yl,f2,rcode,scrat,f3,-0.5,0.5)
         label1='SNR '//filename(j+1)(1:nchar(filename(j+1)))//' (Jy)'
         f2=fname(1:nchar(fname))//'_snr'//
     /     ff(j+1)(1:nchar(ff(j+1)))//'_vs_meddiffspin'//
     /     ff(j)(1:nchar(ff(j)))//'_'//ff(j+1)(1:nchar(ff(j+1)))//'-'//
     /     ff(j+1)(1:nchar(ff(j+1)))//'_'//ff(j+2)(1:nchar(ff(j+2)))//
     /     '.png/PNG'
         call call_calc_plot_median(x41,y4,maxngaul,nct,label1,label2,
     /        xl,yl,f2,rcode,scrat,f3,-0.5,0.5)
        end do


        return
        end




