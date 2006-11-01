
        subroutine basicgrey(arr4,i,j,str1,lab)
        implicit none
        integer i,j,i1,i2,nchar,dumi
        real*4 arr4(i,j),tr(6),low,up,mn1,mx1,mn2,mx2
        real*4 xcur,ycur,xcur1,ycur1,dum4,arr4c(i,j)
        character str1*5,lab*500,ch1

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/

        call copyarray4(arr4,i,j,i,j,1,1,i,j,arr4c)
        call arr2dnz4(arr4c,i,j,low,up)
        if (up/low.gt.50.0) then   ! then convert to LG else keep LN
         call converttolog(arr4c,i,j,low,up)
        end if

c        call pgwindow(0.5,i+0.5,0.5,j+0.5)
c        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
c        call pggray(arr4c,i,j,1,i,1,j,up,low,tr)
c        call pglabel(' ',' ',lab(1:nchar(lab)))
c        call pgwedg('RG',1.5,3.0,up,low,' ')

        return
        end

        
