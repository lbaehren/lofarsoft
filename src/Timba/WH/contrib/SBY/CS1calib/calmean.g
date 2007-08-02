include 'image.g'
include 'os.g'

spid:=3
fid:=1


startch:=32
my_img_restored:=sprintf("d%d_%d.img",startch,spid)
#my_img_restored:=sprintf("d%d_%d.img.obs",startch,spid)
#my_img_restored:=sprintf("d%d_%d.residual",startch,spid)
defimg:="orig.img"
dos.copy(my_img_restored,defimg);
im1:=image(defimg);

startch:=startch+8;
count:=1;
while (startch<224) {
# check for bad channels
if (startch != 4 && startch != 5) {
my_img_restored:=sprintf("d%d_%d.img",startch,spid)
#my_img_restored:=sprintf("d%d_%d.img.obs",startch,spid)
#my_img_restored:=sprintf("d%d_%d.residual",startch,spid)
im2:=image(my_img_restored);
im1.calc('$im1+$im2');
count:=count+1;
im2.close();
}

startch:=startch+8
}

im1.close();
im1:=image(defimg);
avg_str:=sprintf("%s/%d",defimg,count)
print count
im1.calc(avg_str)
#im1.view()
im1.tofits('mean_res.fits')
im1.close()

dos.remove(defimg)
