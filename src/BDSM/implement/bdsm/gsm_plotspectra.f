c! plots all spectra of associations

        subroutine gsm_plotspectra(plotdir,plotspectra,sumngaul,
     /       nf,assoc_arr,nassoc,
     /       freq,tot,dtot,maxngaul,freq0,spin_arr,espin_arr,
     /       avsp00,stdsp00,medsp00,filename,solnname,names,ra,dec,
     /       gsmsolnname,tofit,scratch,n2,makepdf)
        implicit none
        integer sumngaul,nf,assoc_arr(sumngaul,nf),nassoc,maxngaul,i,j
        integer tofit(nf),ftofit(nf),ndigit,ndigit0,n2,error,nplot
        real*8 freq(nf),freq0,tot(nf,maxngaul)
        real*8 ra(nf,maxngaul),dec(nf,maxngaul)
        real*8 sig(nf),spin_arr(sumngaul,5)
        real*8 dtot(nf,maxngaul),espin_arr(sumngaul,5)
        real*8 avsp00,stdsp00,medsp00,dumr
        real*4 freq4(nf),flux4(nf),sig4(nf),x(100),why1(100)
        real*4 mnx,mxx,mny,mxy,y1(nf),y2(nf),dumr4,why2(100)
        character filename(nf)*500,plotdir*500,plotspectra*500
        character names(nf,maxngaul)*40,solnname(nf)*500,str*500
        character gsmsolnname*500,plotname*500,dumc*10,scratch*500
        character otfn*500,incg*500,psname(nassoc)*500,makepdf*500
        integer id,pgopen,nchar,nfit,bnchar,mm,pp,nc

c! get x axis limits
        do i=1,nf
         freq4(i)=log10(freq(i)/1.d6)
        end do
        call range2(freq4,nf,nf,mnx,mxx)

c! get star names and if none, make one from coordinates
        do i=1,nassoc
         do j=1,nf
          if (assoc_arr(i,j).ne.0) then
           if (names(j,assoc_arr(i,j)).eq.'') then
            call coord2name(ra(j,assoc_arr(i,j)),dec(j,assoc_arr(i,j)),
     /           filename(j),names(j,assoc_arr(i,j)))

           end if
          end if
         end do
        end do

c! now define qts to plot
        nplot=0
        ndigit0=int(log10(nassoc*1.0))+1
        do i=1,nassoc
         nfit=0
         do j=1,nf
          if (assoc_arr(i,j).ne.0) then
           nfit=nfit+1
           freq4(nfit)=log10(freq(j)/1.d6)
           flux4(nfit)=log10(tot(j,assoc_arr(i,j)))
           sig4(nfit)=dtot(j,assoc_arr(i,j))
           ftofit(nfit)=tofit(j)
           y1(nfit)=log10(tot(j,assoc_arr(i,j))+dtot(j,assoc_arr(i,j)))
           y2(nfit)=log10(tot(j,assoc_arr(i,j))-dtot(j,assoc_arr(i,j)))
          end if
         end do

c! only plot if more than 1 point       
         if (nfit.gt.1) then
          nplot=nplot+1

c! get file name
          write (str,*) nplot
          ndigit=int(log10(nplot*1.0))+1
          dumc=''
          do j=1,ndigit0-ndigit
           dumc=dumc(1:nchar(dumc))//'0' 
          end do
          plotname=plotdir(1:nchar(plotdir))//gsmsolnname
     /     (1:nchar(gsmsolnname))//'/'//gsmsolnname
     /     (1:nchar(gsmsolnname))//'_'//dumc(1:nchar(dumc))//
     /     str(bnchar(str):nchar(str))//'.png/PNG'
          psname(nplot)=gsmsolnname(1:nchar(gsmsolnname))//'_'//
     /     dumc(1:nchar(dumc))//str(bnchar(str):nchar(str))//'.ps'

