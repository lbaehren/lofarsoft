c! this is make the 4 lists for comparing sources in 3 lists. called by srccomp.f
c! n's and f's have nsrc and fname in decr order of #srcs

        subroutine makelists(n1,n2,n3,f1,f2,f3,tol)
        implicit none
        integer n1,n2,n3,i,j,dumi
        integer listall(n1+n2+n3,3),list1(n1,3)
        integer list2(n2,3),list3(n3,3)
        character f1*10,f2*10,f3*10,fname*8
        real*8 ra1(n1),ra2(n2),ra3(n3),dec1(n1),dec2(n2),dec3(n3)
        integer ncommon2,match,common2(100),ncommon3,common3(100)
        real*8 tol,dist2(100),listdist1(n1,2),dist3(100),integ1(n1)
        integer indexlistall,place
        real*8 listdist2(n2,2),listdist3(n3,2),dist
        integer place1,place2,place3
        character command*300

c! read in the coords as accessing arrays is faster than repeated reading from file
        call readcoords(f1,n1,ra1,dec1)
        call readcoords(f2,n2,ra2,dec2)
        call readcoords(f3,n3,ra3,dec3)
        
        call system('rm -f a')
        call system("awk '{print $11}' SADSTAR > a")
        open(unit=21,file='a',status='old')       ! file 3
        do 106 i=1,n1
         read (21,*) integ1(i)
106     continue
        close(21)

c! first did list1 cntrprt in 2 and 3, then 2 in 3 but that doesnt work cos multiple
c! srcs in one cat can find same cntrprt in others which u arent allowing for lists 2 and 3
c! so now repeat proc for list1 for lists 2 and 3 as well. there will be 3 repeats, thats ok
c! cant have listsad listsfind etc but list1 list2 list3

        call makecomparison(n1,n2,n3,list1,ra1,ra2,ra3,dec1,dec2,dec3,
     /       tol,listdist1,1,2,3)
        call makecomparison(n2,n3,n1,list2,ra2,ra3,ra1,dec2,dec3,dec1,
     /       tol,listdist2,2,3,1)
        call makecomparison(n3,n1,n2,list3,ra3,ra1,ra2,dec3,dec1,dec2,
     /       tol,listdist3,3,1,2)

c! print out the lists
        fname='list1'
        call printlist(list1,fname,n1,n1)
        fname='list2'
        call printlist(list2,fname,n2,n2)
        fname='list3'
        call printlist(list3,fname,n3,n3)

c! make listall now including problem cases
        do 150 i=1,n1                             !  put list1 in first
         do 155 j=1,3
          listall(i,j)=list1(i,j)
155      continue
150     continue                          
        indexlistall=n1
        do 160 i=1,n2                             !  now put remaining list2 in
         if (list2(i,1).eq.0) then
          indexlistall=indexlistall+1
          do 165 j=1,3
           listall(indexlistall,j)=list2(i,j)
165       continue
         end if
160     continue
        do 170 i=1,n3                             !  now put remaining list3 in
         if (list3(i,1).eq.0.and.list3(i,2).eq.0) then
          indexlistall=indexlistall+1
          do 175 j=1,3
           listall(indexlistall,j)=list3(i,j)
175       continue
         end if
170     continue                                  ! listall has all including problems
        fname='listall'
        call printlist(listall,fname,indexlistall,n1+n2+n3) ! print listall

c! now to check for the problem cases -- two kinds. 
c! first is when you have geometry like a---b---c so a,b b,c are cntrprts but not a,c
c! second is when two diff srcs in list u are operating on latch on to same cntrprt in other
c! list. 
c! cant zabardasti impose solution for these as geom cud be genuine due to multiple sources
c! not clearly identifiable in all three lists. so need to flag as problems and write out.
c! call getplace(i,3,list1,n1,1,place)   ! in list1 where list2(n,3)=i, get list2(n,1) 
c! getplace not used now but keep in case u need it later.
c!
c! check duplicates in fields 2,3 in list1 etc. sort by fld2 for eg, then do uniq -D for
c! nonzero entry values.  sort +2 -g list2 | uniq -f 2 -D | sed '/[0-9]*[ ][0-9]*[ ]0/d'
c! for checking for duplicates in field 3 of list2. deletes 0 in last field.
c! sort syntax CHANGED !! old sort +n file is now sort -k n+1 file.
        
c! check for duplication and then for a--b--c geom for each list 

        call system('rm -f aa')
        open(unit=21,file='aa',status='unknown')
