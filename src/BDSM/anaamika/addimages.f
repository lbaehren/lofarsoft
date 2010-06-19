
        subroutine addimages(of,scratch,runcode)
        implicit none
        character scratch*500,str*500,str1*500,ch1,f1*500,fname*500
        character extn*20,fg*500,dir*500,keyword*500,editor*500
        character comment*599,wtf*500,of*500,runcode*2
        integer nchar,n,m,l,num,n0,m0,l0,error,i,niter
        logical same,exists
        real*8 keyvalue

cf2py   intent(in) scratch,runcode,of

        write (*,*) '   Assuming images of same size have '//
     /              'same astrometry !'
444     continue
        write (*,'(a,$)') '    list (a)ll images or with (s)tring ? : '
        read (*,*) ch1
        if (ch1.ne.'a'.and.ch1.ne.'s') goto 444
        str=''
        if (ch1.eq.'s') then
         write (*,'(a,$)') '    string to search ? '
         read (*,*) str
         str='*'//str(1:nchar(str))
        end if
        str=str(1:nchar(str))//'*.img'
        str1="cd "//scratch(1:nchar(scratch))//"; ls -ltr "//
     /    str(1:nchar(str))//"|awk '{print $9}'|sed 's/\\.img//' > "//
     /    scratch(1:nchar(scratch))//'/a'
        call system(str1)

        fg="paradefine"
        extn=""
        dir="./"
        keyword="editor" 
        call get_keyword(fg,extn,keyword,editor,keyvalue,
     /    comment,"s",dir,error)

        niter=1
333     continue
        same=.true.
        f1='a'
        call getline(f1,scratch,num)
        call system('rm -fr '//scratch(1:nchar(scratch))//'b')
        open(unit=31,file=scratch(1:nchar(scratch))//'a')
        open(unit=32,file=scratch(1:nchar(scratch))//'b',
     /       access='append')
        extn='.img'
        do i=1,num
         read (31,*) fname
         call readarraysize(fname,extn,n,m,l)
         if (i.eq.1) then
          n0=n
          m0=m
          l0=l
         end if
         write (32,*) fname(1:nchar(fname)),' ',n,m,l
         if (n.ne.n0.or.m.ne.m0.or.l.ne.l0) same=.false.
        end do
        close(31)
        close(32)
        
        if ((.not.same).or.niter.eq.1) then
         write (*,*) '   Not all files are the same size !! '
         write (*,*) '   Edit the list to remove discrepant files'
         write (*,'(a,$)') '    press any key after editing '
         call system(editor(1:nchar(editor))//' '//
     /        scratch(1:nchar(scratch))//'b')
         read (*,'(a1)') ch1
         call system('cp '//scratch(1:nchar(scratch))//'b '//
     /        scratch(1:nchar(scratch))//'a')
         niter=niter+1
         goto 333
        end if
        if (num.eq.0) then
         write (*,*) '   No files give, quitting'
         goto 555
        end if

        wtf=''
334     write (*,'(a,$)') 
     /     '    Weights are (u)nity, clipped-(r)ms, from (f)ile : '
        read (*,*) ch1 
        if (ch1.ne.'u'.and.ch1.ne.'r'.and.ch1.ne.'f') goto 334
        if (ch1.eq.'f') then
445      write (*,'(a,a,$)') '  Name of weights file in '//
     /          scratch(1:nchar(scratch))//' : '
         read (*,*) wtf
         extn='.img'
         if (.not.exists(wtf,scratch,extn)) goto 445
         call getline(wtf,scratch,i)
         if (i.ne.num) then
          write (*,'(2(a17,i4))') 
     /         '   No. of files : ',num,' but no. of wts : ',i
          goto 445
         end if
        end if

        if (l.eq.1) then
         call sub_addimages(scratch,num,n,m,wtf,ch1,of,runcode)
        else
        end if
555     continue

        return
        end
c!
c!
c!
        subroutine sub_addimages(scratch,num,n,m,wtf,ch1,of,runcode)
        implicit none
        integer num,n,m,i,j,inum,nchar
        real*8 im(n,m),addim(n,m),wts(num),std,av,sumwt,nsig
        character scratch*500,extn*20,fname*500,ch1*1,wtf*500
        character runcode*2,of*500

        extn='.img'
        call initialiseimage(addim,n,m,n,m,0.d0)

        if (ch1.eq.'f') then
         open(unit=32,file=scratch(1:nchar(scratch))//'b')
         do i=1,num
          read (32,*) wts(i)
         end do
         close(32)
        end if
        if (ch1.eq.'u') then
         do i=1,num
          wts(i)=1.d0
         end do
        end if
        if (ch1.eq.'r') then
         nsig=5.d0
         open(unit=32,file=scratch(1:nchar(scratch))//'b')
         do i=1,num
          read (32,*) fname
          call readarray_bin(n,m,im,n,m,fname,extn)
          call sigclip(im,n,m,1,1,n,m,std,av,nsig)
          wts(i)=1.d0/std/std
         end do
         close(32)
        end if
        call vec_mean(wts,num,1,num,sumwt)
        sumwt=sumwt*n

        open(unit=32,file=scratch(1:nchar(scratch))//'b')
        do inum=1,num 
         read (32,*) fname
         call readarray_bin(n,m,im,n,m,fname,extn)
         do j=1,m
          do i=1,n
           addim(i,j)=addim(i,j)+im(i,j)*wts(inum)/sumwt
          end do
         end do
        end do
        close(32)

        call writearray_bin2D(addim,n,m,n,m,of,runcode)

        return
        end


