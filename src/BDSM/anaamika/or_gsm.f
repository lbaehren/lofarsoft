c! initial version of Optimised Radio Global Sky Model (OR_GSM)
c! first run makegsmparadefine
c! except for plotting qtys as fn of dist from centre and also if you do gdist_pa
c! for assoc_multiasrl, u dont need to worry about xpix and ypix in gaul files.
c! As long as flag = 0. change that now, so for general catalogue, flag is always 0.

        implicit none
        include "constants.inc"
        character fitsdir*500,fitsname*500,srldir*500,scratch*500
        character gsmsolnname*500,runcode*2,qcfullname*500,filename*500
        character extname*16,ofname*500,writeout*500,imagename*500
        character dumc*500,extn*20,solnname*500,fullname*500
        character plotdir*500,plotspectra*500,spinflux*500,plotpairs*500
        integer nchar,error,nf,tofit,nfr
        logical ransubbdsm

        write (*,*) 
        write (*,*) '  Starting OR_GSM (ver f1.1.8)'
        call read_gsmparadefine(error,fitsdir,scratch,srldir,
     /       gsmsolnname,plotspectra,runcode,plotdir,spinflux,
     /       plotpairs)

        nf=0   ! nf is number of valid files with BDSM run.
        open(unit=21,file=scratch(1:nchar(scratch))//'gsm_fits_list')
        open(unit=22,file=scratch(1:nchar(scratch))//'gsm_fits_good')
110     read (21,*,END=100) fitsname,solnname,tofit 
        fullname=fitsname(1:nchar(fitsname))//'.'//
     /           solnname(1:nchar(solnname))
        if (ransubbdsm(scratch,srldir,fitsdir,fullname,solnname)) then
         nf=nf+1 
         write (22,*) fitsname(1:nchar(fitsname)),' ',
     /           solnname(1:nchar(solnname)),' ',tofit
        end if
        goto 110
100     continue
        close(21)
        close(22)

        nfr=0
        open(unit=21,file=scratch(1:nchar(scratch))//'gsm_freqs')
115     read (21,*,END=200) dumc
        if (dumc.eq.'END') goto 200
        nfr=nfr+1 
        goto 115
200     continue
        close(21)

        call sub_orgsm(nf,nfr,fitsdir,scratch,srldir,gsmsolnname,
     /       runcode,plotdir,plotspectra,spinflux,plotpairs)
        
        end
c!
c!
c!
        subroutine sub_orgsm(nf,nfr,fitsdir,scratch,srldir,gsmsolnname,
     /             runcode,plotdir,plotspectra,spinflux,plotpairs)
        implicit none
        integer nf,nchar,i,j,tofit(nf),nfr
        character fitsdir*500,scratch*500,srldir*500,gsmsolnname*500
        character runcode*2,plotdir*500,filename(nf)*500,plotspectra*500
        character solnname(nf)*500,file1*500,file2*500,ofn*500
        character spinflux*500,plotpairs*500
        real*8 tol,bmmaj(nf),bmmin(nf),bmpa(nf)
        logical ranasrl

        write (*,*) '  Creating GSM with ',nf,' files'
        open(unit=23,file=scratch(1:nchar(scratch))//'gsm_fits_good')
        do i=1,nf
         read (23,*) filename(i),solnname(i),tofit(i)
        end do
        close(23)
        ofn=srldir(1:nchar(srldir))//'gsm_table_'//
     /       gsmsolnname(1:nchar(gsmsolnname))
        open(unit=25,file=ofn,form='formatted')
        ofn=srldir(1:nchar(srldir))//'gsm_calcf_'//
     /       gsmsolnname(1:nchar(gsmsolnname))
        open(unit=26,file=ofn,form='formatted')
        ofn=srldir(1:nchar(srldir))//'gsm_posns_'//
     /       gsmsolnname(1:nchar(gsmsolnname))
        open(unit=27,file=ofn,form='formatted')
        ofn=srldir(1:nchar(srldir))//'gsm_workf_'//
     /       gsmsolnname(1:nchar(gsmsolnname))
        open(unit=28,file=ofn,form='formatted')

        call sub_gsm_beam(nf,filename,solnname,scratch,bmmaj,
     /       bmmin,bmpa)

        write (*,*) '  Checking for 2-list associations'
c! associate every pair
        do i=1,nf
         do j=i+1,nf
          write (*,*) 'pair ',i,j
          tol=0.d0
c         tol=500.d0
c          write (*,*) ' setting tol to 500 arcsec !!!!'
          file1=filename(i)(1:nchar(filename(i)))//'.'//
     /          solnname(i)(1:nchar(solnname(i)))
          file2=filename(j)(1:nchar(filename(j)))//'.'//
     /          solnname(j)(1:nchar(solnname(j)))
          if (.not.ranasrl(scratch,filename(i),solnname(i),filename(j),
     /         solnname(j))) 
     /     call associatesrl(file1,file2,scratch,srldir,runcode,tol)
          call get_asrl_params(28,i,j,file1,file2,scratch,
     /         srldir,runcode,tol,spinflux,nf,bmmaj,bmmin,bmpa) ! and then decide based on this.
         end do
        end do

c! associate all together
        call call_multi_asrl(nf,nfr,filename,solnname,scratch,srldir,
     /       25,26,27,plotdir,plotspectra,gsmsolnname,tofit,spinflux,
     /       plotpairs)

        write (*,*) '  Output : '//srldir(1:nchar(srldir))//'gsm_table_'
     /       //gsmsolnname(1:nchar(gsmsolnname))
        write (*,*) '  Output : '//srldir(1:nchar(srldir))//'gsm_calcf_'
     /       //gsmsolnname(1:nchar(gsmsolnname))
        write (*,*) '  Figures in  '//plotdir(1:nchar(plotdir))//
     /   gsmsolnname(1:nchar(gsmsolnname))//'/gsm_'//gsmsolnname
     /   (1:nchar(gsmsolnname))//'.pdf'
        close(25)
        close(26)
        close(27)
        close(28)


        return
        end
c!
c!
c!
        subroutine sub_gsm_beam(nf,filename,solnname,scratch,bmmaj,
     /       bmmin,bmpa)
        implicit none
        integer nf,nchar,i,error
        character filename(nf)*500,solnname(nf)*500,scratch*500,f1*500
        character extn*20,keyword*500,keystrng*500,comment*500
        real*8 bmmaj(nf),bmmin(nf),bmpa(nf)

        extn='.header'
        do i=1,nf
         f1=filename(i)(1:nchar(filename(i)))//'.'//
     /          solnname(i)(1:nchar(solnname(i)))
         keyword='BMAJ'
         call get_keyword(f1,extn,keyword,keystrng,bmmaj(i),
     /             comment,'r',scratch,error)
         keyword='BMIN'
         call get_keyword(f1,extn,keyword,keystrng,bmmin(i),
     /             comment,'r',scratch,error)
         keyword='BPA'
         call get_keyword(f1,extn,keyword,keystrng,bmpa(i),
     /             comment,'r',scratch,error)
         if (error.ne.0) then
          bmmaj(i)=1.d0
          bmmin(i)=1.d0
          bmpa(i)=0.d0
         end if        
        end do 

        return
        end





