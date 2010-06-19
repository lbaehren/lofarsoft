
        implicit none
        integer i,j,gpi
        parameter (gpi=2)
        real*8 xpix(gpi),ypix(gpi),bmaj(gpi)
        real*8 bmin(gpi),bpa(gpi)
        real*8 fwhm1,fwhm2

        xpix(1)=483.79017d0
        xpix(2)=483.842421d0
        ypix(1)=748.11867995d0
        ypix(2)=741.96593175d0

        i=1
        j=2

        bmaj(1)=12.109668d0
        bmin(1)=3.79554658d0
        bpa(1)=139.0067797d0
        bmaj(2)=5.820053d0
        bmin(2)=3.594009d0
        bpa(2)=41.1697886d0

        call gdist_pa(i,j,gpi,xpix,ypix,bmaj,bmin,bpa,fwhm1)
        call gdist_pa(j,i,gpi,xpix,ypix,bmaj,bmin,bpa,fwhm2)

    
        write (*,*) fwhm1, fwhm2

        end

