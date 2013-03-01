CCcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     
c     Ephemeris file reading and writing code (Fortran 77)
c     
c     redwards 28/Nov/97
c     redwards production version 9/Feb/98
c     redwards added *_lun 24 Mar 98
c     
c     Consists of two symmetrical functions : rd_eph and wr_eph
c     to read and write pulsar ephemeris data
c     
c     Arguments [() denotes arrays. Elements corrsespond to psr parameters]:
c     character*256 fname      -- Filename for ephemeris data
c     integer parmStatus()     -- Whether known/fitted, 0=unknown 
c     -- 1=known 2=fit(ted) for
c     character*32 value_str() -- string value for each parameter
c     real*8 value_double()    -- double precision value for each parameter
c     integer value_integer()  -- integer value for each parameter
c     real*8 error_double()    -- double precision error in each parameter
c     Returns: nonzero(1) iff successful.
c     
c     Notes:
c     -- values are stored as appropriate to particular parameter.
c     See keys.dat for different formats. readeph also stores
c     a string copy of every known field in value_str
c     
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc



c     implicit none             
c     include 'keyinfo.com'       

c     Dummy main program

c     integer parmStatus(NUM_KEYS)
c     character*32 value_str(NUM_KEYS)
c     real*8 value_double(NUM_KEYS)
c     real*8 error_double(NUM_KEYS)
c     integer value_integer(NUM_KEYS)
c     character*80 fin /'t.eph'/
c     character*80 fout /'out.eph'/
c     integer res, readeph, writeeph


c     res = readeph(fin, parmStatus, value_str, 
c     +  value_double, value_integer, error_double)
c     res = writeeph(fout, parmStatus, value_str, 
c     +  value_double, value_integer, error_double)
c     end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     
c     rd_eph_lun:
c
c     takes a lun to read an ephemeris from.
c     
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      integer function rd_eph_lun(un,parmStatus,value_str,
     +     value_double, value_integer, error_double)
      implicit none 
      include 'keyinfo.com'
      integer parmStatus(NUM_KEYS)
      character*32 value_str(NUM_KEYS)
      real*8 value_double(NUM_KEYS)
      integer value_integer(NUM_KEYS)
      real*8 error_double(NUM_KEYS)
      integer un

      integer lineNum, isOldEphem, keypos
      character*80 buffer
      integer convert(NUM_KEYS)

c     the function that does the parsing
      integer rd_eph_str
c     the function that corrects the units upon completion
      integer convertUnits

      isOldEphem = 0      ! flag for "this is an old ephemeris file"
c     First, say we don't yet know any parameters
      do keypos=1, NUM_KEYS
         parmStatus(keypos) = 0
         error_double(keypos) = 0.0d0
         convert(keypos) = 0
      end do

c     Loop over lines
      lineNum = 0

      do while (.true.)
         lineNum = lineNum + 1
         read(un, '(a80)', end=10)buffer ! read a line

         rd_eph_lun = rd_eph_str (parmStatus,value_str,value_double,
     +        value_integer, error_double, convert, isOldEphem,
     +        buffer)

      if (rd_eph_lun.eq.2) then
         write (*,'("Error parsing line ",i3,", skipping.")') lineNum
      endif

      end do                    ! end of file processing loop

 10   continue                  ! read() jumps here at end of file

c     Convert any units from old input files
      rd_eph_lun = convertUnits(value_double, error_double,
     +     parmStatus, convert)

      return

      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c     rd_eph_str - reads a single string into appropriate place in
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      integer function rd_eph_str (parmStatus,value_str,
     +        value_double, value_integer, error_double, convert, 
     +        isOldEphem, buffer)
      implicit none
      include 'keyinfo.com'
      integer parmStatus(NUM_KEYS)
      character*32 value_str(NUM_KEYS)
      real*8 value_double(NUM_KEYS)
      integer value_integer(NUM_KEYS)
      real*8 error_double(NUM_KEYS)
      integer convert(NUM_KEYS)
      integer isOldEphem
      character*(*) buffer

      integer toklen, keypos
      logical fitThis, isAlias

      integer tokpos, ll, length, tmp
      logical strmatch, found
      character*80 tok
      real*8 hmstoturns, dmstoturns, prec

      rd_eph_str = 1
      fitThis = .false.
      keypos = 0
      ll = length(buffer)
      tokpos = 1 
      call citem(buffer, ll, tokpos, tok, toklen)
      if (toklen.le.0) goto 70  ! Blank line. Skip
      call upcase(tok)
      
