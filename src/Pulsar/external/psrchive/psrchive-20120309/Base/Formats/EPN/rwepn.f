C version 16/08/2002 fixing bug that prevents more than one subband per pol
c version 12/09/2002 problem with format 77/78 when freq was > 1000 MHz
c                    changed from f12.8 to f13.7
c==============================================================================
c
c     File: "rwepn.f"
c     
c     This file contains the generalised EPN format reading/writing routines
c     It has been tested on an HP system with f77 -c rwepn.f to produce the 
c     object file. It requires the accompanying include file "epnhdr.inc"
c
c==============================================================================
       subroutine rwepn(filename,readwri,recno,padout)
c==============================================================================
c
c     Subroutine to read or write data in EPN format. All parameters are
c     defined in the include file "epnhdr.inc" which should lie in the
c     same directory as this file. Passed down variables to this routine
c     are:
c
c     filename - character*80  - name of this EPN file
c     readwri  - integer*4     - switch: -1 => read, +1 => write
c                                        +2 => write sequence
c     recno    - integer*4     - record number to read
c
c     Usage:
c
c     To incorporate this routine into your pulsar data reduction
c     package, include the file "epnhdr.inc" in your source code
c     and set up the appropriate variables before.
c
c     Features:
c
c     When writing is requested, the routine will keep the file
c     open for successive writes by default. To override this, pass
c     down recno as -1, the presently opened file will be closed
c     and a new one opened.
c
c     If an error occurs in reading, recno is returned as -999
c
c     By the very nature of the format, record lengths must be fixed.
c     So if you want to have different lengthed data streams in the
c     same archive you need to switch "padout" on. As its name suggests,
c     this pads out the binned data to its maximum length so that
c     the records are fixed. A draw back is of course that it uses
c     up more space. Therefore if it is known that all the records
c     are of the same lenght anyway (eg for a stream of single pulses)
c     then it is recommended to switch padout off.
c
c==============================================================================
c
c     Created by Dunc Lorimer (dunc@mpifr-bonn.mpg.de) November 1996....
c
c     12/11/1996   Initial version
c     15/11/1996   Implemented common blocks
c     21/05/1997   Changed to HEX storage
c     01/07/1997   Added tres and fluxflag to header
c     12/11/2000   Added readwri +2 for single pulses
c                  New version 6.1!
c
c
c==============================================================================
c
      implicit none
      include 'epnhdr.inc'
c
c     Data buffers...
c
      integer*4 intdata(maxbin) 
      character*4 chrdata(maxbin)
      real*4 tmp(maxbin)
c
c     The maximum length of an EPN file is the 6 line header, plus
c     maxblk sets of maxbin bin data streams. Each stream of maxbin
c     bins occupies (maxbin/20) sets of 80 character lines plus a 2
c     line header. The max number of charcters per record is:
c
c     (6+maxblk*(maxbin/20+2))*80 
c
c     One can tune this depending on the size of the data-sets...
c     the default is maxblk=8, maxbin=4160 -> maxrec=66080
c     suitable smaller setting for e.g. a PC would be...
c                    maxblk=4, maxbin=1040 -> maxrec=17760.
c     N.B. Both maxbin and maxblk are specified in "epnhdr.inc"
c
      integer maxrec
      parameter(maxrec=(6+maxblk*(maxbin/20+2))*80)
      character line(maxrec/80)*80, recrd*(maxrec)
c
c==============================================================================
c
c     Local variables
c
      integer lun, lfil, nlin, next, i, j, k, iras, ipol,
     &        wrecno, recln, maxint, lct
      real rras, pvno, vno
      character*1 dsign, lin1*12, line40*40, uprow*3,dumaris*4
      logical reading,writing,filex,opened,first

      save 

      data opened/.false./, first/.true./
      uprow=char(27)//'[A'
      line40='----------------------------------------'
c
c==============================================================================
c
c     Sort out whether reading or writing
c
c      write(*,*) 'READWRITE flag: ',readwri, recno,opened

      reading=(readwri.eq.-1)
      writing=(.not.reading)
      if (readwri.ne.-1.and.readwri.ne.1.and.readwri.ne.2) then
         write(*,'(''Unknown read/write option passed to rwepn!!'')')
         write(*,'(''Your "readwri" integer reads: '',i4)')readwri
         stop
      endif
