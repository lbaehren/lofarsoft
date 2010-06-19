c! fit logI=logI0+spin0*log(f/f0)!+spin1*log^2(f/f0)
c! log10(I) = a(1) + a(2) * log10(f/f0)

        subroutine fitspix(l,spec,freq,fmask,f0,sigma,I0,spin0)
        implicit none
        integer l,k,ia(2),fmask(l),dumi
        real*8 spec(l),freq(l),sigma(l),f0,I0,spin0,spin1
        real*8 x(l),y(l),e(l),aa,bb,chi2,q,siga,sigb
        real*8 a(2),covar(2,2),alpha(2,2),chisq,alamda
        real*4 x4(l),y4(l),y41(l)

        data ia/1,1/
        
        dumi=0
        a(1)=0.d0
        do k=1,l
         x(k)=dlog10(freq(k)/f0)
         y(k)=dlog10(spec(k))
         e(k)=sigma(k)/spec(k)
         if (fmask(k).eq.1) then
          dumi=dumi+1
          a(1)=a(1)+spec(k)
         end if
        end do
        a(2)=-0.7d0
        a(1)=dlog10(a(1)/dumi)


        call pgbegin(0,'/xs',1,1)
         x4(1)=y(1)
         x4(2)=y(1)
        do k=1,l
         if (y(k).gt.x4(1)) x4(1)=y(k)
         if (y(k).lt.x4(2)) x4(2)=y(k)
        end do
        call pgwindow(-0.045,-0.055,x4(2)-0.01,x4(1)+0.01)
        call pgbox('BCNST',0.0,0,'BCNST',0.0,0)
         do k=1,l       
          write (*,*) x(k),y(k),fmask(k),e(k)
         x4(k)=x(k)
         y4(k)=y(k)
        end do
        call pgpoint(l,x4,y4,17)
        call pgline(l,x4,y4)

        call linfitm(x,y,fmask,l,e,1,aa,bb,siga,sigb,chi2,q)

        write (*,*) aa,bb,siga,sigb

         do k=1,l       
         y41(k)=(aa+bb*x(k))
         end do
        call pgsls(2)
        call pgline(l,x4,y41)
        call pgsls(1)
        read (*,*)
         call pgend
        
        return
        end



