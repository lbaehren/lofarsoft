c! for now only HEAT (take from ds9 files)
         
        subroutine set_colourmap(colourscheme,clr1,clr2)    ! sets colourmap
        implicit none
        include "pg_sls.inc"
        integer clr1,clr2,i,j
        character colourscheme*20
        real indx,red,green,blue,set_linearcolour

        if (colourscheme.eq.'heat') then                  !  HEAT !
         call pgscir(clr1,clr2)
         do i=clr1,clr2
          indx=(i-clr1)*1.0/(clr2-clr1)
          if (indx.le.0.34) then 
           red=set_linearcolour(indx,0.0,0.34,0.0,1.0)
          else
           red=1.0
          end if
          green=set_linearcolour(indx,0.0,1.0,0.0,1.0)
          if (indx.le.0.65) then 
           blue=0.0
          else
           if (indx.le.0.98) then 
            blue=set_linearcolour(indx,0.65,0.98,0.0,1.0)
           else
            blue=1.0
           end if
          end if
          call pgscr(i,red,green,blue)
         end do
        end if

        if (colourscheme.eq.'cool') then                  !  COOL !
         call pgscir(clr1,clr2)
         do i=clr1,clr2
          indx=(i-clr1)*1.0/(clr2-clr1)
          if (indx.le.0.29) then 
           red=0.0
          else
           if (indx.le.0.76) then 
            red=set_linearcolour(indx,0.29,0.76,0.0,0.1)
           else
            red=set_linearcolour(indx,0.76,1.0,0.1,1.0)
           end if
          end if
          if (indx.le.0.22) then 
           green=0.0
          else
           if (indx.le.0.96) then 
            green=set_linearcolour(indx,0.22,0.96,0.0,1.0)
           else
            green=1.0
           end if
          end if
          if (indx.le.0.53) then 
           blue=set_linearcolour(indx,0.0,0.53,0.0,1.0)
          else
           blue=1.0
          end if
          call pgscr(i,red,green,blue)
         end do
        end if

        if (colourscheme.eq.'rainbow') then              ! RAINBOW !
         call pgscir(clr1,clr2)
         do i=clr1,clr2
          indx=(i-clr1)*1.0/(clr2-clr1)
          if (indx.le.0.2) then 
            red=set_linearcolour(indx,0.0,0.2,1.0,0.0)
          else
           if (indx.le.0.6) then 
            red=0.0
           else
            if (indx.le.0.8) then 
             red=set_linearcolour(indx,0.6,0.8,0.0,1.0)
            else
             red=1.0
            end if
           end if
          end if
          if (indx.le.0.2) then 
           green=0.0
          else
           if (indx.le.0.4) then 
            green=set_linearcolour(indx,0.2,0.4,0.0,1.0)
           else
            if (indx.le.0.8) then 
             green=1.0
            else
             green=set_linearcolour(indx,0.8,1.0,1.0,0.0)
            end if
           end if
          end if
          if (indx.le.0.4) then 
           blue=1.0
          else
           if (indx.le.0.6) then 
            blue=set_linearcolour(indx,0.4,0.6,1.0,0.0)
           else
            blue=0.0
           end if
          end if
          call pgscr(i,red,green,blue)
         end do
        end if

        if (colourscheme.eq.'sls') then              ! SLS !
         call pgscir(clr1,clr2)
         do i=clr1,clr2
          indx=(i*199.0+(clr2-200.0*clr1))/(clr2-clr1)
          j=int(indx)
          call pgscr(i,slsr(j),slsg(j),slsb(i))
         end do
        end if

        if (colourscheme.eq.'sauron') then              ! SAURON !
         call pgscir(clr1,clr2)
         do i=clr1,clr2
          indx=(i-clr1)*1.0/(clr2-clr1)
          if (indx.le.43.5/256.0) then 
           red=0.0
           green=0.0
           blue=set_linearcolour(indx,0.0,43.5/256.0,0.0,1.0)
          end if
          if (indx.gt.43.5/256.0.and.indx.le.86.0/256.0) then 
           red=set_linearcolour(indx,43.5/256.0,86.0/256.0,0.0,0.4)
           green=set_linearcolour(indx,43.5/256.0,86.0/256.0,0.0,0.85)
           blue=1.0
          end if
          if (indx.gt.86.0/256.0.and.indx.le.106.0/256.0) then 
           red=set_linearcolour(indx,86.0/256.0,106.0/256.0,0.4,0.5)
           green=set_linearcolour(indx,86.0/256.0,106.0/256.0,0.85,1.0)
           blue=1.0
          end if
          if (indx.gt.106.0/256.0.and.indx.le.118.5/256.0) then 
           red=set_linearcolour(indx,106.0/256.0,118.5/256.0,0.5,0.3)
           green=1.0
           blue=set_linearcolour(indx,106.0/256.0,118.5/256.0,1.0,0.7)
          end if
          if (indx.gt.118.5/256.0.and.indx.le.128.5/256.0) then 
           red=set_linearcolour(indx,118.5/256.0,128.5/256.0,0.3,0.0)
           green=set_linearcolour(indx,118.5/256.0,128.5/256.0,1.0,0.9)
           blue=set_linearcolour(indx,118.5/256.0,128.5/256.0,0.7,0.0)
          end if
          if (indx.gt.128.5/256.0.and.indx.le.138.5/256.0) then 
           red=set_linearcolour(indx,128.5/256.0,138.5/256.0,0.0,0.7)
           green=set_linearcolour(indx,128.5/256.0,138.5/256.0,0.9,1.0)
           blue=0.0
          end if
          if (indx.gt.138.5/256.0.and.indx.le.151.0/256.0) then 
           red=set_linearcolour(indx,138.5/256.0,151.0/256.0,0.7,1.0)
           green=1.0
           blue=0.0
          end if
          if (indx.gt.151.0/256.0.and.indx.le.171.0/256.0) then 
           red=1.0
           green=set_linearcolour(indx,151.0/256.0,171.0/256.0,1.0,0.85)
           blue=0.0
          end if
          if (indx.gt.171.0/256.0.and.indx.le.213.5/256.0) then 
           red=1.0
           green=set_linearcolour(indx,171.0/256.0,213.5/256.0,0.85,0.0)
           blue=0.0
          end if
          if (indx.gt.213.5/256.0.and.indx.le.256.0/256.0) then 
           red=set_linearcolour(indx,213.5/256.0,256.0/256.0,1.0,0.9)
           green=set_linearcolour(indx,213.5/256.0,256.0/256.0,0.0,0.9)
           blue=set_linearcolour(indx,213.5/256.0,256.0/256.0,0.0,0.9)
          end if
          call pgscr(i,red,green,blue)
         end do
        end if


        if (colourscheme.eq.'grey') then                   ! GREYSCALE !
         call pgscir(clr1,clr2)
         open(unit=21,file='pg_greyscale',status='old')
         do i=1,100
          read (21,*) j,red,green,blue
          if (i.ge.clr1.and.i.le.clr2) call pgscr(j,red,green,blue)
         end do
         close(21)
        else
         open(unit=21,file='pg_greyscale',status='old')
         do i=1,99-clr2
          read (21,*) j,red,green,blue
          call pgscr(clr2+1+j,red,green,blue)
         end do
         close(21)
        end if



        return
        end
c!
c!
c!
        function set_linearcolour(indx,c1,c2,cr1,cr2)
        implicit none
        real c1,c2,cr1,cr2,indx,set_linearcolour

        set_linearcolour=(indx*(cr2-cr1)+(c2*cr1-c1*cr2))/(c2-c1)

        return
        end