c
c     If writing required and recno=-1, then close a previously
c     opened file (if one is open) and proceed
c
      if (writing.and.recno.eq.-1.and.opened) then
         close(unit=lun)
         opened=.false.
      endif
c
c     Get a free logical unit number
c
      if (.not.opened) then 
         call glun(lun)
c         write (*,*) 'CALLING GLUN AGAIN!'
      end if

c
c     Check whether the file exists or not....
c     
c     For reading: the routine will stop if not
c     For writing: the routine will append to the previous file
c
      lfil=index(filename,' ')-1
      inquire(file=filename(1:lfil),exist=filex)
c      write (*,*) '>',filename,'<',lfil
c
c     Open the input file
c
      if (reading) then
         if (filex) then
c
c          Read in the very first line, and work out the
c          required record length...(number of 80 chr lines)
c     
           open(unit=lun,file=filename(1:lfil),status='old',
     &          access='direct',form='unformatted',recl=12)
           read(lun,rec=1) lin1
           read(lin1,'(8x,i4)') counter
           close(unit=lun)
           recln=counter*80
c           write(*,*) 'opening again...', counter, recln
            
c
c          Now open as a direct access unformatted file
c
           open(unit=lun,file=filename(1:lfil),status='old',
     &          access='direct',form='unformatted',recl=recln)
           if (recno.lt.1) recno=1
           read(lun,rec=recno,err=999) recrd(1:recln)
           close(unit=lun)
         else
           stop 'EPN file does not exist!'
         endif
      endif
      if (writing) then
c
c       Establish out how many 'lines' will make up this record...
c
        if (padout) then
           recln=maxrec ! use maximum record length
           nlin=maxbin/20
           npol=maxblk
           counter=6+npol*(nlin+2)
        else
           nlin=nbin/20
           next=nbin-nlin*20
           if (next.gt.0) nlin=nlin+1
           counter=6+npol*nfreq*(nlin+2) ! changed by adding nfreq factor
           recln=counter*80
c           write (*,*)counter,recln,npol,nlin
        endif

c        write(*,*) 'EPN open status: ', opened

        if (opened) then
c
c          Increase the internal record counter
c
           wrecno=wrecno+1
        else
c
c          Reset the internal record counter...
c
           if (readwri.eq.1)  wrecno=1
           if (readwri.eq.2) wrecno=recno
           if (wrecno.lt.1) wrecno=1
           recno=wrecno
           
c           write (*,'(a3,i5.5)') uprow,wrecno

c           write(*,*)
c           write(*,*) 'WRECNO: ',wrecno

           open(unit=lun,file=filename(1:lfil),status='unknown',
     &          access='direct',form='unformatted',recl=recln)
           opened=.true.

c           write(*,*) 'OPENED after opening: ',opened
        endif
      end if
c
c     As from Version 5.0 data is scaled into hex format...
c     This gives an increas in dynamic range by over a factor 6
c
      pvno=5.0
      maxint=65535
c
c     As from Version 6.0, actual resolution of the data is
c     recorded, the factor fcal is no longer recorded, instead
c     the flag fluxflag is written into line 4 to signify whether
c     the data are flux calibrated or not. (1.7.1997)
c
      pvno=6.0
c
c     As from Version 6.1, EPN files can contain up to
c     99999 records rather than the previous 9999. 
c     
c     (12.11.2000 Aris Karastergiou)
c
      pvno=6.1
      lct=1
c
c     Line 1
c
      if (reading) then
        read(recrd((lct-1)*80+1:lct*80),11) version, counter, history
        read(version(4:),'(f5.2)') vno
        if (vno.lt.pvno) then
           if (first) then
c     write(*,*) 'Reading an old EPN file - Version',vno
              first=.false.
           endif
           maxint=9999
        endif
      else
        vno=pvno
        write(version,'(a3,f5.2)') 'EPN',vno
        write(line(lct),11) version, counter, history
      endif
c
c     Un-comment this line for tracing...
c      write(*,11) version, counter, history
c
 11   format(a8,i4,a68)
c
c     Line 2
c
      lct=lct+1
      if (reading) then
        read(recrd((lct-1)*80+1:lct*80),22) 
     &  jname,cname,pbar,dm,rm,catref,bibref
      else
        write(line(lct),22) jname, cname, pbar, dm, rm, catref, bibref
      endif
