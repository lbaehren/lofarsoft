

        implicit none
        integer ndata,i
        parameter (ndata=8)
        real*8 x(ndata),y(ndata),sig(ndata)
        real*8 a,b,chi2,q,siga,sigb

        data x/1.45129879e+09,1.43124020e+09,1.41118161e+09,
     /   1.39112301e+09,1.37106442e+09,1.35100582e+09,1.33094723e+09,
     /   1.31088864e+09/
        data y/0.00023554,0.00021757,0.00024432,0.00026804,
     /    0.0003184,0.0003785,0.00039437,0.00046039/
        data sig/3.52717845e-05,3.32313838e-05,3.58887777e-05,
     /     3.79227091e-05,3.73481751e-05,3.95627171e-05,
     /    4.04449899e-05,3.94616665e-05/

        do i = 1,8
         x(i) = log10(x(i)/200.d6)
         sig(i) = abs(sig(i)/y(i))/2.303
         y(i) = log10(y(i))
        end do

        call linfit(x,y,ndata,sig,1,a,b,siga,sigb,chi2,q)
        write (*,*) a,b
        write (*,*) siga,sigb

        end


