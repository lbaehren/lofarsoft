c! assume single peak in image and compute mask out to 3 sigma.
c! shud do climbing hill kind of thing but for now do simple.
c! go around the peak at every pixel width and if any inner ones is > 3sigma, mark 1.
c! do this one more and if u get more number this time, means there is a tentacle so do again
c! till no new numbers of 1 and then go to one extra outer and so on. 
c! no, not that easy cos u can have peak very near edge of field. lets see.
c
c! ok, cheat now. calculate 2nd moment, take 2-3 times that and include all > 3sig pts in that.
c! 
c! #####  assume sources are positive

        subroutine formisland_basic(image,x,y,n,m,mask,nsig,sav,fname)
        implicit none
        integer x,y,n,m,i,j,mask(x,y),box,mask1(x,y)
        integer xmax,ymax,round,i1,j1,nchar
        real*8 image(x,y),m1,m2(2),m3(2)
        real*8 maxv,std,av,nsig,image1(x,y)
        character fn*500,sav*1,fname*500

        call maxarray(image,x,y,n,m,xmax,ymax,maxv)
        call sigclip(image,x,y,1,1,n,m,std,av,3)
        call initialisemask(mask1,x,y,n,m,0)
        mask1(xmax,ymax)=1

        call moment(image,x,y,n,m,m1,m2,m3)
        box=round(sqrt(m3(1)*m3(2))*2.35d0*2.5d0/2.d0)   ! 2.5 times fwhm boxsize

        do 100 i=m2(1)-box,m2(1)+box
         do 110 j=m2(2)-box,m2(2)+box
          if ((image(i,j)-av).ge.nsig*std) mask1(i,j)=1
110      continue
100     continue

c! now add two more pixels width around the area. 
        call copyarrayint(mask1,x,y,1,1,n,m,mask)
        do 200 i=1,n
         do 210 j=1,m
          if (mask1(i,j).ne.0) then
           do 230 i1=i-2,i+2 
            do 240 j1=j-2,j+2 
             mask(i1,j1)=1
240         continue
230        continue
          end if
210      continue
200     continue

        if (sav.eq.'y') then
         fn=fname(1:nchar(fname))//'.mask'
         call imageint2r(mask,x,y,n,m,image1)
         call writearray_bin(image1,x,y,n,m,fn)
        end if

        return
        end

