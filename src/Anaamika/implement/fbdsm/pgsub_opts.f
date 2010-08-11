c! all the options

        subroutine pgsub_op1(vport,blc,trc,i,j,ch1,chold)
        implicit none
        integer i,j
        real*4 blc(2),trc(2),xcur,ycur,xcur1,ycur1,vport(4)
        character ch1*1,chold*1,ch11*1
        
        call pgvport(vport(1),vport(2),vport(3),vport(4))
        call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
        call pgcurs(xcur,ycur,ch11)
        call pgband(2,1,xcur,ycur,xcur1,ycur1,ch11)
        call setzoom(xcur,ycur,xcur1,ycur1,blc,trc,i,j)
        chold=ch1
        
        return
        end


        subroutine pgsub_op2(ch1,chold)
        implicit none
        character ch1*1,chold*1

        chold=ch1
        
        return
        end

        subroutine pgsub_op3(colour,colourscheme,ch1,chold)
        implicit none
        logical colour
        character ch1*1,chold*1,colourscheme*20

        colour=.false.
        colourscheme='grey'
        chold=ch1

        return
        end

        subroutine pgsub_op4(colour,colourscheme,str1,blacki,ch1,chold)
        implicit none
        logical colour
        integer blacki
        character ch1*1,chold*1,colourscheme*20,str1*5

        colour=.true.
        call get_colourscheme(colourscheme,str1,blacki)
        chold=ch1

        return
        end

        subroutine pgsub_op5(mn1,mx1,mn2,mx2,low,up,ch1,chold)
        implicit none
        character ch1*1,chold*1,ch11*1
        real*4 low,up,mn1,mx1,mn2,mx2,xcur1,ycur1,xcur2

        call pgvport(0.1,0.9,0.1,0.3)
        call pgwindow(mn1,mx1,mn2,mx2)
        call pgband(6,1,mn1,mn2,xcur1,ycur1,ch11)
        call pgmove(xcur1,mn2)
        call pgdraw(xcur1,mx2)
        call pgband(6,1,mn1,mn2,xcur2,ycur1,ch11)
        call pgmove(xcur2,mn2)
        call pgdraw(xcur2,mx2)
        low=min(xcur1,xcur2)
        up=max(xcur1,xcur2)
        chold=ch1

        return
        end


        subroutine pgsub_op6(dum4,blc,trc,i,j,arr4,low,up,ch1,chold)
        implicit none
        character ch1*1,chold*1,ch11*1
        integer i,j,x,y,round4
        real*4 low,up,xcur1,ycur1,xcur,ycur,dum4,blc(2),trc(2),arr4(i,j)

        call pgvport(0.1,0.9,dum4,0.9)
        call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
        call pgcurs(xcur,ycur,ch11)
        call pgband(2,1,xcur,ycur,xcur1,ycur1,ch11)
        call setzoom(xcur,ycur,xcur1,ycur1,blc,trc,i,j)
        call minmaxarr4(arr4,i,j,round4(blc(1)),round4(blc(2)),
     /       round4(trc(1)),round4(trc(2)),low,up)
        chold=ch1

        return
        end

        subroutine pgsub_op7(low,up,ch1,chold,logged,low1)
        implicit none
        character ch1*1,chold*1
        real*4 low,up,low1,l,u
        logical logged

        if (logged) then
         l=10.0**low-low1
         u=10.0**up-low1
        else    
         l=low
         u=up
        end if
334     write (*,'(a,1Pe12.4,a,1Pe12.4,a,$)') 
     /         '   Enter pixrange (',l,' to ',u,' : '
        read (*,*) l,u
        if (l.eq.u) goto 334
        if (l.gt.u) call xchangr8(l,u)
        if (logged) then
         low=log10(l+low1)
         up=log10(u+low1)
        else
         low=l
         up=u
        end if
        chold=ch1

        return
        end

        subroutine pgsub_op8(srldir,dum4,blc,trc,ch1,chold,
     /       colourscheme,starc,linec,blacki,scratch,srlname1,code)
        implicit none
        character ch1*1,chold*1,srlname*500,srldir*500,colourscheme*20
        character extn*20,scratch*500,srlname1*500,code*2
        integer starc,linec,blacki,nchar
        real*4 dum4,blc(2),trc(2)
        logical exists
        
