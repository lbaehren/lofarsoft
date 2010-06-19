
        subroutine calc_gsm(nf,nfr,sumngaul,scratch,spin_arr,espin_arr,
     /    avra,avdec,gsmsolnname,nassoc,filename,solnname,
     /    tofit,freq0,eavra,eavdec,assoc_arr,spindef,tot,etot,
     /    maxngaul,freq)
        implicit none
        integer sumngaul,nf,nfr,i,nchar,tofit(nf),j,nassoc,dumi1
        integer code,code2,sumcode,inf,maxngaul
        real*8 spin_arr(sumngaul,5),espin_arr(sumngaul,5)       
        real*8 avra(sumngaul),avdec(sumngaul),fr(nfr)
        real*8 eavra(sumngaul),eavdec(sumngaul),codef,spindef
        character scratch*500,gsmsolnname*500,f1*500,f2*500,str10*10
        character filename(nf)*500,solnname(nf)*500
        character dumc*500
        real*8 lsp00,usp00,lsp0010,usp0010,lsp11,usp11,lsp001011
        real*8 usp001011,freq0,s0010(sumngaul),s11(sumngaul),fl,efl
        real*8 s001011(sumngaul),meds11,meds0010,meds001011
        integer assoc_arr(sumngaul,nf)
        real*8 tot(nf,maxngaul),etot(nf,maxngaul),freq(nf)

