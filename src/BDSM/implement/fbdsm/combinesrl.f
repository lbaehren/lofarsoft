c! combines gaul and srl - to make master list for wenss.
c! list of gaul to combine is in scratch/filen.gaulist

        subroutine combinesrl(filen,scratch,srldir)
        implicit none
        character cmd*500,filen*500,scratch*500,srldir*500,ch1*1
        integer nchar,nfiles

        write (*,*) '  Assuming no backslashes in file names ... '
        write (*,*) '  Edit file to include gaussian lists ... '
        cmd="ls "//srldir(1:nchar(srldir))//
     /      "*.gaul | sed 's/\\.gaul$//' | sed 's/.*\\///' > "//
     /      scratch(1:nchar(scratch))//filen(1:nchar(filen))//
     /      ".gaulist; gvim "//scratch(1:nchar(scratch))//
     /      filen(1:nchar(filen))//".gaulist"
        call system(cmd)
        write (*,'(a,$)') '   Press any key to start processing ... '
        read (*,'(a1)') ch1

        call getline(filen,scratch,nfiles)
        call sub_combinesrl(filen,scratch,srldir,nfiles)

        return
        end
c!
c!   
c!
        subroutine sub_combinesrl(filen,scratch,srldir,nfiles)
        implicit none
        character cmd*500,filen*500,scratch*500,srldir*500
        integer nchar,nfiles,n,m,nisl(nfiles),nsrc(nfiles)
        character filename(nfiles)*500,f1*500,ffmt*500
        integer gpi(nfiles),nffmt,i

        open(unit=21,file=srldir(1:nchar(srldir))//
     /       filen(1:nchar(filen))//'.csrl.gaul')
        open(unit=22,file=scratch(1:nchar(scratch))//
     /       filen(1:nchar(filen))//'.gaulist')
        do i=1,nfiles
         read (22,*) filename(i)
         call sourcelistheaders(filename(i),f1,n,m,nisl(i),nsrc(i),
     /        gpi(i),nffmt,ffmt,srldir)
        end do
        close(22)

        do i=1,nfiles
         call sub_csrl_readgaul(22,filename(i),nisl(i),nsrc(i),
     /        gpi(i),ffmt,srldir)
        end do

        close(21)
        

        return
        end
c!
c!
c!
        subroutine sub_csrl_readgaul(n1,fname,n_isl,n_src,g_pi,
     /             ffmt,srldir)
        implicit none
        integer n1,n_isl,n_src,g_pi,gaulidg(n_src),islidg(n_src)
        integer flagg(n_src)
        real*8 totalg(n_src),dtotalg(n_src),peakg(n_src),dpeakg(n_src)
        real*8 rag(n_src),erag(n_src),decg(n_src),edecg(n_src)
        real*8 dumrg(n_src)
        real*8 bmajg(n_src),ebmajg(n_src),bming(n_src),ebming(n_src)
        real*8 bpag(n_src),ebpag(n_src),dbmajg(n_src),edbmajg(n_src)
        real*8 dbming(n_src),edbming(n_src),dbpag(n_src),edbpag(n_src)
        real*8 sstdg(n_src),savg(n_src),rstdg(n_src),ravg(n_src)
        real*8 chisqg(n_src),qg(n_src)
        character ffmt*500,srldir*500,names*30,codeg(n_src)*4
        character fname*500

c        call readgaul(fname,n_src,n_src,srldir,gaulidg,islidg,flagg,totalg,
c     /    dtotalg,peakg,dpeakg,ragg,erag,decg,edecg,dumr,dumr,
c     /    dumr,dumr,bmajg,dbmajg,bming,dbming,bpag,dbpag,dbmajg,
c     /    edbmajg,dbming,edbming,dbpag,edbpag,
c     /    sstdg,savg,rstdg,ravg,chisqg,qg,codeg,names)
c
c        open(unit=21,file=srldir(1:nchar(srldir))//f2(1:nchar(f2))//
c     /       '.srl.bin',form='unformatted')
c       do i=1,nrows
c         read (21) numsrc(i),sflag(i),code(i),total(i),etotfl(i),
c     /     speak(i),epeakfl(i),max_peak(i),epeakfl(i),sra(i),era(i),
c     /     sdec(i),edec(i),mra(i),era(i),mdec(i),edec(i),
c     /     bmaj(i),ebmaj(i),bmin(i),ebmin(i),bpa(i),ebpa(i),
c     /     dbmaj(i),edbmaj(i),dbmin(i),edbmin(i),dbpa(i),edbpa(i),
c     /     rmsisl(i),ngau(i)
c        end do
c        close(21)
cc!


        return
        end





