
        subroutine clip(f1,f2)
        implicit none
        integer n,m
        character f1*(*),extn*10,f2*(*)

        extn='.img'
        call readarraysize(f1,extn,n,m)
        call sub_clip(f1,f2,n,m)
        
        return
        end

        subroutine sub_clip(f1,f2,n,m)
        implicit none
        integer i,j,nchar,n,m,mn(2),mx(2),dumi,round
        real*8 std,av,mnv,mxv,kappa,stdclip,avclip
        character f1*(*),ch1*1,strdev*5,lab*500,extn*10,f2*(*),str1
        real*8 clipmn,clipmx,clipv,dumr1,dumr2,image(n,m)
        
        call get_imagestats(f1,3.d0,n,m,std,av,stdclip,avclip,
     /       mx,mn,mxv,mnv)
        write (*,'(a27,e9.2,a4,e9.2)') '   Image intensity range : ',
     /       mnv,' to ',mxv

        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)

        strdev='/xs'
        lab=' '
        call plotimage2(image,n,m,n,m,stdclip,avclip)

        write (*,'(a,$)') '   Enter clipping range, clipped value : '
        read (*,*) dumr1,dumr2,clipv
        clipmn=min(dumr1,dumr2)
        clipmx=max(dumr1,dumr2)
333     write (*,'(a,$)') '   Clip (o)utside or (i)nside this range : '
        read (*,*) str1
        if (str1.ne.'i'.and.str1.ne.'o') goto 333

        if (str1.eq.'i') then
         do i=1,n
          do j=1,m
           if (image(i,j).ge.clipmn.and.image(i,j).le.clipmx) 
     /         image(i,j)=clipv
          end do
         end do
        else
         do i=1,n
          do j=1,m
           if (image(i,j).lt.clipmn.or.image(i,j).gt.clipmx) 
     /         image(i,j)=clipv
          end do
         end do
        end if

        call writearray_bin(image,n,m,n,m,f2,'mv') 
        
        return
        end


