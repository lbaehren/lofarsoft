
        subroutine call_qc_plot_associations(scratch,srldir,runcode,
     /     filename,bdsmsolnname,qcsolnname,plotdir,ofname)
        implicit none
        character scratch*500,srldir*500,runcode*2,filename*500
        character bdsmsolnname*500,qcsolnname*500,scrat*500
        character comment*500,keyword*500,keystrng*500,extn*20,dir*500
        character fg*500,plotdir*500,ofname*500
        real*8 dumr
        integer num_asrl,nlines,i,nchar,npasl,error

        write (*,*) '  Plotting associated source lists'
        fg="qcparadefine"
        extn=""
        dir="./"
        keyword="num_asrl"
        call get_keyword(fg,extn,keyword,keystrng,dumr,
     /    comment,'r',dir,error)
        num_asrl=int(dumr)

c! qc_pasl_opts has (x,y) of things to plot
        call getlines_paslopts(scratch,npasl)
        call system('rm -fr '//plotdir(1:nchar(plotdir))//
     /       qcsolnname(1:nchar(qcsolnname))) 
        call system('mkdir '//plotdir(1:nchar(plotdir))//
     /       qcsolnname(1:nchar(qcsolnname))) 
        call qc_plot_associations(scratch,srldir,num_asrl,npasl,plotdir,
     /       ofname,qcsolnname)

        return
        end
c!
c!
c!
        subroutine qc_plot_associations(scratch,srldir,num_asrl,npasl,
     /             plotdir,ofname,qcsolnname)
        implicit none
        character scratch*500,srldir*500,scrat*500,str*500,plotdir*500
        character ofname*500,qcsolnname*500
        integer num_asrl,nlines,i,nchar,npasl,xopt(npasl),yopt(npasl)
        integer dumi,hh,mm,dd,ma,nnum
        real*8 ss,sa,racasa,deccasa,ra0(npasl),dec0(npasl)
        
        racasa=350.84999d0  ! both in deg, J2000.0
        deccasa=58.815d0

