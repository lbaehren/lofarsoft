        subroutine trial(image,x,y,n,m)
        implicit none
        integer x,y,n,m,numco,xco(2*n+2*m),yco(2*n+2*m),i
        integer xc,yc,wid
        real*8 image(x,y),image1(n,m)
        real*4 tr(6),arr4(n,m)

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/

        write (*,*) '   Enter centre and width'
        read (*,*) xc,yc,wid
        call getco_ringwidth(image,x,y,n,m,xc,yc,wid,
     /             xco,yco,numco)
        
        call initialiseimage(image1,n,m,n,m,0.d0)
        do 100 i=1,numco
         image1(xco(i),yco(i))=1.d0 
100     continue 
        
        call array8to4(image1,n,m,arr4,n,m)
        call pgbegin(0,'/xs',1,1)
        call pgwindow(0.5,n*1.0+0.5,0.5,m*1.0+0.5)
        call pgbox('BCINGST',0.0,0,'BCIGNST',0.0,0)
        call pggray(arr4,n,m,1,n,1,m,1.0,0.0,tr)
        call pgbox('BCINGST',0.0,0,'BCIGNST',0.0,0)
        call pgend


        return
        end