c     Is it a comment line?
      if ((tok(1:1) .eq. '#') 
     +     .or. (tok(1:2) .eq. 'C ')) goto 70 ! Ignore line.
      if ((isOldEphem.eq.1)          ! Expect possible fit flag first.
     +     .and. (tok(2:2).eq.' ') ! Is it only 1 char long?
     +     .and. (.not. (tok(1:1).eq.'P')) ! and not a keyword...
     +     .and. (.not. (tok(1:1).eq.'E'))) then
         fitThis = .true.       ! Was a fit flag. Get next token..
         call citem(buffer, ll, tokpos, tok, toklen)
         if (toklen.le.0) goto 62 ! Error...
      end if
      call upcase(tok)

c     We now should have a key. Find it in the list
      if (strmatch("NAME",tok)) then ! NAME is a special case!
         isOldEphem = 1
         keypos = -1
         found = .true.
      else
         found = .false.
         keypos = 1
      end if
      do while ((keypos .le. NUM_KEYS) .and. .not. found)
         if (strmatch(parmNames(keypos), tok)) 
     +        found = .true.
         keypos = keypos + 1
      end do
      keypos = keypos - 1

      if (found) then
         found = .true.
         if (keypos .gt. -1) parmStatus(keypos) = 0 ! set this shortly
      else 
         if (isAlias(tok,keypos, convert)) then
            found = .true.
            if (keypos .gt. -1) parmStatus(keypos) = 0 ! set this shortly
         end if
      end if

      if (found) then 
c     We now know the key index. 
         call citem(buffer, ll, tokpos, tok, toklen)
         if (toklen.le.0) goto 60 ! Error...

         if (tok(1:1).eq.'#')  then ! Comment. ignore the line
            parmStatus(keypos) = 0
         else
c     keypos = -2 signifies NAME input parameter. Work out if it should be
c     PSRJ or PSRB, and don't include any prefix letter in the value 
            if (keypos.eq.-2) then
               if (tok(1:1).eq.'B') then
                  keypos = EPH_PSRB 
               else
                  keypos = EPH_PSRJ
               end if
               if ((tok(1:1).eq.'J').or.(tok(1:1).eq.'B')) then
                  value_str(keypos)(1:) = tok(2:toklen)
               else
                  value_str(keypos) = tok(1:toklen) 
               end if
            else
               value_str(keypos) = tok(1:toklen) ! save ASCII format
            end if

c     Now, store the value in the way specified in parmTypes (see keys.dat)
            if (parmTypes(keypos) .eq. 1) then
               read(tok,*,err=63)value_double(keypos)
            else if (parmTypes(keypos) .eq. 2) then !h:m:s
               value_double(keypos) = hmsToTurns(tok,prec)
            else if (parmTypes(keypos) .eq. 3) then !d:m:s
               value_double(keypos) = dmsToTurns(tok,prec)
            else if (parmTypes(keypos) .eq. 4) then
               call realTo2part(tok, value_integer(keypos),
     +              value_double(keypos))
            else if (parmTypes(keypos) .eq. 5) then
               read(tok,'(i10)',err=64)value_integer(keypos)
            end if
c     Next token, if present.
            call citem(buffer, ll, tokpos, tok, toklen)
            if ((toklen.gt.0).and.(tok(1:1).ne.'#')) then
               call upcase(tok)
c     Get a fit flag if present, for new format files
               if ((isOldEphem.eq.0).and.(tok(2:2).eq.' ')) then  
                  fitThis = (tok(1:1).ge.'1')
