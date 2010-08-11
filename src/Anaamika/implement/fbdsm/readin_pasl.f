
        subroutine readin_pasl(fn,scrat,scratch,npairs,var,num)
        implicit none
        include "constants.inc"
        character fn*500,scrat*500,f1*500,mname*500,sname*500
        character extn*20,keyword*500,comment*500,keystrng*500
        character scratch*500
        integer npairs,i,nchar,num,j,err
        real*8 var(num,npairs),dumr,freq1,freq2,keyvalue
        real*8 cbmajM,cbminM,cbmajS,cbminS

cf2py   intent(in) fn, scrat, scratch, npairs, num
cf2py   intent(out) var

        j=0
        do i=1,nchar(scrat)-1
         if (scrat(i:i+1).eq.'__') j=i
        end do
        if (j.lt.2.or.j.gt.nchar(scrat)-2) then
         write (*,*) ' Something wrong with file name'
         write (*,*) ' Ending now'
         goto 444
        end if
        mname=scrat(1:j-1)
        sname=scrat(j+2:nchar(scrat))
        extn=".header"
        f1=mname
        keyword="BMAJ"
        call get_keyword(f1,extn,keyword,keystrng,cbmajM,
     /    comment,"r",scratch,err)
        keyword="BMIN"
        call get_keyword(f1,extn,keyword,keystrng,cbminM,
     /    comment,"r",scratch,err)
        f1=sname
        keyword="BMAJ"
        call get_keyword(f1,extn,keyword,keystrng,cbmajS,
     /    comment,"r",scratch,err)
        keyword="BMIN"
        call get_keyword(f1,extn,keyword,keystrng,cbminS,
     /    comment,"r",scratch,err)

        open(unit=21,file=fn(1:nchar(fn)),status='old')
        read (21,*) freq1,freq2
        do i=1,npairs
         read (21,*) var(18,i),var(22,i),var(23,i),var(1,i),var(24,i),
     /    var(2,i),var(3,i),var(4,i),
     /    var(5,i),var(6,i),var(20,i),var(21,i),var(45,i)
         read (21,*) var(19,i),var(25,i),var(26,i),var(7,i),var(27,i),
     /    var(8,i),var(9,i),var(10,i),var(11,i),var(12,i),var(13,i),
     /    var(46,i)

         var(15,i)=var(6,i)-var(12,i)
         var(16,i)=var(4,i)/var(10,i)
         var(17,i)=var(5,i)/var(11,i)
         var(14,i)=var(1,i)/var(7,i)
         var(31,i)=var(1,i)-var(7,i)
         var(28,i)=var(22,i)/var(25,i)
         dumr=(var(2,i)-var(8,i))/rad
         if (abs(dumr*rad).le.360.d0.and.abs(dumr*rad).gt.300.d0) then
          if (dumr.gt.0.d0) dumr=dumr-360.d0/rad
          if (dumr.lt.0.d0) dumr=dumr+360.d0/rad
         end if
         var(34,i)=i*1.d0
c         call correctrarad(dumr)   ! makes all radiff positive!! idiot.
         if (dumr*rad.gt.360.d0) dumr=dumr-360.d0/rad
         dumr=min(dumr,2.d0*pi-dumr)
         var(29,i)=dumr*rad*3600.d0
     /             *dcos(0.5d0/rad*(var(3,i)+var(9,i)))
         var(30,i)=(var(3,i)-var(9,i))*3600.d0
         var(32,i)=var(10,i)*var(11,i)/(var(4,i)*var(5,i))*
     /             cos(var(15,i)/rad)
         var(35,i)=log(var(1,i)/var(7,i))/log(freq1/freq2)
         var(36,i)=log(var(22,i)/var(25,i))/log(freq1/freq2)
         var(37,i)=var(22,i)/var(1,i)
         var(38,i)=var(25,i)/var(7,i)
         var(39,i)=var(4,i)*var(5,i)
         var(40,i)=var(10,i)*var(11,i)
         var(41,i)=var(4,i)/(cbmajM*3600.d0)
         var(42,i)=var(5,i)/(cbminM*3600.d0)
         var(43,i)=var(10,i)/(cbmajS*3600.d0)
         var(44,i)=var(11,i)/(cbminS*3600.d0)
         var(47,i)=var(1,i)/var(45,i)  ! snr
         var(48,i)=var(7,i)/var(46,i)
         var(49,i)=var(22,i)/var(7,i)
         var(50,i)=var(1,i)/var(25,i)
        end do
        close(21)
444     continue
        
        return
        end

