
        subroutine readheader4fits(filen,extn,ctype,crpix,cdelt,crval,
     /         crota,bmaj,bmin,bpa,nn,scratch)
        implicit none
        integer nn,error,nchar
        real*8 crpix(nn),cdelt(nn),crval(nn),crota(nn)
        real*8 bmaj,bmin,bpa,keyvalue
        character ctype(nn)*8,extn*20,filen*500,keyword*500,keystrng*500
        character comment*500,scratch*500
        
        if (nn.ne.3) write (*,*) '  !! readheader4fits problem !!!'

        extn='.header'
        keyword='CTYPE1'
        call get_keyword(filen,extn,keyword,keystrng,keyvalue,
     /           comment,'s',scratch,error) 
        ctype(1)=keystrng(1:nchar(keystrng))
        keyword='CTYPE2'
        call get_keyword(filen,extn,keyword,keystrng,keyvalue,
     /           comment,'s',scratch,error) 
        ctype(2)=keystrng(1:nchar(keystrng))
        keyword='CTYPE3'
        call get_keyword(filen,extn,keyword,keystrng,keyvalue,
     /           comment,'s',scratch,error) 
        ctype(3)=keystrng(1:nchar(keystrng))
        keyword='CRPIX1'
        call get_keyword(filen,extn,keyword,keystrng,crpix(1),
     /           comment,'r',scratch,error) 
        keyword='CRPIX2'
        call get_keyword(filen,extn,keyword,keystrng,crpix(2),
     /           comment,'r',scratch,error) 
        keyword='CRPIX3'
        call get_keyword(filen,extn,keyword,keystrng,crpix(3),
     /           comment,'r',scratch,error) 
        keyword='BMAJ'
        call get_keyword(filen,extn,keyword,keystrng,bmaj,
     /           comment,'r',scratch,error) 
        keyword='BMIN'
        call get_keyword(filen,extn,keyword,keystrng,bmin,
     /           comment,'r',scratch,error) 
        keyword='BPA'
        call get_keyword(filen,extn,keyword,keystrng,bpa,
     /           comment,'r',scratch,error) 
        keyword='CRVAL1'
        call get_keyword(filen,extn,keyword,keystrng,crval(1),
     /           comment,'r',scratch,error) 
        keyword='CRVAL2'
        call get_keyword(filen,extn,keyword,keystrng,crval(2),
     /           comment,'r',scratch,error) 
        keyword='CRVAL3'
        call get_keyword(filen,extn,keyword,keystrng,crval(3),
     /           comment,'r',scratch,error) 
        keyword='CDELT1'
        call get_keyword(filen,extn,keyword,keystrng,cdelt(1),
     /           comment,'r',scratch,error) 
        keyword='CDELT2'
        call get_keyword(filen,extn,keyword,keystrng,cdelt(2),
     /           comment,'r',scratch,error) 
        keyword='CDELT3'
        call get_keyword(filen,extn,keyword,keystrng,cdelt(3),
     /           comment,'r',scratch,error) 
        keyword='CROTA1'
        call get_keyword(filen,extn,keyword,keystrng,crota(1),
     /           comment,'r',scratch,error) 
        keyword='CROTA2'
        call get_keyword(filen,extn,keyword,keystrng,crota(2),
     /           comment,'r',scratch,error) 
        keyword='CROTA3'
        call get_keyword(filen,extn,keyword,keystrng,crota(3),
     /           comment,'r',scratch,error) 

        return
        end


