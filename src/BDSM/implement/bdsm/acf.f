c! to compute autocorrelation of image. surely this is wrong
c! no it isnt.

        subroutine acf(f1,f2)
        implicit none
        integer n,m,l
        character f1*(*),f2*(*),extn*20

cf2py   intent(in) f1,f2

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'
        call sub_acf(f1,f2,n,m)
        
        return
        end
c!
c!
        subroutine sub_acf(f1,f2,n,m)
        implicit none
        integer n,m,nchar,i,j,i1,j1,two
        character f1*(*),f2*(*),lab*500,strdev*5,str1*1,extn*20
        real*8 image1(n,m),dumr,image2(2*n-1,2*m-1)   ! image1 is main array and image2 is ac
        real*8 s1,ac

        extn='.img'
        call readarray_bin(n,m,image1,n,m,f1,extn)

        two=1
        dumr=log(float(n))/log(2.0)
        if (float(int(dumr)).ne.dumr) two=0
        dumr=log(float(m))/log(2.0)
        if (float(int(dumr)).ne.dumr) two=0
        if (two.eq.0) then
         write (*,*) '   ###  Size of array not power of 2'
         write (*,*) '   ###         Exiting ... '
         goto 333
        end if

        s1=0.d0
        do 300 j=1,m
         do 310 i=1,n
          s1=s1+image1(i,j)*image1(i,j)
310      continue
300     continue

        do 320 i=-(n-1),(n-1)
         do 311 j=-(m-1),(m-1)
          image2(i+n,j+m)=0.d0
311      continue
320     continue

        write (*,'(a,$)') '   '
        do 100 i=-(n-1),(n-1)
         do 110 j=-(m-1),(m-1)
          
          do 120 i1=1,n
           do 130 j1=1,m
            if (i1+i.le.n.and.i1+i.ge.1.and.
     /          j1+j.le.m.and.j1+j.ge.1)
     /       image2(i+n,j+m)=image2(i+n,j+m)+
     /                image1(i1,j1)*image1(i1+i,j1+j)
130        continue
120       continue
          image2(i+n,j+m)=image2(i+n,j+m)/s1

110      continue
         if (mod(i,10).eq.0) write (*,'(a1,$)') '.'
100     continue
        write (*,*) 

        i1=2*n-1
        j1=2*m-1
        lab='Auto-correlation of image'
        call writearray_bin2D(image2,i1,j1,i1,j1,f2,'mv')
        call greyandslice(image2,i1,j1,i1,j1,lab)

        write (*,'(a39,$)') '   Want to fit gaussian to ACF (y)/n ? '
        read (*,*) str1
        if (str1.eq.'n') goto 333
        call callfitgaussian(f2)

333     continue

        return
        end


        subroutine greyandslice(arr,x,y,n,m,lab)
        implicit none
        integer x,y,n,m,i,j,cross,k,n1,m1
        real*8 arr(x,y)
        real*4 arr4(n,m),mn,mx,tr(6)
        real*4 ex(n*n),why(n*n)
        character lab*500

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/

        call array8to4(arr,x,y,arr4,n,m)
        call range2d(arr4,n,m,n,m,mn,mx)
        call pgbegin(0,'/xs',1,1)
        call pgvport(0.1,0.9,0.3,0.9)
        call pgwindow(0.5,n*1.0+0.5,0.5,m*1.0+0.5)
        call pgbox('BCINST',0.0,0,'BCINST',0.0,0)
        call pggray(arr4,n,m,1,n,1,m,mx,mn,tr)
        call pglabel(' ',' ',lab)
                
        n1=(n+1)/2
        m1=(m+1)/2
c! now azimuthal plot, badly done
        cross=n1/2
        k=0
        mx=0.0
        do 135 i=n1-cross,n1+cross
         do 140 j=m1-cross,m1+cross
          k=k+1
          ex(k)=sqrt(((i-n1)**2.d0)+((j-m1)**2.d0))   ! pix sep 
          why(k)=arr4(i,j)
          if (why(k).gt.mx) mx=why(k)
140      continue
135     continue
        call sort2_4(n*n,k,ex,why)

        call pgvport(0.1,0.9,0.1,0.3)
        call pgwindow(-0.5,cross*1.0,mn*0.9,mx*1.1)
        call pgmove(-0.5,0.0)
        call pgdraw(j*1.0,0.0)
        call pgbox('BNST',0.0,0,'BCNVST',0.0,0)
        call pgpoint(k,ex,why,17)
        call pgtext(0.4*j,mx-0.3*(mx-mn),'Diagonal slice plot')
        call pgend
        

        return
        end




