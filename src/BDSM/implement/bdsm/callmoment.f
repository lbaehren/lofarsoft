c! moments above 3 sigma

        subroutine callmoment(f1)
        implicit none
        character f1*(*),extn*10
        real*8 m1,m2(2),m3(2)
        integer nchar,n,m

        extn='.img'
        call readarraysize(f1,extn,n,m)

        call moment(f1,n,m,m1,m2,m3)

        write (*,'(5x,a11,1Pe9.2,a3)') 'moment 0 = ',m1,' Jy'
        write (*,'(5x,a11,a1,0Pf5.1,a1,0Pf5.1,a1)') 
     /    'moment 1 = ','(',m2(1),',',m2(2),')'
        write (*,'(5x,a11,0pf4.1,a1,0pf4.1,a7)') 
     /    'moment 2 = ',m3(1)*2.35,',',m3(2)*2.35,' pixels'

        return
        end
c!
c!
        subroutine moment(f1,n,m,m1,m2,m3)
        implicit none
        integer n,m,i,j
        character f1*(*),extn*10
        real*8 image(n,m),m1,m2(2),m3(2),std,av
        
        m1=0.d0
        m2(1)=0.d0
        m2(2)=0.d0
        m3(1)=0.d0
        m3(2)=0.d0

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        call sigclip(image,n,m,1,1,n,m,std,av,3)
        do 100 i=1,n
         do 110 j=1,m
          if (image(i,j)-av.ge.3.d0*std) then
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


        subroutine momentmask(image,mask,x,y,n,m,m1,m2,m3)
        implicit none
        integer x,y,n,m,i,j,mask(x,y)
        real*8 image(x,y),m1,m2(2),m3(2),std,av
        
        m1=0.d0
        m2(1)=0.d0
        m2(2)=0.d0
        m3(1)=0.d0
        m3(2)=0.d0

        call sigclip(image,x,y,1,1,n,m,std,av,3)
        do 100 i=1,n
         do 110 j=1,m
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