c
c     Un-comment this line for tracing...
c      write(*,22) jname, cname, pbar, dm, rm, catref, bibref
c
 22   format(a12,a12,f16.12,f8.3,f10.3,a6,a8,8x)
c
c     Line 3
c
      lct=lct+1
      if (reading) then
        read(recrd((lct-1)*80+1:lct*80),33) 
     &               rah,ram,iras,rras,dsign,ded,dem,des, 
     &               telname, epoch, opos, paflag, timflag
        ras=float(iras)+rras
        if (dsign.eq.'-') ded=-1*ded
        if (dsign.eq.'-'.and.ded.eq.0) then
           dem=-1*dem
           des=-1.0*des
        endif
      else
        rras=ras-int(ras)
        iras=int(ras)
        dsign='+'
        if (index(jname,'-').gt.0) dsign='-'
        if (ded.lt.0) ded=-1*ded
        write(line(lct),33) rah, ram, iras, rras, dsign, ded, dem, des, 
     &               telname, epoch, opos, paflag, timflag
      endif
c
c     Un-comment this line for tracing... 
c      write(*,33) rah, ram, iras, rras, dsign, ded, dem, des, telname, 
c     &           epoch, opos, paflag, timflag
c
 33    format(3i2.2,f4.3,a1,2i2.2,f6.3,a8,f10.3,f8.3,a1,a1,31x)
c
c     Line 4 - (X,Y,Z) of telescope - NEW - implemented in V 6.0
c
      if (vno.ge.6.0) then
        lct=lct+1
        if (reading) then
           read(recrd((lct-1)*80+1:lct*80),44) xtel,ytel,ztel
        else
           write(line(lct),44) xtel,ytel,ztel
        endif
      endif
 44   format(3f17.5,29x) 
c
c     Line 5
c
      lct=lct+1
      if (reading) then
         if (vno.ge.6.1) then
            read(recrd((lct-1)*80+1:lct*80),57) 
     &           cdd,cdm,cdy,scanno,subscan,npol,nfreq, 
     &           nbin, tbin, tres, nint, ncal, lcal, fluxflag
         else
            if (vno.eq.6.0) then
               read(recrd((lct-1)*80+1:lct*80),55) 
     &              cdd,cdm,cdy,scanno,dumaris,npol,nfreq, 
     &              nbin, tbin, tres, nint, ncal, lcal, fluxflag
            else
               read(recrd((lct-1)*80+1:lct*80),56) 
     &              cdd,cdm,cdy,scanno,subscan,npol,nfreq, 
     &              nbin, tbin, nint, ncal, lcal, fcal
            endif
         endif
      else
       write(line(lct),57) cdd, cdm, cdy, scanno, subscan, npol, nfreq, 
     &        nbin, tbin, tres, nint, ncal, lcal, fluxflag
      endif
c
c     Un-comment this line for tracing...
c      write(*,57) cdd, cdm, cdy, scanno, subscan, npol, nfreq, nbin,
c     &             tbin, tres, nint, ncal, lcal, fluxflag
c      write(*,56) cdd, cdm, cdy, scanno, subscan, npol, nfreq, nbin,
c     &             tbin, nint, ncal, lcal, fcal
c
 55   format(2i2.2,i4.4,i4.4,a4,i2,i4,i4,2f12.6,i6,i4,i4,a1,14x) !v>6.0
 56   format(2i2.2,i4.4,2i4.4,i2,i4,i4,f12.6,i6,i4,i4,f8.3,20x)
 57   format(2i2.2,i4.4,i4.4,i5.5,i2,i4,i4,2f12.6,i6,i4,i4,a1,14x) !v=6.1
c
c     Line 6 - Blank at the moment
c
      if (vno.ge.6.0) then
        lct=lct+1
        write(line(lct),66) line40,line40
      endif
 66   format(2a40) 
c      
c     Loop around each polarisation...
c

      do ipol=1,npol*nfreq ! multiplied by nfreq
c
c       sub-header line 1
c
        lct=lct+1

