c! nnf has ii, beta nmax centre, std and av and nng has the cf

        subroutine write_shapfiles(ii,beta,nmax,xcen,ycen,cf,stdr,
     /             avr,nnf,nng,delx,dely,ffmt,blc,trc)
        implicit none
        integer ii,nmax,nnf,nng,delx,dely,i,j,blc(2),trc(2)
        real*8 beta,xcen,ycen,cf(nmax,nmax),stdr,avr
        character ffmt*500

        write (nnf,777) ii,nmax,beta,xcen+delx,ycen+dely,stdr,
     /                  avr,blc,trc
        write (nng,666) ii
        do i=1,nmax
         do j=1,nmax-i
          write (nng,888) cf(i,j)
         end do
        end do
        write (nng,*)
777     format(i7,3x,i3,1x,1Pe11.3,1x,2(0Pf11.5,1x),2(1Pe11.3,1x),2x,
     /         4(i5,1x))
888     format(0Pf11.5,1x,$)
666     format(i7,1x,$)
        ffmt="(i7,3x,i3,1x,1Pe11.3,1x,2(0Pf11.5,1x),2(1Pe11.3,1x),"//
     /       "2x,4(i5,1x))"

        return
        end


