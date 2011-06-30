
        subroutine av_psf(ncut,bmaj,bmin,bpa,cbmaj,cbmin,cbpa,
     /       peak,im_rms,wtavbm,wtstdbm,f_sclip)
        implicit none
        integer ncut,i,f_sclip(ncut)
        real*8 bmaj(ncut),bmin(ncut),bpa(ncut),cbmaj,cbmin,cbpa 
        real*8 peak(ncut),im_rms(ncut),dumr,w,dd
        real*8 dumr1(3),w1,wtavbm(3),wtstdbm(3),avpa,stdpa

        do i=1,3
         wtavbm(i)=0.d0
         wtstdbm(i)=0.d0
         dumr1(i)=0.d0
        end do
        w1=0.d0
        dumr=0.d0
        avpa=0.d0
        stdpa=0.d0

        do i=1,ncut
         w=peak(i)/im_rms(i)
         if (f_sclip(i).eq.0) then
          wtavbm(1)=wtavbm(1)+bmaj(i)*w
          wtavbm(2)=wtavbm(2)+bmin(i)*w
          wtavbm(3)=wtavbm(3)+bpa(i)*w
          if (bpa(i).lt.90.d0) then
           avpa=avpa+bpa(i)*w
          else
           avpa=avpa+(bpa(i)-180.d0)*w
          end if
          dumr=dumr+w
         end if
        end do
        wtavbm(1)=wtavbm(1)/dumr
        wtavbm(2)=wtavbm(2)/dumr
        wtavbm(3)=wtavbm(3)/dumr
        avpa=avpa/dumr

        do i=1,ncut
         w=peak(i)/im_rms(i)
         if (f_sclip(i).eq.0) then
          dumr1(1)=dumr1(1)+w*bmaj(i)*bmaj(i)
          dumr1(2)=dumr1(2)+w*bmin(i)*bmin(i)
          dumr1(3)=dumr1(3)+w*bpa(i)*bpa(i)
          if (bpa(i).lt.90.d0) then
           stdpa=stdpa+w*bpa(i)*bpa(i)
          else
           stdpa=stdpa+w*(bpa(i)-180.d0)*(bpa(i)-180.d0)
          end if
          w1=w1+w*w
         end if
        end do
        dd=dumr*dumr-w1
        wtstdbm(1)=sqrt((dumr1(1)-wtavbm(1)*wtavbm(1)*dumr)*dumr/dd)
        wtstdbm(2)=sqrt((dumr1(2)-wtavbm(2)*wtavbm(2)*dumr)*dumr/dd)
        wtstdbm(3)=sqrt((dumr1(3)-wtavbm(3)*wtavbm(3)*dumr)*dumr/dd)
        stdpa=sqrt((stdpa-avpa*avpa*dumr)*dumr/dd)

        if (stdpa.lt.wtstdbm(3)) then   
         wtstdbm(3)=stdpa
         wtavbm(3)=avpa
        end if

        write (*,*) wtavbm
        write (*,*) wtstdbm
        write (*,*) (wtavbm(1)-cbmaj*3600)/wtstdbm(1),
     /              (wtavbm(2)-cbmin*3600)/wtstdbm(2),
     /              (wtavbm(3)-cbpa)/wtstdbm(3)

        return
        end