c! plot fluxes
          id=pgopen(plotname(1:nchar(plotname)))
          call pgslct(id)
          call pgvport(0.1,0.95,0.13,0.95)
          call pgslw(3)
          call pgsch(1.4)
          call range2(y1,nf,nfit,dumr4,mxy)
          call range2(y2,nf,nfit,mny,dumr4)
          if (nfit.eq.1) then
           mny=y2(1)*0.9
           mxy=y1(1)*1.1
          end if
          call pgwindow(mnx,mxx,mny,mxy)
          call pgbox('BCNLST',0.0,0,'BCNLST',0.0,0)
          call pglabel('Freq (MHz)','Flux (Jy)',' ')
          do j=1,nfit
           if (ftofit(j).eq.1) then
            call pgpoint(1,freq4(j),flux4(j),16)
           else
            call pgpoint(1,freq4(j),flux4(j),22)
           end if
          end do
          call pgerry(nfit,freq4,y1,y2,1.0)

c! plot spectra
          if (spin_arr(i,1).gt.-990.d0) then
           do j=1,100
            x(j)=mnx+(j-1)*(mxx-mnx)/99.0            ! freq in log10(Mhz)
            why1(j)=log10(spin_arr(i,1))+spin_arr(i,2)*
     /              (x(j)+6.0-log10(freq0))
           end do
           call pgline(100,x,why1)
          end if
 
          if (spin_arr(i,3).gt.-900.d0) then   ! magiv value is -999
           do j=1,100
            x(j)=mnx+(j-1)*(mxx-mnx)/99.0            ! freq in log10(Mhz)
            dumr4=x(j)+6.0-log10(freq0)
            why2(j)=log10(spin_arr(i,3))+spin_arr(i,4)*dumr4+
     /              spin_arr(i,5)*dumr4*dumr4
           end do
           call pgsls(2)
           call pgline(100,x,why2)
           call pgsls(1)
          end if

c! write source name
          call pgsch(0.8)
          str='Association :'
          call sub_gsm_plot(1,i*1.d0,mnx+0.7*(mxx-mnx),
     /         mxy-0.05*(mxy-mny)-0.03*(mxy-mny),str)
          do j=1,nf
           if (tofit(j).eq.1) then
            if (assoc_arr(i,j).ne.0) then
             str=filename(j)(1:nchar(filename(j)))//'.'//
     /         solnname(j)(1:nchar(solnname(j)))//' : '//
     /         names(j,assoc_arr(i,j))
            else
             str=filename(j)(1:nchar(filename(j)))//'.'//
     /         solnname(j)(1:nchar(solnname(j)))//' : '
            end if
            call pgtext(mnx+0.05*(mxx-mnx),mny+0.05*(mxy-mny)+(j-1)*
     /           0.03*(mxy-mny),str(1:nchar(str)))
           end if
          end do

c! write fit values
          if (spin_arr(i,1).gt.-990.d0) then
           str='I\\d00\\u : '
           call sub_gsm_plot(2,spin_arr(i,1),mnx+0.7*(mxx-mnx),
     /          mxy-0.05*(mxy-mny)-0.06*(mxy-mny),str)
           str='\\ga\\d00\\u : '
           call sub_gsm_plot(1,spin_arr(i,2),mnx+0.7*(mxx-mnx),
     /          mxy-0.05*(mxy-mny)-0.09*(mxy-mny),str)
          end if
          if (spin_arr(i,3).gt.-990.d0) then
           str='I\\d10\\u : '
           call sub_gsm_plot(2,spin_arr(i,3),mnx+0.7*(mxx-mnx),
     /          mxy-0.05*(mxy-mny)-0.12*(mxy-mny),str)
           str='\\ga\\d10\\u : '
           call sub_gsm_plot(1,spin_arr(i,4),mnx+0.7*(mxx-mnx),
     /          mxy-0.05*(mxy-mny)-0.15*(mxy-mny),str)
           str='\\ga\\d11\\u : '
           call sub_gsm_plot(1,spin_arr(i,5),mnx+0.7*(mxx-mnx),
     /          mxy-0.05*(mxy-mny)-0.18*(mxy-mny),str)
          end if
          call pgsch(1.0)
 
          call pgclos
         end if  ! if > 1 pt to plot
        end do

