#define MAIN_
#include "see.hpp"

/*--- To split filename from the path ----*/

char *SelectFileName (char * str)
{
 char *s = str;
 int len = (int)strlen(s);
 s += len;
 while (*(--s) != '/' && len--) ;
 return (++s);
 }
 
/*----------- To merge a string and a number -----------*/

char * glue ( char * s, int d )
{
 char * dest;
        dest = new char[MaxString+1];
	memset(dest, 0, MaxString+1);
	sprintf(dest, "%s%d", s, d);
	return dest;
	delete (dest);
}

/*--------- To average an array ----------------*/
     
long average ( float * mass, long sz )
{
 float sum;
 int ostatok = (sz%aver == 0 ? 0 : 1);

for (long i=0, j, k=0;i<sz;i+=aver, k++) {
 sum = 0.;
 for (j=i;j<(i+aver >= sz ? sz : i+aver);j++) sum += mass[j];
 mass[k] = sum / ((float)(j-i));
                                          }
 return (sz/aver + ostatok);    
}

/*--------- To smooth an array -----------------*/

long smoothing ( float * mass, long sz )
{
 float sum = 0., tmp; 
 smooth = (smooth > sz ? sz : smooth);

  for(long r=0;r<smooth;r++) sum += mass[r];
   tmp     = mass[0];
   mass[0] = sum/smooth; 
  for(long t=0;t<sz-smooth;t++) {
   sum = sum+mass[smooth+t]-tmp;
   tmp = mass[t+1];
   mass[t+1] = sum/smooth;
                                   }
 return (sz - smooth + 1);  
}
				       
/*==========================Prompting===================================*/

