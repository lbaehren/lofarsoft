
      subroutine fortopen (r2flun, r2fname)
      integer        r2flun
      character*(*)  r2fname

      open(r2flun,file=r2fname,form='unformatted',status='old',err=998)
      rewind r2flun
      go to 999
 998  write(*,'("fortopen: Cannot open ",a)') r2fname
 999  end

      subroutine fortclose (r2flun)
      integer        r2flun

      close (r2flun)
      end
 
      subroutine resread (r2flun, mjd, resiturns, resitimes,
     +     phase, freq, weight, error, preres, stat)
      
      integer r2flun
      double precision resiturns,mjd
      double precision phase,preres
      double precision weight, resitimes
      double precision freq,error
      integer stat

      read(r2flun, IOSTAT=stat) mjd, resiturns, resitimes,
     +     phase, freq, weight, error, preres
      end

      subroutine covn (r2flun, nparam)

      integer r2flun
      integer nparam

      read(r2flun) nparam
      rewind r2flun

      end

      subroutine covread (r2flun, nparam, param_name, gcor, sig, cov)

      integer r2flun
      integer nparam

      character*5 param(nparam)
      double precision cov(nparam,nparam)
      double precision sig(nparam)
      double precision gcor(nparam)

      integer mm, j, k

      do 10 n=1,nparam
 10      read(r2flun) mm,j,param(j),gcor(j),sig(j),(cov(j,k),k=1,mm)
         
      end
 