c! put them all in a nice ps file
        if (makepdf.eq.'true') then
        incg='echo "\\includegraphics[width=3in,height=3in]{'
        otfn=plotdir(1:nchar(plotdir))//gsmsolnname
     /   (1:nchar(gsmsolnname))//'/gsm_'//gsmsolnname(1:nchar
     /   (gsmsolnname))//'.tex'
        call system('cp '//scratch(1:nchar(scratch))//
     /   'gsm_template.tex '//otfn(1:nchar(otfn)))
        open(unit=n2+3,file=scratch(1:nchar(scratch))//'a_gsm')
        write (n2+3,'(a12)') '#! /bin/tcsh'
        do i=1,nplot-5,6
         write (n2+3,*) 'echo "\\begin{figure}" >>'//otfn(1:nchar(otfn))
         write (n2+3,*) incg(1:nchar(incg))//psname(i)(1:nchar
     /    (psname(i)))//'} ~~~~~~~~~~~~" >> '//otfn(1:nchar(otfn))
         write (n2+3,*) incg(1:nchar(incg))//psname(i+1)(1:nchar
     /    (psname(i+1)))//'} \\\\\\ \\\\\\\\\\\\\\\\\\ " >> '
     /    //otfn(1:nchar(otfn))
         write (n2+3,*) incg(1:nchar(incg))//psname(i+2)(1:nchar
     /    (psname(i+2)))//'}~~~~~~~~~~~~" >> '//otfn(1:nchar(otfn))
         write (n2+3,*) incg(1:nchar(incg))//psname(i+3)(1:nchar
     /    (psname(i+3)))//'} \\\\\\ \\\\\\\\\\\\\\\\\\ " >> '
     /    //otfn(1:nchar(otfn))
         write (n2+3,*) incg(1:nchar(incg))//psname(i+4)(1:nchar
     /    (psname(i+4)))//'}~~~~~~~~~~~~" >> '//otfn(1:nchar(otfn))
         write (n2+3,*) incg(1:nchar(incg))//psname(i+5)(1:nchar
     /    (psname(i+5)))//'} \\\\\\ \\\\\\\\\\\\\\\\\\ " >> '
     /    //otfn(1:nchar(otfn))
         write (n2+3,*) 'echo "\\end{figure}" >> '//otfn(1:nchar(otfn))
         write (n2+3,*) 'echo "\\clearpage" >> '//otfn(1:nchar(otfn))
        end do
        write (n2+3,*) 'echo "\\end{document}" >> '//otfn(1:nchar(otfn))
        write (n2+3,*) 'cd '//plotdir(1:nchar(plotdir))//
     /   gsmsolnname(1:nchar(gsmsolnname))//'; latex '//
     /   otfn(1:nchar(otfn))//' > '//
     /   scratch(1:nchar(scratch))//'gsm_latex_log'
        write (n2+3,*) 'cd '//plotdir(1:nchar(plotdir))//
     /   gsmsolnname(1:nchar(gsmsolnname))//'; dvipdf '//
     /   'gsm_'//gsmsolnname(1:nchar(gsmsolnname))
        close(n2+3)
        write (*,*) '  Latexing file with plots'
        call system('source '//scratch(1:nchar(scratch))//'a_gsm')
        end if

        return
        end
c!
c!
c!
        subroutine sub_gsm_plot(formm,dumr,x,y,str1)
        implicit none
        real*4 x,y,dumr4
        real*8 dumr
        character str*500,str1*500
        integer nchar,mm,pp,nc,formm

        dumr4=dumr
        call get_pgnumb(dumr4,mm,pp)
        call pgnumb(mm,pp,formm,str,nc)
        call pgtext(x,y,str1(1:nchar(str1))//str(1:nc))
        
        return
        end