void Help ( char *s )
 {
  printf("\nPROGRAMM %s\n"
         "It is used to visualize given data\n\n"
	 "[USAGE]: %s [options] <file(s) with data>\n\n"
	 "[OPTIONS]:\n"
	 "-a, --aver   <value>          - average procedure with number of points = value\n"
	 "                                default - no\n"
	 "-s, --smooth <value>          - smooth procedure with number of points = value\n"
	 "                                default - no. If option -a is used first average procedure\n"
	 "                                is carried out and second --- smooth procedure\n"
	 "-l, --left   <left boundary>  - left boundary of used window, default = 0\n"
	 "-w, --size   <size of window> - size of used window, default - equal to size of file\n"
	 "-d, --down   <value>          - lower limit of y-values to draw, default - min y-value\n"
	 "-u, --up     <value>          - upper limit of y-values to draw, default - max y-value\n"
	 "-t, --type   <type of record> - type of record of data, i.e. for example, float, unsigned short\n"
	 "                                or char. If you want to plot data recorded in unsigned char format,\n"
	 "                                you have to write in command line: -t uc , where uc means unsigned\n"
	 "                                char. In this programm following groups of symbols are provided:\n"
	 "                                  uc - unsigned char\n"
         "                                  sc - signed char\n"
	 "                                  us - unsigned short\n"
	 "                                  ss - signed short\n"
	 "                                  ui - unsigned int\n"
	 "                                  si - signed int\n"
	 "                                  ul - unsigned long\n"
	 "                                  sl - signed long\n"
	 "                                   f - float\n"
	 "                                   d - double\n"
	 "                                  ld - long double\n"
	 "                                default - f (float)\n"
#if HAVE_STRUCT_DECL_IN_TEMPL_FUNC	 
	 "--conv                        - convert data from Host Byte Order to Network Byte Order\n"
	 "                                and vice versa;  default - no\n"
#endif	 
	 "--seq                         - to view all files sequently, one by one pressing some key\n"
	 "--cycle                       - with option --seq makes the process of files preview non-stop\n"
	 "--group      <number>         - with option --seq allows to show simultaneous the number files.\n"
	 "                                The number must be >= 1 and <= the quantity of all files.\n"
	 "                                Default, number is 1\n"
	 "--timer      <time>           - with option --seq (and maybe --cycle) sets time interval between\n"
	 "                                replacements one file to another in automatic mode. So, this\n"
	 "                                option plays role of slide show. If time < 0, slide show is off\n"
	 "--shift      <value>          - value to shift data from different files one from another,\n"
	 "                                default = 0 (don't shift)\n"
	 "--nocover                     - don't cover previous file(s) by the next one(s)\n"
	 "                                default - cover\n"
	 "-o, --source <filename>       - name of file with graphics resources to use, default - programm\n"
	 "                                will use file \".See\" in your home directory. If your file or\n"
	 "                                file \".See\" don't exist it will be created\n"
	 "--path       <path>           - path to the output file(s), default - the same\n"
	 "                                where are the source files\n"
	 "-b, --block  <size of block>  - size of block to work in case of large files.\n"
	 "                                If it's pointed then the image is forming by\n"
	 "                                appending consecutively the blocks of selected size;\n"
	 "                                default - equal to size of file (in points).\n"
	 "--refresh    <time>           - sets time interval (in ms) to reread file if it is modified.\n"
	 "                                If time < 0, file won't be reread. Default, time = 5000\n"
	 "--postscript                  - don't visualize data but create postscript file at once\n"
	 "--header <bytes>              - skip header in bytes, default - 0\n"
	 "-h, --help                    - help\n\n"
	 "[SPECIAL OPTIONS]:\n"
	 "-bg, -background <color>      - set the background color of full window ignoring the\n"
	 "                                X resource file .See\n"
	 "-fg, -foreground <color>      - set the foreground color in full window ignoring the\n"
	 "                                X resource file .See\n"
	 "-display <display>            - set the display to show window, for example odessa:0 ,\n"
	 "                                where odessa is a hostname, 0 - display number\n"
	 "-fn, -font <font>             - set the font in full window\n"
	 "-xrm <\"resource:value\">       - this option allows to set some value for some resource.\n"
	 "                                For example, option -xrm \"See.height:300\" allows to set\n"
	 "                                the height of full window to the value of 300\n\n"
	 "[KEYS]:\n"
	 "<q>          - quit\n"
	 "<Space>      - zoom region\n"
	 "<Esc>        - unzoom region for one level back\n"
	 "<i>          - save image in ppm-format file. If there is only one source file then name of\n"
	 "               the output file is \"<source filename>-<number of zoom iterations>.ppm\".\n"
	 "               Otherwise, the name of the output file is \"<name of the first source file>--\n"
	 "               <name of the second source file>-<number of zoom iterations>.ppm\".\n"
	 "               You can also define path to the output file(s) using option --path\n"
	 "<t>          - save visible data in txt-file(s). If you draw several files then you will get\n"
	 "               several txt-files in the output. Name of the each output file is\n"
	 "               <source filename>-<number of zoom iterations>.txt\n"
	 "               You can also define path to the output file(s) using option --path\n"
	 "<b>          - save visible data in binary format file(s). This key is analogous to the <t>-key\n"
	 "               but for output file name has at the end \".bin\" rather than \".txt\"\n"
	 "<l>          - save visible data to the postscript file (landscape). This key is analogous to the <t>-key\n"
	 "               but for output file name has at the end \".ps\" rather than \".txt\"\n"
	 "<p>          - the same as <p> but save to the postscript Portrait file\n"
	 "<g>          - save visible data to the gif file\n"
	 "<Left>       - move window to left\n"
	 "<Right>      - move to right\n"
	 "<+>          - increase the size of window\n"
	 "<->          - decrease the size of window\n"
	 "<PgUp>       - increase the rate to move (or change size) the window\n"
	 "<PgDn>       - decrease the rate to move (or change size) the window\n"
	 "<Tab>        - change the active boundary of window to show y-value in\n"
	 "               the footnote string of X-window\n"
	 "<Enter>      - view next file if option --seq is used\n"
	 "<BackSpace>  - view previous file if option --seq is used\n\n"
	 "[SPECIAL KEYS]:\n"
	 "<LCtrl | RCtrl> - redraw the full window\n"
	 "<LCtrl + RCtrl> - switch on/off the mode of auto redrawing of full window\n"
	 "                  if BackingStore mode of Window Manager doesn't work\n\n"
	 "[MOUSE]:\n"
	 "<Button1>    - move window\n"
	 "<Button2>    - increase size of window\n"
	 "<Button3>    - decrease size of window\n\n", s, s);
  exit(0);
 }
 
/*===================Parsing the command line===========================*/

