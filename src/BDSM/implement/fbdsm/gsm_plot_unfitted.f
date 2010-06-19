c! plots stuff for the unfitted freq.s

        subroutine gsm_plot_unfitted(plotdir,plotspectra,sumngaul,
     /    nf,assoc_arr,nassoc,freq,tot,dtot,maxngaul,freq0,spin_arr,
     /    espin_arr,filename,solnname,ra,dec,dra,ddec,racen,
     /    deccen,gsmsolnname,tofit,scratch,calcflux0,calcflux1,
     /    bm_pixs,cdeltarr,srldir,avra,avdec)
        implicit none
        integer sumngaul,nf,assoc_arr(sumngaul,nf),nassoc,maxngaul,i,j
        integer tofit(nf)
        real*8 freq(nf),freq0,tot(nf,maxngaul),magic
        real*8 ra(nf,maxngaul),dec(nf,maxngaul),dumr
        real*8 dra(nf,maxngaul),ddec(nf,maxngaul)
        real*8 spin_arr(sumngaul,5),racen(nf),deccen(nf)
        real*8 dtot(nf,maxngaul),espin_arr(sumngaul,5),spindef
        character filename(nf)*500,plotdir*500,plotspectra*500
        character solnname(nf)*500
        character gsmsolnname*500,scratch*500,code1*1
        character xlab*30,ylab*30,nam*500,code*2,srldir*500
        integer id,pgopen,nchar,nfit,bnchar,mm,pp,nc,ii,inf
        real*8 calcflux0(nf,maxngaul),calcflux1(nf,maxngaul)
        real*4 rat(nassoc,nf),ra4(nassoc,nf),dec4(nassoc,nf)
        real*4 distcen4(nassoc,nf),ratio(nassoc)
        real*4 dist4(nassoc,nf),obsf(nassoc,nf),calcf(nassoc,nf)
        real*8 rap(nassoc),decp(nassoc),avra(sumngaul),avdec(sumngaul)
        integer flag(nassoc,nf),dumi
        real*8 bm_pixs(3,nf),bm_pix(3),cdeltarr(3,nf),cdelt(3)

        magic=-999.d0
c! calc ratio of obs and calc flux at all unfitted freq. flag=0 => take
        do i=1,nassoc
         do inf=1,nf
          if (tofit(inf).eq.0) then         
           if (assoc_arr(i,inf).eq.0) then
            obsf(i,inf)=magic
            calcf(i,inf)=magic
            rat(i,inf)=magic
            flag(i,inf)=1
           else
            obsf(i,inf)=tot(inf,assoc_arr(i,inf))
            calcf(i,inf)=calcflux0(inf,assoc_arr(i,inf))
            if (obsf(i,inf).eq.magic.or.calcf(i,inf).eq.magic) then
             rat(i,inf)=magic
             flag(i,inf)=1
            else
             rat(i,inf)=calcf(i,inf)/obsf(i,inf)
             flag(i,inf)=0
            end if
           end if
          end if
         end do
       end do

c! store all plotting variables
        do inf=1,nf
         if (tofit(inf).eq.0) then
          do i=1,nassoc
           if (flag(i,inf).eq.0) then
            ra4(i,inf)=ra(inf,assoc_arr(i,inf))
            dec4(i,inf)=dec(inf,assoc_arr(i,inf))
            call justdist(racen(inf),ra(inf,assoc_arr(i,inf)),
     /       deccen(inf),dec(inf,assoc_arr(i,inf)),dumr)  ! dist in arcsec from centre
            distcen4(i,inf)=dumr
            call justdist(avra(i),ra(inf,assoc_arr(i,inf)),avdec(i),
     /       dec(inf,assoc_arr(i,inf)),dumr)  ! dist in arcsec from centre
            dist4(i,inf)=dumr
           if (dumr.ne.dumr) write (*,*) i,inf,dist4(i,inf),avra(i),
     /      ra(inf,assoc_arr(i,inf)),avdec(i),dec(inf,assoc_arr(i,inf))
           end if
          end do
         end if
        end do

c! plot now against ra, dec, dist from centre, obs flux, 
c! calc flux, dist from median posn of assoc.
        xlab='Observed flux (Jy)'
        ylab='Flux ratio'
        nam='fluxrat_obsflux'
        code='L '
        call sub_plot_unfitted(gsmsolnname,plotdir,filename,
     /     solnname,obsf,rat,tofit,flag,xlab,ylab,nf,nassoc,nam,code,
     /     freq)
        xlab='Calculated flux (Jy)'
        nam='fluxrat_calcflux'
        code='L '
        call sub_plot_unfitted(gsmsolnname,plotdir,filename,
     /     solnname,calcf,rat,tofit,flag,xlab,ylab,nf,nassoc,nam,code,
     /     freq)
        xlab='R.A.'
        nam='fluxrat_RA'
        code='  '
        call sub_plot_unfitted(gsmsolnname,plotdir,filename,
     /     solnname,ra4,rat,tofit,flag,xlab,ylab,nf,nassoc,nam,code,
     /     freq)
        xlab='Dec'
        nam='fluxrat_Dec'
       code='  '
        call sub_plot_unfitted(gsmsolnname,plotdir,filename,
     /     solnname,dec4,rat,tofit,flag,xlab,ylab,nf,nassoc,nam,code,
     /     freq)
        xlab='Distance from centre of image (") '
        nam='fluxrat_distcen'
        code='  '
        call sub_plot_unfitted(gsmsolnname,plotdir,filename,solnname,
     /     distcen4,rat,tofit,flag,xlab,ylab,nf,nassoc,nam,code,
     /     freq)
        xlab='Distance from sources centre'
        nam='S-rat_dist'
        code='  '
        call sub_plot_unfitted(gsmsolnname,plotdir,filename,
     /     solnname,dist4,rat,tofit,flag,xlab,ylab,nf,nassoc,nam,code,
     /     freq)
        