c     Get the error/comment field
                  call citem(buffer, ll, tokpos, tok, toklen)
                  call upcase(tok)
               end if
            end if

c     Set the "parameter status" for this thing.
            
            if (fitThis) then
               parmStatus(keypos)=parmStatus(keypos)+2 !known and fit
            else
               parmStatus(keypos)=parmStatus(keypos)+1 !known but not fit
            endif
c     Process error value if allowed and present
            if (parmError(keypos).and.(toklen.gt.0)
     +           .and.(tok(1:1).ne.'#')) then
c     Handle old ephemeris position values (RAJ and DECJ)
               if ((isOldEphem.eq.1) .and. 
     +              ((keypos.eq.EPH_RAJ)
     +              .or.(keypos.eq.EPH_DECJ))) then
                  read(tok(1:toklen),'(i10)', err=61), tmp
                  error_double(keypos) = dfloat(tmp)*prec
               else
                  read(tok(1:toklen),*, err=65)
     +                 error_double(keypos)
               end if
               if (error_double(keypos).lt.0.0d0) ! invalid
     +              error_double(keypos) = 0.0d0
               if ((isOldEphem.eq.1) 
     +              .and. (error_double(keypos).gt.0.0d0)) then
                  fitThis = .true.
               end if
            end if
c     Ignore the line if there's no error value and the value is zero
            if ((error_double(keypos).le.0.0d0)
     +           .and.(parmTypes(keypos).eq.1)
     +           .and.(value_double(keypos).eq.0.0d0)) then
               parmStatus(keypos) = 0
            end if
         end if                 ! end of if value=comment then, else...
      end if                    ! end of if keyword found then...
      goto 70                   ! no errors, skip over
c     This is the general error handler for processling a line
 60   write (*,'("Error citem.")')
      goto 69
 61   write (*,'("Error isOldEphem citem toklen.")')
      goto 69
 62   write (*,'("Error empty token.")')
      goto 69
 63   write (*,'("Error parsing double from ",a)')tok
      goto 69
 64   write (*,'("Error parsing int from ",a)')tok
      goto 69
 65   write (*,'("Error parsing double error from ",a)')tok
      goto 69
 69   continue
c     There was an error. If we parsed a keyword, flag it as unknown
c     just in case
      if (keypos .gt. 0) parmStatus(keypos) = 0
      rd_eph_str = 2

 70   continue
      return

      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c     rd_eph
c
c     Takes a filename to read an ephemeris from.
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      integer function rd_eph(fname,parmStatus,value_str,
     +     value_double, value_integer, error_double)
      implicit none 
      include 'keyinfo.com'
      character*(*) fname
      integer parmStatus(NUM_KEYS)
      character*32 value_str(NUM_KEYS)
      real*8 value_double(NUM_KEYS)
      integer value_integer(NUM_KEYS)
      real*8 error_double(NUM_KEYS)
      integer getlun, un, iost, rd_eph_lun
 

      un=getlun()
      open(un,file=fname,status='old',iostat=iost, err=20)
      rd_eph = rd_eph_lun(un,parmStatus,value_str,
     +     value_double, value_integer, error_double)
      close(un)
      return
 20   rd_eph = 0
      write(*,21) fname,iost
      return
 21   format ('File ',a,' could not be opened',
     +     ' (error', i6,')')
      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c     wr_eph_str
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      integer function wr_eph_str (line, parNum, value_str,
     +     value_double, value_integer, error_double, parmStatus)
      implicit none 
      include 'keyinfo.com'
      character*(*) line
      integer parNum
      character*32 value_str
      real*8 value_double
      integer value_integer
      real*8 error_double
      integer parmStatus

      integer linelen, wordlen
      integer length
      character*256 word 
      character*256 word2
      word = " "
      word2 = " "

c     Write keyword
      write(line,"(a)") parmNames(parNum)(1:maxKeyLen)
      linelen = maxKeyLen + 2