c         write(*,*) 'reading: ',ipol, lct

        if (reading) then
          if (vno.ge.6.0) then
          read(recrd((lct-1)*80+1:lct*80),77) idfield(ipol),nband(ipol),
     &    navg(ipol),f0(ipol),f0u(ipol),df(ipol),dfu(ipol),tstart(ipol)
          
          else
          f0u(ipol)=' GHz'
          dfu(ipol)=' MHz'
          read(recrd((lct-1)*80+1:lct*80),78) idfield(ipol),nband(ipol),
     &    navg(ipol),f0(ipol),df(ipol),tstart(ipol)
          endif
        else
          if (f0u(ipol).eq.' ') f0u(ipol)=' GHz'  ! Default units
          if (dfu(ipol).eq.' ') dfu(ipol)=' MHz'  ! Default units
          write(line(lct),77) idfield(ipol),nband(ipol),
     &    navg(ipol),f0(ipol),f0u(ipol),df(ipol),dfu(ipol),tstart(ipol)
        endif

c
c       Un-comment this line for tracing...
c       write(*,77) idfield(ipol),nband(ipol),navg(ipol),f0(ipol), 
c     &              f0u(ipol),df(ipol),dfu(ipol),tstart(ipol)

c       write(*,78) idfield(ipol),nband(ipol),navg(ipol),f0(ipol), 
c     &              df(ipol),tstart(ipol)
c
 77     format(a8,i4,i4,f12.7,a8,f12.6,a8,f17.5,7x)
 78     format(a8,i4,i4,f12.7,f12.6,f17.5,23x)


        if (writing) then 
c
c         Zero the integer and character arrays
c
          do i=1,maxbin
            intdata(i)=0
	    chrdata(i)='0000'
          enddo
c
c         Scale the data
c     
          do k=1,nbin
             tmp(k)=rawdata(ipol,k)
          enddo
          call rawtfint(tmp,nbin,intdata,
     &         scale(ipol),offset(ipol),rms(ipol),readwri,maxint)
c
c         Convert to a hex string
c
          do k=1,nbin
             call b102hex(intdata(k),chrdata(k))
          enddo
        endif
c
c       Sub-header line 2
c
        lct=lct+1
        if (reading) then
          read(recrd((lct-1)*80+1:lct*80),88)  scale(ipol),offset(ipol),
     &    rms(ipol),papp(ipol)
        else
          write(line(lct),88) scale(ipol),offset(ipol),
     &                            rms(ipol),papp(ipol)
        endif
c
c       Un-comment this line for tracing...
c        write(*,88) scale(ipol), offset(ipol), rms(ipol), papp(ipol)
c
 88     format(3e12.6,f16.12,28x)
c
c       Now we finally get to the data!!
c
c
c       Read the data (20 i4s per 80 column line)
c
        nlin=(counter-4)/npol/nfreq-2
        if (vno.ge.6.0) nlin=(counter-6)/npol/nfreq-2
        k=1
        do i=1,nlin
          lct=lct+1
          if (reading) then
            read(recrd((lct-1)*80+1:lct*80),99)(chrdata(j),j=k,k+19)
          else
            write(line(lct),99)(chrdata(j),j=k,k+19)
          endif
c
c         Un-comment this line for tracing...
c          write(*,99)(chrdata(j),j=k,k+19)
c
          k=k+20
        enddo
c
c       Format for the data
c
 99     format(20a4)
        if (reading) then
c
c         Convert hex string to integer for versions from 5.0
c
          do k=1,nbin
             if (vno.ge.5.0) then
                call hex2b10(chrdata(k),intdata(k))
             else
                read(chrdata(k),'(i4.4)') intdata(k)
             endif
          enddo
c
c         De-scale the data
c     
          call rawtfint(tmp,nbin,intdata,
     &         scale(ipol),offset(ipol),rms(ipol),readwri,maxint)
          do k=1,nbin
             rawdata(ipol,k)=tmp(k)
          enddo
        endif
      enddo
c
c     Write the record...
c
      if (writing) then
        do i=1,lct
          j=(i-1)*80+1
          k=j+79
          recrd(j:k)=line(i)
        enddo
c        write (*,*) wrecno
        write(lun,rec=wrecno) recrd(1:k)
        if (readwri.eq.2) then
           close(lun)
           opened=.false.
        end if
c        if (opened) stop
        return
      endif
c
c     Bail out!
c
      return
c
c     On error return this record number
c
 999  recno=-999
      end
