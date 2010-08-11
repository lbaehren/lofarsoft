c! arranges arr in descending order and rearranges brr and crr
c! where arr barr and carr are all real*8
      SUBROUTINE sort3_1(x,n,arr,brr,crr)
      INTEGER n,M,NSTACK,x
      real*8 arr(x)
      integer brr(x),crr(x)
      PARAMETER (M=7,NSTACK=200)
      INTEGER i,ir,j,jstack,k,l,istack(NSTACK)
      REAL*8 a,temp
      integer b,btemp

      jstack=0
      l=1
      ir=n
1     if(ir-l.lt.M)then
        do 12 j=l+1,ir
          a=arr(j)
          b=brr(j)
          c=crr(j)
          do 11 i=j-1,1,-1
            if(arr(i).ge.a)goto 2
            arr(i+1)=arr(i)
            brr(i+1)=brr(i)
            crr(i+1)=crr(i)
11        continue
          i=0
2         arr(i+1)=a
          brr(i+1)=b
          crr(i+1)=c
12      continue
        if(jstack.eq.0)return
        ir=istack(jstack)
        l=istack(jstack-1)
        jstack=jstack-2
      else
        k=(l+ir)/2
        temp=arr(k)
        arr(k)=arr(l+1)
        arr(l+1)=temp
        btemp=brr(k)
        brr(k)=brr(l+1)
        brr(l+1)=btemp
        btemp=crr(k)
        crr(k)=crr(l+1)
        crr(l+1)=btemp
        if(arr(l+1).lt.arr(ir))then
          temp=arr(l+1)
          arr(l+1)=arr(ir)
          arr(ir)=temp
          btemp=brr(l+1)
          brr(l+1)=brr(ir)
          brr(ir)=btemp
          btemp=crr(l+1)
          crr(l+1)=crr(ir)
          crr(ir)=btemp
        endif
        if(arr(l).lt.arr(ir))then
          temp=arr(l)
          arr(l)=arr(ir)
          arr(ir)=temp
          btemp=brr(l)
          brr(l)=brr(ir)
          brr(ir)=btemp
          btemp=crr(l)
          crr(l)=crr(ir)
          crr(ir)=btemp
        endif
        if(arr(l+1).lt.arr(l))then
          temp=arr(l+1)
          arr(l+1)=arr(l)
          arr(l)=temp
          btemp=brr(l+1)
          brr(l+1)=brr(l)
          brr(l)=btemp
          btemp=crr(l+1)
          crr(l+1)=crr(l)
          crr(l)=btemp
        endif
        i=l+1
        j=ir
        a=arr(l)
        b=brr(l)
        c=crr(l)
3       continue
          i=i+1
        if(arr(i).gt.a)goto 3
4       continue
          j=j-1
        if(arr(j).lt.a)goto 4
        if(j.lt.i)goto 5
        temp=arr(i)
        arr(i)=arr(j)
        arr(j)=temp
        btemp=brr(i)
        brr(i)=brr(j)
        brr(j)=btemp
        btemp=crr(i)
        crr(i)=crr(j)
        crr(j)=btemp
        goto 3
5       arr(l)=arr(j)
        arr(j)=a
        brr(l)=brr(j)
        brr(j)=b
        crr(l)=crr(j)
        crr(j)=c
        jstack=jstack+2
        if(jstack.gt.NSTACK) then
         write (*,*) 'NSTACK too small in sort3_1'
         stop
        end if
        if(ir-i+1.le.j-l)then
          istack(jstack)=ir
          istack(jstack-1)=i
          ir=j-1
        else
          istack(jstack)=j-1
          istack(jstack-1)=l
          l=i
        endif
      endif
      goto 1
      END
