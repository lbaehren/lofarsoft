      SUBROUTINE  Q7APL(NN, N, P, J, R, IERR)
C     *****PARAMETERS.
      INTEGER NN, N, P, IERR
      REAL J(NN,P), R(N)
C
C     ..................................................................
C     ..................................................................
C
C     *****PURPOSE.
C     THIS SUBROUTINE APPLIES TO R THE ORTHOGONAL TRANSFORMATIONS
C     STORED IN J BY QRFACT
C
C     *****PARAMETER DESCRIPTION.
C     ON INPUT.
C
C        NN IS THE ROW DIMENSION OF THE MATRIX J AS DECLARED IN
C             THE CALLING PROGRAM DIMENSION STATEMENT
C
C        N IS THE NUMBER OF ROWS OF J AND THE SIZE OF THE VECTOR R
C
C        P IS THE NUMBER OF COLUMNS OF J AND THE SIZE OF SIGMA
C
C        J CONTAINS ON AND BELOW ITS DIAGONAL THE COLUMN VECTORS
C             U WHICH DETERMINE THE HOUSEHOLDER TRANSFORMATIONS
C             IDENT - U*U.TRANSPOSE
C
C        R IS THE RIGHT HAND SIDE VECTOR TO WHICH THE ORTHOGONAL
C             TRANSFORMATIONS WILL BE APPLIED
C
C        IERR IF NON-ZERO INDICATES THAT NOT ALL THE TRANSFORMATIONS
C             WERE SUCCESSFULLY DETERMINED AND ONLY THE FIRST
C             ABS(IERR) - 1 TRANSFORMATIONS WILL BE USED
C
C     ON OUTPUT.
C
C        R HAS BEEN OVERWRITTEN BY ITS TRANSFORMED IMAGE
C
C     *****APPLICATION AND USAGE RESTRICTIONS.
C     NONE
C
C     *****ALGORITHM NOTES.
C     THE VECTORS U WHICH DETERMINE THE HOUSEHOLDER TRANSFORMATIONS
C     ARE NORMALIZED SO THAT THEIR 2-NORM SQUARED IS 2.  THE _USE_ OF
C     THESE TRANSFORMATIONS HERE IS IN THE SPIRIT OF (1).
C
C     *****SUBROUTINES AND FUNCTIONS CALLED.
C
C      D7TPR - FUNCTION, RETURNS THE INNER PRODUCT OF VECTORS
C
C     *****REFERENCES.
C     (1) BUSINGER, P. A., AND GOLUB, G. H. (1965), LINEAR LEAST SQUARES
C        SOLUTIONS BY HOUSEHOLDER TRANSFORMATIONS, NUMER. MATH. 7,
C        PP. 269-276.
C
C     *****HISTORY.
C     DESIGNED BY DAVID M. GAY, CODED BY STEPHEN C. PETERS (WINTER 1977)
C     CALL ON V2AXY SUBSTITUTED FOR DO LOOP, FALL 1983.
C
C     *****GENERAL.
C
C     THIS SUBROUTINE WAS WRITTEN IN CONNECTION WITH RESEARCH
C     SUPPORTED BY THE NATIONAL SCIENCE FOUNDATION UNDER GRANTS
C     MCS-7600324, DCR75-10143, 76-14311DSS, AND MCS76-11989.
C
C     ..................................................................
C     ..................................................................
C
C     *****LOCAL VARIABLES.
      INTEGER K, L, NL1
C     *****FUNCTIONS.
      REAL  D7TPR
      EXTERNAL  D7TPR, V2AXY
C
C  ***  BODY  ***
C
      K = P
      IF (IERR .NE. 0) K = IABS(IERR) - 1
      IF ( K .EQ. 0) GO TO 999
C
      DO 20 L = 1, K
         NL1 = N - L + 1
         CALL V2AXY(NL1, R(L), - D7TPR(NL1,J(L,L),R(L)), J(L,L), R(L))
 20   CONTINUE
C
 999  RETURN
C  ***  LAST LINE OF  Q7APL FOLLOWS  ***
      END