c     Write value
      if (parmTypes(parnum) .eq. 0) then
         write(word(2:),'(a)')value_str
      else if (parmTypes(parnum) .eq. 1) then
         write(word,'(1p,d23.16)')value_double
      else if (parmTypes(parnum) .eq. 2) then ! h:m:s
         call turnsToHms(value_double,word)
      else if (parmTypes(parnum) .eq. 3) then ! d:m:s
         call turnsToDms(value_double,word)
      else if (parmTypes(parnum) .eq. 4) then
         call twoPartToReal(value_integer,
     +        value_double, word(2:))
      else if (parmTypes(parnum) .eq. 5) then
         call intToStr(value_integer, word(2:))
      end if

c     Write fit (if any)
      wordlen = length(word)
      line(linelen:linelen+wordlen) = word(1:wordlen)
      linelen = linelen + wordlen
      if (parmStatus .eq. 2) then ! Fit flag
         line(linelen+1:) = "1"
         linelen = linelen + 2
      endif

c     Write error
      if (parmError(parnum).and.(error_double.gt.0.0d0)) then

c     if (parmTypes(parnum) .eq. EPH_RAJ) then ! h:m:s. turns-->sec
c     write(line(linelen:),'(1p,d13.6)'),
c     +             86400.0d0*error_double
c     else if (parmTypes(parnum) .eq. EPH_DECJ) then ! turns-->arcsrc
c     write(line(linelen:),'(1p,d13.6)'),
c     +             1296000.0d0*error_double
c     else
         write(line(linelen:),'(1p,d13.6)')error_double
c     end if 
      end if

      wr_eph_str = 1
      return
      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c     wr_eph_lun
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      integer function wr_eph_lun(un,parmStatus,value_str,
     +     value_double, value_integer, error_double)
      implicit none 
      include 'keyinfo.com'
      integer parmStatus(NUM_KEYS)
      character*32 value_str(NUM_KEYS)
      real*8 value_double(NUM_KEYS)
      integer value_integer(NUM_KEYS)
      real*8 error_double(NUM_KEYS)

      integer un, parnum
      character*256 line 
      integer length
      integer wr_eph_str

      line = " "

c     For all parameters, write out info...
      do parnum=1,NUM_KEYS
         if (parmStatus(parnum) .gt. 0) then

            wr_eph_lun = wr_eph_str (line, parnum, value_str(parnum),
     +           value_double(parnum), value_integer(parnum),
     +           error_double(parnum), parmStatus(parnum))

c     write to file
            write(un,'(a)') line(1:length(line)) 
         end if
      enddo
      wr_eph_lun = 1
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c     wr_eph
c
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

      integer function wr_eph(fname,parmStatus,value_str,
     +     value_double, value_integer, error_double)
      implicit none 
      include 'keyinfo.com'
      character*(*) fname
      integer parmStatus(NUM_KEYS)
      character*32 value_str(NUM_KEYS)
      real*8 value_double(NUM_KEYS)
      integer value_integer(NUM_KEYS)
      real*8 error_double(NUM_KEYS)

      integer un, getlun, iost, wr_eph_lun

      un=getlun()
      open(un,file=fname,status='unknown',iostat=iost, err=220)
      wr_eph = wr_eph_lun(un,parmStatus,value_str,
     +     value_double, value_integer, error_double)
      close(un)
      return
 220  wr_eph = 0
      write(*,221) fname,iost
      return
 221  format ('File ',a,' could not be opened',
     +     ' (error', i6,')')
      end

ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
c convertUnits : looks at the 'convert' array and converts the units
c   (in a parameter-name-specific manner) into the desired ones.
c Works only with real-valued parameters.
c
ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      integer function convertUnits (value_double,
     +     error_double, parmStatus, convert)
      implicit none
      include 'keyinfo.com'
      real*8 value_double(NUM_KEYS)
      real*8 error_double(NUM_KEYS)
      integer parmStatus(NUM_KEYS), convert(NUM_KEYS)

      logical p0known, p1known
      real*8 p0, p1, p2, f0, f1, f2
      real*8 p0relerr, p1relerr, p2relerr, f0relerr, f1relerr
      real*8 piOn180, scat_factor, ln10