455     write (*,'(a,$)') '   Sourcelistname : ' 
        if (code(1:1).eq.'d') then
         write (*,*) srlname1(1:nchar(srlname1))
         srlname=srlname1
        else
         read (*,*) srlname
        end if
        extn='.gaul'
        if (.not.exists(srlname,srldir,extn)) then
         code(1:1)='u'
         goto 455
        end if
        call pgvport(0.1,0.9,dum4,0.9)
        call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
        if (chold.ne.ch1) then
         if (colourscheme.eq.'grey') then
          starc=1 
         else
          starc=0 
         end if
         linec=0
        end if
        starc=starc+1
        linec=linec+1
        call tvstar(srlname,srldir,starc,linec,scratch,blacki)
        chold=ch1

        return
        end

        subroutine pgsub_op9(srldir,dum4,blc,trc,blacki,scratch,ch1,
     /             chold)
        implicit none
        character srldir*500,starname*500,solnname*500,extn*20,fn*500
        character ch1*1,chold*1,fi*500,ffmt*500,str*500,scratch*500
        character dumc*500
        real*4 dum4,blc(2),trc(2)
        real*8 dumr
        integer blacki,n,m,nisl,nsrc,gpi,nffmt,nchar,ngau
        logical exists

486     write (*,'(a,$)') '   filename solnname : ' 
        read (*,*) starname,solnname
        str=starname(1:nchar(starname))//'.'//
     /      solnname(1:nchar(solnname))
        extn='.gaul.star'
        if (.not.exists(str,srldir,extn)) goto 486
        extn='.gaul'
        if (.not.exists(str,srldir,extn)) goto 486
        call pgvport(0.1,0.9,dum4,0.9)
        call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
        fi=starname(1:nchar(starname))//'.'//
     /       solnname(1:nchar(solnname))//'.gaul.star'
        call getline(fi,srldir,nsrc)
        fi=starname(1:nchar(starname))//'.'//
     /       solnname(1:nchar(solnname))
        fn=srldir(1:nchar(srldir))//fi(1:nchar(fi))//'.gaul'   
        open(unit=22,file=fn(1:nchar(fn)),form='formatted') 
        call readhead_srclist(22,19,'Number_of_gaussians',dumc,ngau,
     /       dumr,'i')
        close(22)
         write (*,*) 'ngau = ',ngau
        call namestars(starname,solnname,srldir,blacki,scratch,
     /       nsrc,ngau)
        chold=ch1

        return
        end


        subroutine pgsub_opA(scratch,dum4,blc,trc,starc,linec,ch1,chold,
     /                 blacki,srldir)
        implicit none
        character scratch*500,srlname*500,extn*20,ch1*1,chold*1
        character srldir*500
        real*4 dum4,blc(2),trc(2)
        integer blacki,linec,starc
        logical exists

425     write (*,'(a,$)') '   Sourcelistname : ' 
        read (*,*) srlname
        extn='.islandlist'
        if (.not.exists(srlname,scratch,extn)) goto 425
        call pgvport(0.1,0.9,dum4,0.9)
        call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
        if (chold.ne.ch1) then
         starc=1 
         linec=0
        end if
        starc=starc+1
        linec=linec+1
        call plot_islnumbers(srldir,srlname,starc,linec,scratch,blacki)
        chold=ch1

        return
        end

        subroutine pgsub_opB(f1,scratch,dum4,blc,trc,ch1,chold)
        implicit none
        character scratch*500,f1*500,extn*20,ch1*1,chold*1
        real*4 dum4,blc(2),trc(2)
        logical exists
        
        extn='.header'
        if (exists(f1,scratch,extn)) then
         call pgvport(0.1,0.9,dum4,0.9)
         call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
         call drawcoords(blc,trc,f1,3,scratch)
        end if
        chold=ch1

        return
        end

        subroutine pgsub_opC(f1,scratch,dum4,blc,trc,starc,id1,id2,
     /     str1,hi,arr4,i,j,up,low,tr,lab,cod1,mn1,mx1,mn2,mx2,
     /     lowh,uph,colourscheme,blacki)
        implicit none
        character scratch*500,f1*500,extn*20,ch1*1,chold*1,cmd*500
        character f2*500,str1*5,lab*500,cod1*2,colourscheme*20
        logical exists
        integer starc,linec,blacki,id1,id2,i,j,nchar,hi
        real*4 arr4(i,j),tr(6),low,up,mn1,mx1,mn2,mx2,blc(2),trc(2)
        real*4 uph,lowh,dum4

        write (*,*) '    AssociateSourceList Plotfiles are : '
        cmd="ls -ltr "//scratch(1:nchar(scratch))//
     /      "*.pasf | sed 's/.pasf//' > b; cat b"
        call system(cmd)