c! create img file of ratio and make fits file.
c! for now just plot circles whose sizes are prop to ratio, on the image.
        code1='m'
        do i=1,nf
         if (tofit(i).eq.0) then         
          dumi=0
          do j=1,nassoc
           if (flag(j,i).eq.0) then
            dumi=dumi+1
            ratio(dumi)=rat(j,i)
            rap(dumi)=ra(i,assoc_arr(j,i))
            decp(dumi)=dec(i,assoc_arr(j,i))
           end if
          end do        
          bm_pix(1)=bm_pixs(1,i)
          bm_pix(2)=bm_pixs(2,i)
          bm_pix(3)=bm_pixs(3,i)
          cdelt(1)=cdeltarr(1,i)
          cdelt(2)=cdeltarr(2,i)
          cdelt(3)=cdeltarr(3,i)
c          call plotqty_as_im(scratch,srldir,plotdir,filename(i),
c     /         solnname(i),ratio,rap,decp,nassoc,dumi,code1,bm_pix,
c     /         nf,cdelt,gsmsolnname)
         end if
        end do

        return
        end
c!
c!
c!
        subroutine sub_plot_unfitted(gsmsolnname,plotdir,filename,
     /     solnname,x4,y4,tofit,flag,xlab,ylab,nf,nassoc,nam,code,freq)
        implicit none
        integer nf,nassoc,tofit(nf),dumi,i,j
        character filename(nf)*500,plotdir*500,nam*500,xlab*30,title*500
        character solnname(nf)*500,gsmsolnname*500,ylab*30,chr1*1
        character plotname*500,code*2,xb*6,yb*6,str3*500,rcode*2
        integer id,pgopen,nchar,bnchar,mm,pp,nc,round
        real*8 freq(nf)
        real*4 x4(nassoc,nf),y4(nassoc,nf),xplot(nassoc),yplot(nassoc)
        real*4 mnx,mxx,mny,mxy
        integer flag(nassoc,nf)

        do i=1,nf
         if (tofit(i).eq.0) then         

         write (str3,*) round(freq(i)/1.d6)
         if (str3(nchar(str3):nchar(str3)).eq.'.') 
     /       str3(nchar(str3):nchar(str3))=' '
          plotname=plotdir(1:nchar(plotdir))//
     /     gsmsolnname(1:nchar(gsmsolnname))//'/'//
     /     gsmsolnname(1:nchar(gsmsolnname))//'_'//
     /     str3(bnchar(str3):nchar(str3))//'MHz_'//
     /     filename(i)(1:nchar(filename(i)))//'_'//
     /     solnname(i)(1:nchar(solnname(i)))//'_'//
     /     nam(1:nchar(nam))//'.png/PNG'

          open(unit=26, file=
     /     plotdir(1:nchar(plotdir))//
     /     gsmsolnname(1:nchar(gsmsolnname))//'/'//
     /     gsmsolnname(1:nchar(gsmsolnname))//'_'//
     /     str3(bnchar(str3):nchar(str3))//'MHz_'//
     /     nam(1:nchar(nam))//'.data')

          dumi=0
          xb='BCNST'
          yb='BCNST'
          do j=1,nassoc
           if (flag(j,i).eq.0) then
            dumi=dumi+1
            xplot(dumi)=x4(j,i)
            yplot(dumi)=y4(j,i)
            write (26, *) x4(j,i),y4(j,i)

            if (code(1:1).eq.'L') then
             xplot(dumi)=log10(xplot(dumi))
             xb(6:6)='L'
            end if
            if (code(2:2).eq.'L') then
             yplot(dumi)=log10(yplot(dumi))
             yb(6:6)='L'
            end if
           end if
          end do        
          close(26)

          chr1='d'
          rcode='aq'
          title=' '
          call call_plot_menu(xplot,yplot,nassoc,dumi,xlab,
     /        ylab,chr1,xb,yb,plotname,rcode,title)
         end if
        end do
        
        return
        end
c!
c!
c!
        subroutine call_plot_menu(x4,y4,x,n,xlab,ylab,chr1,xl,yl,
     /              psname,rcode,title)
        implicit none
        character xlab*30,ylab*30,chr1*1,xl*6,yl*6,psname*500
        character rcode*2,title*500
        integer n,i,x,nchar
        real*4 x4(x),y4(x),xplot(n),yplot(n)

        do i=1,n
         xplot(i)=x4(i)
         yplot(i)=y4(i)
        end do
        call plot_menu(xplot,yplot,n,xlab,ylab,chr1,xl,yl,
     /              psname,rcode,title,'n')

        return
        end
c