c==============================================================================
      subroutine rawtfint(raw,nbin,intdata,scale,offset,rms,dirn,maxint)
c==============================================================================
c
c     Routine to convert "raw" (i.e. floating point) to/from integer
c     format. In this case, the integer may vary between 0 and maxint.
c     Raw data is passed down from site software and stored as integers
c     in the EPN files.
c
c     The direction of the conversion is controlled by "dirn":
c
c     dirn = +1 : raw -> int
c     dirn = -1 : int -> raw
c
c==============================================================================
c    
      implicit none
c
c     Passed down variables
c
      real raw(*)
      real*8 scale, offset, rms
      integer nbin, intdata(*), dirn, maxint
c
c     Local variables
c
      real dmin, dmax, sumsq, pmax, sum, mean
      integer i, ibmax, nshift
      if (dirn.eq.1.or.dirn.eq.2) then
c
c       Scale the data:
c
c       Find minimum & maximum values of the data
c
        dmin = raw(1)
        dmax = raw(1)
        do i=1,nbin
          dmax=max(raw(i),dmax)
          dmin=min(raw(i),dmin)
        enddo
        offset=dmin
        scale=float(maxint)/(dmax-dmin)
c
c       Now do the scaling
c
        do i=1,nbin
          intdata(i)=int((raw(i)-offset)*scale)
        enddo
c
c-------------------------------------------------------------
c       If no rms has been supplied by the user, have a go at 
c       calculating it from the wings of the profile after 
c       shifting it so that its peak lies at bin number nbin/2
c-------------------------------------------------------------
c
        if (rms.eq.0.0) then
          pmax=-1.0e32
          ibmax=0
          do i=1,nbin
             if (raw(i).gt.pmax) then
                ibmax=i
                pmax=raw(i)
             endif
          enddo
          nshift=nbin/2-ibmax
          call shift_prof(raw,nbin,nshift)
          sumsq=0.0
          sum=0.0
          do i=1,nbin/15
            sumsq=sumsq+raw(i)*raw(i)
            sumsq=sumsq+raw(nbin-i+1)*raw(nbin-i+1)
            sum=sum+raw(i)
            sum=sum+raw(nbin-i+1)
          enddo
          rms=sqrt(sumsq/float(2*nbin/15))
          mean=sum/float(2*nbin/15)
        endif
      else if (dirn.eq.-1) then
c
c       Prepare to de-scale the data:
c
        do i=1,nbin
          raw(i)=real(offset+dble(intdata(i))/scale)
        enddo
      else
c
c       Silly option was passed down...
c
        write(*,*) 'Invalid value for dirn passed to rawtfint : ',dirn
        stop
      endif
c
c     Job done!
c
      end
c==============================================================================
      integer function nepnrec(filename)
c==============================================================================
c
c     Function to find out the number of records in an EPN file.
c
c     This can in principle be done with an INQUIRE statement but
c     it is system dependent. The following approach uses a simple
c     binary search algorithm to find the last record which is far
c     quicker than reading in every record in very large EPN files.
c
c     if the file doesn't exist nepnrec is returned as 0
c     nepnrec = -1 if an error occurred whilst reading the file
c
c     Created 96/12/03 DRL@MPIfR
c     Modified October 1997 to return -1 on error
c
c
      implicit none
      character*80 filename

      character lin1*12,recrd*80
      integer irec,istat,counter,recln,hirec,lorec,orec,lun
      logical filex
c
c     Check to see whether the EPN file exists...
c     return nepnrec=0 if it doesn't
c
      nepnrec=0
      inquire(file=filename,exist=filex)
      if (.not.filex) return 
c
c     It exists! Open it and find out the true record length..
c
      call glun(lun)
      nepnrec=-1 ! This will be the value of nepnrec if an error occurs
      irec=-1
      open(unit=lun,file=filename,status='old',access='direct',
     &     form='unformatted',recl=12,err=999)
      read(lun,rec=1,err=999)lin1
      read(lin1,'(8x,i4)',err=999)counter
      close(unit=lun)
      recln=counter*80
      if (recln.le.0) return ! Error
c
c     Find out crude lower and upper bounds for the last record
c
      open(unit=lun,file=filename,status='old',access='direct',
     &     form='unformatted',recl=recln,err=999)
      irec=1
      istat=0
      do while(.true.)
         read(lun,rec=irec,iostat=istat) recrd
         if (istat.ne.0) goto 998
         irec=irec*2
      enddo
 998  hirec=irec
      lorec=irec/2
