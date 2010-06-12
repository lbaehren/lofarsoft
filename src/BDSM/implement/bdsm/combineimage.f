c! to add two images

        subroutine combineimage(f1,f2,f3,str1)
        implicit none
        character f1*(*),f2*(*),f3*(*),str1*3,extn*10
        integer i,j,n,m,n1,m1,nchar

        extn='.img'
        call readarraysize(f1,extn,n,m)
        call readarraysize(f2,extn,n1,m1)

        if (n.ne.n1.or.m.ne.m1) then
         write (*,*) '  ### The two images are not of same size'
         write (*,*) '  ###           Exiting .... '
         goto 333
        end if
        call sub_combineimage(f1,f2,f3,str1,n,m)
333     continue
        
        return
        end
c!
c!
        subroutine sub_combineimage(f1,f2,f3,str1,n,m)
        implicit none
        character f1*(*),f2*(*),f3*(*),str1*3,extn*10
        integer i,j,n,m,n1,m1,nchar
        real*8 arr1(n,m),arr2(n,m),add(n,m),dum,keyvalue

        extn='.img'
        call readarray_bin(n,m,arr1,n,m,f1,extn)
        call readarray_bin(n1,m1,arr2,n,m,f2,extn)

444     continue
        if (str1.eq.'sub') dum=-1.d0
        if (str1.eq.'add') dum=1.d0

        if (str1.eq.'add'.or.str1.eq.'sub') then
         do 100 i=1,n
          do 110 j=1,m
           add(i,j)=arr1(i,j)+dum*arr2(i,j)
110       continue
100      continue
        else
         if (str1.eq.'mul') then
          do 200 i=1,n
           do 210 j=1,m
            add(i,j)=arr1(i,j)*arr2(i,j)
210        continue
200       continue
         end if
         if (str1.eq.'div') then
          do 300 i=1,n
           do 310 j=1,m
            add(i,j)=arr1(i,j)/arr2(i,j)
310        continue
300       continue
         end if
        end if

        call writearray_bin(add,n,m,n,m,f3,'mv')

        return
        end