c! first get the plotting options
        open(unit=27,file=scratch(1:nchar(scratch))//'qc_pasl_opts')
        do i=1,npasl
         read (27,*) xopt(i),yopt(i)
         if (xopt(i).eq.33.or.yopt(i).eq.33) then
          backspace(27)
          read (27,'(a500)') str
          call num_num(str,nnum)  ! number of numbers in str
          if (nnum.ne.8) then  !  Cas A
           ra0(i)=racasa
           dec0(i)=deccasa
          else
           read (str,*) dumi,dumi,hh,mm,ss,dd,ma,sa
           ra0(i)=(hh+mm/60.d0+ss/3600.d0)*15.d0
           dec0(i)=abs(dd)+abs(ma)/60.d0+abs(sa)/3600.d0
           if (dd.lt.0.or.ma.lt.0.or.sa.lt.0.d0) dec0(i)=-dec0(i)
          end if
         end if
        end do
        close(27)

c! qc_pasl_list has the list of filenames.pasl
        open(unit=28,file=scratch(1:nchar(scratch))//
     /       ofname(1:nchar(ofname)),access='append')
        open(unit=27,file=scratch(1:nchar(scratch))//'qc_pasl_list')
        do i=1,num_asrl
         read (27,*) scrat
         call getnlines_pasl(scrat,scratch,nlines)
         if (mod(nlines,2).ne.0) then
          write (*,*) ' !!!!!  ERROR !!!'
         else
          call qc_pasl(scrat,scratch,srldir,nlines/2,npasl,xopt,
     /                 yopt,ra0,dec0,plotdir,28,qcsolnname)
         end if
        end do
        close(27)
        close(28)

        return
        end
c!
c!
c!
        subroutine qc_pasl(scrat,scratch,srldir,npairs,npasl,xopt,
     /                     yopt,ra0,dec0,plotdir,n1,qcsolnname)
        implicit none
        character scrat*500,scratch*500,srldir*500,title(40)*30
        character fn*500,extn*20,label(40)*30,fname*500,qcsolnname*500
        character chr1*1,xl*6,yl*6,plotdir*500,rcode*2,ofname*500
        integer nchar,npairs,i,idm(npairs),ids(npairs),iquit
        integer hh,mm,dd,ma,iplot,n1
        real*8 var(100,npairs),dumr,ss,sa,ra,dec
        real*4 xplot(npairs),yplot(npairs)
        integer npasl,xopt(npasl),yopt(npasl),id(2)
        real*8 ra0(npasl),dec0(npasl)

        data label/'Peak (srl 1) ','R.A. (srl 1)','Dec (srl 1)',
     /    'Bmaj (srl 1)','Bmin (srl 1)','Bpa (srl 1)','Peak (srl 2)',
     /    'R.A. (srl 2)','Dec (srl 2)','Bmaj (srl 2)','Bmin (srl 2)',  ! 11
     /    'Bpa (srl 2)','Distance ','Peak ratio (srl 1/2)',
     /    'Bpa diff (srl 1-2)','Bmaj ratio (srl 1/2)',
     /    'Bmin ratio (srl 1/2)','Src id (srl 1)','Src id (srl 2)',    ! 19
     /    'src residrms','isl residrms','Total (srl 1)','eTot (srl 1)',
     /    'ePeak (srl 1)','Total (srl 2)','eTot (srl 2)',
     /    'ePeak (srl 2)','Total ratio (srl 1/2)','RAdiff (srl 1-2)',
     /    'DECdiff (srl 1-2)','index','','','','','','','','',''/
        data title/'PeakM','RAM','DecM)','BmajM','BminM','BpaM','PeakS',
     /    'RAS','DecS','BmajS','BminS','BpaS','Dist','PeakRatio',
     /    'BpaDiff','BmajRatio','BminRatio','SrcidM','SrcidS',
     /    'Src_residrms','Isl_residrms','TotalM','eTotM','ePeakM',
     /    'TotalS','eTotS','ePeakS','TotalRatio','RAdiff','DECdiff',
     /    'index','','','','','','','','',''/

        chr1='d'
        rcode=' q' 
        extn='.pasf'
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//
     /     extn(1:nchar(extn))
        call readin_pasl(fn,scrat,scratch,npairs,var,100)
        
        do iplot=1,npasl
         if (xopt(iplot).eq.33.or.yopt(iplot).eq.33) then    ! dist from ref posn
          do i=1,npairs
           call justdist(ra0(iplot),var(8,i),dec0(iplot),var(9,i),
     /          var(33,i))
          end do
         end if

         id(1)=xopt(iplot)
         id(2)=yopt(iplot)
         call setup_pasl(xl,yl,npairs,var,100,xplot,yplot,id)

         fname=plotdir(1:nchar(plotdir))//
     /         qcsolnname(1:nchar(qcsolnname))//'/'//
     /         scrat(1:nchar(scrat))//'_'//
     /         title(xopt(iplot))(1:nchar(title(xopt(iplot))))//'_'//
     /      title(yopt(iplot))(1:nchar(title(yopt(iplot))))//'.ps/VPS'
         write (n1,*) fname(1:nchar(fname))
         call plot_menu(xplot,yplot,npairs,label(xopt(iplot)),
     /        label(yopt(iplot)),chr1,xl,yl,fname,rcode,scrat,'n')
        end do  ! each plot

        return
        end
c!
c!
c!
        subroutine getlines_paslopts(scratch,npasl)
        implicit none
        integer npasl,nchar
        character scratch*500,str*500

        open(unit=27,file=scratch(1:nchar(scratch))//'qc_pasl_opts')
        npasl=0
333     continue
        read (27,*) str
        if (str(1:3).ne.'END') then
         npasl=npasl+1
         goto 333
        end if
        close(27)

        return
        end





