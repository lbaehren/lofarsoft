

        implicit none
        integer n,m
        character f1*500,extn*10
        real*8 image(21,21),image1(11,11)
        real*8 x1a(11),x2a(11),y2a(11,11),dumr
        real*8 images(21,21),imagep(21,21)
        integer i,j

        n=21
        m=21
        f1='opp'
        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        do i=1,21
         do j=1,21
          images(i,j)=image(i,j)
          imagep(i,j)=image(i,j)
          if (mod(i,2).ne.0.and.mod(j,2).ne.0) then
           image1((i+1)/2,(j+1)/2)=image(i,j)
           x1a((i+1)/2)=i*1.d0
           x2a((j+1)/2)=j*1.d0
          end if
         end do
        end do 

        call splie2(x1a,x2a,image1,11,11,y2a)

        do i=1,21
         do j=1,21
          if (mod(i,2).eq.0.and.mod(j,2).eq.0) then
           call splin2(x1a,x2a,image1,y2a,11,11,i*1.d0,j*1.d0,
     /          images(i,j))
           call polin2(x1a,x2a,image1,11,11,i*1.d0,j*1.d0,
     /          imagep(i,j),dumr)
          write (*,*) i,j
          end if
         end do
        end do 

        f1='oppspline'
        call writearray_bin(images,21,21,21,21,f1,'mv')
        f1='opppolin'
        call writearray_bin(imagep,21,21,21,21,f1,'mv')
        f1='oppalter'
        call writearray_bin(image1,11,11,11,11,f1,'mv')

        return
        end

