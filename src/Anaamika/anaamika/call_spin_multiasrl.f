c! spectral indices as poly in log
c!
c! assoc_arr(i,j) has the gaul id of the jth gaul file of the ith association. 0 => no gaus
c! for list j.
c! assoc_ind(i,j) has the row number in assoc_arr for the jth gaul list for the ith gaus
c! for list j, ie, is the association number.

c! do normal spectral index anyway and also next term, thats enuff. 999
c! means not enuff data points
c! spin_arr is I0, spin for 1 term and I0, spin0, spin1 for two terms in log
c! in order of I0, spin0,   I10, spin10, spin11

        subroutine call_spin_multiasrl(sumngaul,nf,assoc_arr,
     /     nassoc,freq,tot,dtot,maxngaul,freq0,spin_arr,espin_arr,
     /     tofit,ra,dec,dra,ddec,avra,avdec,eavra,eavdec,filename)
        implicit none
        integer sumngaul,nf,assoc_arr(sumngaul,nf),nassoc,maxngaul,i
        integer j,k,nfit,dumi,tofit(nf)
        real*8 freq(nf),freq0,tot(nf,maxngaul),xfit(nf),yfit(nf),spin0
        real*8 spin10,spin11,sig(nf),I00,spin_arr(sumngaul,5),I10
        real*8 dtot(nf,maxngaul),espin_arr(sumngaul,5),fr
        real*8 spin00(nassoc),avra(sumngaul),avdec(sumngaul)
        real*8 ra(nf,maxngaul),dec(nf,maxngaul),meanra,emra
        real*8 dra(nf,maxngaul),ddec(nf,maxngaul),meandec,emdec
        real*8 eavra(sumngaul),eavdec(sumngaul)
        character filename(nf)*500
        
        dumi=0
        do i=1,nassoc
         nfit=0
         do j=1,nf
          if (assoc_arr(i,j).ne.0) then
           if (tofit(j).eq.1) then
            fr=freq(j)
            if (dec(j,assoc_arr(i,j)).gt.74.9d0.and.
     /         (filename(j)(1:5).eq.'wenss'.or.filename(j)(1:5).eq.
     /         'WENSS')) fr = 352.d6
            nfit=nfit+1
            xfit(nfit)=fr/freq0
            yfit(nfit)=tot(j,assoc_arr(i,j))
            sig(nfit)=dtot(j,assoc_arr(i,j))
           end if
          end if
         end do
         if (nfit.le.1) then
          do k=1,5
           spin_arr(i,k)=-999.d0 
           espin_arr(i,k)=0.d0
          end do
         else
          call sub_spin_masrl_1(xfit,yfit,sig,nf,nfit,spin_arr(i,2),
     /         spin_arr(i,1),espin_arr(i,1),espin_arr(i,2)) ! fit str8 line anyway
          dumi=dumi+1
          spin00(dumi)=spin_arr(i,2)
          if (nfit.eq.2) then
           do k=3,5
            spin_arr(i,k)=-999.d0 
            espin_arr(i,k)=0.d0 
           end do
          else    ! nfit > 2
           call sub_spin_masrl_2(xfit,yfit,sig,nf,nfit,spin_arr(i,4),
     /           spin_arr(i,5),spin_arr(i,3),espin_arr(i,3),
     /           espin_arr(i,4),espin_arr(i,5))
          end if
         end if
        end do

c! calculate centroid position for each association
        do i=1,nassoc
         call get_meanposn_multiasrl(nf,maxngaul,sumngaul,ra,
     /        dec,assoc_arr,dra,ddec,meanra,meandec,i,emra,emdec)
        avra(i)=meanra
        avdec(i)=meandec
        eavra(i)=emra
        eavdec(i)=emdec
        end do

        return
        end
c!
c!
c! log(I)=log(I00) + spin0*log(f/f0)
        subroutine sub_spin_masrl_1(xfit,yfit,sig,nf,nfit,spin0,I00,
     /             siga,sigb)
        implicit none
        integer i,nf,nfit,ia(2)
        real*8 xfit(nf),yfit(nf),sig(nf),chisq
        real*8 xf(nfit),yf(nfit),sigma(nfit),spin0,I00
        real*8 a(2),alamda,covar(2,2),alpha(2,2)
        real*8 siga,sigb,chi2,q,a1,b1,dumr1,dumr2,dumr3

        ia(1)=1
        ia(2)=1
        do i=1,nfit
         xf(i)=dlog10(xfit(i))
         yf(i)=dlog10(yfit(i))
         sigma(i)=abs(sig(i)/yfit(i))
        end do

