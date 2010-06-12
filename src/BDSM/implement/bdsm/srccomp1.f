c! this is to compare results of SAD, sfind of MIRIAD and SExtractor, in files
c! $RUNFIL/SADSTAR1, $FITS/sfind.log, ~/sex/sex.cat.
c!
c! make star file of sextractor called SEXSTAR in desc order of ra.
c! run sf2star to make star file of sfind called SFINDSTAR
c! SAD star file is already imported as SADSTAR1, all in $RUNFIL which --> SADSTAR
c!
c! to compare, use full files, which is SADFILE, SFINDFILE and SEXFILE 
c! and make sad_cat, sfind_cat and sex_cat which has ra, dec in deg

c! Oct 2006 -- Modifying this now to plot BDSM, SAD and SEX instead.
c! sex -c b_small.sex /data2/niruj/image/fits/BOOTES.FITS
c! MFTABLE and SADSTAR1 through MFREAD.02S

c! damn -- sex doesnt know NCP. so use sexcorrect to change from TAN to NCP

        implicit none
        integer nsrcsex,i,nsrcsad,nsrcbdsm
        character command*300,filen*500,f1*10,f2*10,f3*10
        character filen1*30,dir*500
        integer n1,n2,n3,dumi
        real*8 tol
        real*8 peakD(9000),integD(9000),raD(9000),decD(9000),bmajD(9000)
        real*8 bminD(9000),bpaD(9000)
        real*8 dpeakD(9000),dintegD(9000),draD(9000),ddecD(9000)
        real*8 dbminD(9000),dbpaD(9000),dbmajD(9000)
        real*8 sizemajD(9000),sizeminD(9000),sizepaD(9000)
        real*8 dsizemajD(9000),dsizeminD(9000),dsizepaD(9000)
        real*8 residrmsD(9000),respeakD(9000),resintD(9000)
        integer hhD(9000),mmD(9000),ddD(9000),maD(9000)
        real*8 ssD(9000),saD(9000)
        integer hhB(9000),mmB(9000),ddB(9000),maB(9000)
        real*8 ssB(9000),saB(9000)
        real*8 peakB(9000),integB(9000),raB(9000),decB(9000),bmajB(9000)
        real*8 bminB(9000),bpaB(9000)
        real*8 dpeakB(9000),dintegB(9000),draB(9000),ddecB(9000)
        real*8 dbminB(9000),dbpaB(9000),dbmajB(9000)
        real*8 sizemajB(9000),sizeminB(9000),sizepaB(9000)
        real*8 dsizemajB(9000),dsizeminB(9000),dsizepaB(9000)
        real*8 residrmsB(9000),respeakB(9000),resintB(9000)
        integer hhX(9000),mmX(9000),ddX(9000),maXX(9000)
        real*8 ssX(9000),saX(9000),dpeakX(9000)
        real*8 raX(9000),decX(9000),magisocorrX(9000),magerrX(9000)
        real*8 bmajX(9000),bminX(9000),bpaX(9000),peakX(9000)
        real*8 dbmajX(9000),dbminX(9000),dbpaX(9000),fwhmX(9000)
        character sD(9000)*1,sX(9000)*1,sB(9000)*1
        integer indexlistall,l,m,n,el(9000),em(9000),en(9000),np
        real*8 mean,rms,dg,beamD(9000),beamB(9000),beamX(9000)
        real*8 dbeamD(9000)
        real*8 distDB(9000),distDX(9000),distBX(9000),unity(9000)
        real*8 radi,r1,r2,d1,d2,dposD(9000),dposB(9000)
        integer order(3),lmn(9000,3),elmn(3)
        
        radi=3.14159d0/180.d0

        dg=3600.d0

c! create star files for sex for aips. 
        command="sed -e '1,11d' sex.cat | awk '{print $2,$3,$4,$5,"
     /    //"$6,$7,$8,$9,$10,$11,$12}'| sort -g -r > sex1.cat"
        call system(command)

        filen='sex1.cat'
        dir='./'
        call getline(filen,dir,nsrcsex)
        call sex2star(filen,nsrcsex)

c! create the workfiles and cat files for comparison
c --- for SAD 
        open(unit=21,file='MFTABLE',status='old')
        read (21,*)
        read (21,'(14x,i8)') nsrcsad
        close(21)
        call makesadcat(nsrcsad)

c --- for BDSM
        call makebdsmcat(nsrcbdsm)

c --- for SEX
        call makesexcat(nsrcsex) 