c
c     Now search for the last record within these bounds iteratively
c
      istat=0
      orec=0
      do while(orec.ne.irec)
         orec=irec
         irec=lorec+(hirec-lorec)/2
         read(lun,rec=irec,iostat=istat) recrd
         if (istat.eq.0) then
            lorec=irec
         else
            hirec=irec
         endif
      enddo
 999  continue
      close(unit=lun)
      nepnrec=irec
      end
c============================================================================== 
      subroutine dattim(date,hh,mm,ss)
c============================================================================== 
c
c     Returns the time obtained by truncating the MJD passed down
c     as date
c
      implicit none
      double precision date,dummy,ss
      integer hh,mm


      dummy=abs(date-int(date))

      dummy=dummy*24.0
      hh=int(dummy)

      dummy=abs(dummy-int(dummy))

      dummy=dummy*60.0
      mm=int(dummy)

      dummy=abs(dummy-int(dummy))

      dummy=dummy*60.0
      ss=dummy

      end
c==============================================================================
C nicked from pgplot
C*GRGLUN -- get a Fortran logical unit number (Sun/Convex-UNIX)
C+
      SUBROUTINE GLUN(LUN)
      INTEGER LUN
C
C Get an unused Fortran logical unit number. 
C Returns a Logical Unit Number that is not currently opened.
C After GRGLUN is called, the unit should be opened to reserve
C the unit number for future calls.  Once a unit is closed, it
C becomes free and another call to GRGLUN could return the same
C number.  Also, GRGLUN will not return a number in the range 1-9
C as older software will often use these units without warning.
C
C Arguments:
C  LUN    : receives the logical unit number, or -1 on error.
C--
C 12-Feb-1989 [AFT/TJP].
C DRL adapted to subroutine GLUN for use with stand-alone software
C 16-Jul-1993 @ JB
c==============================================================================
      INTEGER I
      LOGICAL QOPEN
C---
      DO 10 I=60,20,-1
         INQUIRE (UNIT=I,  OPENED=QOPEN)
         IF (.NOT.QOPEN) THEN
            LUN = I
            RETURN
         END IF
 10   CONTINUE
C     none left
      STOP 'RAN OUT OF LUNs!' 
      END
c==============================================================================
      subroutine b102hex(b10no,hexstr)
c==============================================================================

      implicit none

      integer b10no
      character*4 hexstr
      character*1 hex(16)
      integer i,j,no,num,dig
      data hex/'0','1','2','3','4','5','6','7','8','9',
     &         'A','B','C','D','E','F'/

      no=b10no
      dig=0

      do i=4,1,-1
         do j=15,0,-1
            num=int(float(j)*16.0**float(i-1))
            if (num.le.no) goto 5
         enddo
 5       continue
         dig=dig+1
         hexstr(dig:dig)=hex(j+1)
         no=no-num
      enddo

      end
c==============================================================================
      subroutine hex2b10(hexstr,b10no)
c==============================================================================

      implicit none

      integer b10no
      character*4 hexstr
      character*1 hex(16)
      integer i,j,dig
      data hex/'0','1','2','3','4','5','6','7','8','9',
     &         'A','B','C','D','E','F'/

      b10no=0
      dig=0

      do i=4,1,-1
         do j=1,16
            if (hexstr(i:i).eq.hex(j)) goto 5
         enddo
 5       continue
         b10no=b10no+int(16.0**float(dig)*float(j-1))
         dig=dig+1
      enddo

      end
c===============================================================================
      subroutine shift_prof(profile,nbins,shift)
c===============================================================================
c
c     shifts the profile in the array profile() with nbins bins by the 
c     number of bins passed down in shift. shift>0 means shift forward,
c     shift<0 means shift backwards.
c
      implicit none

      real profile(*)
      integer nbins, shift
c
c     local variables
c
      integer i,j
      real dummy

      if (shift.lt.0) shift=nbins+shift
      do i=1,shift
         dummy=profile(nbins)
         do j=nbins,2,-1
            profile(j)=profile(j-1)
         enddo
         profile(1)=dummy
      enddo

      end