c     Who knows how many digits are significant? This should do...
      parameter (piOn180 = .017453292519943295769236907684886d0)
c     This value is 10^-6 * (1000/408)^-4.4 for conversion of scattering times.
      parameter (scat_factor = 1.935997d-08)
c     This is the natural logarithm of 10.0, for use in scattering->tau 
c     Error conversion
      parameter (ln10 = 2.30258509299404568401d0)
      
c     Work with freqency stuff first since the conversions are interdependent.
      p0known = .false.
      p1known = .false.
c     Change P values to F values, if present.
c     Up to the same number parameters
      if ((parmStatus(EPH_F).gt.0).and.
     +     (convert(EPH_F).eq.1)) then ! P->F0
         p0known = .true.
         p0 = value_double(EPH_F)
         value_double(EPH_F) = 1.0d0/p0
         f0 = value_double(EPH_F)
         p0relerr = abs(error_double(EPH_F)/p0)
         f0relerr = p0relerr
         error_double(EPH_F) = abs(f0*p0relerr)
      end if
      if ((parmStatus(EPH_F1).gt.0).and.
     +     (convert(EPH_F1).eq.1)) then ! Pdot -> F1
         if (.not. p0known) then
            write(*,'("Error: Pdot without P! Ignoring it.")')
            parmStatus(EPH_F1) = 0
         else
            p1known = .true.
            p1 = value_double(EPH_F1)*1.0d-15 ! Get in to s.s^{-1}
            value_double(EPH_F1) = -p1/(p0*p0)
            f1 = value_double(EPH_F1)
            p1relerr = abs(1.0d-15*error_double(EPH_F1)/p1)
            f1relerr = sqrt(p1relerr*p1relerr +
     +           4.0d0*p0relerr*p0relerr)
            if (p1relerr.gt.0.0d0) then ! Make sure we were given an error
               error_double(EPH_F1) = abs(f1*f1relerr)
            else
               error_double(EPH_F1) = 0.0d0
            end if
         end if
      end if
      if ((parmStatus(EPH_F2).gt.0).and.
     +     (convert(EPH_F2).eq.1)) then ! Pdotdot-> F2
         if (.not. (p0known.and.p1known)) then
            write(*,'("Error: Pdotdot without P, Pdot! Ignoring it.")')
            parmStatus(EPH_F2) = 0
         else
c Old Jodrell stuff had it in units of 10^-24 s s^-2.
c If this is the case, convert it to s s^-2 as per Melb/ATNF old standard
            if (value_double(EPH_F2) .gt. 1.0d-12) then
               value_double(EPH_F2) = value_double(EPH_F2)*1.0d-24
               error_double(EPH_F2) = error_double(EPH_F2)*1.0d-24
            end if
            p2 = value_double(EPH_F2)
            value_double(EPH_F2) = 2.0d0*f1*f1/f0 - p2*f0*f0
            f2 = value_double(EPH_F2)
            p2relerr = (error_double(EPH_F2)/p2)

            if (p2relerr.gt.0.0d0) then ! Make sure we were given an error
             
c     \sigma^2_{F2} = \sigma^2_{F1}*(dF2/dF1)^2 + ... F0 .... P2 
               error_double(EPH_F2) = sqrt(
     +              (f1*f1relerr*4.0d0*f1/f0)**2 +
     +              (f0*f0relerr*2.0d0*(f1*f1/(f0*f0) + p2*f0))**2 +
     +              (p2*p2relerr*f0*f0)**2 )
            else
               error_double(EPH_F2) = 0.0d0
            end if
         end if
      end if
      if ((parmStatus(EPH_F3).gt.0).and.
     +     (convert(EPH_F3).eq.1)) then ! "VTRDOT" (f3/10^-30) -> f3 
         value_double(EPH_F3) = value_double(EPH_F3) * 1.0d-30
         error_double(EPH_F3) = error_double(EPH_F3) * 1.0d-30
      end if

