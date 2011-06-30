c! this is my own associate sourcelists program, mainly to test output srl of BDSM 
c! against the input sourcelist which made the image. this is only for comparing 2 srls
c! but shud have all capabilities of AWE AssociateSourcelist, eventually.
c! cop stuff from makelists.f written for srccomp.f 
c! CHANGE FORMAT GAUL

        subroutine associatesrl(master,sec,scratch,srldir,rcode,tol)
        implicit none
        character master*500,sec*500
        integer n,m,nchar,gpi,error
        integer nsrcm,nffmtm,nislm,nsrcs,nffmts,nisls
        character f1m*500,f1s*500,ffmtm*500,ffmts*500,ch1
        character fg*500,extn*20,keyword*500,dir*500,scratch*500
        character comment*500,srldir*500,rcode*2
        real*8 keyvalue,tol

        call sourcelistheaders(master,f1m,n,m,nislm,nsrcm,gpi,nffmtm,
     /       ffmtm,srldir)
        call sourcelistheaders(sec,f1s,n,m,nisls,nsrcs,gpi,nffmts,
     /       ffmts,srldir)
        ch1='y'
        if (rcode(1:1).eq.'v') then
         if (f1m(1:nchar(f1m)).ne.f1s(1:nchar(f1s))) then
          write (*,*) '  Input images are different (',f1m(1:nchar(f1m))
     /                ,' & ',f1s(1:nchar(f1s)),')'
          write (*,'(a,$)') '   Want to continue (y)/n : '
          read (*,'(a1)') ch1
          if (ichar(ch1).eq.32) ch1='y'
         end if
        end if

        if (ch1.eq.'y') call sub_associatesrl(master,sec,nsrcm,
     /     nsrcs,scratch,srldir,rcode,tol)
        
        return
        end
c!
c! -----------------------------  SUBROUTINES  -------------------------------
c!
        subroutine sub_associatesrl(master,sec,nsrcm,nsrcs,
     /             scratch,srldir,rcode,tol)
        implicit none
        character master*500,sec*500,scrat*500,srldir*500
        integer n,m,nchar,isrcm,isrcs,dsum(5),gpi,error
        integer nsrcm,nffmtm,nislm,nsrcs,nffmts,nisls,fac
        integer flagm(nsrcm),flags(nsrcs),dumim(nsrcm),dumis(nsrcs)
        character f1m*500,f1s*500,ffmtm*500,ffmts*500,ch1,scratch*500
        real*8 peakm(nsrcm),ram(nsrcm),decm(nsrcm),bmajm(nsrcm)
        real*8 bminm(nsrcm),bpam(nsrcm),rstdm(nsrcm),ravm(nsrcm)
        real*8 totalm(nsrcm),dpeakm(nsrcm),dtotalm(nsrcm),dumrm(nsrcm)
        real*8 peaks(nsrcs),ras(nsrcs),decs(nsrcs),bmajs(nsrcs)
        real*8 rstds(nsrcs),ravs(nsrcs),dumrs(nsrcs),rmsm(nsrcm)
        real*8 sstdm(nsrcm),savm(nsrcm),sstds(nsrcs),savs(nsrcs)
        real*8 totals(nsrcs),dpeaks(nsrcs),dtotals(nsrcs),rmss(nsrcs)
        real*8 bmins(nsrcs),bpas(nsrcs),meds,tol,medm
        real*8 dbmajs(nsrcs),dbmins(nsrcs),dbpas(nsrcs)
        real*8 dbmajm(nsrcm),dbminm(nsrcm),dbpam(nsrcm),freq1,freq2
        character codem(nsrcm)*4,codes(nsrcs)*4,rcode*2
        character namesm(nsrcm)*30,namess(nsrcs)*30

cf2py   intent(in) master, sec, nsrcm, nsrcs, scratch, srldir, rcode, tol

        call sourcelistheaders(master,f1m,n,m,nislm,nsrcm,gpi,nffmtm,
     /       ffmtm,srldir)
        call sourcelistheaders(sec,f1s,n,m,nisls,nsrcs,gpi,nffmts,
     /       ffmts,srldir)

