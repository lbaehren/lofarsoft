function this = pf1(p)
% constructor for prefilter 1
this.p=p;

fnam=sprintf(p.pf1_coeff_file,p.fft_size,p.pf1_nof_taps,p.pf1_ipol,p.pf1_coeff_w);
[coeff,scale]=pf_coeff(p.fft_size,p.pf1_nof_taps,p.pf1_ipol,p.pf1_coeff_w);

this.coeff = coeff;
this.rscale= 1/scale;
this.nxt_out_dat=[];

for ia=1:p.nof_antennas
    for ip=1:p.nof_polarizations
        for ib=1:p.fft_size
          this.state{ia,ip,ib}=zeros(1,p.pf1_nof_taps-1);
        end
    end
end
this=node(class(this, 'pf1'));