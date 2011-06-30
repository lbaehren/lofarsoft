
        subroutine filledges(ar,x,y,n,m,bs,mask)
        implicit none
        integer n,m,bs,x,y,mask(x,y)
        real*8 ar(x,y)

        call fillup(ar,x,y,n,m,1,1,(bs+1)/2,(bs+1)/2,bs,mask)   
        call fillup(ar,x,y,n,m,n-bs/2+1,1,n-bs/2,(bs+1)/2,bs,mask)
        call fillup(ar,x,y,n,m,1,m-bs/2+1,(bs+1)/2,m-bs/2,bs,mask)
        call fillup(ar,x,y,n,m,n-bs/2+1,m-bs/2+1,n-bs/2,m-bs/2,bs,mask)

        return
        end


        subroutine fillup(arr1,x,y,n,m,blc1,blc2,v1,v2,bs,mask)
        implicit none
        integer n,m,blc1,blc2,v1,v2,bs,i,j,x,y,mask(x,y)
        real*8 arr1(x,y)

        do 100 i=blc1,blc1+bs/2-1
         do 110 j=blc2,blc2+bs/2-1
          arr1(i,j)=arr1(v1,v2)
          mask(i,j)=1
110      continue
100     continue

        return
        end


        subroutine fillbt(arr,x,y,n,m,bs,mask)
        implicit none
        integer n,m,bs,i,j,x,y,mask(x,y)
        real*8 arr(x,y)
        
        do 150 i=(bs+1)/2,n-(bs+1)/2+1     ! bottom and top
         do 160 j=1,bs/2
          arr(i,j)=arr(i,(bs+1)/2)
          mask(i,j)=1
          arr(i,m-bs/2+j)=arr(i,m-bs/2)
          mask(i,m-bs/2+j)=1
160      continue
150     continue

        return
        end


        subroutine filllr(arr,x,y,n,m,bs,mask)
        implicit none
        integer n,m,bs,i,j,x,y,mask(x,y)
        real*8 arr(x,y)
        
        do 170 i=1,bs/2                            ! left and right
         do 180 j=(bs+1)/2,m-(bs+1)/2+1
          arr(i,j)=arr((bs+1)/2,j)
          mask(i,j)=1
          arr(n-bs/2+i,j)=arr(n-bs/2,j)
          mask(n-bs/2+i,j)=1
180      continue
170     continue

        return
        end


