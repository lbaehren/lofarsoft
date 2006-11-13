%  Display PSF reconstruction results
% Works only for a pupil 128*128 sampling

psf_resolution_factor = 4;  % should be 4
psf_sample_angle = 14.1807;
pick1=middle(reconstructed_PSF,psf_resolution_factor);


xx=257:385;
reconstructed_sqrt_psf=sqrt(reconstructed_PSF(pick1,pick1));
reconstructed_psfline=reconstructed_sqrt_psf(:,64);

subplot(311)
% mesh(reconstructed_sqrt_psf);

imagesc(reconstructed_sqrt_psf);
axis off
axis square
ax=axis;
ax(2)=length(reconstructed_sqrt_psf);
ax(4)=length(reconstructed_sqrt_psf);
axis(ax);
title( ['Reconstructed \surd{PSF}  Strehl ratio=' num2str(reconstructed_strehl,'%5.3f') ] );

% 
% 
% subplot(321)
% mesh(integrated_psf);

subplot(323)



plot(sqrt(reconstructed_PSF(214:300,257))./max(sqrt(reconstructed_PSF(:))),'r-*','MarkerSize',2)
hold on

ax=axis;
ax(2)=length(reconstructed_PSF(214:300,257));
ax(3)=min(sqrt(reconstructed_PSF(214:300,257))./max(sqrt(reconstructed_PSF(:))))-.05;
ax(4)=1;
axis(ax);
legend('Reconstructed \surd{PSF} X-cut')
subplot(324)



plot(sqrt(reconstructed_PSF(257,214:300))./max(sqrt(reconstructed_PSF(:))),'r-*','MarkerSize',2)
hold on

ax=axis;
ax(2)=length(reconstructed_PSF(257,214:300));
ax(3)=min(sqrt(reconstructed_PSF(257,214:300))./max(sqrt(reconstructed_PSF(:))))-.05;
ax(4)=1;
axis(ax);
legend('reconstructed \surd{PSF} Y-cut')

subplot(325)
hold on
log_B=log(reconstructed_B(129:256,129));
plot(1:128,log_B,'r-*','MarkerSize',2)
legend('reconstructed Log(OTF) X-cut')

ax=axis;
coords=isinf(log_B);

log_B(coords)=0;
ax(2)=length(log_B);
ax(3)=min(log_B)-1;
ax(4)=1;
axis(ax)

subplot(326)

plot(log(reconstructed_B(129:256,129)),'r-*','MarkerSize',2)
axis(ax)
legend('reconstructed Log(OTF) Y-cut')

