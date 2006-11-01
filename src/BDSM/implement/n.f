c! This is suite of very basic image processing tasks. Creates noise images, source
c! images, and operates on them. Fairly self explanatory. 
c!     ------ R. Niruj Mohan, 2005, Sterrewacht, Leiden.

        implicit none
        integer m,n
        character opt*2,err*1
        

        write (*,*)
        write (*,*) '  All output images will be saved'
        write (*,*) '  Enter inputs in a single line '
333     continue
        write (*,*)    
        write (*,'(a15,$)') '   Option ---> '
        read (*,*) opt

        end
