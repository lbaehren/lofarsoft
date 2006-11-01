c! take .pasf file and plot each against the other

        subroutine plot_associatelist1(scrat,scratch)
        implicit none
        character scrat*500,fn*500,extn*10,scratch*500
        integer nchar,nlines
        
        extn='.pasf'
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//
     /     extn(1:nchar(extn))
        call system('rm -f '//scratch(1:nchar(scratch))//'aa')
        call system('wc -l '//fn(1:nchar(fn))//' > '//
     /       scratch(1:nchar(scratch))//'aa')
        open(unit=31,file=scratch(1:nchar(scratch))//'aa',status='old')
        read(31,*) nlines
        close(31)
        call system('rm -f '//scratch(1:nchar(scratch))//'aa')
        if (mod(nlines,2).ne.0) then
         write (*,*) ' !!!!!  ERROR !!!'
        else
         call sub_plot_associatelist1(scrat,nlines/2,scratch)
        end if
        
        return
        end
c!
c! -----------------------------  SUBROUTINES  -------------------------------
c!
        subroutine sub_plot_associatelist1(scrat,npairs,scratch)
        implicit none
        include "constants.inc"
        character scrat*500,fn*500,extn*10,label(30)*30,scratch*500
        integer nchar,npairs,i,idm(npairs),ids(npairs),id(2),iquit
        real*8 var(30,npairs)
        real*4 xplot(npairs),yplot(npairs)
        
        data label/'Flux (srl 1) ','R.A. (srl 1)','Dec (srl 1)',
     /    'Bmaj (srl 1)','Bmin (srl 1)','Bpa (srl 1)','Flux (srl 2)',
     /    'R.A. (srl 2)','Dec (srl 2)','Bmaj (srl 2)','Bmin (srl 2)',  ! 11
     /    'Bpa (srl 2)','Distance ','Flux ratio (srl 1/2)',
     /    'Bpa diff (srl 1-2)','Bmaj ratio (srl 1/2)',
     /    'Bmin ratio (srl 1/2)','Src id (srl 1)','Src id (srl 2)'     ! 19
     /    ,'src residrms','isl residrms','','','','','','','','',''/

        extn='.pasf'
        fn=scratch(1:nchar(scratch))//scrat(1:nchar(scrat))//
     /     extn(1:nchar(extn))
        open(unit=21,file=fn(1:nchar(fn)),status='old')
        do i=1,npairs
         read (21,*) var(18,i),var(1,i),var(2,i),var(3,i),var(4,i),
     /               var(5,i),var(6,i),var(20,i),var(21,i)
         read (21,*) var(19,i),var(7,i),var(8,i),var(9,i),var(10,i),
     /               var(11,i),var(12,i),var(13,i)
         var(15,i)=var(6,i)-var(12,i)
         var(16,i)=var(4,i)/var(10,i)
         var(17,i)=var(5,i)/var(11,i)
         var(14,i)=var(1,i)/var(7,i)
         var(22,i)=var(10,i)*var(11,i)/(var(4,i)*var(5,i))*
     /             cos(var(15,i)/rad)
         var(22,i)=var(21,i)
        end do
        close(21)

        call writemenu_plotasrl(iquit)
        write (*,*) 
333     write (*,'(a,$)') '  Enter numbers to plot : '
        read (*,*) id(1),id(2)
        if (max(id(1),id(2)).gt.iquit) goto 333
        if (id(1).ne.iquit.and.id(2).ne.iquit) then
         do i=1,npairs
          xplot(i)=var(id(1),i)
          yplot(i)=var(id(2),i)
         end do 
         call plot_menu(xplot,yplot,npairs,label(id(1)),label(id(2)))
         goto 333
        end if
        
        return
        end
c!
c!      ----------
c!
        subroutine writemenu_plotasrl(nmenu)               ! defines order of var
        implicit none
        integer nmenu

        write (*,*) 
        write (*,*) ' 1. fluxM      2. raM         3. decM  '
        write (*,*) ' 4. bmajM      5. bminM       6. bpaM  '
        write (*,*) ' 7. fluxS      8. raS         9. decS  '
        write (*,*) '10. bmajS     11. bminS      12. bpaS  '
        write (*,*) '13. dist      14. fluxratio  15. bpadiff'
        write (*,*) '16. bmajratio 17. bminratio  18. idM '
        write (*,*) '19. idS       20. src resrms 21. isl resrms' 
        write (*,*) '22. expe      23. quit '
        write (*,*) 
        nmenu=23

        return
        end
c!
c!      ----------
c!
        subroutine plot_menu(xplot,yplot,n,label1,label2)
        implicit none
        integer n
        real*4 xplot(n),yplot(n),mnx,mxx,mny,mxy,mny1,mxy1
        real*4 minxo,maxxo,mnx1,mxx1,dum41,dum42,minyo,maxyo
        character label1*30,label2*30

        call range_vec4mxmn(xplot,n,n,mnx,mxx)
        call range_vec4mxmn(yplot,n,n,mny,mxy)
        minxo=mnx
        maxxo=mxx
        minyo=mny
        maxyo=mxy
        if (label1(1:6).eq.label2(1:6)) then
         mnx=min(mnx,mny)
         mny=mnx
         mxx=max(mxx,mxy)
         mxy=mxx
        end if
        call gethistpara(n,n,xplot,minxo,maxxo,mnx1,mxx1)
        call gethistpara(n,n,yplot,minyo,maxyo,mny1,mxy1)

c        call pgbegin(0,'/xs',1,1)
c        call pgvport(0.22,0.77,0.35,0.9)
c        call pgwindow(mnx,mxx,mny,mxy)
c        call pgbox('BCNST',0.0,0,'BCNST',0.0,0)
c        call pglabel(label1,label2,' ')
c        call pgpoint(n,xplot,yplot,3)

c        call pgvport(0.05,0.44,0.05,0.25)
c        call pgwindow(minxo,maxxo,mnx1,mxx1)
c        call pgbox('BCNST',0.0,0,'BCVNST',0.0,0)
c        call pghist(n,xplot,minxo,maxxo,100,1)
        dum41=minxo+0.6*(maxxo-minxo)
        dum42=mxx1-0.3*(mxx1-mnx1)
c        call pgtext(dum41,dum42,label1)

c        call pgvport(0.56,0.95,0.05,0.25)
c        call pgwindow(minyo,maxyo,mny1,mxy1)
c        call pgbox('BCNST',0.0,0,'BCVNST',0.0,0)
c        call pghist(n,yplot,minyo,maxyo,100,1)
        dum41=minyo+0.6*(maxyo-minyo)
        dum42=mxy1-0.3*(mxy1-mny1)
c        call pgtext(dum41,dum42,label2)

c        call pgend
 
        return
        end



