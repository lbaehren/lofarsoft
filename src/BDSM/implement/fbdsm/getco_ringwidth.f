c! this will, give central coord and thickness, output the (x,y) of square around which 
c! is a width off from centre, and thickness 1, taking into account edge of image

        subroutine getco_ringwidth(image,x,y,n,m,xc,yc,wid,
     /             xco,yco,numco)
        implicit none
        integer x,y,n,m,i,j,xc,yc,wid
        real*8 image(x,y)
        integer blcx,blcy,trcx,trcy,numco,xco(2*n+2*m),yco(2*n+2*m)
        integer sidex,sidey,num
        
c! get the corners first
        blcx=xc-wid
        if (blcx.lt.1) blcx=1
        blcy=yc-wid
        if (blcy.lt.1) blcy=1
        
        trcx=xc+wid
        if (trcx.gt.n) trcx=n
        trcy=yc+wid
        if (trcy.gt.m) trcy=m

c! number of coords
        sidex=trcx-blcx+1
        sidey=trcy-blcy+1
        numco=((sidex-1)+(sidey-1))*2

c! now get each side - need to get it anticlockwise from blc
        do 100 i=1,sidex-1      ! bottom
         xco(i)=blcx+(i-1)
         yco(i)=blcy
100     continue
        num=sidex-1
        do 110 i=1,sidey-1      ! right
         xco(i+num)=trcx
         yco(i+num)=blcy+(i-1)
110     continue
        num=num+sidey-1

        do 120 i=1,sidex-1      ! top
         xco(i+num)=trcx-(i-1)
         yco(i+num)=trcy
120     continue
        num=num+sidex-1
        do 130 i=1,sidey-1      ! left
         xco(i+num)=blcx
         yco(i+num)=trcy-(i-1)
130     continue

        if (wid.eq.0) then
         numco=1
         xco(1)=xc
         yco(1)=yc
        end if
        
        return
        end













