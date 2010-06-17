c! can tesselate entire image but thats pointless and memory wasteful.
c! just tesselate the sources instead. use volrank to inspect if u want.

        subroutine do_voronoi(n,m,imagename,ncut,ngens,ngensS,dumi,peak,
     /    im_rms,ntiles,volnum,xpix,ypix,scratch,fullname,generators,
     /    gencode,primarygen,tess_method,tess_sc,tess_fuzzy)
        implicit none
        integer n,m,ncut,ngens,volnum(ncut),i,j,k,ngensmod
        integer minind(n,m),nchar,tess_method,ngensS,dumi
        integer ntiles,tile_ngen(ngens),genlist(ngens,ngens)
        real*8 peak(ncut),im_rms(ncut)
        real*8 snr(ncut),snrgens(ngens),xgens(ngens),ygens(ngens)
        real*8 snrgensS(dumi),xgensS(dumi),ygensS(dumi)
        real*8 xtile(ngens),ytile(ngens),snrtile(ngens)
        real*8 xpix(ncut),ypix(ncut),dumr,tess_fuzzy
        character imagename*500,extn*20,fn*500,runcode*2,scratch*500
        character fullname*500,generators*500,primarygen*500
        character gencode*4,tess_sc*1

c! read in voronoi generators
        fn=scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
     /     '.vorogensP'
        open(unit=21,file=fn)
         read (21,*) 
         do i=1,ngens   ! in descending order of snr
          read (21,*) xgens(i),ygens(i),snrgens(i)
         end do
        close(21)
        if (generators(1:5).eq.'field'.and.ngensS.gt.0) then
         fn=scratch(1:nchar(scratch))//fullname(1:nchar(fullname))//
     /      '.vorogensS'
         open(unit=21,file=fn)
          read (21,*) 
          do i=1,ngensS   ! in descending order of snr
           read (21,*) xgensS(i),ygensS(i),snrgensS(i)
          end do
         close(21)
        end if

c! edit primary voronoi generator list. each tile has a tile centre
c! and can have more than one generator to be averaged. tile_ngen(ntiles)=num of gen/tile
c! and genlist(ntiles,i=1,tile_gen(ntiles))=num in ngen list
c! and then u modify that using secondary list in tesselate
        call edit_vorogenlist(ngens,xgens,ygens,snrgens,ntiles,
     /       tile_ngen,genlist,xtile,ytile,snrtile)

c! do the actual thing. in many ways.
        runcode='av'
        ngensmod=ngens
        call tesselate(n,m,ngens,ngensmod,xgens,ygens,snrgens,
     /     ngensS,dumi,xgensS,ygensS,snrgensS,tess_method,tess_sc,
     /     tess_fuzzy,generators,gencode,fullname,xtile,ytile,snrtile)

        return
        end
c!
c!
c! group together if gens are closer than frac of dist to third closest
c! frac=1/4 for now.
        subroutine edit_vorogenlist(ngens,xgens,ygens,snrgens,
     /    ntiles,tile_ngen,genlist,xtile,ytile,snrtile)
        implicit none
        integer ngens,i,j,iwksp(ngens),indi(ngens)
        integer flag(ngens),ntiles,tile_ngen(ngens),genlist(ngens,ngens)
        real*8 xgens(ngens),ygens(ngens),snrgens(ngens)
        real*8 distgens(ngens),wksp(ngens),frac
        real*8 xtile(ngens),ytile(ngens),snrtile(ngens)

        call initialise_int_vec(tile_ngen,ngens,0)
        call initialisemask(genlist,ngens,ngens,ngens,ngens,0)
        do i=1,ngens
         flag(i)=0
        end do

        frac=1.d0/4.d0
        ntiles=0
        do i=1,ngens
         do j=1,ngens
          distgens(j)=sqrt((xgens(i)-xgens(j))*(xgens(i)-xgens(j))+
     /                 (ygens(i)-ygens(j))*(ygens(i)-ygens(j)))
          indi(j)=j
         end do
         call sort3_2i(ngens,distgens,indi,wksp,iwksp)  ! first is itself, look at 2nd and 3rd
         if (distgens(2).lt.frac*distgens(3)) then
          if (flag(indi(2))+flag(i).eq.0) then ! not already deleted from other pair
           ntiles=ntiles+1
           tile_ngen(ntiles)=tile_ngen(ntiles)+2
           genlist(ntiles,1)=i
           genlist(ntiles,2)=indi(2)
           xtile(ntiles)=(xgens(i)*snrgens(i)+xgens(indi(2))*
     /            snrgens(indi(2)))/(snrgens(i)+snrgens(indi(2)))
           ytile(ntiles)=(ygens(i)*snrgens(i)+ygens(indi(2))*
     /            snrgens(indi(2)))/(snrgens(i)+snrgens(indi(2)))
           snrtile(ntiles)=snrgens(i)+snrgens(indi(2))
           flag(i)=1
           flag(indi(2))=1
          end if
         else
          if ((distgens(2)+distgens(3)).lt.frac*distgens(4)) then ! for 3 close by srcs
           if (flag(indi(2))+flag(indi(3))+flag(i).eq.0) then ! not already deleted from others
            ntiles=ntiles+1
            tile_ngen(ntiles)=tile_ngen(ntiles)+3
            genlist(ntiles,1)=i
            genlist(ntiles,2)=indi(2)
            genlist(ntiles,3)=indi(3)
            xtile(ntiles)=(xgens(i)*snrgens(i)+xgens(indi(2))*
     /             snrgens(indi(2))+xgens(indi(3))*snrgens(indi(3)))/
     /             (snrgens(i)+snrgens(indi(2))+snrgens(indi(3)))
            ytile(ntiles)=(ygens(i)*snrgens(i)+ygens(indi(2))*
     /             snrgens(indi(2))+ygens(indi(3))*snrgens(indi(3)))/
     /             (snrgens(i)+snrgens(indi(2))+snrgens(indi(3)))
            snrtile(ntiles)=snrgens(i)+snrgens(indi(2))+snrgens(indi(3))
            flag(i)=1
            flag(indi(2))=1
            flag(indi(3))=1
           end if
          else
           ntiles=ntiles+1
           xtile(ntiles)=xgens(i)
           ytile(ntiles)=ygens(i)
           snrtile(ntiles)=snrgens(i)
           tile_ngen(ntiles)=tile_ngen(ntiles)+1
           genlist(ntiles,1)=i
          end if
         end if
        end do

        return
        end



