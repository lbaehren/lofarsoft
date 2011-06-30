

        implicit none
        real arr(2880,2880),mn,mx,tr(6),indx,red,green,blue
        real*8 rand,arr8(2880,2880)
        integer i,j,seed,id,pgopen,c1,c2,n,m,clr1,clr2
        character f1*500,extn*20
        
        data tr/0.0,1.0,0.0,0.0,0.0,1.0/

        extn='.img'
        f1='cs1_l02339'
        n=2880
        m=2880
        call readarray_bin(n,m,arr8,n,m,f1,extn)

        seed=-233
        mn=100.0
        mx=-100.0
        do i=1,2880
         do j=1,2880
c          call ran1(seed,rand)
c          arr(i,j)=rand
c          arr(i,j)=(i+j)*0.1d0
           arr(i,j)=arr8(i,j)
          if (arr(i,j).lt.mn) mn=arr(i,j)
          if (arr(i,j).gt.mx) mx=arr(i,j)
         end do
        end do

        mn=-10.0
        mx=14.0

        
        id=pgopen('/xs')
        clr1=1
        clr2=99
        call pgscir(clr1,clr2)
        call pgqcir(c1,c2)
        if (c1.ne.clr1.or.c2.ne.clr2) write (*,*) ' BADDDDDDDDDDD'

        do i=clr1,clr2
         call pgqcr(i,red,green,blue)
         write (*,*)  i,red,green,blue
         indx=(i-clr1)*1.0/(clr2-clr1)
         if (indx.le.0.34) then 
          red=(indx-0.0)/(0.34-0.0)
         else
          red=1.0
         end if
         green=(indx-0.0)/(1.0-0.0)
         if (indx.le.0.65) then 
          blue=0.0
         else
          if (indx.le.0.98) then 
           blue=(indx-0.65)/(0.98-0.65)
          else
           blue=1.0
          end if
         end if
         call pgscr(i,red,green,blue)
        end do

        call pgvport(0.1,0.9,0.1,0.9)
        call pgwindow(0.5,n*1.0,0.5,m*1.0)
        call pgbox('BCNST',0.0,0,'BCNST',0.0,0)

        call pgimag(arr,n,m,1,n,1,m,mn,mx,tr)
        call pgwedg('RI',1.5,3.0,mx,mn,' ')
c        call pggray(arr,10,10,1,10,1,10,mx,mn,tr)
        call pgclos

        return
        end