c     "INCBIN" inclination of binary orbit in degrees -> sini
      if ((parmStatus(EPH_SINI).gt.0)
     +     .and.(convert(EPH_SINI).eq.1)) then
         error_double(EPH_SINI) = piOn180*error_double(EPH_SINI) *
     +        cos(piOn180*value_double(EPH_SINI))
         value_double(EPH_SINI) = sin(piOn180*value_double(EPH_SINI))
      end if
      if ((parmStatus(EPH_SINI_2).gt.0)
     +     .and.(convert(EPH_SINI_2).eq.1)) then
         error_double(EPH_SINI_2) = piOn180*error_double(EPH_SINI_2) *
     +        cos(piOn180*value_double(EPH_SINI_2))
         value_double(EPH_SINI_2) = sin(piOn180*
     +        value_double(EPH_SINI_2))
      end if
      if ((parmStatus(EPH_SINI_3).gt.0)
     +     .and.(convert(EPH_SINI_3).eq.1)) then
         error_double(EPH_SINI_3) = piOn180*error_double(EPH_SINI_3) *
     +        cos(piOn180*value_double(EPH_SINI_3))
         value_double(EPH_SINI_3) = sin(piOn180*
     +        value_double(EPH_SINI_3))
      end if

c     "SCATTER" scattering in microseconds at 408MHz
c     -> Tau, log10(scattering in seconds at 1GHz). assume t scales as nu^-4.4.
c     redwards 13-Mar-98 update: Tau is not a timing parameter. Removed

c      if ((parmStatus(EPH_TAU).gt.0).and.
c     +     (convert(EPH_TAU).eq.1.)) then
c         error_double(EPH_TAU) = error_double(EPH_TAU)
c     +        /(ln10*value_double(EPH_TAU))
c         value_double(EPH_TAU) = log10(value_double(EPH_TAU) * 
c     +        scat_factor)
c      endif

      convertUnits = 1
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     isAlias -- tests a keyword to see if it is one of the special
c     aliased keywords due to past ambiguity of file formats.
c     returns true iff found and leaves the index in keynum.
c     May set a conversion code in convert(), for later use by
c     convertUnits to translate data from different input units
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      logical function isAlias(s, keynum, convert)
      implicit none
      include 'keyinfo.com'
      character*(*) s
      integer keynum
      integer convert(NUM_KEYS)
      logical strmatch

      keynum = -1

      if (strmatch("DM0",s)) then
         keynum = EPH_DM
      else if (strmatch("DDOT",s)) then
         keynum = EPH_DM1

