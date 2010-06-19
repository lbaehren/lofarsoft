
      SUBROUTINE ZR32RL (NVAL, NP, INB, OUTB)
C-----------------------------------------------------------------------
C! convert 32-bit IEEE floating buffer to local REAL values
C# Binary
C-----------------------------------------------------------------------
C   Converts from 32 bit IEEE floating format to local single precision.
C
C   The IEEE format is:
C
C                1         2         3
C      01234567890123456789012345678901
C      seeeeeeeemmmmmmmmmmmmmmmmmmmmmmm
C
C   where sign = -1 ** s, exponent = eee..., mantissa = 1.mmmmm...
C
C   The value is given by:
C
C      value = sign * 2 **(exp-127) * mantissa
C
C   Note: these values have a "hidden" bit and must always be normalized
C   The IEEE nan (not a number) values are used to indicate an invalid
C   number; a value with all bits set is recognized as a "nan".
C
C   The AIPS internal format for an invalid number is the value which
C   has the same bit pattern as 'INDE'.
C
C   The IEEE special values (-0., ± Infty) are not recognized.
C
C   A multiplication by a factor of 4.0 converts between VAX F and IEEE
C   32 bit formats.
C
C   Inputs:
C      NVAL   I      Number of values to convert
C      NP     I      First value in INB to convert
C      INB    R(*)   32-bit IEEE format values
C   Output:
C      OUTB   R(*)   Local format values ("nan" values are replaced
C                    with AIPS' indefinite value = 'INDE')
C   Generic version - does IEEE and VAX F formats for 32-bit machines,
C   is stubbed with STOP for all others.
C-----------------------------------------------------------------------
      INTEGER   NVAL, NP
      REAL      INB(*), OUTB(*)
C
      INTEGER   LOOP, I4NAN, I4TEST, ZAND, I, L4NAN, I4NANV
      REAL      R4TEST
      INCLUDE 'DDCH.INC'
      INCLUDE 'DMSG.INC'
      EQUIVALENCE (I4TEST, R4TEST)
C                                       = 7F800000 mask for exponent
      DATA I4NAN /2139095040/
C                                       = 00007F80 mask for exponent VMS
      DATA I4NANV /32640/
C-----------------------------------------------------------------------
      IF (NVAL.LE.0) GO TO 999
C                                       only works for 32-bit words
      IF (NBITWD.NE.32) THEN
         WRITE (MSGTXT,1000) NBITWD
         CALL MSGWRT (10)
         STOP 'ZR32RL NEEDS LOCAL DEVELOPMENT'
      ELSE
         IF (BYTFLP.GT.1) THEN
            L4NAN = I4NANV
         ELSE
            L4NAN = I4NAN
            END IF
C                                       IEEE floating-point - copy
C                                       VAX F - multiply by 4
         IF ((SPFRMT.EQ.1) .OR. (SPFRMT.EQ.2)) THEN
            CALL ZBFLIP (4, NVAL, INB(NP), OUTB(1))
            DO 20 LOOP = 1,NVAL
               R4TEST = OUTB(LOOP)
C                                       Do not use L4NAN since I4NAN is
C                                       in the same order as the local
C                                       reals.
               I = ZAND (I4NAN, I4TEST)
               IF (I.EQ.0) THEN
                  OUTB(LOOP) = 0.0
               ELSE IF (I.EQ.I4NAN) THEN
                  OUTB(LOOP) = FBLANK
C                                       VAX F - multiply by 4
               ELSE IF (SPFRMT.EQ.2) THEN
                  OUTB(LOOP) = OUTB(LOOP) * 4.0
                  END IF
 20            CONTINUE
C                                       VAX G, IBM not easy
         ELSE
            WRITE (MSGTXT,1020) SPFRMT
            CALL MSGWRT (10)
            STOP 'ZR32RL NEEDS LOCAL DEVELOPMENT'
            END IF
         END IF
C
 999  RETURN
C-----------------------------------------------------------------------
 1000 FORMAT ('ZR32RL: REQUIRES LOCAL DEVELOPMENT FOR NBITWD =',I7)
 1020 FORMAT ('ZR32RL: REQUIRES LOCAL DEVELOPMENT FOR SPFRMT =',I7)
      END

