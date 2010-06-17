c!

        subroutine call_qc_overlaygaussians(srldir,scratch,runcode,
     /        filename,bdsmfullname,qcfullname,n,m,l0,l,plotdir,ofname)
        implicit none
        integer n,m,l0,l,error,nchar
        character srldir*500,scratch*500,runcode*2,qcfullname*500
        character extn*20,psname*500,lab*500,colourscheme*20
        character plotdir*500,ofname*500
        character filename*500,choice*500,dir*500,fg*500,keyword*500
        character keystrng*500,comment*500,dumc*500,bdsmfullname*500
        real*8 array(n,m),dumr

        write (*,*) '  Overplotting gaussians on image'
        extn='.img'
        call readarray_bin(n,m,array,n,m,filename,extn)

        fg="qcparadefine"
        extn=""
        dir="./"
        keyword="overlay_colour"
        call get_keyword(fg,extn,keyword,dumc,dumr,
     /    comment,"s",dir,error)
        colourscheme=dumc(1:20)
        keyword="overlay_code"
        call get_keyword(fg,extn,keyword,choice,dumr,
     /    comment,"s",dir,error)

        psname='.qc_overlay.ps'
        open(unit=28,file=scratch(1:nchar(scratch))//
     /       ofname(1:nchar(ofname)),access='append')
        lab=' '
        call qc_overlaygaussians(array,psname,n,m,n,m,lab,0,1,'hn',
     /       1,scratch,filename,srldir,colourscheme,choice,bdsmfullname,
     /       bdsmfullname,qcfullname,plotdir,28)
        close(28)

        return
        end
c!
c!
c!
        subroutine qc_overlaygaussians(array,psname,x,y,i,j,lab,hi,zero,
     /    cod1,zm,scratch,f1,srldir,cs,choice,bdsmfullname,srlname,
     /    qcfullname,plotdir,n1)
        implicit none
        integer i,j,i1,i2,nchar,x,y,hi,dumi,zero,zm,round4,starc,n1
        integer error,linec,pgopen,id1,id2
        integer blackm,blacki   ! image and menu r diff devices so colourmaps are diff
        integer nffmt,gpi,nisl,nsrc,n,m,nchoices,ichoi
        real*8 array(x,y),keyvalue,rmsclip,avclip
        real*4 arr4(i,j),tr(6),low,up,mn1,mx1,mn2,mx2
        real*4 blc(2),trc(2),xcur,ycur,xcur1,ycur1,dum4
        real*4 xcur2,uph,lowh,d1,d2,d3,low1,vport(4)
        character devi*500
        character str1*5,lab*500,ch1,cod1*2,srlname*500,scratch*500,ch11
        character chold*1,srldir*500,fg*500,extn*20,dir*500,keyword*500
        character comment*500,f1*500,f2*500,cmd*1000,qcfullname*500
        character cs*20,fi*500,ffmt*500,psname*500,choip*1
        character starname*500,solnname*500,choice*500,keystrng*500
        character code2*2,bdsmfullname*500,plotdir*500
        logical exists,colour,logged

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/

        call setup_grey(starc,linec,chold,array,x,y,i,j,low,up,arr4,
     /       zero,lowh,uph,hi,mn1,mx1,mn2,mx2,low1,logged)
        
        code2='d '
        extn='.bstat'
        keyword='rms_clip'
        call get_keyword(bdsmfullname,extn,keyword,keystrng,rmsclip,
     /       comment,'r',scratch,error)
        keyword='mean_clip'
        call get_keyword(bdsmfullname,extn,keyword,keystrng,avclip,
     /       comment,'r',scratch,error)
        low=avclip-4.0*rmsclip
        up=avclip+20.0*rmsclip
        if (logged) then 
         low=log10(low+low1)
         up=log10(up+low1)
        end if

        devi=plotdir(1:nchar(plotdir))//'/gsm_'//
     /     qcfullname(1:nchar(qcfullname))//
     /     psname(1:nchar(psname))//'/CPS'
        write (n1,*) devi
        id2=pgopen(devi(1:nchar(devi)))
