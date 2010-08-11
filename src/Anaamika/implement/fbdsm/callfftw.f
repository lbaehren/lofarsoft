
        subroutine callfftw(f1,f2,n,m,op,scratch,runcode)
        implicit none
        include "fftw3.f"
        integer n,i,j,m
        integer*8 plan
        character f1*500,f2*500,op*500,scratch*500,extn*20,runcode*2
        real*8 image(n,m),im(n,m),image1(n,m)
        double complex fft1(n/2+1,m),fft2(n/2+1,m),fft(n/2+1,m)

cf2py   intent(in) f1,f2,n,m,scratch,op,runcode

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        call readarray_bin(n,m,image1,n,m,f2,extn)

        call dfftw_plan_dft_r2c_2d(plan,n,m,image,fft1,FFTW_ESTIMATE)
        call dfftw_execute(plan)
        call dfftw_destroy_plan(plan)
        call dfftw_plan_dft_r2c_2d(plan,n,m,image1,fft2,FFTW_ESTIMATE)
        call dfftw_execute(plan)
        call dfftw_destroy_plan(plan)

        do i=1,n/2+1
         do j=1,m
          fft(i,j)=fft1(i,j)*fft2(i,j)
         end do
        end do

        call dfftw_plan_dft_c2r_2d(plan,n,m,fft,image,FFTW_ESTIMATE)
        call dfftw_execute(plan)
        call dfftw_destroy_plan(plan)
        call imageop(image,n,m,im,'mul',1.d0/n/m) 

        call writearray_bin2D(im,n,m,n,m,op,runcode)

        return
        end
