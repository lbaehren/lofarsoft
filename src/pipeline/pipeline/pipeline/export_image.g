#!/usr/bin/env glish

include 'viewer.g'

dp:=dv.newdisplaypanel();

for(i in 3:len(argv))
{
    path:=split(argv[i],'/');
    imfile:=path[len(path)];

    print paste(argv[i], "->", spaste(imfile, ".ps"));

    dd:=dv.loaddata(argv[i],'raster');
    dp.register(dd);

    dd.setoptions([titletext=[value=imfile]]);
    dd.setoptions([axislabelswitch=[value=T],xgridtype=[value='Full grid'],ygridtype=[value='Full grid']]);
    #dd.setoptions([colormode=[value=colormap]]); <-- seems to screw things up, cannot do 'Adjust' in the GUI after this.
    dd.setoptions([colormap=[value='Hot Metal 2']]);
    dd.setoptions([wedge=[value=T]]);

    man:=dp.canvasprintmanager();
    man.writeps(spaste(imfile, ".ps"), 'a4', F, 300);

    dp.unregister(dd);
}

dp.done();
exit;
