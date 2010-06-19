c! form islands in a better way. idea mostly claudio's. start from peak, look at
c! all surrounding pixels, if ok put 2. look surr pixels of these rank 2, if ok, put 3
c! and so on. may be slow. shall check.
c! hey, it works well. and fast. right now only detects island around the peak pixel.

        subroutine formisland_rank(image,x,y,n,m,mask,nsig,sav,fname)
        implicit none
        integer x,y,n,m,i,j
        integer mask(x,y),nchar,xmax,ymax
        real*8 image(x,y),nsig,image1(x,y),maxv
        real*8 std,av
        character sav*1,fname*(*),fn*500
        integer ndigit,rank(x,y),nrank(100),co_now(n*m),fac
        integer xsurr(8),ysurr(8),co_next(n*m),now,next,i8

        call maxarray(image,x,y,n,m,xmax,ymax,maxv)
        call sigclip(image,x,y,1,1,n,m,std,av,3.d0)
        call initialisemask(mask,n,m,n,m,0)
        call initialisemask(rank,n,m,n,m,0)
        ndigit=int(log10(max(n,m)*1.0))+1      ! number of digits in max(n,m)
        fac=int(10**ndigit)

        now=1
        nrank(now)=1          ! number of pix with rank=1 is 1
        rank(xmax,ymax)=now
        co_now(1)=ymax*fac+xmax

333     continue
        next=now+1
        nrank(next)=0
        do 100 i=1,nrank(now)
         call getsurr_8pix(co_now(i),xsurr,ysurr,fac)   ! x,ysurr has surrounding pix
         do 110 i8=1,8
          if (image(xsurr(i8),ysurr(i8))-av.ge.nsig*std) then ! has emission
           if (rank(xsurr(i8),ysurr(i8)).eq.0) then     ! new good pixel
            rank(xsurr(i8),ysurr(i8))=next
            nrank(next)=nrank(next)+1 
            co_next(nrank(next))=ysurr(i8)*fac+xsurr(i8)
           end if
          end if
110      continue
100     continue
        now=next   ! make old=new
        do 120 i=1,nrank(next)
         co_now(i)=co_next(i)
120     continue
        if (nrank(now).gt.0) goto 333

        do 200 j=1,m
         do 210 i=1,n     ! now set mask properly
          if (rank(i,j).gt.0) mask(i,j)=1
210      continue
200     continue

        if (sav.eq.'y') then
         fn=fname(1:nchar(fname))//'.rank'
         call imageint2r(rank,n,m,n,m,image1)
         call writearray_bin2D(image1,n,m,n,m,fn,'mv')
         fn=fname(1:nchar(fname))//'.mask'
         call imageint2r(mask,n,m,n,m,image1)
         call writearray_bin2D(image1,n,m,n,m,fn,'mv')
        end if
        
        return
        end

