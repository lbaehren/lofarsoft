          PROGRAM JCB002
C Version 1:
C Modified 1999-02-15 (Burley) to delete my email address.
C Modified 1997-05-21 (Burley) to accommodate compilers that implement
C INT(I1-I2) as INT(I1)-INT(I2) given INTEGER*2 I1,I2.
C
C Version 0:
C Written by James Craig Burley 1997-02-20.
C
C Purpose:
C Determine how compilers handle non-standard IDIM
C on INTEGER*2 operands, which presumably can be
C extrapolated into understanding how the compiler
C generally treats specific intrinsics that are passed
C arguments not of the correct types.
C
C If your compiler implements INTEGER*2 and INTEGER
C as the same type, change all INTEGER*2 below to
C INTEGER*1.
C
      INTEGER*2 I0, I4
      INTEGER I1, I2, I3
      INTEGER*2 ISMALL, ILARGE
      INTEGER*2 ITOOLG, ITWO
      INTEGER*2 ITMP
      LOGICAL L2, L3, L4
C
C Find smallest INTEGER*2 number.
C
      ISMALL=0
 10   I0 = ISMALL-1
      IF ((I0 .GE. ISMALL) .OR. (I0+1 .NE. ISMALL)) GOTO 20
      ISMALL = I0
      GOTO 10
 20   CONTINUE
C
C Find largest INTEGER*2 number.
C
      ILARGE=0
 30   I0 = ILARGE+1
      IF ((I0 .LE. ILARGE) .OR. (I0-1 .NE. ILARGE)) GOTO 40
      ILARGE = I0
      GOTO 30
 40   CONTINUE
C
C Multiplying by two adds stress to the situation.
C
      ITWO = 2
C
C Need a number that, added to -2, is too wide to fit in I*2.
C
      ITOOLG = ISMALL
C
C Use IDIM the straightforward way.
C
c      I1 = IDIM (ILARGE, ISMALL) * ITWO + ITOOLG
C
C Calculate result for first interpretation.
C
      I2 = (INT (ILARGE) - INT (ISMALL)) * ITWO + ITOOLG
C
C Calculate result for second interpretation.
C
      ITMP = ILARGE - ISMALL
      I3 = (INT (ITMP)) * ITWO + ITOOLG
C
C Calculate result for third interpretation.
C
      I4 = (ILARGE - ISMALL) * ITWO + ITOOLG
C
C Print results.
C
      PRINT *, 'ILARGE=', ILARGE
      PRINT *, 'ITWO=', ITWO
      PRINT *, 'ITOOLG=', ITOOLG
      PRINT *, 'ISMALL=', ISMALL
      PRINT *, 'I1=', I1
      PRINT *, 'I2=', I2
      PRINT *, 'I3=', I3
      PRINT *, 'I4=', I4
      PRINT *
      L2 = (I1 .EQ. I2)
      L3 = (I1 .EQ. I3)
      L4 = (I1 .EQ. I4)
      IF (L2 .AND. .NOT.L3 .AND. .NOT.L4) THEN
         PRINT *, 'Interp 1: IDIM(I*2,I*2) => IDIM(INT(I*2),INT(I*2))'
         STOP
      END IF
      IF (L3 .AND. .NOT.L2 .AND. .NOT.L4) THEN
         PRINT *, 'Interp 2: IDIM(I*2,I*2) => INT(DIM(I*2,I*2))'
         STOP
      END IF
      IF (L4 .AND. .NOT.L2 .AND. .NOT.L3) THEN
         PRINT *, 'Interp 3: IDIM(I*2,I*2) => DIM(I*2,I*2)'
         STOP
      END IF
      PRINT *, 'Results need careful analysis.'
      END
