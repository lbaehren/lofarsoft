
        subroutine basicgrey(arr4,i,j,str1,lab)
        implicit none
        integer i,j,i1,i2,nchar,dumi
        real*4 arr4(i,j),low,up,mn1,mx1,mn2,mx2
        real*4 std,av
        real*4 xcur,ycur,xcur1,ycur1,dum4,arr4c(i,j),low1
        real*8 nsig
        character str1*5,lab*500,ch1

        call copyarray4(arr4,i,j,i,j,1,1,i,j,arr4c)
        call arr2dnz4(arr4c,i,j,low,up)
        if (abs(up/low).gt.50.0) then   ! then convert to LG else keep LN
         call converttolog(arr4c,i,j,low,up,low1)
        end if
        nsig=5.d0
        call sigclip4(arr4c,i,j,1,1,i,j,std,av,nsig)
        call sub_basicgrey(i,j,av,std,low,up,arr4c,lab)

        return
        end

        subroutine sub_basicgrey(i,j,av,std,low,up,arr4c,lab)
        implicit none
        integer i,j,i1,i2,nchar,dumi,nsig
        real*4 arr4(i,j),tr(6),low,up,mn1,mx1,mn2,mx2
        real*4 std,av
        real*4 xcur,ycur,xcur1,ycur1,dum4,arr4c(i,j),low1
        character str1*5,lab*500,ch1

        data tr/0.0,1.0,0.0,0.0,0.0,1.0/

        call pgwindow(0.5,i+0.5,0.5,j+0.5)
        call pgbox('BCINST',0.0,0,'BICNST',0.0,0)
        low=av-10.0*std
        up=av+10.0*std
        call pggray(arr4c,i,j,1,i,1,j,up,low,tr)
        call pglabel(' ',' ',lab(1:nchar(lab)))
        call pgwedg('RG',1.5,3.0,up,low,' ')

        return
        end

        
