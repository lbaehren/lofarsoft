
        implicit none
        character str*500
        integer num


        str='   23  222.0 3 4 5  6   '
        call num_num(str,num)
        write (*,*) num
        str='23  222.0 3 4 5  6   '
        call num_num(str,num)
        write (*,*) num
        str='23  222.0 3 4 5  6'
        call num_num(str,num)
        write (*,*) num

        return
        end


        
