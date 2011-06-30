c! moments above 3 sigma

        subroutine callmoment(f1)
        implicit none
        character f1*(*),extn*20
        real*8 m1,m2(2),m3(2)
        integer nchar,n,m,l

cf2py   intent(in) f1

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        if (l.gt.1) write (*,*) '  Using 2d array for 3d image !!!'

        call moment(f1,n,m,m1,m2,m3)
        write (*,'(5x,a11,1Pe9.2,a3)') 'moment 0 = ',m1,' Jy'
        write (*,'(5x,a11,a1,0Pf8.2,a1,0Pf8.2,a1)') 
     /    'moment 1 = ','(',m2(1),',',m2(2),')'
        write (*,'(5x,a11,0pf8.2,a1,0pf8.2,a7)') 
     /    'moment 2 = ',m3(1)*2.35,',',m3(2)*2.35,' pixels'

        return
        end
c!
c!
        subroutine moment(f1,n,m,m1,m2,m3)
        implicit none
        integer n,m,i,j,nchar
        character f1*500,extn*20,f2*500
        real*8 image(n,m),m1,m2(2),m3(2),std,av
        real*8 mimage(n,m)
        
        m1=0.d0
        m2(1)=0.d0
        m2(2)=0.d0
        m3(1)=0.d0
        m3(2)=0.d0

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        call sigclip(image,n,m,1,1,n,m,std,av,3.d0)
        do 100 j=1,m
         do 110 i=1,n
          if (image(i,j)-av.ge.3.d0*std) then
           m1=m1+image(i,j)
           m2(1)=m2(1)+image(i,j)*i
           m2(2)=m2(2)+image(i,j)*j
           m3(1)=m3(1)+image(i,j)*i*i
           m3(2)=m3(2)+image(i,j)*j*j
           mimage(i,j)=image(i,j)
          else
           mimage(i,j)=0.d0
          end if
110      continue
100     continue
        m2(1)=m2(1)/m1
        m2(2)=m2(2)/m1
        m3(1)=sqrt(m3(1)/m1-m2(1)*m2(1))
        m3(2)=sqrt(m3(2)/m1-m2(2)*m2(2))
        f2=f1(1:nchar(f1))//'.mommask'
        call writearray_bin2D(mimage,n,m,n,m,f2,'mv')
        
        return
        end


        subroutine momentmask(image,mask,x,y,n,m,m1,m2,m3)
        implicit none
        integer x,y,n,m,i,j,mask(x,y)
        real*8 image(x,y),m1,m2(2),m3(2),std,av
        
        m1=0.d0
        m2(1)=0.d0
        m2(2)=0.d0
        m3(1)=0.d0
        m3(2)=0.d0

        call sigclip(image,x,y,1,1,n,m,std,av,3.d0)
        do 100 j=1,m
         do 110 i=1,n
          if (image(i,j)-av.ge.3.d0*std) then
           if (mask(i,j).eq.1) then
            m1=m1+image(i,j)
            m2(1)=m2(1)+image(i,j)*i
            m2(2)=m2(2)+image(i,j)*j
            m3(1)=m3(1)+image(i,j)*i*i
            m3(2)=m3(2)+image(i,j)*j*j
           end if
          end if
110      continue
100     continue
        m2(1)=m2(1)/m1
        m2(2)=m2(2)/m1
        m3(1)=sqrt(m3(1)/m1-m2(1)*m2(1))
        m3(2)=sqrt(m3(2)/m1-m2(2)*m2(2))
        
        return
        end

        subroutine momentmaskonly(image,mask,x,y,n,m,m1,m2,m3)
        implicit none
        integer x,y,n,m,i,j,mask(x,y)
        real*8 image(x,y),m1,m2(2),m3(2)
        
cf2py   intent(in) image,mask,n,m
cf2py   intent(out) m1,m2,m3

        m1=0.d0
        m2(1)=0.d0
        m2(2)=0.d0
        m3(1)=0.d0
        m3(2)=0.d0

        do 100 j=1,m
         do 110 i=1,n
          if (mask(i,j).eq.1) then
           m1=m1+image(i,j)
           m2(1)=m2(1)+image(i,j)*i
           m2(2)=m2(2)+image(i,j)*j
           m3(1)=m3(1)+image(i,j)*i*i
           m3(2)=m3(2)+image(i,j)*j*j
          end if
110      continue
100     continue
        m2(1)=m2(1)/m1
        m2(2)=m2(2)/m1
        m3(1)=sqrt(m3(1)/m1-m2(1)*m2(1))
        m3(2)=sqrt(m3(2)/m1-m2(2)*m2(2))
        
        return
        end

        subroutine momentonlymaskonly(mask,x,y,n,m,m1,m2,m3)
        implicit none
        integer x,y,n,m,i,j,mask(x,y)
        real*8 m1,m2(2),m3(2)
        
        m1=0.d0
        m2(1)=0.d0
        m2(2)=0.d0
        m3(1)=0.d0
        m3(2)=0.d0

        do 100 j=1,m
         do 110 i=1,n
          if (mask(i,j).eq.1) then
           m1=m1+1.d0
           m2(1)=m2(1)+i*1.d0
           m2(2)=m2(2)+j*1.d0
           m3(1)=m3(1)+i*i*1.d0
           m3(2)=m3(2)+j*j*1.d0
          end if
110      continue
100     continue
        m2(1)=m2(1)/m1
        m2(2)=m2(2)/m1
        m3(1)=sqrt(m3(1)/m1-m2(1)*m2(1))
        m3(2)=sqrt(m3(2)/m1-m2(2)*m2(2))
        
        return
        end

