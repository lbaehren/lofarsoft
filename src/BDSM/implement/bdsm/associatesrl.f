c! this is my own associate sourcelists program, mainly to test output srl of BDSM 
c! against the input sourcelist which made the image. this is only for comparing 2 srls
c! but shud have all capabilities of AWE AssociateSourcelist, eventually.
c! cop stuff from makelists.f written for srccomp.f 

        subroutine associatesrl(master,sec)
        implicit none
        character master*(*),sec*(*)
        integer n,m,nchar,gpi
        integer nsrcm,nffmtm,nislm,nsrcs,nffmts,nisls
        character f1m*500,f1s*500,ffmtm*500,ffmts*500,ch1
        character fg*500,extn*10,keyword*500,dir*500,scratch*500
        character comment*500,srldir*500
        real*8 keyvalue

        fg="paradefine"
        extn=""
        dir="./"
        keyword="scratch"
        call get_keyword(fg,extn,keyword,scratch,keyvalue,
     /    comment,"s",dir)
        keyword="srldir"
        call get_keyword(fg,extn,keyword,srldir,keyvalue,
     /    comment,"s",dir)

        call sourcelistheaders(master,f1m,n,m,nislm,nsrcm,gpi,nffmtm,
     /       ffmtm,srldir)
        call sourcelistheaders(sec,f1s,n,m,nisls,nsrcs,gpi,nffmts,
     /       ffmts,srldir)
        ch1='y'
        if (f1m(1:nchar(f1m)).ne.f1s(1:nchar(f1s))) then
         write (*,*) '  Input images are different (',f1m(1:nchar(f1m))
     /               ,' & ',f1s(1:nchar(f1s)),')'
         write (*,'(a,$)') '   Want to continue (y)/n : '
         read (*,'(a1)') ch1
         if (ichar(ch1).eq.32) ch1='y'
        end if

        if (ch1.eq.'y') call sub_associatesrl(master,sec,nsrcm,
     /     nsrcs,scratch,srldir)
        
        return
        end
c!
c! -----------------------------  SUBROUTINES  -------------------------------
c!
        subroutine sub_associatesrl(master,sec,nsrcm,nsrcs,
     /             scratch,srldir)
        implicit none
        character master*(*),sec*(*),scrat*100,srldir*500
        integer n,m,nchar,isrcm,isrcs,dsum(5),gpi
        integer nsrcm,nffmtm,nislm,nsrcs,nffmts,nisls
        character f1m*500,f1s*500,ffmtm*500,ffmts*500,ch1,scratch
        real*8 fluxm(nsrcm),ram(nsrcm),decm(nsrcm),bmajm(nsrcm)
        real*8 bminm(nsrcm),bpam(nsrcm),rstdm(nsrcm),ravm(nsrcm)
        real*8 fluxs(nsrcs),ras(nsrcs),decs(nsrcs),bmajs(nsrcs)
        real*8 rstds(nsrcs),ravs(nsrcs)
        real*8 sstdm(nsrcm),savm(nsrcm),sstds(nsrcs),savs(nsrcs)
        real*8 bmins(nsrcs),bpas(nsrcs),meds,tol,medm
        real*8 dbmajs(nsrcs),dbmins(nsrcs),dbpas(nsrcs)
        real*8 dbmajm(nsrcm),dbminm(nsrcm),dbpam(nsrcm)

        call sourcelistheaders(master,f1m,n,m,nislm,nsrcm,gpi,nffmtm,
     /       ffmtm,srldir)
        call sourcelistheaders(sec,f1s,n,m,nisls,nsrcs,gpi,nffmts,
     /       ffmts,srldir)

c! this will depend inside on format of srl 
        call readgaul(master,nsrcm,fluxm,ram,decm,bmajm,bminm,bpam,
     /              dbmajm,dbminm,dbpam,sstdm,savm,rstdm,ravm,srldir)
        call readgaul(sec,nsrcs,fluxs,ras,decs,bmajs,bmins,bpas,
     /              dbmajs,dbmins,dbpas,sstds,savs,rstds,ravs,srldir)
        call call_getsrcsize(bmajm,bminm,nsrcm,medm)
        call call_getsrcsize(bmajs,bmins,nsrcs,meds)
        write (*,'(a,f6.3,a,f6.3)') '   Median sizes : ',medm,' & ',meds
        write (*,'(a,$)') '   Tolerance : '
        read (*,*) tol
        write (*,'(a,$)') '   Associating sourcelists '
        write (*,*) master(1:nchar(master)),' (',nsrcm,' srcs) & ',
     /              sec(1:nchar(sec)),' (',nsrcs,' srcs).'
        
        scrat=master(1:nchar(master))//'_'//sec(1:nchar(sec))
        call make_associatelist1(nsrcm,ram,decm,nsrcs,ras,decs,tol,
     /       scrat,master,sec,dsum,scratch)

        call write_associatelist1(nsrcm,fluxm,ram,decm,bmajm,bminm,bpam,
     /       sstdm,savm,rstdm,ravm,nsrcs,fluxs,ras,decs,bmajs,bmins,bpas
     /       ,sstds,savs,rstds,ravs,tol,scrat,master,sec,dsum,scratch)

        write (*,'(a,$)') '   Plot the association list (y)/n ? : '
        read (*,'(a1)') ch1
        if (ichar(ch1).eq.32) ch1='y'
        if (ch1.eq.'y') call plot_associatelist1(scrat)

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




