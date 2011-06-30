
        subroutine readarraysize(f1,extn,n,m,l)
        implicit none
        character f1*500,fn*500
        integer n,m,nchar,error,l
        character fg*500,extn*20,keyword*500,comment*500,dir*500
        character scratch*500,extn1*20
        real*8 keyvalue

cf2py intent(in) f1, extn
cf2py intent(out) n,m,l
        
        fg="paradefine"
        extn1=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn1,keyword,scratch,keyvalue,
     /    comment,"s",dir,error)

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        open(unit=21,file=fn(1:nchar(fn)),status='old',
     /       form='unformatted')
        read (21) n,m,l
        close(21)
        
        return
        end

c! read in array from binary file
        subroutine readarray_bin(n,m,arr,x,y,f1,extn)
        implicit none
        integer n,m,i,j,x,y,nchar,error
        real*8 arr(x,y),keyvalue
        character f1*500,fn*500
        character fg*500,extn*20,keyword*500,comment*500,dir*500
        character scratch*500,extn1*20

cf2py intent(in) n,m,x,y,f1,extn
cf2py intent(out) arr

        fg="paradefine"
        extn1=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn1,keyword,scratch,keyvalue,
     /    comment,"s",dir,error)

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        open(unit=22,file=fn(1:nchar(fn)),status='old',
     /       form='unformatted')
        read (22) n,m
        call readarray_bin_data(22,n,m,arr,x,y)
        close(22)

        return
        end

        subroutine readarray_bin_int(n,m,arr,x,y,f1,extn,str)
        implicit none
        integer n,m,i,j,x,y,nchar
        integer arr(x,y),error
        character f1*500,fn*500,str*20
        character fg*500,extn*20,keyword*500,comment*500,dir*500
        character scratch*500,extn1*20
        real*8 keyvalue

        fg="paradefine"
        extn1=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn1,keyword,scratch,keyvalue,
     /    comment,"s",dir,error)

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        open(unit=22,file=fn(1:nchar(fn)),status='old',
     /       form='unformatted')
        read (22) n,m,str
        call readarray_bin_int_data(22,n,m,arr,x,y)
        close(22)

        return
        end

        subroutine readarraysize_bin_int(n,m,f1,extn,str)
        implicit none
        integer n,m,nchar,error
        character f1*500,fn*500,str*20
        character fg*500,extn*20,keyword*500,comment*500,dir*500
        character scratch*500,extn1*20
        real*8 keyvalue

        fg="paradefine"
        extn1=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn1,keyword,scratch,keyvalue,
     /    comment,"s",dir,error)

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        open(unit=22,file=fn(1:nchar(fn)),status='old',
     /       form='unformatted')
        read (22) n,m,str
        close(22)

        return
        end


        subroutine readarray_bin_data(nn,n,m,arr,x,y)
        implicit none
        integer x,y,n,m,i,j,nn
        real*8 dumr(m),arr(x,y)

        do 100 i=1,n
         read (nn) dumr
         do 110 j=1,m
          arr(i,j)=dumr(j)
110      continue
100     continue
        
        return
        end

        subroutine readarray_bin_int_data(nn,n,m,arr,x,y)
        implicit none
        integer x,y,n,m,i,j,nn
        integer dumr(m),arr(x,y)

        do 100 i=1,n
         read (nn) dumr
         do 110 j=1,m
          arr(i,j)=dumr(j)
110      continue
100     continue
        
        return
        end


c! read in array from text file

        subroutine readarray_asc(n,m,arr,x,y,fn)
        implicit none
        integer n,m,i,j,fn,x,y
        real*8 dumr(m),arr(x,y)

        do 100 i=1,n
         read (fn,*) dumr
         do 110 j=1,m
          arr(i,j)=dumr(j)
110      continue
100     continue

        return
        end

        subroutine readarray_bin3D(n,m,l,arr,x,y,z,f1,extn)
        implicit none
        integer n,m,z,i,j,k,l,x,y,nchar,error
        real*8 dumr(m),arr(x,y,z)
        character f1*500,fn*500
        character fg*500,extn*20,keyword*500,comment*500,dir*500
        character scratch*500,extn1*20
        real*8 keyvalue

        fg="paradefine"
        extn1=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn1,keyword,scratch,keyvalue,
     /    comment,"s",dir,error)

        fn=scratch(1:nchar(scratch))//f1(1:nchar(f1))//
     /     extn(1:nchar(extn))
        open(unit=21,file=fn(1:nchar(fn)),status='old',
     /       form='unformatted')
        read (21) n,m,l
        do 100 k=1,l
         do 110 i=1,n
          read (21) dumr
           do 120 j=1,m
            arr(i,j,k)=dumr(j)
120        continue
110       continue
100      continue
        close(21)

        return
        end