c! now to check for common sources with sep < tol
c! what about multiple sources ? take file with max srcs and match 
c! and then list remaining sources in other 2 files
c! 4 lists - listall, listsex, listsad, listbdsm
c! listall has 3 fields - 0 or # for each, starting with maxlist, then other two
c! listbdsm has 3 fields - 1st is bdsm num, 2nd is sex cntrprt (0,#), 3--> bdsm
c! so listsad is like initial part of listall before two mins has left over srcs.
c! n1, n2, n3 and f1 f2 f3 are #srcs and name of _cat file in desc order of #src
c! ok, finished makelists.f -- has the lists is list1, list2, list3, listall and problem.
c! well, no --- listall and list1,2,3 are not sad,bdsm,sex but the lists in decreasing
c! order of #srcs so now shud make back before plotting. 

        write (*,*) 
        write (*,'(a32,$)') '    Enter tolerance in arcsec : '
        read (*,*) tol           ! in arcsec
        call arrange(nsrcsad,nsrcbdsm,nsrcsex,n1,n2,n3,f1,f2,f3,order)
        call makelists(n1,n2,n3,f1,f2,f3,tol)
        filen='problem'
        call getline(filen,dir,dumi)
        write (*,*) 
        write (*,*) '   Output files are : '
        write (*,*) '   list1, list2, list3, listall, problem'
        write (*,'(a30,i3)') '    Number of problem cases : ',dumi
        write (*,*) 

c! now to compare coords and fluxes etc between the three lists
c! read in the values from the FILES
        
        open(unit=21,file='SADFILE',status='old')
        do 300 i=1,nsrcsad
         read (21,'(23(e14.8,2x))') raD(i),decD(i),peakD(i),
     /     integD(i),bmajD(i),bminD(i),bpaD(i),dpeakD(i),dintegD(i),
     /     draD(i),ddecD(i),dbmajD(i),dbminD(i),dbpaD(i),
     /     sizemajD(i),sizeminD(i),sizepaD(i),dsizemajD(i),
     /     dsizeminD(i),dsizepaD(i),residrmsD(i),respeakD(i),
     /     resintD(i)
         call convertra(raD(i),hhD(i),mmD(i),ssD(i))
         call convertdec(decD(i),sD(i),ddD(i),maD(i),saD(i))
         bmajD(i)=bmajD(i)*dg
         bminD(i)=bminD(i)*dg
         dbmajD(i)=dbmajD(i)*dg
         dbminD(i)=dbminD(i)*dg
         beamD(i)=bmajD(i)*bminD(i)
         dbeamD(i)=dbmajD(i)*dbminD(i)
         dposD(i)=sqrt((draD(i))**2.d0+(ddecD(i))**2.d0)*dg
300     continue
        close(21)

        open(unit=21,file='BDSMFILE',status='old')
        do 310 i=1,nsrcbdsm
         read (21,*) raB(i),draB(i),decB(i),ddecB(i),integB(i),
     /    dintegB(i),peakB(i),dpeakB(i),bmajB(i),dbmajB(i),bminB(i),
     /    dbminB(i),bpaB(i),dbpaB(i),sizemajB(i),dsizemajB(i),
     /    sizeminB(i),dsizeminB(i),sizepaB(i),dsizepaB(i),residrmsB(i)
         call convertra(raB(i),hhB(i),mmB(i),ssB(i))
         call convertdec(decB(i),sB(i),ddB(i),maB(i),saB(i))
         beamB(i)=bmajB(i)*bminB(i)
         dposB(i)=sqrt((draB(i))**2.d0+(ddecB(i))**2.d0)
310     continue
        close(21)

c! from ratios derive ratio of 65 from both BDSM/SEX and SAD/SEX
        open(unit=21,file='SEXFILE',status='old')
        do 320 i=1,nsrcsex
         read (21,*) raX(i),decX(i),magisocorrX(i),magerrX(i), 
     /        bmajX(i),bminX(i),bpaX(i),dbmajX(i),dbminX(i),dbpaX(i),
     /        fwhmX(i)
         call convertra(raX(i),hhX(i),mmX(i),ssX(i))
         call convertdec(decX(i),sX(i),ddX(i),maXX(i),saX(i))
         peakX(i)=65.0d0*(10.d0**(-0.4d0*magisocorrX(i)))
         dpeakX(i)=65.0d0*(10.d0**(-0.4d0*magerrX(i)))
         bmajX(i)=bmajX(i)*sqrt(8.d0*dlog(2.d0)) ! sigma --> fwhm
         bminX(i)=bminX(i)*sqrt(8.d0*dlog(2.d0)) ! sigma --> fwhm
         bmajX(i)=bmajX(i)*dg
         bminX(i)=bminX(i)*dg
         dbmajX(i)=dbmajX(i)*dg
         dbminX(i)=dbminX(i)*dg
         beamX(i)=bmajX(i)*bminX(i)
         fwhmX(i)=(fwhmX(i)*3600.d0)**2.d0
320     continue
        close(21)

c! now to print out stuff

        filen='listall'
        call getline(filen,dir,indexlistall)
        open(unit=21,file='listall',status='old')
        open(unit=22,file='opfile',status='unknown')
        open(unit=23,file='rat_sad_bdsm',status='unknown')
        open(unit=24,file='rat_sad_sex',status='unknown')
        open(unit=25,file='rat_bdsm_sex',status='unknown')
        do 400 i=1,indexlistall
         read (21,*) elmn(1),elmn(2),elmn(3)
	 l=elmn(order(1))
	 m=elmn(order(2))
	 n=elmn(order(3))
	 
         write (22,'(a14,i4,a11,i4,1x,i4,1x,i4)') ' Listall id : ',i,
     /         ';  list is ',l,m,n
c! write RA and Dec
         write (22,*) ' COORDINATES : '
         if (l.ne.0) then
          write (22,779) '   sad   : ',hhD(l),mmD(l),ssD(l),
     /      draD(l)*dg/15.d0,sD(l),ddD(l),maD(l),saD(l),ddecD(l)*dg
         else
          write (22,'(a11)') '   sad   : '
         end if
         if (m.ne.0) then
          write (22,779) '   bdsm : ',hhB(m),mmB(m),ssB(m),
     /      draB(m)/15.d0,sB(m),ddB(m),maB(m),saB(m),ddecB(m)
         else
          write (22,'(a11)') '   bdsm : '
         end if
         if (n.ne.0) then
          write (22,789) '   sex   : ',hhX(n),mmX(n),ssX(n),sX(n),
     /           ddX(n),maXX(n),saX(n)
         else
          write (22,'(a11)') '   sex   : '
         end if

c! flux densities; commented lines are for erxcluding strong discrepant 
c! srcs which r multiple
         if (l.ne.0.and.m.ne.0) then
	  write (23,*) peakD(l)/peakB(m),integD(l)/integB(m)
c	  if (peakD(l).lt.0.03) write (23,*) peakD(l)*1.d3/peakB(m),
c     /       integD(l)*1.d3/integB(m)
c	  if (peakD(l).ge.0.03.and.((peakD(l)*1.d3/peakB(m)).lt.1.2.
c     /      and.(peakD(l)*1.d3/peakB(m)).gt.0.8))
c     /    write (23,*) peakD(l)*1.d3/peakB(m),integD(l)*1.d3/integB(m)
         end if
         if (l.ne.0.and.n.ne.0) write (24,*) integD(l)*1.d3/peakX(n)
         if (m.ne.0.and.n.ne.0) write (25,*) integB(m)*1.d3/peakX(n)

         write (22,*) ' FLUX DENSITIES : '
         if (l.ne.0) then
          write (22,780) '   sad   : ',peakD(l)*1.d3,dpeakD(l)*1.d3,
     /                         integD(l)*1.d3,dintegD(l)*1.d3
         else
          write (22,'(a11)') '   sad   : '
         end if
         if (m.ne.0) then
          write (22,780) '   bdsm : ',peakB(m)*1.d3,
     /           dpeakB(m)*1.d3,integB(m)*1.d3,dintegB(m)*1.d3
         else
          write (22,'(a11)') '   bdsm : '
         end if
         if (n.ne.0) then
          write (22,781) '   sex   : ',peakX(n),dpeakX(n)
         else
          write (22,'(a11)') '   sex   : '
         end if

c! Beam parameters
         write (22,*) ' SIZE PARAMETERS : '
         if (l.ne.0) then
          write (22,782) '   sad   : ',bmajD(l),bminD(l),bpaD(l),
     /           dbmajD(l),dbminD(l),dbpaD(l)
         else
          write (22,'(a11)') '   sad   : '
         end if
         if (m.ne.0) then
          write (22,782) '   bdsm : ',bmajB(m),bminB(m),bpaB(m)+180.d0
         else
          write (22,'(a11)') '   bdsm : '
         end if
         if (n.ne.0) then
          write (22,782) '   sex   : ',bmajX(n),bminX(n),bpaX(n),
     /           dbmajX(l),dbminX(l),dbpaX(l)
         else
          write (22,'(a11)') '   sex   : '
         end if
         if (l.ne.0) then
          write (22,*) ' DECONVOLVED SIZE : '
          write (22,782) '   sad   : ',sizemajD(l)*dg,sizeminD(l)*dg,
     /       sizepaD(l),dsizemajD(l)*dg,dsizeminD(l)*dg,dsizepaD(i)
         end if
         write (22,*)
400     continue
        close(21)
        close(22)
        close(25)
        close(24)
        close(23)

c! plot stuff with SAD peak flux to see if there is a corr in differences

        open(unit=21,file='listall',status='old')
        do 410 i=1,indexlistall
         read (21,*) lmn(i,1),lmn(i,2),lmn(i,3)
410     continue
        close(21)
	call rearrange(indexlistall,lmn,order,el,em,en)

c        call pgbegin(0,'?',3,3)
        do 610 i=1,9000
         unity(i)=1.d0
610     continue

c! if el(i)=0 and el(i)=0.and.em(i).eq.0 x=peakD*1.d3;y=peakD/peakB*1d3,
c! log,log,win(1,1),labels 
c! ############  PLOT NUMBER 1 ##############################
c! first row is flux ratios
        call callplot(indexlistall,el,el,em,0,peakD,1.d3,peakD,peakB,
     /    1.d0,1,0,1,1,'Peak SAD            ','SAD_peak/BDSM_peak ')
        call callplot(indexlistall,el,el,en,0,peakD,1.d3,integD,peakX,
     /    1.d3,1,0,2,1,'Peak SAD            ','SAD_integ/SEX_peak  ')
        call callplot(indexlistall,el,em,en,0,peakD,1.d3,integB,peakX,
     /    1.d0,1,0,3,1,'Peak SAD            ','BDSM_integ/SEX_peak')

c! next row is beam area ratios
        call callplot(indexlistall,el,el,em,0,peakD,1.d0,beamD,beamB,
     /    1.d0,1,0,1,2,'Peak SAD            ','SAD_beam/BDSM_beam ')
        call callplot(indexlistall,el,el,en,0,peakD,1.d0,beamD,beamX,
     /    1.d0,1,1,2,2,'Peak BDSM          ','SAD_beam/SEX_beam   ')
        call callplot(indexlistall,el,em,en,0,peakD,1.d0,beamB,beamX,
     /    1.d0,1,1,3,2,'Peak SEX            ','BDSM_beam/SEX_beam ')

c! next row is relative distance
        do 600 i=1,indexlistall
         if (el(i).ne.0.and.em(i).ne.0) call justdist(raD(el(i)),
     /      raB(em(i)),decD(el(i)),decB(em(i)),distDB(i))
         if (el(i).ne.0.and.en(i).ne.0) call justdist(raD(el(i)),
     /      raX(en(i)),decD(el(i)),decX(en(i)),distDX(i))
         if (em(i).ne.0.and.en(i).ne.0) call justdist(raB(em(i)),
     /      raX(en(i)),decB(em(i)),decX(en(i)),distBX(i))
         if (distDB(i).eq.0.d0) distDB(i)=1.d-3
         if (distDX(i).eq.0.d0) distDX(i)=1.d-3
         if (distBX(i).eq.0.d0) distBX(i)=1.d-3
600     continue
        call callplot(indexlistall,el,el,em,1,peakD,1.d0,distDB,unity,
     /    1.d0,1,1,1,3,'Peak SAD            ','Dist SAD-BDSM      ')
        call callplot(indexlistall,el,el,en,1,peakD,1.d0,distDX,unity,
     /    1.d0,1,1,2,3,'Peak SAD            ','Dist SAD-SEX        ')
        call callplot(indexlistall,el,em,en,1,peakD,1.d0,distBX,unity,
     /    1.d0,1,1,3,3,'Peak SAD            ','Dist BDSM-SEX      ')
c        call pgend

c        call pgbegin(0,'?',3,3)
c! ############  PLOT NUMBER 2 ##############################
c! first row is 
        call callplot(indexlistall,el,el,em,0,peakD,1.d3,integD,integB,
     /    1.d0,1,0,1,1,'Peak SAD            ','SAD_integ/BDSM_integ')
        call callplot(indexlistall,el,el,el,0,peakD,1.d3,integD,peakD,
     /    1.d0,1,1,2,1,'Peak SAD            ','SAD_Integ/Peak SAD  ')
        call callplot(indexlistall,em,em,em,0,peakB,1.d0,integB,peakB,
     /    1.d0,1,1,3,1,'Peak BDSM          ','Integ/Peak BDSM    ')

c! second row is for tests
        call callplot(indexlistall,el,el,em,0,peakD,1.d3,beamD,beamB,
     /    1.d0,1,0,1,2,'Peak SAD            ','SAD_beam/BDSM_beam ')
        call callplot(indexlistall,el,el,en,0,peakD,1.d3,beamD,fwhmX,
     /    1.d0,1,1,2,2,'Peak SAD            ','SAD_beam/SEX_FWHM   ')
        call callplot(indexlistall,el,em,en,0,peakD,1.d3,beamB,fwhmX,
     /    1.d0,1,1,3,2,'Peak SAD            ','BDSM_beam/SEX_FWHM ')

c! third row is for error/quantity 
        call callplot(indexlistall,el,el,el,0,peakD,1.d3,dpeakD,unity,
     /    1.d0,1,1,1,3,'Peak SAD            ','err_peak SAD        ') 
        call callplot(indexlistall,em,em,em,0,peakB,1.d0,dpeakB,unity,
     /    1.d0,1,1,2,3,'Peak BDSM          ','err_peak BDSM      ')
        call callplot(indexlistall,en,en,en,0,peakX,1.d0,dpeakX,unity
     /   ,1.d0,1,1,3,3,'Peak SEX            ','err_peak SEX        ')

c        call pgend

c        call pgbegin(0,'?',3,3)
c! ############  PLOT NUMBER 3 ##############################
c! first row is 
        call callplot(indexlistall,el,el,em,0,peakD,1.d3,residrmsD,
     /    unity,1.d3,1,1,1,1,'Peak SAD            ',
     /                       'residual rms        ')
        call callplot(indexlistall,el,el,em,0,respeakD,1.d3,residrmsD,
     /    unity,1.d3,1,1,1,2,'residual peak SAD   ',
     /                       'residual rms        ')
        call callplot(indexlistall,el,el,el,0,peakD,1.d3,integD,peakD,
     /    1.d0,1,0,1,3,'peak SAD            ','Integ/peak SAD      ')

        call callplot(indexlistall,el,el,el,0,peakD,1.d3,draD,raD,
     /    dg,1,1,2,1,'Peak SAD            ','err_RA/RA SAD        ')
        call callplot(indexlistall,el,el,el,0,peakD,1.d3,ddecD,decD,
     /    dg,1,1,2,2,'Peak SAD            ','err_Dec/Dec SAD      ')
        call callplot(indexlistall,em,em,em,0,peakB,1.d0,integB,peakB,
     /    1.d0,1,0,2,3,'peak BDSM          ','Integ/peak BDSM    ')

        call callplot(indexlistall,el,em,em,0,peakD,1.d3,draB,raB,
     /    dg,1,1,3,1,'Peak SAD            ','err_RA/RA SAD        ')
        call callplot(indexlistall,el,em,em,0,peakD,1.d3,ddecB,decB,
     /    dg,1,1,3,2,'Peak SAD            ','err_Dec/Dec SAD      ')

c        call pgend

	
c        call pgbegin(0,'?',1,1)
        call callplot(indexlistall,el,el,em,0,peakD,1.d0,beamD,beamB,
     /    1.d0,1,0,1,1,'Peak flux (mJy) SAD ','Source size SAD/BDSM')
c        call callplot(indexlistall,el,el,em,0,peakD,1.d3,peakD,peakB,
c     /    1.d3,1,0,1,1,'Peak flux (mJy) SAD ','Peak SAD/BDSM      ')
c        call callplot(indexlistall,el,el,en,0,peakD,1.d3,peakD,peakX,
c     /    1.d3,1,0,1,2,'Peak flux (mJy) SAD ','Peak SAD/SExtractor ')
c        call pgend




779     format(a11,i2,1x,i2,1x,f6.3,1x,'(',f5.3,')  ',a1,i2,1x,i2,1x,
     /         f5.2,' (',f5.3,')')
789     format(a11,i2,1x,i2,1x,f6.3,10x,a1,i2,1x,i2,1x,f5.2)
780     format(a11,'Peak = ',f7.3,' +/- ',f7.3,' mJy;  Integ = ',
     /         f7.3,' +/- ',f7.3,' mJy')
781     format(a11,'Peak = ',f7.3,' +/- ',f7.3,' mJy;  Integ = ',
     /         f7.3,' +/- ',f7.3,' mJy')
782     format(a11,f5.2,'" X ',f5.2,'" X ',f6.1,' ; Err = ',
     /         f5.2,'" X ',f5.2,'" X ',f6.1)
        filen1='rat_sad_bdsm'
        write (*,*)
        call calcstat(filen1,mean,rms,1)
        write (*,'(a34,f6.3,a9,f6.3,$)') ' Ratio of SAD/BDSM flux : '//
     /        'Mean = ',mean,' & RMS = ',rms
        call calcstat(filen1,mean,rms,2)
        write (*,'(a16,f6.3,a9,f6.3)') '; Integ ratio = '
     /              ,mean,' & RMS = ',rms
        filen1='rat_sad_sex'
        call calcstat(filen1,mean,rms,1)
        write (*,'(a34,f6.3,a9,f6.3)') ' Ratio of SAD/SEX   flux : '//
     /        'Mean = ',mean,' & RMS = ',rms
        filen1='rat_bdsm_sex'
        call calcstat(filen1,mean,rms,1)
        write (*,'(a34,f6.3,a9,f6.3)') ' Ratio of BDSM/SEX flux : '//
     /        'Mean = ',mean,' & RMS = ',rms
        write (*,*)
        

        end


c ------------------------------------------------------------------------------------------ c
c
c                                ## SUBROUTINES ##
c
c ------------------------------------------------------------------------------------------ c

        subroutine sex2star(filen,nn)
        implicit none
        character filen*500,s*1
        integer nn,hh,mm,dd,ma,i
        real*8 ra,dec,ss,sa,dumr,fwhm

        call system('rm -f SEXSTAR')
        open(unit=21,file=filen,status='old')
        open(unit=22,file='SEXSTAR',status='unknown')
        do 100 i=1,nn
         read (21,*) ra,dec,dumr,dumr,dumr,dumr,dumr,dumr,dumr,dumr,fwhm
         call convertra(ra,hh,mm,ss)
         call convertdec(dec,s,dd,ma,sa)
         write (22,777) hh,mm,ss,s,dd,ma,sa,fwhm*3600.d0,fwhm*3600.d0
100     continue        
        close(21)
        close(22)
777     format(i2,1x,i2,1x,f5.2,1x,a1,i2,1x,i2,1x,f5.2,1x,f5.2,1x,
     /         f5.2,' 0 22')

        return
        end

c ------------------------------------------------------------------------------------------ c

        subroutine makesadcat(nsrc)
        implicit none
        integer num,nsrc,i,dumi,hh,mm,dd,ma
        real*8 dumr1,dumr2,dumr3,dumr4,dumr5,ara
        real*8 peak(9000),integ(9000),ra(9000),dec(9000),bmaj(9000)
        real*8 bmin(9000),bpa(9000)
        real*8 dpeak(9000),dinteg(9000),dra(9000),ddec(9000),dbmaj(9000)
        real*8 dbmin(9000),dbpa(9000)
        real*8 sizemaj(9000),sizemin(9000),sizepa(9000)
        real*8 d1sizemaj(9000),d1sizemin(9000),d1sizepa(9000)
        real*8 d2sizemaj(9000),d2sizemin(9000),d2sizepa(9000)
        real*8 dsizemaj(9000),dsizemin(9000),dsizepa(9000)
        real*8 residrms(9000),respeak(9000),resint(9000)
        real*8 dumr,ss,sa
        character filen*500,s*1,command*300,dir*500

        call system('rm -f a SADFILE sad_cat')
        call system('grep "E[+-]" MFTABLE > a')
        filen='a'
        dir='./'
        call getline(filen,dir,num)
        if ((num/nsrc).ne.int(num/nsrc)) then
         write (*,*) ' *******************'
         write (*,*) ' ***** PROBLEM *****'
         write (*,*) ' *******************'
c!         pause
        end if

c! read the parameters from MFTABLE
        open(unit=21,file='a',status='old')
        do 110 i=1,nsrc
         read (21,*) dumr,dumr,peak(i),integ(i),dumr,dumr,
     /               bmaj(i),bmin(i),bpa(i)
110     continue        
        do 120 i=1,nsrc
         read (21,*) dumr,dpeak(i),dinteg(i),dra(i),
     /               ddec(i),dbmaj(i),dbmin(i),dbpa(i)
120     continue        
        do 130 i=1,nsrc
         read (21,*) dumr,sizemaj(i),sizemin(i),sizepa(i),d1sizemaj(i),
     /           d1sizemin(i),d1sizepa(i),d2sizemaj(i),d2sizemin(i),
     /           d2sizepa(i),residrms(i),respeak(i)
         dsizemaj(i)=0.5d0*(d1sizemaj(i)+d2sizemaj(i))
         dsizemin(i)=0.5d0*(d1sizemaj(i)+d2sizemin(i))
         dsizepa(i)=0.5d0*(d1sizepa(i)+d2sizepa(i))
130     continue        
        do 150 i=1,nsrc
         read (21,*) resint(i)
150     continue        
        close(21)

c! sort and make workfile
        open(unit=21,file='SADSTAR1',status='old')
        do 285 i=1,5
         read (21,*) 
285     continue
        do 280 i=1,nsrc
         read (21,*) dumi,ra(i),dec(i)
280     continue
        close(21)
        open(unit=21,file='SADFILE',status='unknown')
        do 300 i=1,nsrc
         write (21,'(24(e14.8,2x))') ra(i),dec(i),peak(i),integ(i),
     /            bmaj(i),bmin(i),bpa(i),dpeak(i),dinteg(i),dra(i),
     /      ddec(i),dbmaj(i),dbmin(i),dbpa(i),sizemaj(i),
     /      sizemin(i),sizepa(i),dsizemaj(i),dsizemin(i),dsizepa(i),
     /      residrms(i),respeak(i),resint(i),ara
300        continue
        close(21)
        call system('sort -r -g SADFILE > a')
        call system('mv a SADFILE')

c! make cat and star file
        command="grep E+ SADSTAR1 | "//
     /          "awk \'{print $2,$3,$4,$5,$6,$7,$8}\' >a"
        call system(command)
        call system('sort -g -r a > SADSTAR2')
        call system('rm -f sad_cat SADSTAR')
        open(unit=21,file='SADSTAR2',status='old')
        open(unit=22,file='sad_cat',status='unknown')
        open(unit=23,file='SADSTAR',status='unknown')
        do 200 i=1,nsrc
         read (21,*) ra(i),dec(i),dumr1,dumr2,dumr3,dumr4,dumr5
         write (22,*) ra(i),dec(i)
         call convertra(ra(i),hh,mm,ss)
         call convertdec(dec(i),s,dd,ma,sa)
         write (23,778) hh,mm,ss,s,dd,ma,sa,dumr1*3600.d0,dumr2*3600.d0,
     /                  dumr3,int(dumr4),dumr5
200     continue
        close(21)
        close(22)
        close(23)
        call system('rm -f SADSTAR2')
778     format(i2,1x,i2,1x,f5.2,1x,a1,i2,1x,i2,1x,f5.2,1x,f5.2,1x,
     /         f5.2,1x,f5.1,i2,1x,f8.5)

        return
        end

c ------------------------------------------------------------------------------------------ c

        subroutine makebdsmcat(ngau)
        implicit none 
        integer nchar,ngau
        real*8 dumr
        character command*500,gname*500,dumc*500,str*10
        character filen*500,dir*500

        call system('rm -f a BDSMFILE bdsm_cat')
 
c! this is to write out ra, dec
        gname='/data/niruj_pers/image/BIGWENSS.trial.gaul'
        open(unit=21,file=gname(1:nchar(gname)),form='formatted')
        call readhead_srclist(21,19,'Number_of_gaussians',dumc,ngau,
     /       dumr,'i')
        close(21)
        call int2str(ngau,str)
        command="tail -"//str(1:nchar(str))//" "//gname(1:nchar(gname))
     /      //" | awk '{ if ($3==0) print $8, $10}' | "
     /      //"sort -r -g > bdsm_cat"
        call system(command)

c! write out all relevant paras
        command="tail -"//str(1:nchar(str))//" "//gname(1:nchar(gname))
     /   //"|awk '{ if ($3==0) "
     /   //"print $8,$9,$10,$11,$4,$5,$6,$7,$16,$17,$18,$19,$20,"
     /   //"$21,$22,$23,$24,$25,$26,$27,$30}' | sort -r -g > BDSMFILE"
        call system(command)
        dir="./"
        filen="BDSMFILE"
        call getline(filen,dir,ngau)

        return
        end

c ------------------------------------------------------------------------------------------ c
        subroutine makesexcat(nsrcsex) 
        implicit none
        integer nsrcsex

        call system('rm -f a b SEXFILE sex_cat')
        call system("sed -e '1,11d' sex.cat > a")
        call system("awk '{$1=\"\"};{print}' a > b")
        call system('sort -r -g b > a')
        call system("awk '{print $1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11}' 
     /        a > SEXFILE")
        call system("awk '{print $1,$2}' a > sex_cat")
        call system('rm -f a b')

        return
        end

c ------------------------------------------------------------------------------------------ c

        subroutine arrange(nsad,nfind,nsex,n1,n2,n3,f1,f2,f3,order)
        implicit none
        integer nsad,nfind,nsex,n1,n2,n3,narr(3),order(3)
        character f1*10,f2*10,f3*10
        character fname(3)*10

        data fname/'sad_cat','bdsm_cat','sex_cat'/
        narr(1)=nsad
        narr(2)=nfind
        narr(3)=nsex
	order(1)=1
	order(2)=2
	order(3)=3

        if (narr(3).gt.narr(2)) then
         call xchang(narr,fname,3,2)
	 call xchang1(order,3,2)
        end if
        if (narr(2).gt.narr(1)) then
         call xchang(narr,fname,2,1)
	 call xchang1(order,2,1)
        end if
        if (narr(3).gt.narr(2)) then
         call xchang(narr,fname,3,2)
	 call xchang1(order,3,2)
        end if

        n1=narr(1)
        n2=narr(2)
        n3=narr(3)
        f1=fname(1)
        f2=fname(2)
        f3=fname(3)

        return
        end

c ------------------------------------------------------------------------------------------ c

        subroutine xchang(arr,fn,i,j)
        implicit none
        integer arr(3),i,j,dumi
        character fn(3)*10,dumf*10

        dumi=arr(i)
        arr(i)=arr(j)
        arr(j)=dumi
        dumf=fn(i)
        fn(i)=fn(j)
        fn(j)=dumf

        return
        end

c ------------------------------------------------------------------------------------------ c

	subroutine xchang1(or,i,j)
	implicit none
	integer or(3),i,j,dumi

	dumi=or(i)
	or(i)=or(j)
	or(j)=dumi

	return
	end

c ------------------------------------------------------------------------------------------ c
c! convert from list1,list2,list3 to sad, bdsm, sex
	subroutine rearrange(num,lmn,order,el,em,en)
	implicit none
	integer order(3),lmn(9000,3),el(9000),em(9000),en(9000),num,i

	do 100 i=1,num
         el(i)=lmn(i,order(1))
         em(i)=lmn(i,order(2))
         en(i)=lmn(i,order(3))
100     continue

	return
	end

c ------------------------------------------------------------------------------------------ c

c! nn number of column to take (peak and intreg for sad/bdsm
        subroutine calcstat(f,mean,rms,nn)
        implicit none
        character f*30
        real*8 mean,rms,dumr(2)
        integer i,n,nchar,nn

        call system('rm -f b')
        call system('wc -l '//f(1:nchar(f))//' > b')
        open(unit=26,file='b',status='old')
         read (26,*) n
        close(26)
        
        mean=0
        rms=0
        open(unit=26,file=f,status='old')
         do 100 i=1,n
          if (nn.eq.2) read (26,*) dumr(1),dumr(2)
          if (nn.eq.1) read (26,*) dumr(1)
          mean=mean+dumr(nn)
100      continue
        close(26)
        mean=mean/n

        open(unit=26,file=f,status='old')
         do 110 i=1,n
          if (nn.eq.2) read (26,*) dumr(1),dumr(2)
          if (nn.eq.1) read (26,*) dumr(1)
          rms=rms+(dumr(nn)-mean)*(dumr(nn)-mean)
110      continue
        close(26)
        rms=sqrt(rms/n)
        call system('rm -f b')

        return
        end

c ------------------------------------------------------------------------------------------ c
c! is cindex=0, then xv,ynum,yden is (cN(i)) and if=1, then of (np)
        subroutine callplot(listn,c1,c2,c3,cindex,xv,f1,ynum,yden,f2,
     /             l1,l2,p1,p2,xl,yl)
        implicit none
        integer c1(9000),c2(9000),c3(9000),np,l1,l2,p1,p2,listn,i
        integer cindex
        real*8 xv(9000),ynum(9000),yden(9000),f1,f2
        character xl*20,yl*21
        real*4 x4(9000),y4(9000)

        np=0
        do 420 i=1,listn
         if (c1(i).ne.0) then
          if (c2(i).ne.0.and.c3(i).ne.0) then
	   if (yden(c3(i)).eq.0.0.or.(l2.eq.1.and.
     /         ynum(c2(i)).eq.0.0)) then
	    write (*,*) ' Dividing by zero; flagged'
	   else
            np=np+1 
            if (l1.eq.1) x4(np)=dlog10(xv(c1(i))*f1)
            if (l1.eq.0) x4(np)=xv(c1(i))*f1
            if (cindex.eq.0) then
             if (l2.eq.1) y4(np)=dlog10(ynum(c2(i))*f2/yden(c3(i)))
             if (l2.eq.0) y4(np)=ynum(c2(i))*f2/yden(c3(i))
            else
             if (l2.eq.1) y4(np)=dlog10(ynum(i)*f2/yden(i))
             if (l2.eq.0) y4(np)=ynum(i)*f2/yden(i)
            end if
	   end if
          end if
         end if
420     continue
c        call pgplott(x4,y4,np,l1,l2,p1,p2,xl,yl) ! x,y,log,log,win(1,1)

        return
        end

        
c ------------------------------------------------------------------------------------------ c

        subroutine pgplott(x,y,np,l1,l2,n,m,xlab,ylab)
        implicit none
        real*4 x(9000),y(9000),mnx,mny,mxx,mxy
        integer np,l1,l2,n,m,nchar,i
        character xlab*20,ylab*21,xb*6,yb*6

c        call pgpanl(n,m)
c	call pgslw(3)
c	call pgsch(2.1)
c        call pgvport(0.15,0.9,0.15,0.9)
        call range2(x,9000,np,mnx,mxx)
        call range2(y,9000,np,mny,mxy)
c! @@@@@@@@@@@@@@@@@@@@@@@@
        mny=0.4
        mxy=2.0
c! @@@@@@@@@@@@@@@@@@@@@@@@
c        call pgwindow(mnx,mxx,mny,mxy)
cc	call pglabel(xlab(1:nchar(xlab)),ylab(1:nchar(ylab)),' ')
c	call pglabel(xlab,ylab,' ')
cc        if (m.eq.1) call pgwindow(mnx,mxx,0.7,1.3)
cc        if (m.eq.2) call pgwindow(mnx,mxx,0.0,2.0)
cc	if (m.eq.1) call pglabel(' ',ylab(1:nchar(ylab)),' ')
cc	if (m.eq.2) call pglabel(xlab(1:nchar(xlab)),ylab(1:nchar(ylab)),' ')
cc        if (m.eq.1) call pgtext(0.6,1.35,
 
        xb='BCNST'
        yb='BCNST'
        if (l1.eq.1) xb='BCNSTL' 
        if (l2.eq.1) yb='BCNSTL' 
c        call pgbox(xb(1:nchar(xb)),0.0,0,yb(1:nchar(yb)),0.0,0)
c        call pgsch(1.0)
c        call pgpoint(np,x,y,3)

        return
        end

c ------------------------------------------------------------------------------------------ c



