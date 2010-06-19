c!
c! this is to max bad solutions so as to not write a '*' in .gaul file. stupid formatting.
c ffmt="(3(i5,1x),4(1Pe11.3,1x),4(0Pf13.9,1x),"16(0Pf11.5,1x),4(1Pe11.3,1x),
c!       0Pf7.3,1x,0Pf7.3,5(1x,i5),6(1x,1Pe11.3))"
c! CHANGE FORMAT
        subroutine maxit(fl_s,d,a,m6,delx,dely,d1,d2)
        implicit none
        integer m6,d,delx,dely
        real*8 a(m6),d1,d2
        character fl_s*5

        if (fl_s.ne.'00000') then
         if (fl_s(1:1).eq.'1'.and.abs(a(1+d)).gt.1.d99) 
     /        a(1+d)=9.9d90 
         if (fl_s(1:1).eq.'2'.and.abs(a(1+d)).lt.1.d99) 
     /        a(1+d)=0.d0 
         if (fl_s(2:2).eq.'1'.and.abs(a(2+d)+delx).gt.9999.90) 
     /        a(2+d)=9999.99-delx
         if (fl_s(2:2).eq.'2'.and.abs(a(2+d)+delx).gt.9999.90) 
     /        a(2+d)=0.d0-delx
         if (fl_s(3:3).eq.'1'.and.abs(a(3+d)+delx).gt.9999.90) 
     /        a(3+d)=9999.99-delx
         if (fl_s(3:3).eq.'2'.and.abs(a(3+d)+dely).gt.9999.90) 
     /        a(3+d)=0.d0-dely
         if (fl_s(4:4).eq.'1'.and.abs(a(4+d)*d1).gt.9999.90) 
     /        a(4+d)=9999.99d0/d1
         if (fl_s(4:4).eq.'2'.and.abs(a(4+d)*d1).gt.9999.90) 
     /        a(4+d)=0.d0 
         if (fl_s(5:5).eq.'1'.and.abs(a(5+d)*d2).gt.9999.90) 
     /        a(5+d)=9999.99d0/d2
         if (fl_s(5:5).eq.'2'.and.abs(a(5+d)*d2).gt.9999.90) 
     /        a(5+d)=0.d0 
        end if

        return
        end

