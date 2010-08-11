c! fit stuff
c! break number ct into sections and fit each by power law. cos easier this way. 
c! if nbin is 100 then get 5 % error. 

        implicit none
        integer i,ia(2),j,i1,nbin
        real*8 s(500),n(500),dif(500),sig(500)
        real*8 alamda,chisq,covar(2,2),alpha(2,2),a(2)
        real*8 xfit(500),yfit(500)
        real*4 x4(500),y4(500),y41(500),ydif4(500)
        real*4 mnx,mxx,mny,mxy,ex(500),a1(500),a2(500)
        
        data ia/1,1/
        data a/1.0,-1.0/
        nbin=100

        open(unit=21,file='trial',status='old')
        do 100 i=1,500
         read (21,*) s(i),n(i),dif(i)
         dif(i)=n(i)
100     continue
        close(21)

        call pgbegin(0,'/xs',1,1)
        do 200 i=1,500
         x4(i)=dlog10(s(i))
         y4(i)=dlog10(dif(i))
200     continue
        call pgvport(0.1,0.9,0.5,0.9)
        call range2(x4,500,500,mnx,mxx)
        call range2(y4,500,500,mny,mxy)
        call pgwindow(mnx,mxx,mny,mxy)
        call pgbox('BCNLST',0.0,0,'BCLNST',0.0,0)
        call pgpoint(500,x4,y4,20)
        call pgtext(mnx+0.5*(mxx-mnx),mxy-0.2*(mxy-mny),
     /               'data - thick line')
        call pgtext(mnx+0.5*(mxx-mnx),mxy-0.4*(mxy-mny),
     /               'fit  - thin line (shifted)')

        do 210 i=1,500-(nbin-1),nbin
         do 220 j=i,i+nbin-1
         i1=j-i+1
         xfit(i1)=dlog10(s(j))
         yfit(i1)=dlog10(dif(j))
         x4(i1)=xfit(i1)
         sig(i1)=1.d0
220      continue

         alamda=-1.d0
         do 420 j=1,20
          call mrqmin(xfit,yfit,sig,nbin,nbin,a,ia,2,covar,alpha,2,
     /        chisq,alamda,2)
420      continue
         alamda=0.d0
          call mrqmin(xfit,yfit,sig,nbin,nbin,a,ia,2,covar,alpha,2,
     /        chisq,alamda,2)
         ex((i-1)/nbin+1)=xfit(1)
         a1((i-1)/nbin+1)=a(1)
         a2((i-1)/nbin+1)=a(2)
        
         do 230 j=i,i+nbin-1
          i1=j-i+1
          y41(i1)=(a(1)+xfit(i1)*a(2))
          ydif4(i1)=((10.d0**y41(i1))-(10.d0**yfit(i1)))
     /             /(10.d0**yfit(i1)) *1.d2
          y41(i1)=y41(i1)-0.5
230      continue

         call pgvport(0.1,0.9,0.5,0.9)
         call pgwindow(mnx,mxx,mny,mxy)
         call pgbox('BCNLST',0.0,0,'BCLNST',0.0,0)
         call pgline(nbin,x4,y41)

         call pgvport(0.1,0.9,0.3,0.5)
         call pgwindow(mnx,mxx,-10.0,10.0)
         call pgbox('BCNLST',0.0,0,'BCVNST',0.0,0)
         call pgline(nbin,x4,ydif4)
210     continue
        call pglabel(' ','Residue (%)',' ')

        call pgvport(0.1,0.9,0.2,0.3)
        call range2(a1,int(500/nbin),int(500/nbin),mny,mxy)
        call pgwindow(mnx,mxx,mny,mxy)
        call pgbox('BCNLST',0.0,0,'BCVNST',0.0,0)
        call pgline(int(500/nbin),ex,a1)
        call pglabel(' ','Constant',' ')

        call pgvport(0.1,0.9,0.1,0.2)
        call range2(a2,int(500/nbin),int(500/nbin),mny,mxy)
        call pgwindow(mnx,mxx,mny,mxy)
        call pgbox('BCNLST',0.0,0,'BCVNST',0.0,0)
        call pgline(int(500/nbin),ex,a2)
        call pglabel(' ','Slope',' ')

        call pgend


        return
        end

