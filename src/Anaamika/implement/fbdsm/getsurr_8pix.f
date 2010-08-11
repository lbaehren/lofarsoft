c! gets coords of surrounding 8 pixels of a pixel.

        subroutine getsurr_8pix(cen,xs,ys,fac)
        implicit none
        integer cen,xs(8),ys(8),x,y,i,j,k,fac

        y=int(cen/fac)
        x=cen-y*fac
        k=0
        do 100 i=x-1,x+1
         do 110 j=y-1,y+1
          if (i.ne.x.or.j.ne.y) then
           k=k+1
           xs(k)=i
           ys(k)=j
          end if
110      continue
100     continue

        return
        end

