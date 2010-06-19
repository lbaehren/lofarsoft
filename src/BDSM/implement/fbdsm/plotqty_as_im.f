c! plot a qty on image as circle with size prop value
c! will first write a star file, and then convert to gaul using st2g and
c! then plot using call_qc_overlaygaussians instead of coding again.

        subroutine plotqty_as_im(scratch,srldir,plotdir,fname,
     /             sname,ratio,rap,decp,nassoc,n,code,bm_pix,
     /             cdelt,gsmsolnname)
        implicit none
        integer nassoc,n,hh,mm,dd,ma,x,y,z,i,nchar
        character sname*500,code*1,s*1,f1*500,srlname*500,runcode*2
        character scratch*500,srldir*500,plotdir*500,fname*500,extn*20
        character gsmsolnname*500,dirn*500,fn*500
        real*8 rap(nassoc),decp(nassoc),ss,sa
        real*8 cdelt(3),bm_pix(3)
        real*4 ratio(nassoc),med,medstd,mn,mx,low,up,sizes(nassoc)
        real*4 mn1,mx1,sizea

c! calc stats
        call calcmedian4(ratio,nassoc,1,n,med)
        call calcmedianstd4(ratio,nassoc,n,med,medstd)
        call range2xakt(ratio,nassoc,n,mn,mx)

c! first calculate sizes 
        low=sqrt(bm_pix(1)*bm_pix(2))/1.5
        up=sqrt(bm_pix(1)*bm_pix(2))*4.0
        if (code.eq.'l') then  ! linear 
         do i=1,n
          sizes(i)=(low-up)/(mn-mx)*ratio(i)+(up*mn-low*mx)/(mn-mx)
         end do
        end if
        if (code.eq.'m') then  ! -5 to 5 med-sig around median
         mn1=max(med-5.0*medstd,mn)
         mx1=min(med+5.0*medstd,mx)
         do i=1,n
          sizes(i)=(low-up)/(mn1-mx1)*ratio(i)+
     /             (up*mn1-low*mx1)/(mn1-mx1)
          if (ratio(i).lt.mn1) sizes(i)=low
          if (ratio(i).gt.mx1) sizes(i)=up
         end do
        end if
        if (code.eq.'L') then  ! log scale 
         if (mn.lt.0.0) then
          mn1=log10(mn-mn+1.0)
          mx1=log10(mx-mn+1.0)
         else
          mn1=log10(mn)
          mx1=log10(mx)
         end if
         do i=1,n
          sizes(i)=(low-up)/(mn1-mx1)*log10(ratio(i))+
     /             (up*mn1-low*mx1)/(mn1-mx1)
         end do
        end if
        
c! write star file : solnname is p2s
        fn=srldir(1:nchar(srldir))//fname(1:nchar(fname))//
     /     '.'//sname(1:nchar(sname))//'.p2s.star'
        open(unit=21,file=fn(1:nchar(fn)),form='formatted') 
        do i=1,n
         call convertra(rap(i),hh,mm,ss)
         call convertdec(decp(i),s,dd,ma,sa)
         sizea=sizes(i)*(sqrt(abs(cdelt(1)*cdelt(2)))*3600.0)  ! pix-->asec
         if (abs(dd).ge.10) then
           write (21,888) hh,mm,ss,s,dd,ma,sa,
     /           sizea,sizea,0.d0,4,1.d0,' '
          else
           write (21,889) hh,mm,ss,s,0,dd,ma,sa,
     /           sizea,sizea,0.d0,4,1.d0,' '
         end if
        end do
        close(21)
888     format(i2,1x,i2,1x,f6.3,1x,a1,i2,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a30)  ! from writeaipsfiles.f + nam(i)
889     format(i2,1x,i2,1x,f6.3,1x,a1,i1,i1,1x,i2,1x,f6.3,1x,f9.4,1x,
     /         f9.4,1x,f7.2,1x,i2,1x,f13.7,1x,a30)  ! from writeaipsfiles.f + nam(i)

c! convert star to gaul
        runcode='aq'
        f1=fname(1:nchar(fname))//'.'//sname(1:nchar(sname))
        srlname=fname(1:nchar(fname))//'.'//sname(1:nchar(sname))//
     /       '.p2s'
        call star2gaulsrl(srlname,f1,runcode,scratch,srldir,'q')

c! plot them
        extn='.img'
        call readarraysize(fname,extn,x,y,z)
        srlname=fname(1:nchar(fname))//'.'//sname(1:nchar(sname))//
     /       '.p2s.s2g'
        dirn=plotdir(1:nchar(plotdir))//'/'//
     /       gsmsolnname(1:nchar(gsmsolnname))
        call sub_plotqty(scratch,srldir,fname,sname,srlname,
     /       dirn,x,y,gsmsolnname)


        return
        end

        subroutine sub_plotqty(scratch,srldir,filename,solnname,
     /             srlname,plotdir,n,m,gsmsolnname)
        implicit none
        integer n,m,nchar
        character scratch*500,srldir*500,filename*500,solnname*500
        character srlname*500,extn*20,psname*500,cs*20,choice*500
        character fullname*500,plotdir*500,lab*500,pdir*500
        character gsmsolnname*500
        real*8 image(n,m)

        extn='.img'
        call readarray_bin(n,m,image,n,m,filename,extn)
        psname='.gsm.fluxratio.ps'
        lab=' '
        cs='grey'
        choice='8D'
        fullname=filename(1:nchar(filename))//'.'//
     /           solnname(1:nchar(solnname))
        pdir=plotdir(1:nchar(plotdir))//
     /       gsmsolnname(1:nchar(gsmsolnname))//'/'
        open(unit=21,file=scratch(1:nchar(scratch))//'/scratch')
        close(21)

        call qc_overlaygaussians(image,psname,n,m,n,m,lab,0,1,'hn',
     /    1,scratch,filename,srldir,cs,choice,fullname,srlname,fullname,
     /    plotdir,21)
        close(21)

        return
        end