c! for external catalogues, sigma=0. that makes siga, sigb as NAN
        dumr1=0.d0
        dumr2=1.d0
        do i=1,nfit
         dumr1=dumr1+sigma(i)
         dumr2=dumr2*sigma(i)
         if (sigma(i).ne.0.d0) dumr3=sigma(i)
        end do
        if (dumr1.eq.0.d0) then
         do i=1,nfit
          sigma(i)=abs(1.d0/yfit(i))
         end do
        end if
        if (dumr2.eq.0.d0) then
         do i=1,nfit
          if (sigma(i).eq.0.d0) sigma(i)=dumr3
         end do
        end if
        
        a(2)=(yf(1)-yf(nfit))/(xf(1)-xf(nfit)) 
        a(1)=yf(1)-a(2)*xf(1)

        alamda=-1.d0
        do i=1,20
         call mrqmin(xf,yf,sigma,nfit,nfit,a,ia,2,covar,alpha,2,chisq,
     /        alamda,6)  ! call spinfunc0
        end do
        alamda=0.d0
        call mrqmin(xf,yf,sigma,nfit,nfit,a,ia,2,covar,alpha,2,chisq,
     /       alamda,6)  ! call spinfunc0

        I00=10.d0**a(1)
        spin0=a(2)
        call linfit(xf,yf,nfit,sigma,1,a1,b1,siga,sigb,chi2,q)

        return
        end
c!
c!
c! log(I)=log(I10) + spin10*log(f/f0) + spin11*(log(f/f0)^2)
        subroutine sub_spin_masrl_2(xfit,yfit,sig,nf,nfit,spin10,
     /             spin11,I10,siga,sigb,sigc)
        implicit none
        integer i,nf,nfit,ia(3)
        real*8 xfit(nf),yfit(nf),sig(nf),chisq
        real*8 xf(nfit),yf(nfit),sigma(nfit),spin10,I10,spin11
        real*8 a(3),alamda,covar(3,3),alpha(3,3)
        real*8 siga,sigb,sigc

        ia(1)=1
        ia(2)=1
        ia(3)=1
        do i=1,nfit
         xf(i)=dlog10(xfit(i))
         yf(i)=dlog10(yfit(i))
         sigma(i)=abs(sig(i)/yfit(i))
        end do
        a(2)=(yf(1)-yf(nfit))/(xf(1)-xf(nfit)) +1.0
        a(1)=yf(1)-a(2)*xf(1)+1.0
        a(3)=0.1d0

        alamda=-1.d0
        do i=1,20
         call mrqmin(xf,yf,sigma,nfit,nfit,a,ia,3,covar,alpha,3,chisq,
     /        alamda,7)  ! call spinfunc1
        end do
        alamda=0.d0
        call mrqmin(xf,yf,sigma,nfit,nfit,a,ia,3,covar,alpha,3,chisq,
     /       alamda,7)  ! call spinfunc1

        I10=10.d0**a(1)
        spin10=a(2)
        spin11=a(3)

        return
        end
c!
c!
        subroutine get_meanposn_multiasrl(nf,maxngaul,sumngaul,ra,dec,
     /       assoc_arr,dra,ddec,avra,avdec,assocnum,eavra,eavdec)
        implicit none
        integer maxngaul,nf,sumngaul,assoc_arr(sumngaul,nf),i,assocnum
        real*8 ra(nf,maxngaul),dec(nf,maxngaul),dumr1,dumr2,denomra
        real*8 dra(nf,maxngaul),ddec(nf,maxngaul),avra,avdec,denomdec
        real*8 eavra,eavdec

        avra=0.d0
        avdec=0.d0
        denomra=0.d0
        denomdec=0.d0
        eavra=0.d0
        eavdec=0.d0
        do i=1,nf
         if (assoc_arr(assocnum,i).ne.0) then
          dumr1=1.d0/dra(i,assoc_arr(assocnum,i))
          dumr2=1.d0/ddec(i,assoc_arr(assocnum,i))
          dumr1=dumr1*dumr1
          dumr2=dumr2*dumr2
          eavra=eavra+dumr1
          eavdec=eavdec+dumr2
          denomra=denomra+dumr1
          denomdec=denomdec+dumr2
          avra=avra+ra(i,assoc_arr(assocnum,i))*dumr1
          avdec=avdec+dec(i,assoc_arr(assocnum,i))*dumr2
         end if
        end do
        avra=avra/denomra
        avdec=avdec/denomdec
        eavra=1.d0/sqrt(eavra)
        eavdec=1.d0/sqrt(eavdec)

        return
        end



