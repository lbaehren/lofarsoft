c! atrous h fn

        subroutine get_atrous_h(atrous_tr,n3,atrous_b3,n5)
        integer n3,n5
        real*8 atrous_tr(n3,n3)
        real*8 atrous_b3(n5,n5)

        atrous_tr(1,1)=1.d0/16.d0
        atrous_tr(1,2)=1.d0/8.d0
        atrous_tr(1,3)=1.d0/16.d0
        atrous_tr(2,1)=1.d0/8.d0  
        atrous_tr(2,2)=1.d0/4.d0 
        atrous_tr(2,3)=1.d0/8.d0 
        atrous_tr(3,1)=1.d0/16.d0 
        atrous_tr(3,2)=1.d0/8.d0 
        atrous_tr(3,3)=1.d0/16.d0 

        atrous_b3(1,1)=1.d0/256.d0
        atrous_b3(1,2)=1.d0/64.d0
        atrous_b3(1,3)=3.d0/128.d0
        atrous_b3(1,4)=1.d0/64.d0
        atrous_b3(1,5)=1.d0/256.d0
        atrous_b3(2,1)=1.d0/64.d0
        atrous_b3(2,2)=1.d0/16.d0
        atrous_b3(2,3)=3.d0/32.d0
        atrous_b3(2,4)=1.d0/16.d0
        atrous_b3(2,5)=1.d0/64.d0
        atrous_b3(3,1)=3.d0/128.d0
        atrous_b3(3,2)=3.d0/32.d0
        atrous_b3(3,3)=9.d0/64.d0
        atrous_b3(3,4)=3.d0/32.d0
        atrous_b3(3,5)=3.d0/128.d0
        atrous_b3(4,1)=1.d0/64.d0
        atrous_b3(4,2)=1.d0/16.d0
        atrous_b3(4,3)=3.d0/32.d0
        atrous_b3(4,4)=1.d0/16.d0
        atrous_b3(4,5)=1.d0/64.d0
        atrous_b3(5,1)=1.d0/256.d0
        atrous_b3(5,2)=1.d0/64.d0
        atrous_b3(5,3)=3.d0/128.d0
        atrous_b3(5,4)=1.d0/64.d0
        atrous_b3(5,5)=1.d0/256.d0 

        return  
        end