c Binary parameters
      else if (strmatch("A1_1",s)) then
         keynum = EPH_A1    
      else if (strmatch("A1-2",s).or.strmatch("A12",s)) then
         keynum = EPH_A1_2
      else if (strmatch("A1-3",s).or.strmatch("A13",s)) then
         keynum = EPH_A1_3

      else if (strmatch("ECC",s) .or. strmatch("E_1", s)) then
         keynum = EPH_E
      else if (strmatch("E2",s).or.strmatch("E-2",s)) then
         keynum = EPH_E_2
      else if (strmatch("E3",s).or.strmatch("E-3",s)) then
         keynum = EPH_E_3

      else if (strmatch("T0_1",s)) then
         keynum = EPH_T0     
      else if (strmatch("T02",s).or.strmatch("T0-2",s)) then
         keynum = EPH_T0_2
      else if (strmatch("T03",s).or.strmatch("T0-3",s)) then
         keynum = EPH_T0_3

      else if (strmatch("PB_1",s)) then
         keynum = EPH_PB
      else if (strmatch("PB2",s).or.strmatch("PB-2",s)) then
         keynum = EPH_PB_2
      else if (strmatch("PB3",s).or.strmatch("PB-3",s)) then
         keynum = EPH_PB_3

      else if (strmatch("OM_1",s)) then
         keynum = EPH_OM
      else if (strmatch("OM2",s).or.strmatch("OM-2",s)) then
         keynum = EPH_OM_2
      else if (strmatch("OM3",s).or.strmatch("OM-3",s)) then
         keynum = EPH_OM_3

      else if (strmatch("OMDOT_1",s)) then
         keynum = EPH_OMDOT
      else if (strmatch("OMDOT2",s).or.strmatch("OMDOT-2",s)) then
         keynum = EPH_OMDOT_2
      else if (strmatch("OMDOT3",s).or.strmatch("OMDOT-3",s)) then
         keynum = EPH_OMDOT_3

      else if (strmatch("PBDOT_1",s)) then
         keynum = EPH_PBDOT
      else if (strmatch("PBDOT2",s).or.strmatch("PBDOT-2",s)) then
         keynum = EPH_PBDOT_2
      else if (strmatch("PBDOT3",s).or.strmatch("PBDOT-3",s)) then
         keynum = EPH_PBDOT_3


      else if (strmatch("INCBIN",s)) then
         keynum = EPH_SINI
         convert(keynum) = 1
      else if (strmatch("INCBIN-2",s)) then
         keynum = EPH_SINI_2
         convert(keynum) = 1
      else if (strmatch("INCBIN-3",s)) then
         keynum = EPH_SINI_3
         convert(keynum) = 1

      else if (strmatch("MCBIN",s).or.strmatch("M2_1",s)) then
         keynum = EPH_M2
      else if (strmatch("MCBIN-2",s)) then
         keynum = EPH_M2_2
      else if (strmatch("MCBIN-3",s)) then
         keynum = EPH_M2_3

c Pulsar name parameters
      else if (strmatch("RA",s)) then
         keynum = EPH_RAJ
      else if (strmatch("DEC",s)) then
         keynum = EPH_DECJ
      else if (strmatch("PSR",s)) then
         keynum = EPH_PSRJ
c Frequency stuff
      else if (strmatch("F0",s)) then
         keynum = EPH_F
      else if (strmatch("P",s).or.strmatch("P0",s)) then
         keynum = EPH_F
         convert(keynum) = 1    ! Convert period --> freq
      else if (strmatch("P1",s).or.strmatch("PDOT",s)) then
         keynum = EPH_F1
         convert(keynum) = 1
      else if (strmatch("P2",s).or.strmatch("PDDOT",s)
     +        .or.strmatch("PDOTDOT",s)) then
         keynum = EPH_F2
         convert(keynum) = 1
      else if (strmatch("VTRDOT",s)) then ! d3f/dt3 in 10^-30 s^-4
         keynum = EPH_F3
         convert(keynum) = 1
c Scattering. ***** removed . not a timing parameter
c      else if (strmatch("SCATTER",s)) then
c         keynum = EPH_TAU
c         convert(keynum) = 1
      end if

      if (keynum .gt. -1) then
         isAlias = .true.
      else
         isAlias = .false.
      endif

      return
      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     intToStr -- convert an int to a string without leading blanks
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine intToStr(intv, s)
      implicit none
      integer intv
      character*(*) s
      character*20 tmp
      integer len

      if (intv .eq. 0) then
         s = "0"
         return
      end if
      if (intv .eq. 1) then
         s = "1"
         return
      end if
      len = int(log10(abs(float(intv))))
      if (mod(log10(abs(float(intv))),1.0).gt. 0.d0)  then
         len = len + 1
      end if
      if (intv .lt. 0) len = len + 1
      write(tmp,'(i20)') intv
      s = tmp(21-len:20) 
      return
      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     turnsToHms and turnsToDms -- convert from double precision turns
