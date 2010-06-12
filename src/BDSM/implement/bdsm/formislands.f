c! this is to form multiple islands in an image. extends the 1-island program
c! of formisland_rank which was claudio's idea.

        subroutine formislands(f1,f2,runcode,scratch)
        implicit none
        character f1*(*),extn*10,f2*500,ch1*1,runcode*2,scratch*500
        integer n,m

        if (runcode(2:2).eq.'q') write (*,*) '  Forming islands'
        extn='.img'
        call readarraysize(f1,extn,n,m)
        call sub_formisland(f1,n,m,n*m,f2,runcode)

c! next step
        if (f2.ne.' ') then
         if (runcode(1:1).eq.'m') then
          write (*,*) 
          write (*,'(a,$)') '   Continue (island file : make) (y)/n ? '
          read (*,'(a1)') ch1
          if (ichar(ch1).eq.32) ch1='y'
333       continue
          if (ch1.eq.'y') call iland_mat2list(f1,f2,runcode,scratch)
         end if
        end if        

        return
        end

c! ptlist is number of pts over nsig sigma. xptlist and yptlist have the coords.
c! each pixel has rank which is nnnnmmm where mmm is rank and nnnn is island number
c! nnnn is zero first for a pixel till the island is fully identified.
c! first identify each island. then run thro them to check overlaps and hence merging.
c! useful to check merging if we decide later to extend islands by one pixel outward e.g.
        subroutine sub_formisland(f1,n,m,nn,f2,runcode)
        implicit none
        integer n,m,nn
        character f1*500,fn*500,extn*10,cmd*10,history*10,str*5,f2*500
        character comment*500,keystrng*500,keyword*500,dir*500,fg*500
        character scratch*500,runcode
        integer nchar,nptlist,xptlist(nn),yptlist(nn),ipt
        real*8 image(n,m),rrank(n,m),thresh_isl
        real*8 rmsim(n,m),keyvalue,rmsclip,avclip
        integer ndigit,rank(n,m),nrank(100),co_now(n*m),fac,i
        integer xsurr(8),ysurr(8),co_next(n*m),now,next,i8,nisl
        integer island(n,m),maxmem,maxmemsub
        logical pixinimage

c! read in the thresholds and other keywords
        fg="paradefine"
        extn=""
        keyword="scratch"
        dir="./"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)

        extn='.bstat'
        keyword='thresh_isl'
        call get_keyword(f1,extn,keyword,keystrng,thresh_isl,
     /       comment,'r',scratch)
        keyword='rms_clip'
        call get_keyword(f1,extn,keyword,keystrng,rmsclip,
     /       comment,'r',scratch)
        keyword='mean_clip'
        call get_keyword(f1,extn,keyword,keystrng,avclip,
     /       comment,'r',scratch)

c! read in image and the rms image
        extn='.img'
        call readarray_bin(n,m,image,n,m,f1,extn)
        call get_rmsmap(f1,n,m,rmsim)

        call initialisemask(rank,n,m,n,m,0)
        call getlistnsigpt(image,rmsim,n,m,n,m,nptlist,
     /       xptlist,yptlist,nn,f1)

        ndigit=int(log10(max(n,m)*1.0))+1      ! number of digits in max(n,m)
        fac=int(10**ndigit)                    ! for compressing coords
        
        nisl=0
        maxmem=0
        do 400 ipt=1,nptlist
         if (rank(xptlist(ipt),yptlist(ipt)).eq.0) then
          nisl=nisl+1
          now=1
          nrank(now)=1          ! number of pix with rank=1 is 1
          rank(xptlist(ipt),yptlist(ipt))=nisl*1000+now
          co_now(1)=yptlist(ipt)*fac+xptlist(ipt)
 
          maxmemsub=1
333       continue
          next=now+1
          nrank(next)=0
          do 100 i=1,nrank(now)
           call getsurr_8pix(co_now(i),xsurr,ysurr,fac)   ! x,ysurr has surrounding pix
           do 110 i8=1,8
            if (pixinimage(xsurr(i8),ysurr(i8),1,n,1,m)) then
             if (image(xsurr(i8),ysurr(i8))-avclip.ge.thresh_isl*
     /           rmsim(xsurr(i8),ysurr(i8))) then ! has emission
              if (rank(xsurr(i8),ysurr(i8)).eq.0) then     ! new good pixel
               rank(xsurr(i8),ysurr(i8))=nisl*1000+next
               nrank(next)=nrank(next)+1 
               co_next(nrank(next))=ysurr(i8)*fac+xsurr(i8)
               maxmemsub=maxmemsub+1
              end if ! new good pixel
             end if ! is pixel is above threshold
            end if ! if pixel is inside image
110        continue
100       continue
          now=next   ! make old=new
          if (maxmemsub.gt.maxmem) maxmem=maxmemsub
          do 120 i=1,nrank(next)
           co_now(i)=co_next(i)
120       continue
          if (nrank(now).gt.0) goto 333
         end if  ! rank of xpt,ypt ne 0
400     continue

        call rank2islandmat(rank,n,m,island)
        write (*,'(a12,i5,a8)') '   Detected ',nisl,' islands'

        if (runcode(1:1).eq.'m') then
         f2=' '
         write (*,'(a33,$)') '   Save file/trial id (n/y id) : '
         read (*,'(a10)') cmd
        else
         cmd='y '//f2(1:nchar(f2))
        end if
        if (cmd.ne.'n') then 
         if (runcode(1:1).eq.'m') then
          fn=f1(1:nchar(f1))//'.'//cmd(3:nchar(cmd))//'.rank'
          f2=f1(1:nchar(f1))//'.'//cmd(3:nchar(cmd))
         else
          fn=f2(1:nchar(f2))//'.rank'
         end if
         call imageint2r(island,n,m,n,m,rrank)
         call writearray_bin(rrank,n,m,n,m,fn,runcode)
         call int2str(nisl,str)
         call int2str(maxmem,history)
         history=' '//str(1:nchar(str)+1)//history(1:nchar(history))
         call writearray_bin_int(island,n,m,n,m,fn,history,runcode)
        end if
        
        return
        end

        subroutine rank2islandmat(rank,n,m,island)
        implicit none
        integer n,m,rank(n,m),island(n,m),i,j

        do i=1,n
         do j=1,m
          island(i,j)=int(rank(i,j)/1000)
         end do
        end do

        return
        end




