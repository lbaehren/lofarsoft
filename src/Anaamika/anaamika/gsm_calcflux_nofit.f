c! calc fluxes at freqs not fitted and output. 
c! if only one pt then use spindef

        subroutine gsm_calcflux_nofit(plotdir,sumngaul,nf,assoc_arr,
     /    nassoc,freq,tot,dtot,maxngaul,freq0,spin_arr,espin_arr,
     /    filename,solnname,gsmsolnname,tofit,calcflux0,
     /    calcflux1,spindef,dec)
        implicit none
        integer sumngaul,nf,assoc_arr(sumngaul,nf),nassoc,maxngaul,i,j
        integer tofit(nf)
        real*8 freq(nf),freq0,tot(nf,maxngaul),calcflux1(nf,maxngaul)
        real*8 spin_arr(sumngaul,5),calcflux0(nf,maxngaul),magic,fr,fr1
        real*8 dtot(nf,maxngaul),espin_arr(sumngaul,5),spindef
        character filename(nf)*500,plotdir*500
        character solnname(nf)*500
        character gsmsolnname*500
        integer id,pgopen,nchar,nfit,bnchar,mm,pp,nc,ii
        real*8 dec(nf,maxngaul)

        magic=-999.d0
        do j=1,nf
         if (tofit(j).ne.1) then
          do i=1,nassoc
           if (assoc_arr(i,j).ne.0) then
            fr=freq(j)
            if (dec(j,assoc_arr(i,j)).gt.74.9d0.and.
     /         (filename(j)(1:5).eq.'wenss'.or.filename(j)(1:5).eq.
     /         'WENSS')) fr = 352.d6
            if (spin_arr(i,1).ne.magic) then   ! spin0 defined
             calcflux0(j,assoc_arr(i,j))=spin_arr(i,1)*
     /           ((fr/freq0)**spin_arr(i,2))
            else                                 ! else use -0.7
             calcflux0(j,assoc_arr(i,j))=magic   ! if only src in assoc
             do ii=1,nf
              fr1=freq(ii)
              if (assoc_arr(i,ii).ne.0) then
               if (dec(ii,assoc_arr(i,ii)).gt.74.9d0.and.
     /          (filename(ii)(1:5).eq.'wenss'.or.filename(ii)(1:5).eq.
     /          'WENSS')) fr1 = 352.d6
               if (assoc_arr(i,ii).ne.0.and.tofit(ii).eq.1) then
                calcflux0(j,assoc_arr(i,j))=tot(ii,assoc_arr(i,ii))*
     /            ((fr/fr1)**spindef)
               end if
              end if
             end do
            end if
            if (spin_arr(i,3).ne.-999.d0) 
     /      calcflux1(j,assoc_arr(i,j))=10.d0**(dlog10(spin_arr(i,3))+
     /         spin_arr(i,4)*dlog10(fr/freq0)+spin_arr(i,5)*
     /         dlog10(fr/freq0)*dlog10(fr/freq0))
           end if
          end do
         end if
        end do

        return
        end