c     to a string of the form hh:mm:ss.ssssss or dd:.. hours or degrees.
c     uses util function turnsToXms
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine turnsToXms(d, f1, f2, f3, s)
      implicit none
      real*8 d,f1,f2,f3
      character*(*) s
      character*32 word
      integer v1, v2
      real*8 v3

      v1 = int(d/f1)
      v2 = int(mod(abs(d),f1)/f2)
      v3 = mod(abs(d), f2)/f3

      s = " "
      write(s,'(i3.2)') v1        ! "0n" for n < 10
      if (d.lt.0.0) s(1:1) = "-"  ! Sign 

      s(4:4) = ":"

c     The following two fields use 'word' to write into since we need to
c     ignore the first column for the sign (since they are positive, ' ')
      write(word,'(i3)') v2
      call zeropad(word(2:))
      s(5:6) = word(2:3)
      s(7:7) = ":"
      write(word,'(0p,f17.13,1p)') v3 
      call zeropad(word(2:))
      s(8:) = word(2:)
      return
      end

      subroutine turnsToHms(d,s)
      real*8 d
      character*(*) s
      call turnsToXms(d,1d0/24d0,1d0/1440d0,1d0/86400d0,s)
      return
      end

      subroutine turnsToDms(d,s)
      real*8 d
      character*(*) s
      call turnsToXms(d, 1d0/360d0,1d0/21600d0,1d0/1296d3,s)
      return
      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     realTo2part -- subroutine to read a string like "50025.8223...."
c     and convert the integer part and fractional parts separately for
c     precision
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine realTo2part(tok, int,frac)
      character*(*) tok
      integer int, pos, length
      real*8 frac

      pos = index(tok, ".")
      if (pos .eq. 0) then
c     there's no decimal point
         read(tok,*,err=210) int
         frac = 0.0
         return
      end if
c     Otherwise
      read(tok(1:pos-1),*,err=210) int
      read(tok(pos:len(tok)),*,err=210) frac
      return 
 210  write(*,220) tok(1:length(tok)) ! error handler
      return
 220  format ("Error: ephio.f: unable to decipher the following:",a)
      end


cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     twoPartToReal -- produce an output string for the above
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine twoPartToReal(intv,frac,s)
      implicit none
      integer intv, nzeros, i
      real*8 frac
      character*(*) s

      character*32 fracbit
      integer length

c     Make sure it's all proper. Fix it otherwise
      intv = intv + int(frac)
      frac = mod(frac,1.d0)
      s = " "
c     Integer part...
      call intToStr(intv,s)
      s(length(s)+1:) = "."
c     Fractional part...
      fracbit = " "
      write(fracbit,'(0p,d23.16,1p)') frac
c How many extra zeros do we need? ie -1*the exponent used above
      read (fracbit(22:), '(i2)') nzeros
c      write (*,*) fracbit(22:), nzeros
      if (nzeros.gt.0) then
         do i=length(s)+1,length(s)+nzeros
            s(i:) = "0"
         end do
      end if
      s(length(s)+1:) = fracbit(4:19) ! all digits
      return
      end

cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c     getToken : function to get a space/newline-demited token from a string
c     Arguments : str -- string to search from
c     pos -- which position to start searching from.
c     tok -- token returned to here
c     toklen -- length of token left here
c     Effects : leaves the token in tok and sets pos to the next position
c     after the token
c     Returns: true iff successful
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
      logical function getToken(str, pos, tok, toklen)
      implicit none
      character*(*) str
      integer pos, length, toklen
      character*(*) tok 

      integer stlen, tokpos

      tok = " "                 ! clear token
      stlen = length(str)
c     Ignore leading blanks
      do while ((str(pos:pos) .eq. ' ') .and. (pos .le. stlen))
         pos = pos + 1
      end do
      if (pos .gt. stlen) then  ! no token found
         getToken = .false.
         return
      end if
c     copy into tok
      tokpos = 1
      do while ((.not. (str(pos:pos) .eq. ' ')) .and. (pos .le. stlen))
         tok(tokpos:tokpos) = str(pos:pos)
         tokpos = tokpos + 1
         pos = pos + 1
      end do
      toklen = tokpos - 1
      getToken = .true.
      return
      end

