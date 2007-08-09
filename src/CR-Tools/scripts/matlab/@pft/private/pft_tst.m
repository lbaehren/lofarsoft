close all;
clear all;
fft_size=1024;

zz=zeros(1,fft_size/2);
scale=2^19;
scale_mul=2^17;

n_max=1E10;

%generate phase switching pattern
%s=[zeros(1,n_max/2),ones(1,n_max/2)];
%s=s(randperm(n_max));
%s=zeros(1,n_max);
t=0:fft_size-1;

for n=1:n_max
    x=1E-4*complex(randn(1,fft_size),randn(1,fft_size));
    %x=x+5E-4*complex(sin(2*pi*(0.125*t+rand(1))),sin(2*pi*(7/16*t+rand(1))));       
    
    % phase switching
    if mod(n,2)
        x=conj(x);
    end    
    z=pft(x);
    %z=fft(x)/fft_size;
    
    % quantize   
    z=round(scale*z)/scale;
    [y1,y2] =split_fft(z);
    %y2=-y2;
    
    % undo phase switching    
    if mod(n,2)
        y2=-y2;
    end    
    % quantize
    y1=round(y1*scale_mul)/scale_mul;
    y2=round(y2*scale_mul)/scale_mul;    
    zz=zz+y1.*conj(y2);
    zzz=zz*scale^2/n;
    rms_re(n)=sqrt(mean(abs(real(zzz)).^2));
    rms_im(n)=sqrt(mean(abs(imag(zzz)).^2));

    if(mod(n,1000)==0)
        
        figure(1);        
        stairs((0:fft_size/2-1)/fft_size*160,[real(zzz).',imag(zzz).']);
        xlabel('freq (MHz)');
        ylabel('correlation');
        grid on; 
        title(sprintf('N=%i (simple phase switching, noise only)',n));

        figure(2);        
        semilogx(20*log10([rms_re',rms_im']));
        grid on;        
        xlabel('time');
        ylabel('avg power (dB)');
        
        figure(3);        
        semilogx(20*log10([rms_re',rms_im']));
        grid on;        
        xlabel('time');
        ylabel('avg power (dB)');
        hist([real(zzz)',imag(zzz)']);        
        grid on;
        
        drawnow;                
    end;
end

    
    
