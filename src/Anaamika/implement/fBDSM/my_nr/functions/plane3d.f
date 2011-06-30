
        subroutine plane3d(i,j,a,y,dyda,na)
        implicit none
        integer i,j,na
        real*8 y,a(na),dyda(na)

        y=a(1)*i+a(2)*j+a(3)
        dyda(1)=i
        dyda(2)=j
        dyda(3)=1.d0

        return
        end