int ParseCmdLine ( void )
 {  
  int op;
  char *endptr;
  
  struct option long_options[] = { {"help", no_argument, 0, 'h'},
				   {"aver", required_argument, 0, 'a'},
				   {"smooth", required_argument, 0, 's'},
				   {"letf", required_argument, 0, 'l'},
				   {"size", required_argument, 0, 'w'},
				   {"down", required_argument, 0, 'd'},
				   {"up", required_argument, 0, 'u'},
				   {"type", required_argument, 0, 't'},
				   {"source", required_argument, 0, 'o'},
				   {"block", required_argument, 0, 'b'},
				   {"shift", required_argument, 0, 10},
				   {"path", required_argument, 0, 11},
                                   {"nocover", no_argument, 0, 12},
#if HAVE_STRUCT_DECL_IN_TEMPL_FUNC	 
                                   {"conv", no_argument, 0, 13},
#endif				   
                                   {"seq", no_argument, 0, 14},
                                   {"refresh", required_argument, 0, 15},
                                   {"cycle", no_argument, 0, 16},
                                   {"timer", required_argument, 0, 17},
                                   {"group", required_argument, 0, 18},
                                   {"postscript", no_argument, 0, 19},
                                   {"header", required_argument, 0, 20},
				   {0, 0, 0, 0}
                                  };
  
  while((op = getopt_long(ac, av, "t:a:s:hl:w:d:u:o:b:", long_options, 0)) != EOF)
    switch(op){
      case 'a':
        aver = strtol(optarg, &endptr, 10);	
	if ( strcmp(optarg, "\0") && strcmp(endptr, "\0") )
	 {
	   printf("\nError:\nWrong average key -a! ----> %s\n", optarg);
	   exit(1);
	  }
	if (aver <= 0) aver = 1;  
	break;
	
      case 's':
        smooth = strtol(optarg, &endptr, 10);	
	if ( strcmp(optarg, "\0") && strcmp(endptr, "\0") )
	 {
	   printf("\nError:\nWrong smooth key -s! ----> %s\n", optarg);
	   exit(1);
	  }
	if (smooth <= 0) smooth = 1;
	break;

      case 'l':
        le = atol(optarg);
	if (le < 0) le = 0; 
      break;
      
      case 'w':
        si = atol(optarg);
	if (si < 1) si = -1; 
      break;
      
      case 'd':
        minglob = (float)atof(optarg);
	exflag = (exflag == 2 ? 3 : 1);
      break;
      
      case 'u':
        maxglob = (float)atof(optarg);
	exflag = (exflag == 1 ? 3 : 2);
      break;

      case 't':
        memset(type, 0, 2);
	sprintf(type, "%s", optarg);
	if ( strcmp(type, "uc") && strcmp(type, "sc") && strcmp(type, "us") &&
	     strcmp(type, "ss") && strcmp(type, "ui") && strcmp(type, "si") &&
	     strcmp(type, "ul") && strcmp(type, "sl") && strcmp(type,  "f") &&
	     strcmp(type, "d") && strcmp(type, "ld") )
	 {
	  printf("\nError:\nWrong type of filerecord -t! ----> %s\n", type);
	  exit (1);
	  }

	razm = ( !strcmp(type, "uc") ? sizeof(unsigned char)  : ( !strcmp(type, "sc") ? sizeof(signed char)  :
	       ( !strcmp(type, "us") ? sizeof(unsigned short) : ( !strcmp(type, "ss") ? sizeof(signed short) :
	       ( !strcmp(type, "ui") ? sizeof(unsigned int)   : ( !strcmp(type, "si") ? sizeof(signed int)   :
	       ( !strcmp(type, "ul") ? sizeof(unsigned long)  : ( !strcmp(type, "sl") ? sizeof(signed long)  :
	       ( !strcmp(type,  "f") ? sizeof(float)          : ( !strcmp(type,  "d") ? sizeof(double)       :
	       ( !strcmp(type, "ld") ? sizeof(long double)    : sizeof(float) )))))))))));
        break;
	
      case 'o':
	memset(XResourceFile, 0, MaxString+1);
	sprintf(XResourceFile, "%s", optarg);
      break;
      
      case 'b':
        block = atol(optarg);
	if (block <= 0) block = -1;
      break;
      
      case 10:
        Shift = (float)atof(optarg);
      break;
	
      case 11:
	  if( OutputPath ) {
	    OutputPath = 0;
	    free(OutputPath);
	                          }
	  if( !( OutputPath = (char *)calloc((int)strlen(optarg), sizeof(char *)) ) )
	    { printf("Not enough memory!\n"), exit(1); }
	  memcpy(OutputPath, optarg, (int)strlen(optarg));
      break;
      
      case 12:
        NoCover = 1;
      break;
      
#if HAVE_STRUCT_DECL_IN_TEMPL_FUNC	 
      case 13:
        Convert = 1;
      break;
#endif

      case 14:
        Seq = true;
      break;
      
      case 15:
        timint = atol(optarg);
      break;
      
      case 16:
        Cycle = true;
      break;
      
      case 17:
        slide_time  = atol(optarg);
      break;
      
      case 18:
        Group  = atol(optarg);
      break;
      
      case 19:
        Post  = true;
      break;
      
      case 20:
        header = atoi(optarg);
	if (header < 0) header = 0;
      break;
      
      case 'h':
        Help(av[0]);
      break;

      case '?':
        Help(av[0]);
      break;      
              }

  return(optind);
 }