c        id2=pgopen('?')
        colour=.false.                    ! start with greyscale
        if (id2.le.0) stop
        call pgslct(id2)
        call pgpap(9.0,1.0)
        call pgpage
        call pgask(.false.)
        starc=1 
        if (hi.eq.1) dum4=0.3
        if (hi.eq.0) dum4=0.1

        nchoices=nchar(choice)
        do ichoi=1,nchoices
         ch1=choice(ichoi:ichoi)
         if (ichoi.gt.1) choip=choice(ichoi-1:ichoi-1)

         if (ichoi.gt.1.and.(choip.eq.'2'.or.choip.eq.'3'.or.
     /       choip.eq.'4'.or.choip.eq.'6')) goto 333
         if (ichoi.gt.1.and.(choip.eq.'1'.or.choip.eq.'5'.or.
     /       choip.eq.'7')) goto 444
         if (ichoi.gt.1.and.(choip.eq.'8'.or.choip.eq.'9'.or.
     /       choip.eq.'A'.or.choip.eq.'B'.or.choip.eq.'C')) goto 234
         if (choip.eq.'D') goto 233
333      continue
         blc(1)=1.0
         blc(2)=1.0
         trc(1)=i*1.0
         trc(2)=j*1.0

444      continue

         call pgslct(id2)
         call pgpage
         call sub_grey_drawimage(id2,hi,blc,trc,arr4,i,j,up,low,tr,lab,
     /     cod1,mn1,mx1,mn2,mx2,lowh,uph,dum4,cs,str1,blacki,logged)
         call pgsci(blacki+1)
         vport(1)=0.1
         vport(2)=0.9
         vport(3)=0.1
         vport(4)=0.9

234      continue

         if (ch1.eq.'1') then                    !! Zoom in
          vport(3)=dum4
          call pgsub_op1(vport,blc,trc,i,j,ch1,chold)
         end if
         if (ch1.eq.'2') then                    !! Zoom out
          call pgsub_op2(ch1,chold)
         end if
         if (ch1.eq.'3') then                    !! Greyscale
          call pgsub_op3(colour,cs,ch1,chold)
         end if
         if (ch1.eq.'4') then                    !! Colour
          call pgsub_op4(colour,cs,str1,blacki,ch1,chold)
         end if
         if (ch1.eq.'5') then                    !! Imagescale - histogram
          call pgsub_op5(mn1,mx1,mn2,mx2,low,up,ch1,chold)
         end if
         if (ch1.eq.'6') then                    !! Imagescale - area
          call pgsub_op6(dum4,blc,trc,i,j,arr4,low,up,ch1,chold)
         end if
         if (ch1.eq.'7') then                    !! Imagescale - pixrange
          call pgsub_op7(low,up,ch1,chold)
         end if
         if (ch1.eq.'8') then                    !! Plot gaussians
          call pgsub_op8(srldir,dum4,blc,trc,ch1,chold,
     /         cs,starc,linec,blacki,scratch,srlname,code2)
         end if
         if (ch1.eq.'9') then                    !! Name stars
          call pgsub_op9(srldir,dum4,blc,trc,blacki,scratch,ch1,chold)
         end if
         if (ch1.eq.'A') then                    !! Plot islands
          call pgsub_opA(scratch,dum4,blc,trc,starc,linec,ch1,chold,
     /                   blacki,srldir)
         end if
         if (ch1.eq.'B') then                    !! Plot coordinate grid
          call pgsub_opB(f1,scratch,dum4,blc,trc,ch1,chold)
         end if
         if (ch1.eq.'C') then                    !! Plot associatelist
          call pgsub_opC(f1,scratch,dum4,blc,trc,starc,id1,id2,str1,hi,
     /   arr4,i,j,up,low,tr,lab,cod1,mn1,mx1,mn2,mx2,lowh,uph,cs,blacki)
         end if
         if (ch1.eq.'D') then                    !! Quit
         end if
233      continue

        end do ! of ichoi
 
        call pgiden
        call pgslct(id2)
        call pgclos

        return
        end

