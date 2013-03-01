      subroutine IRI2RM(Glong,Glati,year,mmdd,UT,az,el,RM)
C-----------------------------------------------------------------
C
C INPUT:  Glong         Geographic Longitude East in degrees
C         Glati         Geographic Latitude North in degrees
C         year          Year, e.g. 1985
C         mmdd (-ddd)   Date (or Day of Year as a negative number)
C         UT            Universal Time in decimal hours
C         az            Azimuth in degrees
C         el            Elevation in degrees
      implicit none

      integer idate, iyyyy, mmdd
      real*8 ra, dec, Glong, Glati,UT
      real mez,az0,el0,freq0,ra0,dec0,az,el

      real GlongObs,GlatObs,GlongX,GlatX ! in deg
      real Ud,Ue,Un,lst,lst1,Aza,Ela,freq
      real hiX,   X, Xlimit , deltaX ! in km
      real pi

      real RM0, dRM, PA_f, f0, RM

      real OUTF(20,100),OARR(50)
      real ALATI,ALONG,DHOUR
      real lati, longi, height, Bn, Be, Bd, Babs, DIMO, year
      integer i, j
      integer JMAG
      LOGICAL jf(30)

c      write(*,*) Glong,Glati,year,mmdd,UT,az,el,RM

      pi=4.*atan(1.0)

      GlongObs=Glong
      GlatObs=Glati

      az0=az
      el0=el


      DHOUR=UT+25  ! in UT
      JMAG=0  ! Using Geographic coor.
      do i=1,30
         jf(i)=.true.
      enddo
      jf(5)=.false.             ! URSI foF2 model
      jf(6)=.false.             ! Newest ion composition model
      jf(21)=.false.            ! ion drift model included
      jf(23)=.false.            ! Te:InterKosmos model
      jf(24)=.false.            ! D region: 3 D-region models
      jf(12)=.false.   ! no write-out
      RM0=0.
      deltaX=2  ! km
      Xlimit=2000./sin(El0/180.*pi)  ! in km
      if(Xlimit.gt.10000.) Xlimit=10000. 
      write(*,*)'Distance limit=',Xlimit

      do i=0,int(Xlimit/deltaX) ! km
         X=float(i)*deltaX ! km 
         call AzElX2Gx(Az0,El0,X,GlongX,GlatX,hiX,GlongObs,GlatObs,
     *    Ud,Ue,Un)
c         write(*,*) i, X, GlongX,GlatX,hiX,year,mmdd,dhour
         call IRI_SUB_ne(JF,JMAG,GlatX,GlongX,int(year),MMDD,DHOUR,
     &        hiX,hiX,1.,OUTF,OARR)
c         write(*,*) i, outf(1,1)
         dRM=0.
         if(outf(1,1).gt.1) then
            call initize
            CALL FELDCOF(YEAR,DIMO)
c            write(*,*) YEAR,DIMO
            CALL FELDG(GlatX,GlongX,HiX,Bn,Be,Bd,Babs)
c            write(*,*)i,DIMo,Bn,Be,Bd

           dRM=outf(1,1) * (Bn*Un+ Be*Ue+ Bd*Ud) /(3.086e+10)
c     Ne (in m^-3); B in G: 10^6; U in km: 1pc=3.086*10^13km
c     *10^-6 to cm^3; * 10^6 to uG;  Net = null;  
           RM0=RM0+dRM
         endif   
c         write(*,*) i, dRM, RM0
      enddo
 99   RM0=RM0*0.810/1000.*deltaX
c      write(*,*)'Az EL RM =',az0,el0, RM0
      RM=RM0
      return
      end

cccc=====================================================================

      Subroutine AzElX2Gx(Az,El,X,GlongX,GlatX,hiX,GlongObs,GlatObs,
     *  Ud,Ue,Un)
      real GlongObs,GlatObs, Az,El, GlongX,GlatX ! in deg
      real X, hiX, R0,RX ! in km
      real GlongX0,GlatX0,Az0,El0  ! in rad
      real ang_SOX, arc_PM, arc_SM, ang_SPM
      real pi, d_Glong
      real Ud,Ue,Un

      R0=6371.2  ! km
      pi=4.*atan(1.0)

      Glong0=GlongObs/180.*pi
      Glat0=GlatObs/180.*pi
      Az0=Az/180.*pi
      El0=El/180.*pi

      RX=sqrt(R0*R0 + X*X - 2.*R0*X*cos(PI/2.+El0))
      hiX=RX-R0  ! done

      ang_SOX=asin( sin(PI/2+El0) * X/RX)  ! in rad
      arc_SM=ang_SOX

      arc_PM=acos(   ! in arc
     *  sin(Glat0)*cos(arc_SM) + cos(Glat0)*sin(arc_SM)*cos(Az0) )

      GlatX=(PI/2.-arc_PM)*180./PI ! done ! in deg

      if(abs(arc_PM-pi*int(arc_PM/pi)).le.0.0001) then
         ang_SPM=0.
      else
         ang_SPM=asin( sin(Az0)*sin(arc_SM)/sin(arc_PM) ) ! in rad
      endif

      d_Glong=ang_SPM ! in rad
      GlongX=GlongObs + d_Glong*180./pi  ! in deg ! done

      Ud= cos(PI/2.-El0-ang_SOX)
      sin_SMP=sin(pi/2.-Glat0)*sin(Az0)/sin(arc_PM)
      if(sin_SMP.gt.1) sin_SMP=1.
      if(sin_SMP.lt.-1)sin_SMP=-1.
      cos_SMP=sqrt(1.0-sin_SMP*sin_SMP)
      if(arc_PM.gt.Glat0) then cos_SMP=-cos_SMP
      Un= sin(PI/2.-El0-ang_SOX) * cos_SMP
      Ue= sin(PI/2.-El0-ang_SOX) * sin_SMP

      return
      end
