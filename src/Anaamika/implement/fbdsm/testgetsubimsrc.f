
        implicit none
        integer x,n
        real*8 bmpix,flux,xposn,yposn

        flux=1.d0
        x=256
        n=7
        xposn=100.3
        yposn=123.3
        bmpix=3
        call get_subim_src(x,n,bmpix,flux,xposn,yposn)

        end