c! check duplication in list1 
        call system('rm -f problem')
        command="sort -k 3 -g list1 | uniq -f 2 -D | "                     ! in 3rd field
     /          //"sed '/[0-9]*[ ][0-9]*[ ]0/d'"
     /          //" | sed 's/$/ list1/g' > problem "
        call system(command)
        command="awk '{print $3,$1,$2}' list1|sort -k 3 -g "
     /        //"|uniq -f 2 -D|"  ! in 2nd field
     /        //"sed '/[0-9]*[ ][0-9]*[ ]0/d' | awk '{print $2,$3,$1}'"
     /          //" | sed 's/$/ list1/g' >> problem "
        call system(command)
c! check a--b--c in list1
        do 300 i=1,n1
         if (list2(list1(i,2),1).ne.i.and.list1(i,2).ne.0) 
     /    write (21,*) list1(i,1),list1(i,2),list1(i,3),' list1'
         if (list3(list1(i,3),1).ne.i.and.list1(i,3).ne.0) 
     /    write (21,*) list1(i,1),list1(i,2),list1(i,3),' list1'
300     continue

c! check in list2
        command="sort -g list2|awk '{print $2,$3,$1}'|uniq -f 2 -D|"        ! in 1st field
     /        //"sed '/[0-9]*[ ][0-9]*[ ]0/d' | awk '{print $3,$1,$2}'"
     /          //" | sed 's/$/ list2/g' >> problem "
        call system(command)
        command="sort -k 3 -g list2 | uniq -f 2 -D | "                        ! in 3rd field
     /          //"sed '/[0-9]*[ ][0-9]*[ ]0/d'"
     /          //" | sed 's/$/ list2/g' >> problem "
        call system(command)
c! check a--b--c in list2
        do 310 i=1,n2
         if (list1(list2(i,1),2).ne.i.and.list2(i,1).ne.0) 
     /    write (21,*) list2(i,1),list2(i,2),list2(i,3),' list2'
         if (list3(list2(i,3),2).ne.i.and.list2(i,3).ne.0) 
     /    write (21,*) list2(i,1),list2(i,2),list2(i,3),' list2'
310     continue

c! check in list3
        command="sort -g list3|awk '{print $2,$3,$1}'|uniq -f 2 -D|"        ! in 1st field
     /        //"sed '/[0-9]*[ ][0-9]*[ ]0/d' |awk '{print $3,$1,$2}'"
     /          //" | sed 's/$/ list3/g' >> problem "
        call system(command)
        command="sort -k 2 -g list3|awk '{print $3,$1,$2}' "
     /          //"|uniq -f 2 -D|" ! in 2nd field
     /          //"sed '/[0-9]*[ ][0-9]*[ ]0/d'|awk '{print $2,$3,$1}'"
     /          //" | sed 's/$/ list3/g' >> problem "
        call system(command)
c! check a--b--c in list3
        do 320 i=1,n3
         if (list1(list3(i,1),3).ne.i.and.list3(i,1).ne.0) 
     /    write (21,*) list3(i,1),list3(i,2),list3(i,3),' list3'
         if (list2(list3(i,2),3).ne.i.and.list3(i,2).ne.0) 
     /    write (21,*) list3(i,1),list3(i,2),list3(i,3),' list3'
320     continue

        close(21)
        call system('cat aa >> problem')
        call system('rm -f aa a')
        command="awk '{print $4,$1,$2,$3}' problem | sort | uniq "   ! remove duplicates 
     /              //"| awk '{print $2,$3,$4,$1}' > a"
        call system(command)
        call system('mv a problem')

        return
        end

c ------------------------------------------------------------------------------------------ c
c
c                                ## SUBROUTINES ##
c
c ------------------------------------------------------------------------------------------ c

        subroutine calcdist(ra1,dec1,ra2,dec2,tol,dist,match)
        implicit none
        real*8 ra1,dec1,ra2,dec2,tol,dist,rad
        real*8 r1,d1,r2,d2
        integer match

        rad=3.14159d0/180.d0

        r1=(ra1-180.d0)*rad
        r2=(ra2-180.d0)*rad
        d1=dec1*rad
        d2=dec2*rad

        dist=acos(dsin(d1)*dsin(d2)+dcos(d1)*dcos(d2)*dcos(r1-r2))
        dist=dist*3600.d0/rad  ! in arcsec

        if (dist.le.tol) then
         match=1
        else
         match=0
        end if

        return
        end

