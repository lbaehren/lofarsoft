
	implicit none
	integer i
	character str1*100,str2*100


	open(unit=21,file='addb',status='old')
	open(unit=22,file='name',status='unknown')
	open(unit=23,file='nick',status='unknown')
	open(unit=24,file='email',status='unknown')
	 do 100 i=1,225
          read (21,'(a100)') str1
	  call changeover(str1,str2)
100	 continue
	close(22)
	close(23)
	close(24)
	close(21)
	
	end



	subroutine changeover(st1,st2)
	implicit none
	character st1*100,st2*100,name*30,nick*30,email*40
	integer i,j

	do 100 i=1,100
         if (st1(i:i).eq.'(') then
	  name=st1(1:i-1)
	  do 110 j=i+1,100
           if (st1(j:j).eq.')') then
	    nick=st1(i+1:j-1)
	    email=st1(j+1:100)
	    goto 120
	   end if
110       continue

	 end if
100     continue

120     continue
          st2=nick//' '//name//' '//email
	 write (22,*) nick 
	 write (23,*) name 
	 write (24,*) email 

	return
	end

