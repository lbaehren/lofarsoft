
        implicit none
        integer x,n,intx,inty,seed
        real*8 sizepix,beampix,flux,a6,xposn,yposn
        real*8 image(50,50)
        character f1*500

        seed=-8332

        x=50
        n=30
        sizepix=10
        beampix=3
        flux=1.d0
        a6=45.d0
        xposn=322.5
        yposn=322.5

        call get_subim_dubsrc(image,x,n,sizepix,beampix,flux,a6,
     /             xposn,yposn,intx,inty,seed)
        
        f1='trial'
        call writearray_asc(image,x,x,n,n,f1,'mv')

        end