456     write (*,'(a,$)') '   Enter filename : '
        read (*,*) f2
        extn='.pasf'
        if (.not.exists(f2,scratch,extn)) goto 456
        extn='.header'
        if (.not.exists(f1,scratch,extn)) goto 456
        call pgvport(0.1,0.9,dum4,0.9)
        call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
        starc=starc+1
        call overplotpasf(f2,f1,scratch,3,starc,id1,id2,str1,
     /    hi,blc,trc,arr4,i,j,up,low,tr,lab,cod1,mn1,mx1,mn2,mx2,
     /    lowh,uph,dum4,colourscheme,blacki)
        chold=ch1

        return
        end

        subroutine pgsub_opD(srldir,dum4,blc,trc,ch1,chold,
     /       colourscheme,starc,linec,blacki,scratch,srlname1,code)
        implicit none
        character ch1*1,chold*1,srlname*500,srldir*500,colourscheme*20
        character extn*20,scratch*500,srlname1*500,code*2
        integer starc,linec,blacki,nchar,nstars,i
        real*4 dum4,blc(2),trc(2),x4,y4
        real*8 x,y
        logical exists
        
455     write (*,'(a,$)') '   coordinate ascii file name : ' 
        read (*,*) srlname
        extn=''
        if (.not.exists(srlname,srldir,extn)) goto 455

        call pgvport(0.1,0.9,dum4,0.9)
        call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)

        open(unit=21,file=scratch(1:nchar(scratch))//
     /       srlname(1:nchar(srlname)))
        call pgsci(blacki)
        call pgsch(2.0)
        read (21,*) nstars
        do i=1,nstars   
         read (21,*) x,y
         x4=x
         y4=y
         call pgpoint(1,x4,y4,3)
        end do
        close(21)
        call pgsch(1.0)

c        if (chold.ne.ch1) then
c         if (colourscheme.eq.'grey') then
c          starc=1 
c         else
c          starc=0 
c         end if
c         linec=0
c        end if
c        starc=starc+1
c        linec=linec+1
c        call tvstar(srlname,srldir,starc,linec,scratch,blacki)
c        chold=ch1

        return
        end

        subroutine pgsub_opE(dum4,blc,trc,ch1,chold,image,n,m)
        implicit none
        integer n,m
        character ch1*1,chold*1,chr*1
        real*4 dum4,blc(2),trc(2),xcur,ycur
        real*8 image(n,m)

444     continue
        call pgvport(0.1,0.9,dum4,0.9)
        call pgwindow(blc(1)-0.5,trc(1)+0.5,blc(2)-0.5,trc(2)+0.5)
        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)

        call pgtext(blc(1)+0.2*(trc(1)-blc(1)),trc(2)+0.05*(trc(2)-
     /    blc(2)),'Click on pixel or anywhere else to quit')
        call pgcurs(xcur,ycur,chr)
        if (xcur.ge.blc(1)-0.5.and.xcur.le.trc(1)+0.5.and.
     /      ycur.ge.blc(2)-0.5.and.ycur.le.trc(2)+0.5) then
         write (*,'(a18,i5,1x,i5,a22,1Pe13.6,a)') 
     /     '   Coordinates  : ',int(xcur),int(ycur),
     /     ' ;     Flux density : ',image(int(xcur),int(ycur))*1.d3,
     /     ' mJy/beam'
        else
         goto 333
        end if
        if (.true.) goto 444
333     continue
        chold=ch1
        
        return
        end

        subroutine pgsub_opF(ch1,chold,logged,array,x,y,arr4,i,j,
     /             low,up,low1)
        implicit none
        character ch1*1,chold*1
        integer x,y,i,j,l,m
        real*8 array(x,y)
        real*4 arr4(i,j),low,up,low1
        logical logged

        if (logged) then
         do m=1,j
          do l=1,i
           arr4(l,m)=array(l,m)
          end do
         end do
         low=10.0**low-low1
         up=10.0**up-low1 
         logged=.false.
        else
         call arr2dnz(array,x,y,i,j,low,up)
         call converttolog(arr4,i,j,low,up,low1)
         logged=.true.
        end if
        chold=ch1

        return
        end