/*-------- To determine the appropriate size of data block ------------------*/

long DataBlockDefinition ( void )
{
  // Obtain the allowed memory size for use
  // If 0 was returned => maxblock < 0 and => the block size is equal to the file size
  unsigned long availmem = getAvailableMemory();
  // empirical constant (memory size roughly used for the variables)
  unsigned long konstanta = 1500 * 1024;  
  long maxblock = ( availmem - konstanta - (next+1 != ac ? (NoCover == 1 ? 4 : 3)*(ac-next) : 0)*sizeof(short int) - sizeof(short int)*(next+1 != ac && NoCover == 1 ? 2 - smooth : 0) ) / ( sizeof(float) + razm + (next+1 != ac && NoCover == 1 ? sizeof(short int)/aver : 0) );
  if (maxblock >= si || maxblock < 2) return si;
   else return maxblock;
}

/*======================= M A I N =========================================*/

int main ( int argc, char *argv[] )
{
 long win;
 struct stat info;

 if (argc == 1) Help(argv[0]);

 type               = new char[2];
 XBaseResourcesFile = new char[MaxString+1];
 XResourceFile      = new char[MaxString+1];
 OutputPath         = new char[MaxString+1];
 if (!type || !XBaseResourcesFile || !XResourceFile || !OutputPath) { printf("Not enough memory!\n"); exit(1); }
 memset(type, 0, 2);
 memset(XBaseResourcesFile, 0, MaxString+1);
 memset(XResourceFile, 0, MaxString+1);
 memset(OutputPath, 0, MaxString+1);
 sprintf(type, "%s", "f");
 sprintf(XBaseResourcesFile, "%s", ".BaseGraphics");
 sprintf(XResourceFile, "%s", ".See");
 sprintf(OutputPath, "%s", "\0");

 // options that should not be considered as graphical ones
 char *options[] = { "-d", "-t", "-b" };
 int opt_kind[] = { 1, 1, 1 }; // what are these options: 1 - with argument, 0 - without argument
 CommandLineOptions clo(argc, argv, options, opt_kind, 3);
 av = clo.remove();
 ac = clo.getN();

 ReadResources(ac, av);
 clo.update(ac, av);
 av = clo.add();
 ac = clo.getN();

 razm = sizeof(float);
 next =  ParseCmdLine();
 if ( ac <= next ) { printf("\nError:\nNot point the file(s)!\n"); exit(1); }

 for (long w=next;w<ac;w++) { 
  if ( stat(av[w], &info) != 0 ) { printf("Error:\nCouldn't open \"%s\"!\n", av[w]); exit(1); }
  win = ( w == next ? (long)((info.st_size-header)/razm) : ( (long)((info.st_size-header)/razm) > win ? (long)((info.st_size-header)/razm) : win));
 }

 if (Group < 1 || Group > ac-next) Group = 1;
 // If continious files changing is setup, then --shift is ignored (if Group=1)
 if (Seq && Group == 1) { Shift = 0.; NoCover = 0; }
 if (!Seq) { Cycle = false; slide_time = -1; Group = 1; } // if Seq is not set up, then Cycle=false and slide show is turned off
 
 if (le >= win) le = 0;
 if (si > win || si == -1) si = win;
 if (le + si > win) si -= (le + si - win);

 if (block == 1) block = 2;
 if (block == -1 || block > si) block = DataBlockDefinition();
 if (aver != 1) aver = (aver > si ? si : aver);
 if (aver != 1) block = (block <= aver ? aver : (block%aver != 0 && block != si ? aver * (long)(block/aver) : block));
 if (smooth != 1) smooth = (smooth > (long)(block/aver) ? (long)(block/aver) : smooth);
 iter = (si == block ? 1 : ((si-aver*(smooth != 1 ? smooth - 1 : 0))%(block-aver*(smooth != 1 ? smooth - 1 : 0)) == 0 ? (si-aver*(smooth != 1 ? smooth - 1 : 0))/(block-aver*(smooth != 1 ? smooth - 1 : 0)) : (si-aver*(smooth != 1 ? smooth - 1 : 0))/(block-aver*(smooth != 1 ? smooth - 1 : 0)) + 1) );

 if (block != si) printf("-- Attention ! -- block working mode switch on / blksize = %ld /\n", block);

 m = new float[block];
 if ( !m ) { printf("Not enough memory!\n"); exit(1); }

 Plotting();
  
 delete [] av;
 delete(type);
 delete(m);
 delete(XBaseResourcesFile);
 delete(XResourceFile);
 delete(OutputPath);

 return 0;
}
 
