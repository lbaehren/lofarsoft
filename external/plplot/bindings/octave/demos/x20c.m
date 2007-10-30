## $Id: x20c.m 7259 2006-12-04 15:06:06Z andrewross $
##
##	plimage demo
##
##

1;

##dbg = 0;
##nosombrero = 0;
##nointeractive = 1;
##f_name = "";


function ix20c
  xdim = 260;
  ydim = 220;


  ## Bugs in plimage():
  ##  -at high magnifications, the left and right edge are ragged, try
  ##  ./x20c -dev xwin -wplt 0.3,0.3,0.6,0.6 -ori 0.5
  ##   
  ## Bugs in x20c.c:
  ##  -if the window is resized after a selection is made on "lena", when
  ##   making a new selection the old one will re-appear.

  ## Parse and process command line arguments

  ##  plMergeOpts(options, "x20c options", NULL);
  ##  plparseopts(&argc, argv, PL_PARSE_FULL);

  ## Initialize plplot

  plinit;

  ## view image border pixels
  if (exist("dbg","var"))
    plenv(1., xdim, 1., ydim, 1, 1);  ## no plot box
    
    ## build a one pixel square border, for diagnostics
    z = zeros(xdim,ydim);
    z(:,ydim) = 1;  ## right
    z(:,1) = 1;     ## left
    z(0,:) = 1;     ## top
    z(xdim,:) = 1;  ## bottom
  
    pllab("...around a blue square."," ","A red border should appear...");

    pplimage(z, 1.0, xdim, 1.0, ydim, 0.0, 0.0, 1.0, xdim, 1.0, ydim);

    pladv(0);
  endif

  ## sombrero-like demo
  if (!exist("nosombrero","var"))
    plcol0(2); ## draw a yellow plot box, useful for diagnostics!
    plenv(0.0, 2.0*pi, 0, 3.0*pi, 1, -1);

    i=0:xdim-1;
    x = i*2.0*pi/(xdim-1);
    i=0:ydim-1;
    y = i*3.0*pi/(ydim-1);

    for i=1:xdim
      r(i,:) = sqrt(x(i)*x(i)+y.*y)+1e-3;
      z(i,:) = sin(r(i,:)) ./ r(i,:);
    endfor

    pllab("No, an amplitude clipped \"sombrero\"", "", "Saturn?");
    plptex(2., 2., 3., 4., 0., "Transparent image");
    pplimage(z, 0., 2.*pi, 0, 3.*pi, 0.05, 1., 0., 2.*pi, 0, 3.*pi); 

    ## save the plot
    if (exist("f_name","var"))
      save_plot(f_name);
    endif
    pladv(0);
  endif

  ## read Lena image
  if (exist("lena.pgm","file"))
    [ret, img, width, height, num_col] = read_img("lena.pgm");
  elseif (exist("../lena.pgm","file"))
    [ret, img, width, height, num_col] = read_img("../lena.pgm");
  else
    plabort("No such file");
    plend1;
    return;
  endif

  ## set gray colormap
  gray_cmap(num_col);

  ## display Lena
  plenv(1., width, 1., height, 1, -1);

  if (!exist("nointeractive","var"))
    pllab("Set and drag Button 1 to (re)set selection, Button 2 to finish."," ","Lena...");
  else
    pllab(""," ","Lena...");
  endif
  
  pplimage(img, 1.0, width, 1.0, height, 0.0, 0.0, 1.0, width, 1.0, height);

  ## selection/expansion demo
  if (!exist("nointeractive","var"))
      [ret, xi, xe, yi, ye] = get_clip;    ## get selection rectangle
      if (ret == 1)
	plend1;
	return;
      endif
  
    ## 
    ## I'm unable to continue, clearing the plot and advancing to the next
    ## one, without hiting the enter key, or pressing the button... help!
    ##
    ## Forcing the xwin driver to leave locate mode and destroying the
    ## xhairs (in GetCursorCmd()) solves some problems, but I still have
    ## to press the enter key or press Button-2 to go to next plot, even
    ## if a pladv() is not present!  Using plbop() solves the problem, but
    ## it shouldn't be needed! 
    ##

    ## plbop();

    ## 
    ## plspause(0), pladv(0), plspause(1), also works,
    ## but the above question remains.
    ## With this approach, the previous pause state is lost,
    ## as there is no API call to get its current state.
    ##

    plspause(0);
    pladv(0);

    ## display selection only
    pplimage(img, 1.0, width, 1.0, height, 0.0, 0.0, xi, xe, ye, yi);

    plspause(1);
    pladv(0);

    ## zoom in selection
    plenv(xi, xe, ye, yi, 1, -1);
    pplimage(img, 1.0, width, 1.0, height, 0.0, 0.0, xi, xe, ye, yi);
    pladv(0);
  endif  

  plend1;
