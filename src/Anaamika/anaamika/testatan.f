
        implicit none
        real*8 x1,y1,x2,y2,dumr1,rad,a

        x1=0.d0
        y1=0.d0
        rad = 180.d0/3.14159

        x2=1.d0
        y2=3.d0
        dumr1=atan(abs((y2-y1)/(x2-x1)))  ! rad, 1st quadrant
        a = dumr1
        call atanproper(dumr1,y2-y1,x2-x1)
        write (*,*) dumr1*rad, a*rad, atan2(y2-y1,x2-x1)*rad
        
        x2=-1.d0
        y2=3.d0
        dumr1=atan(abs((y2-y1)/(x2-x1)))  ! rad, 1st quadrant
        a = dumr1
        call atanproper(dumr1,y2-y1,x2-x1)
        write (*,*) dumr1*rad, a*rad, atan2(y2-y1,x2-x1)*rad
        
        x2=-1.d0
        y2=-3.d0
        dumr1=atan(abs((y2-y1)/(x2-x1)))  ! rad, 1st quadrant
        a = dumr1
        call atanproper(dumr1,y2-y1,x2-x1)
        write (*,*) dumr1*rad, a*rad, atan2(y2-y1,x2-x1)*rad
        
        x2=1.d0
        y2=-3.d0
        dumr1=atan(abs((y2-y1)/(x2-x1)))  ! rad, 1st quadrant
        a = dumr1
        call atanproper(dumr1,y2-y1,x2-x1)
        write (*,*) dumr1*rad, a*rad, atan2(y2-y1,x2-x1)*rad
        


        

        end
