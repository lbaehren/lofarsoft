
        implicit none
        real*8 gaus_o(3),gaus_r(3),gaus_d(3)
        integer error

        gaus_o(1)=5.d0
        gaus_o(2)=5.d0
        gaus_o(3)=0.d0

        gaus_r(1)=3.d0
        gaus_r(2)=3.d0
        gaus_r(3)=0.d0

        call deconv(gaus_o,gaus_r,gaus_d,error)
        write (*,*) gaus_o
        write (*,*) gaus_r
        write (*,*) gaus_d,error

        end


        