c! get freq to write on 1st line of .pasf for spectral index calc in pasl
        call get_freq_cube(master,scratch,freq1,error)
        call get_freq_cube(sec,scratch,freq2,error)

c! this will depend inside on format of srl 
        call readgaul(master,nsrcm,nsrcm,srldir,dumim,dumim,flagm,
     /    totalm,
     /    dtotalm,peakm,dpeakm,ram,dumrm,decm,dumrm,dumrm,dumrm,
     /    dumrm,dumrm,bmajm,dbmajm,bminm,dbminm,bpam,dbpam,dumrm,
     /    dumrm,dumrm,dumrm,dumrm,dumrm,
     /    sstdm,savm,rstdm,ravm,dumrm,dumrm,codem,namesm,
     /    dumim,dumim,dumim,dumim,rmsm,dumrm,dumrm,dumrm,dumrm,dumrm)
        call readgaul(sec,nsrcs,nsrcs,srldir,dumis,dumis,flags,totals,
     /    dtotals,
     /    peaks,dpeaks,ras,dumrs,decs,dumrs,dumrs,dumrs,dumrs,dumrs,
     /    bmajs,dbmajs,bmins,dbmins,bpas,dbpas,dumrs,dumrs,dumrs,dumrs,
     /    dumrs,dumrs,sstds,savs,rstds,ravs,dumrs,dumrs,codes,
     /    namess,dumis,dumis,dumis,dumis,rmss,dumrs,dumrs,dumrs,
     /    dumrs,dumrs)
        call call_getsrcsize(bmajm,bminm,nsrcm,medm)
        call call_getsrcsize(bmajs,bmins,nsrcs,meds)
        if (rcode(1:1).ne.'q') 
     /   write (*,'(a,f9.3,a,f9.3)') '   Median sizes : ',medm,' & ',
     /                                   meds
        if (rcode(1:1).eq.'v ') then
         write (*,'(a,$)') '   Tolerance : '
         read (*,*) tol
        else
         if (tol.eq.0.d0) then
          tol=0.5*max(medm,meds)
          tol=sqrt(medm*meds)
          tol=0.45d0*min(medm,meds)
          write (*,'(a,f9.3,a,f9.3)') '   Median sizes : ',medm,' & ',
     /                                   meds
         end if
         write (*,*) '   Tolerance : ',tol
        end if
        write (*,'(a,$)') '   Associating sourcelists '
        write (*,*) master(1:nchar(master)),' (',nsrcm,' srcs) & ',
     /              sec(1:nchar(sec)),' (',nsrcs,' srcs).'
        
        scrat=master(1:nchar(master))//'__'//sec(1:nchar(sec))
        fac=1
        if (nsrcm.gt.1e4.and.nsrcs.gt.2e3) fac=100
        if (nsrcm.gt.1e6.and.nsrcs.gt.1e5) fac=1000
        call make_associatelist1(nsrcm,ram,decm,nsrcs,ras,decs,tol,
     /       scrat,master,sec,dsum,scratch,flagm,flags,codem,codes,
     /       rcode,fac)

        call write_associatelist1(nsrcm,totalm,dtotalm,peakm,dpeakm,
     /   ram,decm,bmajm,bminm,bpam,sstdm,savm,rstdm,ravm,nsrcs,totals,
     /   dtotals,peaks,dpeaks,ras,decs,bmajs,bmins,bpas,
     /   sstds,savs,rstds,ravs,tol,scrat,master,sec,dsum,scratch,
     /   namesm,namess,rmsm,rmss,rcode,freq1,freq2)

        if (rcode(1:1).eq.'q') goto 333
c        write (*,'(a,$)') '   Plot the association list (y)/n ? : '
c        read (*,'(a1)') ch1
c        if (ichar(ch1).eq.32) ch1='y'
c        if (ch1.eq.'y') call plot_associatelist1(scrat,scratch)
333     continue

        return
        end



        subroutine call_getsrcsize(bmajm,bminm,nn,meds)
        implicit none
        integer nn,i,round
        real*8 bmajm(nn),bminm(nn),avsize,bm(nn),selectnr,meds

        do i=1,nn
         bm(i)=sqrt(bmajm(i)*bminm(i))
        end do
        meds=selectnr(round(nn/2.d0),nn,bm)

        return
        end