c ------------------------------------------------------------------------------------------ c
c! in l1 where list(n,n2)=n1, get list(n,n4) as pl else 0
        subroutine getplace(n1,n2,l,n3,n4,pl)
        implicit none
        integer n1,n2,n3,n4,pl,i
        integer l(n3,3)

        pl=0
        do 100 i=1,n3
         if (l(i,n2).eq.n1) then
          pl=l(i,n4)
         end if
100     continue

        return
        end

c ------------------------------------------------------------------------------------------ c

c! put in procedure now to make lists 1,2,3 in rotation. three repeats but thats ok.
c! but format is (1,2,3) of lists are always list1,list2,list3 and now rotated !!!
c! but ldist1 is indeed rotated.
        subroutine makecomparison(en1,en2,en3,l1,r1,r2,r3,
     /             d1,d2,d3,tol,ldist1,l,m,n)
        implicit none
        integer en1,en2,en3,l1(en1,3),l2(en2,3),l3(en3,3)
        real*8 r1(en1),r2(en2),r3(en3),d1(en1),d2(en2),d3(en3)
        integer i,j,ncommon2,match,common2(100),ncommon3,common3(100)
        integer l,m,n
        real*8 dist2(100),dist3(100),tol,ldist1(en1,2),dist

        do 100 i=1,en1
         l1(i,l)=i
         ncommon2=0
         ncommon3=0

c! now read 2nd catalog
         do 110 j=1,en2
          if (abs(d1(i)-d2(j)).le.1.d0/60.d0) then  ! Xcorr if decs < 5 min apart
           call calcdist(r1(i),d1(i),r2(j),d2(j),tol,dist,match)
           if (match.eq.1) then             ! store values of match # ncommon2 (<100)
            ncommon2=ncommon2+1
            dist2(ncommon2)=dist
            common2(ncommon2)=j
c! now make sure common2 and dist2 of common2 is always for least distance
            if (ncommon2.gt.1) then
             if (dist2(ncommon2).gt.dist2(ncommon2-1)) then
              call xchangi(common2(ncommon2),common2(ncommon2-1))
              call xchangr8(dist2(ncommon2),dist2(ncommon2-1))
             end if
            end if
           end if
          end if
110      continue                           
         if (ncommon2.eq.0) then
          l1(i,m)=0
          ldist1(i,1)=9999.9
         else
          l1(i,m)=common2(ncommon2)
          ldist1(i,1)=dist2(ncommon2)
         end if
         
c! now read 3rd catalog
         do 120 j=1,en3
          if (abs(d1(i)-d3(j)).le.1.d0/60.d0) then  ! Xcorr if decs < 5 min apart
           call calcdist(r1(i),d1(i),r3(j),d3(j),tol,dist,match)
           if (match.eq.1) then             ! store values of match # ncommon3 (<100)
            ncommon3=ncommon3+1
            dist3(ncommon3)=dist
            common3(ncommon3)=j
c! now make sure common3 and dist3 of common3 is always for least distance
            if (ncommon3.gt.1) then
             if (dist3(ncommon3).gt.dist3(ncommon3-1)) then
              call xchangi(common3(ncommon3),common3(ncommon3-1))
              call xchangr8(dist3(ncommon3),dist3(ncommon3-1))
             end if
            end if
           end if
          end if
120      continue                           
         if (ncommon3.eq.0) then
          l1(i,n)=0
          ldist1(i,2)=9999.9
         else
          l1(i,n)=common3(ncommon3)
          ldist1(i,2)=dist3(ncommon3)
         end if
100     continue

        return
        end

c ------------------------------------------------------------------------------------------ c

        subroutine printlist(list,fname,m,n)
        implicit none
        integer n,list(n,3),i,m
        character fname*8

        open(unit=21,file=fname,status='unknown')
        do 300 i=1,m
         write (21,*) list(i,1),list(i,2),list(i,3),' '
300     continue
        close(21)

        return
        end

c ------------------------------------------------------------------------------------------ c

        subroutine readcoords(f,n,ra,dec)
        implicit none
        character f*10
        integer i,n
        real*8 ra(n),dec(n)

        open(unit=21,file=f,status='old')       ! file 1
        do 103 i=1,n
         read (21,*) ra(i),dec(i)
103     continue
        close(21)

        return
        end

c ------------------------------------------------------------------------------------------ c

