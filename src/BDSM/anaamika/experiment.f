c! to play with

        subroutine experiment(f1,scratch,srldir)
        implicit none
        character f1*500,fn*500,f2*500,extn*20,scratch*500,srldir*500
        real*8 nsig,dumr
        integer nchar,n,m,l,i,j

cf2py   intent(in) f1,scratch,srldir

c        real*8 image(2048,2048)
c        character sav*1
        
c        fn='image/'//f1
c        open(unit=21,file=fn(1:nchar(fn)),status='old',
c     /       form='unformatted')
c        read (21) n,m
c        call readarray_bin(n,m,image,2048,2048,21)
c        close(21)
        
c        call trial(image,2048,2048,n,m)

c        sav='y'
c        nsig=3.d0
c        call formisland_rank(image,2048,2048,n,m,nsig,sav,f1)

c        extn='.img' 
c        call readarraysize(f1,extn,n,m,l)
c        call testfft(f1)
        
c        call islandedit(f1)
        
c        call mulsrc(f1)
c         call plotisland(f1)

c        call associatesrl(f1,f2)

c        call preprocess(f1)

c         call blah(f1)
c         call blah1(f1,scratch)

c        call analysepsf1(f1,scratch,srldir)
c        call callexp(f1,scratch)
c        call callexp1(scratch)
c        call fn_radius(scratch,f1)
c        call convfft(scratch,f1)
        n=1024
c        call callfftw(scratch,n)

        call callfullmoment(f1,scratch)

        return
        end
        
        subroutine callexp(f1,scratch)
        implicit none
        character f1*500,scratch*500,extn*20
        integer n,m,l

        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        call callcallexp(f1,scratch,n,m)

        return
        end
        
        subroutine callcallexp(f1,scratch,n,m)
        implicit none
        character f1*500,scratch*500,extn*20,rc*2
        integer n,m,l,i,j,nchar
        real*8 image(n,m),im1(n,m)

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn) 
        rc='aq'

        do i=1,n
         do j=1,m
          im1(i,j)=dlog10(abs(image(i,j))+0.1d0) 
         end do
        end do
        f1=f1(1:nchar(f1))//'.da'
        call writearray_bin2D(im1,n,m,n,m,f1,rc)


        return
        end
        
        subroutine callexp1(scratch)
        implicit none
        integer n,m,l
        character scratch*500,f1*500,extn*20

        write (*,*) '  name of image '
        read (*,*) f1
        extn='.img'
        call readarraysize(f1,extn,n,m,l)
        call sub_exp(n,m,f1)

        return
        end

        subroutine sub_exp(n,m,f1)
        implicit none
        integer n,m,i,j,seed,ii,jj,siz,dumi
        real*8 image(n,m),std,av,im(n,m)
        real*8 rand,ran(25,25),s1,s2,sig,dumr
        character f1*500,extn*20,f2*500
        
        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        call arrstat(image,n,m,1,1,n,m,std,av)
        write (*,*) ' original ',std,av
        call getseed(seed)
        write (*,*) 'sigma = '
        read (*,*) sig
        siz=15

        s1=0.d0
        s2=0.d0
        do i=1,siz
         do j=1,siz
          call ran1(seed,rand) 
          ran(i,j)=rand
          dumi=(siz-1)/2
          dumr=(i-dumi)*(i-dumi)+(j-dumi)*(j-dumi)
          ran(i,j)=dexp(-dumr/(2.d0*sig*sig))
          s1=s1+rand
         end do
        end do
        do i=1,siz
         do j=1,siz
          ran(i,j)=ran(i,j)/s1
          s2=s2+ran(i,j)*ran(i,j)
         end do
        end do

        call initialiseimage(im,n,m,n,m,0.d0)
        call ran1(seed,rand)
        do i=siz+1,n-siz-1
         do j=siz+1,m-siz-1
          do ii=1,siz
           do jj=1,siz
            dumi=(siz-1)/2
            im(i,j)=im(i,j)+image(i+(ii-dumi),j+(jj-dumi))*ran(ii,jj)
           end do
          end do
         end do
        end do

        f2='wt'
        call writearray_bin2D(im,n,m,n,m,f2,'mv')
        f2='wts'
        call writearray_bin2D(ran,siz,siz,siz,siz,f2,'mv')
        call arrstat(im,n,m,siz+1,siz+1,n-siz-1,m-siz-1,std,av)
        write (*,*) ' new ',std,av
        write (*,*) ' Sum(w^2) = ',s2,std*sqrt(s2)

        return
        end

