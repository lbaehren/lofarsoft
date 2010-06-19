      PROGRAM EVAL
C **********************************************************************
C        EVAL EVALUATES THE P(D) DISTRIBUTION
C        AND ITS INTEGRAL FROM MINUS INFINITY TO D.
C        SEE NOTES 032972 (!)
C        Based on ApJ, 188, 279  Equation 9
C        Variables:
C           D = deflection assuming k * Omega_e * eta1 = 1.000
C           ETA1 = eta sub 1 from equation 9
C           ETAR = eta ratio = eta2 / eta1, where
C                   eta2 = eta sub 2 from equation 9
C           GAMMA = differential slope of power-law source counts
C        To run EVAL, you must enter the value of GAMMA desired
C        and the corresponding ETAR and ETA1.
C        The values of ETAR and ETA1 for some representative GAMMA are:
C           GAMMA   ETAR     ETA1
C           -----   ----     ----
C           1.5    -1.0000   6.2833
C           1.6    -1.3764   6.5458
C           1.7    -1.9626   7.0233
C           1.8    -3.0776   7.7147
C           1.9    -6.3140   8.6448
C           2.0    no can do
C           2.1    +6.316    11.47
C           2.2    +3.079    13.60
C           2.3    +1.962    16.48
C           2.4    +1.377    20.48
C           2.5    +1.000    26.32
C           2.6    +0.7267   35.38
C           2.7    +0.5095   50.95
C           2.8    +0.3250   82.87
C           2.9    +0.1584   180.51
C        You also have to specify the interval DD between D-values
C        output and the maximum D value to be calculated.  The
C        P(D) distributions all have width about 1, so specifying
C        DD = 0.02 and DMAX = 3.0 is reasonable.
C
C        The output file contains three columns:
C           D = deflection
C           P = P(D)
C           PINT = integral of P(D) from -DMAX to D (should
C                  approach 1.000 at large D)
C
C        91/05/12 REVISION     J. J. CONDON
C **********************************************************************
      CHARACTER*1 CHAR, DEBUG, Q, R
      DATA DEBUG/'D'/,Q/'Q'/,R/'R'/,TWOPI/6.28318531/
      OPEN (UNIT = 33, FILE = 'eval.out', STATUS = 'NEW')
      WRITE( *, *) 'PROGRAM EVAL -- EVALUATES P(D) DISTRIBUTION'
C        READ INPUT PARAMETERS FROM TERMINAL
    5 CONTINUE
      WRITE(*, *) ' ENTER THE N(S) SLOPE GAMMA, ETAR, AND ETA1'
      WRITE(*, *) ' (SEE COMMENTS IN FORTRAN PROGRAM FOR HELP)'
      READ(5, *) GAMMA, ETAR, ETA1
      WRITE(*, *)' ENTER THE OUTPUT D-INTERVAL DD AND MAXIMUM DMAX'
      READ(5, *) DD, DMAX
      WRITE(*, 50) GAMMA,DD,DMAX,ETAR,ETA1
   50 FORMAT(' EVAL INPUTS GAMMA=',F5.2,' DD=',F5.2,' DMAX=',F5.2,
     *', ETAR=',F7.4,' ETA1=',F7.4)
   55 CONTINUE
      WRITE(*, 60)
   60 FORMAT(' TYPE "R" TO RUN, "D" TO DEBUG, OR "Q" TO QUIT')
      READ(5,70)CHAR
   70 FORMAT(A1)
      IPRINT=0
      IF(CHAR.EQ.DEBUG)IPRINT=1
      IF(CHAR.EQ.R.OR.CHAR.EQ.DEBUG)GO TO 100
      IF(CHAR.EQ.Q)STOP
      GO TO 55
  100 CONTINUE
      WRITE(33,50) GAMMA,DD,DMAX,ETAR,ETA1
C        OMEGAM is the minimum upper limit to (lower-case) omega
C        in the integral consistent with reasonable accuracy
      OMEGAM=2.*10.**(1./(GAMMA-1.))
C        DOMEGA is the integration step size
C changed from .0005 to .0001 on 2004 April 24
      DOMEGA=.0001
C        IMAX = number of integration steps
      IMAX=OMEGAM/DOMEGA+.5
      IMAX=IMAX+1
      PDINT=0.
      POLD=0.
C        OUTER D LOOP
      JMAX=DMAX/DD+.5
      KMAX=2*JMAX+1
      IF(IPRINT.EQ.1)WRITE(*, 110)OMEGAM,DOMEGA,ETAR,IMAX,KMAX
  110 FORMAT(' OMEGAM=',E10.3,', DOMEGA=',E10.3,', ETAR=',E10.3,
     *', IMAX=',I6,', KMAX=',I6)
C        Loop over KMAX D-values
      DO 1000 K=1,KMAX
         RK=K
         IF(GAMMA.GT.2.)D=-DMAX+(RK-1.)*DD
         IF(GAMMA.LT.2.)D=(RK-1.)*DD
         P=0.
C        INNER OMEGA LOOP
C        Evaluate the P(D) integral (equation 9)for one D-value
         DO 500 I=1,IMAX
            RI=I
            OMEGA=(RI-1.)*DOMEGA
            OG=OMEGA**(GAMMA-1.)
            COSARG=ETAR*OG+TWOPI*OMEGA*D
            ARG=EXP(-OG)*COS(COSARG)
            P=P+ARG
            IF(I.EQ.1)P=P/2.
  500       CONTINUE
         P=P*2.*DOMEGA
C        INTEGRATE
         PINT=PINT+0.5*DD*(POLD+P)      
         POLD=P
         WRITE(6,600)D,P,PINT
  600    FORMAT(' P(',F6.2,')=',E12.5,' PINT=',E12.5) 
         WRITE(33,600)D,P,PINT
 1000    CONTINUE
      STOP
      END