c! read input file
        open(unit=21,file=scratch(1:nchar(scratch))//'gsm_freqs')
        do i=1,nfr
         read (21,*) fr(i)
         fr(i)=fr(i)*1.d6
        end do
        read (21,*)
        read (21,*) dumc,dumc,lsp00
        read (21,*) dumc,dumc,usp00
        read (21,*) dumc,dumc,lsp0010
        read (21,*) dumc,dumc,usp0010
        read (21,*) dumc,dumc,lsp11
        read (21,*) dumc,dumc,usp11
        read (21,*) dumc,dumc,lsp001011
        read (21,*) dumc,dumc,usp001011
        close(21)

c! calc medians for all the constraints on spectral index
        dumi1=0
        do i=1,nassoc
         if (spin_arr(i,3).gt.-900.d0) then
          dumi1=dumi1+1
          s0010(dumi1)=spin_arr(i,2)-spin_arr(i,4)
          s11(dumi1)=spin_arr(i,5)
          s001011(dumi1)=spin_arr(i,2)-spin_arr(i,4)-spin_arr(i,5)
         end if 
        end do
        if (dumi1.gt.0) then
         call calcmedian(s11,sumngaul,1,dumi1,meds11)
         call calcmedian(s0010,sumngaul,1,dumi1,meds0010)
         call calcmedian(s001011,sumngaul,1,dumi1,meds001011)
        else
         meds11=0.d0
         meds0010=0.d0
         meds001011=0.d0
        end if

c! calc flux and write
        f1=scratch(1:nchar(scratch))//'gsm_'//
     /     gsmsolnname(1:nchar(gsmsolnname))//'_calc_f'
        do i=1,nfr
         call int2str(i,str10)
         f2=f1(1:nchar(f1))//str10(1:nchar(str10))
         open(unit=21,file=f2)
          call sub_calc_gsm_wrhead(nf,tofit,filename,solnname,21,
     /         nassoc,fr(i))
          do j=1,nassoc
           call get_gsm_flux_code(sumngaul,spin_arr,espin_arr,fr(i),
     /       lsp00,usp00,lsp0010,usp0010,lsp11,usp11,lsp001011,
     /       usp001011,fl,efl,code,j,freq0,meds11,meds0010,meds001011,
     /       spindef,maxngaul,nf,assoc_arr,tot,etot,freq)
           code2=0
           sumcode=0
           do inf=1,nf
            if (assoc_arr(j,inf).ne.0) then
             code2=code2+(10**(nf-inf))
             sumcode=sumcode+(10**nf)
            end if
           end do
           code2=sumcode+code2
           write (21,777) j,code,code2,avra(j),eavra(j),avdec(j),
     /     eavdec(j),fl,efl
          end do
         close(21)
        end do
777     format(i8,3x,i2,3x,i6,1x,6(1x,1Pe10.3))

        return
        end
c!
c!
c!
        subroutine sub_calc_gsm_wrhead(nf,tofit,filename,solnname,
     /             n1,nassoc,freq)
        implicit none
        integer n1,nf,tofit(nf),nassoc,totfit,i,nchar
        character filename(nf)*500,solnname(nf)*500
        real*8 freq

        totfit=0
        do i=1,nf
         if (tofit(i).eq.1) totfit=totfit+1
        end do
        
        write (n1,*) 'Frequency_of_GSM_MHz = ',freq
        write (n1,*) 'Number_of_associations = ',nassoc
        write (n1,*) 'Number_of_surveys_used = ',totfit
        do i=1,nf
         if (tofit(i).eq.1) write (n1,*) 'File = ',
     /      filename(i)(1:nchar(filename(i)))//
     /      solnname(i)(1:nchar(solnname(i)))
        end do
        write (n1,*) '#assoc flag    code     RA        eRA         '//
     /        'Dec       eDec       Flux      eFlux '
        write (n1,*) '                       (deg)     (deg)        '//
     /        '(deg)     (deg)      (Jy)       (Jy)  '

        return
        end
c!
c!
c!
        subroutine get_gsm_flux_code(sumngaul,spin_arr,espin_arr,freq,
     /      lsp00,usp00,lsp0010,usp0010,lsp11,usp11,lsp001011,
     /      usp001011,fl,efl,code,i,freq0,meds11,meds0010,meds001011,
     /      spindef,maxngaul,nf,assoc_arr,tot,etot,freqarr)
        implicit none
        integer sumngaul,i,maxngaul,nf,inf
        integer code,findex
        real*8 spin_arr(sumngaul,5),espin_arr(sumngaul,5),fl,efl
        real*8 lsp00,usp00,lsp0010,usp0010,lsp11,usp11,lsp001011
        real*8 spindef,fr1,flf,eflf
        real*8 freq,usp001011,freq0,dumr,meds11,meds0010,meds001011
        real*8 tot(nf,maxngaul),etot(nf,maxngaul),freqarr(nf)
        integer assoc_arr(sumngaul,nf)

        code=0  ! good
        dumr=freq/freq0
        if (spin_arr(i,1).gt.-900.d0) then         !  valid assoc
         if (spin_arr(i,3).lt.-900.d0) then          !  spin0
          fl=spin_arr(i,1)*(dumr**(spin_arr(i,2)))
          efl=fl*(espin_arr(i,1)/spin_arr(i,1)+espin_arr(i,2)*
     /        dlog(dumr))
          efl=abs(efl)
         else
          fl=10.d0**(log10(spin_arr(i,3))+spin_arr(i,4)*abs(log10(dumr))
     /       +spin_arr(i,5)*log(dumr)*log(dumr))
          efl=fl*(espin_arr(i,1)/spin_arr(i,1)+espin_arr(i,2)*
     /        dlog(dumr)+espin_arr(i,5)*dlog(dumr)*dlog(dumr))
          efl=abs(efl)
          if (spin_arr(i,5)-meds11.lt.lsp11.or.
     /        spin_arr(i,5)-meds11.gt.usp11) code=4
          if (spin_arr(i,2)-spin_arr(i,4)-meds0010.lt.lsp0010.or.
     /        spin_arr(i,2)-spin_arr(i,4)-meds0010.gt.usp0010) code=2
          if (spin_arr(i,2)-spin_arr(i,4)-spin_arr(i,5)-meds001011.lt.
     /        lsp0010.or.spin_arr(i,2)-spin_arr(i,4)-spin_arr(i,5)-
     /        meds001011.gt.usp0010) code=3
         end if
        else
         code=-1  ! only one point
         findex=0
         do inf=1,nf
          if (assoc_arr(i,inf).ne.0) findex=inf
         end do
         if (findex.ne.0) then
          flf=tot(findex,assoc_arr(i,findex))
          eflf=etot(findex,assoc_arr(i,findex))
          fr1=freqarr(findex)
          fl=flf*((freq/fr1)**spindef)
          efl=eflf*fl/flf
         else
          code=4
         end if
        end if

        if (code.eq.0) then
         if (spin_arr(i,2).lt.lsp00.or.spin_arr(i,2).gt.usp00) code=1
        end if

        return
        end


