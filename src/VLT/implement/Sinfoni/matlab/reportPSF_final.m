%  Display PSF reconstruction results
% Works only for a pupil 128*128 sampling

psf_resolution_factor = parameters.psf_resolution_factor;
psf_sample_angle = parameters.psf_sample_angle;
pick1=middle(reconstructed_PSF,psf_resolution_factor);
pick2=middle(integrated_corrected_psf,psf_resolution_factor);


xx=257:385;
reconstructed_sqrt_psf=sqrt(reconstructed_PSF(pick1,pick1));
integrated_psf=sqrt(integrated_corrected_psf(pick2,pick2));
reconstructed_psfline=reconstructed_sqrt_psf(:,64);
psfline=integrated_psf(:,64);
real_psf=integrated_corrected_psf;
psf_init_circ=circ_average(real_psf);

subplot(322)
% mesh(reconstructed_sqrt_psf);

imagesc(reconstructed_sqrt_psf);
axis off
axis square
ax=axis;
ax(2)=length(reconstructed_sqrt_psf);
ax(4)=length(reconstructed_sqrt_psf);
axis(ax);
title( ['Reconstructed \surd{PSF} =' num2str(reconstructed_strehl,'%5.3f') ] );

% 
% 
subplot(321)
% mesh(integrated_psf);
imagesc(integrated_psf)
axis off
axis square

ax=axis;
ax(2)=length(integrated_psf);
ax(4)=length(integrated_psf);
axis(ax);
title( ['Real \surd{PSF} Strehl ratio = ' num2str(integrated_strehl,'%5.3f') ] );
subplot(323)



plot(sqrt(reconstructed_PSF(214:300,257))./max(sqrt(reconstructed_PSF(:))),'r-*','MarkerSize',2)
hold on
plot(sqrt(real_psf(214:300,257))./max(sqrt(real_psf(:))),'-.')
plot(sqrt(reconstructed_PSF(214:300,257))./max(sqrt(reconstructed_PSF(:)))-sqrt(real_psf(214:300,257))./max(sqrt(real_psf(:))),'--')
ax=axis;
ax(2)=length(reconstructed_PSF(214:300,257));
ax(3)=min(sqrt(reconstructed_PSF(214:300,257))./max(sqrt(reconstructed_PSF(:)))-sqrt(real_psf(214:300,257))./max(sqrt(real_psf(:))))-.05;
ax(4)=1;
axis(ax);
title('\surd{PSF} X-cut')

subplot(324)



plot(sqrt(reconstructed_PSF(257,214:300))./max(sqrt(reconstructed_PSF(:))),'r-*','MarkerSize',2)
hold on
plot(sqrt(real_psf(257,214:300))./max(sqrt(real_psf(:))),'-.')
plot(sqrt(reconstructed_PSF(257,214:300))./max(sqrt(reconstructed_PSF(:)))-sqrt(real_psf(257,214:300))./max(sqrt(real_psf(:))),'--')

ax=axis;
ax(2)=length(reconstructed_PSF(257,214:300));
ax(3)=min(sqrt(reconstructed_PSF(257,214:300))./max(sqrt(reconstructed_PSF(:)))-sqrt(real_psf(257,214:300))./max(sqrt(real_psf(:))))-.05;
ax(4)=1;
axis(ax);
legend('reconstructed \surd{PSF}','real \surd{PSF}','difference between the 2 \surd{PSF}')
title('\surd{PSF} Y-cut')
subplot(325)
log_B_init=log(real_B(xx,257)./max(real_B(:)));

plot(log_B_init,'-.')
hold on
log_B=log(reconstructed_B(129:256,129));
plot(1:128,log_B,'r-*','MarkerSize',2)
title('Log(OTF) X-cut')
ax=axis;
coords=isinf(log_B);

log_B(coords)=0;
ax(2)=length(log_B);
ax(3)=min(log_B)-1;
ax(4)=1;
axis(ax)

subplot(326)
plot(log(real_B(xx,257)./max(real_B(:))),'-.')
hold on
plot(log(reconstructed_B(129:256,129)),'r-*','MarkerSize',2)
axis(ax)
title('Log(OTF) Y-cut')