endfunction

## read image from file in binary ppm format
function [status, img_f, width, height, num_col] = read_img(fname)

  ## naive grayscale binary ppm reading.
  if ((fp = fopen(fname,"rb")) == -1)
    status = 1;
    img_f = [];
    width = 0;
    height = 0;
    num_col = 0;
    return;
  endif

  ver = fgetl(fp); ## version

  if (strcmp(ver, "P5") == 0) ## I only understand this!
    status = 1;
    img_f = [];
    width = 0;
    height = 0;
    num_col = 0;
    return;
  endif

  while((i=fread(fp,1,"char")) == "#")
    fgetl(fp); ## comments
  endwhile
  fseek(fp,-1,SEEK_CUR);

  width = fscanf(fp,"%d",1); ## width
  height = fscanf(fp,"%d",1); ## height
  num_col = fscanf(fp,"%d\n",1); ## num colors

  img = fread(fp, [width,height],"uchar");
  fclose(fp);

  img_f = fliplr(img);

  status = 0;
endfunction


## save plot
function save_plot(fname)   

  cur_strm = plgstrm;   ## get current stream
  new_strm = plmkstrm;  ## create a new one
    
  plsdev("psc"); ## new device type. Use a known existing driver
  plsfnam(fname); ## file name

  plcpstrm(cur_strm, 0); ## copy old stream parameters to new stream
  plreplot;	## do the save
  plend1; ## close new device

  plsstrm(cur_strm);	## and return to previous one
endfunction

##  get selection square interactively
function [ret, xi, xe, yi, ye] = get_clip

  start = 0;
  sx = zeros(5,1);
  sy = zeros(5,1);
  st = plxormod(1); ## enter xor mode to draw a selection rectangle

  xi = 200.; xe = 330.;
  yi = 280.; ye = 220.;

  keysym = 0;

  if (st)  ## driver has xormod capability, continue
    while(1) 
      st = plxormod(0);
      [retval, state, keysym, button, string, pX, pY, dX, dY, wX, wY, subwin]  = plGetCursor();
      st = plxormod(1);

      if (button == 1) 
	xi = wX; yi = wY;
	if (start)
	  plline(sx, sy); ## clear previous rectangle
	endif

	start = 0;

	sx(1) = xi; sy(1) = yi;
	sx(5) = xi; sy(5) = yi;
      endif
      
      if (state && 0x100) 
	xe = wX; ye = wY;
	if (start)
	  plline(sx, sy); ## clear previous rectangle
	endif
	
	start = 1;
	
	sx(3) = xe; sy(3) = ye;
	sx(2) = xe; sy(2) = yi;
	sx(4) = xi; sy(4) = ye;
	plline(sx, sy); ## draw new rectangle
      endif

      ## Can't use PLK_Return here since PLK_ constant are not 
      ## currently defined for the octave bindings
      if (button == 3 || keysym == 0x0D || keysym == 'Q')
	if (start)
	  plline(sx, sy); ## clear previous rectangle
	endif
	break;
      endif      
    endwhile
    st = plxormod(0); ## leave xor mod
  endif

  if (xe < xi) 
    t=xi; xi=xe; xe=t;
  endif

  if (yi < ye)
    t=yi; yi=ye; ye=t;
  endif

  ret = (keysym == 'Q');
endfunction

## set gray colormap
function gray_cmap(num_col)

  r(1) = g(1) = b(1) = 0.0;
  r(2) = g(2) = b(2) = 1.0;
    
  pos(1) = 0.0;
  pos(2) = 1.0;

  plscmap1n(num_col);
  plscmap1l(1, pos', r', g', b', zeros(2,1));
endfunction

ix20c;
