#define XW_
#include "see.hpp"

/*------- Функция преобразования байтов из (!) HOST в NETWORK и наоборот (!) для
       типов данных с разным размером -------------------------------------------*/

#if HAVE_STRUCT_DECL_IN_TEMPL_FUNC
template <class T>
T nh2hn ( T data, int razm )
{
  char tmp;
  union { T d;
          unsigned char uc[sizeof(T)];
         } b;
  b.d = data;
  for (int i=0;i<razm/2;i++) { tmp = b.uc[i];
                               b.uc[i] = b.uc[razm-1-i];
			       b.uc[razm-1-i] = tmp;
                              }
  return b.d;           
 }
#endif
 
/*----------- Функция считывания файла ----------------------*/

template <class tFormat>
void Read ( tFormat m1, float *m, long l, long siz, int file, char *name )
{
 struct stat inf;
  stat(name, &inf);
  if (l+siz > (long)((inf.st_size-header)/razm)) siz = (long)((inf.st_size-header)/razm) - l;
 
 if ( (file = open(name, O_RDONLY | O_LARGEFILE)) == -1 ) printf("Error:\nCouldn't open \"%s\"!\n", name), exit(1); 
       lseek(file, header+l*razm, SEEK_SET);
       read(file, m1, siz*razm);	     
       close(file);

  memset(m, 0, siz*sizeof(float));
  
#if HAVE_STRUCT_DECL_IN_TEMPL_FUNC
  for(int u=0;u<siz;u++) m[u] = ( Convert == 0 ? (float)m1[u] : ( razm == 1 ? (float)m1[u] : (float)nh2hn(m1[u], razm) ));
#else
  for(int u=0;u<siz;u++) m[u] = (float)m1[u];
#endif
 }
 
/*---------- Функция для обеспечения процедуры считывания ----------*/

void ReadProc ( long j, long lg, long wr )
{
 if (!strcmp(type, "uc")) TYPE(j, unsigned, char, lg, wr)
 if (!strcmp(type, "sc")) TYPE(j, signed, char, lg, wr)
 if (!strcmp(type, "us")) TYPE(j, unsigned, short, lg, wr)
 if (!strcmp(type, "ss")) TYPE(j, signed, short, lg, wr)
 if (!strcmp(type, "ui")) TYPE(j, unsigned, int, lg, wr)
 if (!strcmp(type, "si")) TYPE(j, signed, int, lg, wr) 
 if (!strcmp(type, "ul")) TYPE(j, unsigned, long, lg, wr) 
 if (!strcmp(type, "sl")) TYPE(j, signed, long, lg, wr) 
 if (!strcmp(type, "f"))  TYPE(j, float, , lg, wr) 
 if (!strcmp(type, "d"))  TYPE(j, double, , lg, wr) 
 if (!strcmp(type, "ld")) TYPE(j, long, double, lg, wr)  
}

/*=================================================================================


                             Г Р А Ф И К А 


==================================================================================*/

/*-------- Функция создания файла общих граф. ресурсов ----------------------*/

void CreateBaseGraphicsFile (char * filename) {

 FILE *cfg = fopen(filename, "wt");
             fseek(cfg, 0, SEEK_SET);	    
fprintf(cfg, "! This is a X resource file with base resources\n\n"
             ".display                      : :0\n"
             "*.borderWidth                 : 0\n"
             "*.font                        : 6x10\n"
             "*.mainBox.background          : lightblue\n"
	     "*.mainBox.foreground          : black\n\n"
	     
             "*.PlotBox.background          : #d5d5d5\n"
             "*.PlotBox.foreground          : darkgreen\n"
             "*.PlotBox.borderWidth         : 1\n\n"
	     
             "*.header.height               : 15\n"
             "*.header.background           : lightblue\n"
             "*.header.foreground           : #004800\n\n"
	     
             "*.legend.height               : 15\n"
             "*.legend.background           : lightblue\n"
             "*.legend.foreground           : #004800\n\n"
	     
             "*.hAxe.height                 : 35\n"
             "*.hAxe.background             : lightblue\n"
             "*.hAxe.foreground             : black\n"
             "*.hAxe.name                   : X-axis\n"
             "*.hAxe.scale                  : 1.\n"
             "*.hAxe.shift                  : 0.\n\n"
	     
             "*.vAxe.width                  : 80\n"
             "*.vAxe.background             : lightblue\n"
             "*.vAxe.foreground             : black\n"
             "*.vAxe.scale                  : 1.\n"
             "*.vAxe.shift                  : 0.\n\n"
	     
             "*.vTitle.width                : 1\n"
             "*.vTitle.background           : lightblue\n"
             "*.vTitle.foreground           : black\n"
             "*.vTitle.name                 : Y   a x i s\n\n"
	     
             "*.footnote.height             : 30\n"
             "*.footnote.background         : lightblue\n"
             "*.footnote.foreground         : #565600\n");

             fclose(cfg);
}

/*-----  Функция создания файла ресурсов, специфичных для see -----*/

void CreateSeeXResources (char * filename) {

 FILE *cfg = fopen(filename, "wt");
             fseek(cfg, 0, SEEK_SET);	    
fprintf(cfg, "! This is a See resource file\n\n"
             "#include \".BaseGraphics\"\n\n"
             "See.height                    : 395\n"
             "See.width                     : 491\n"
             "See.minHeight                 : 180\n"
             "See.minWidth                  : 300\n\n"
	     
	     "! Extra resources for header region\n"
	     "! You can use one symbol %s in both of program and file resources\n"
	     "! to show program or file name\n\n"
             "*.header.program              : default\n"
             "*.header.file                 : default\n\n"
	     
	     "! Extra resources for legend region\n\n"
             "*.legend.keys                 : Help: /q/Space/Esc/+/-/->/<-/PgUp/PgDn/Tab/i/g/t/b/p/l/Enter/BSpace/LRCtrl/MOUSE/\n\n"
	     
	     "! Extra resources for footnote region\n\n"
             "*.footnote.first              : default\n"
             "*.footnote.second             : default\n\n"
	     
	     "! Extra resources for PlotBox region\n\n"
             "*.PlotBox.zoomer              : #808000\n"
	     "*.PlotBox.colorNumber         : 6\n"
	     "*.PlotBox.color1              : darkgreen\n"
	     "*.PlotBox.color2              : yellow\n"
	     "*.PlotBox.color3              : red\n"
	     "*.PlotBox.color4              : green\n"
	     "*.PlotBox.color5              : blue\n"
	     "*.PlotBox.color6              : black\n");

             fclose(cfg);
}

// Функция для считывания графических ресурсов из командной строки в базу данных
// получает ссылки на количество аргументов командной строки и ссылку
// на сами эти аргументы
// сначала создается граф. контекст, потом получаем дисплей вместе
// с разбором опций командной строки
void ReadResources (int& argc, char**& argv) {

  struct stat base_inf, inf;
  // определяем наличие файла общих ресурсов, если его нет, то создаем
  if ( stat(XBaseResourcesFile, &base_inf) == 0 ) ; else {
   memset(XBaseResourcesFile, 0, MaxString+1);
   sprintf(XBaseResourcesFile, "%s%s", getenv("HOME"), "/.BaseGraphics");
   if ( stat(XBaseResourcesFile, &base_inf) == 0 ) ; else CreateBaseGraphicsFile (XBaseResourcesFile);
  }
  if ( stat(XResourceFile, &inf) == 0 ) ; else {
   memset(XResourceFile, 0, MaxString+1);
   sprintf(XResourceFile, "%s%s", getenv("HOME"), "/.See");
   if ( stat(XResourceFile, &inf) == 0 ) ; else CreateSeeXResources (XResourceFile);
  }
  
  UpdateSeeXResources (XResourceFile, XBaseResourcesFile);

  XtToolkitInitialize();
  // создаем графический контекст
  appcont = XtCreateApplicationContext();

  // получаем дисплей для данного граф. контекста
  if ((DisId = XtOpenDisplay(appcont, NULL, "See", "See", NULL, 0, &argc, argv)) == NULL) {
   printf("XtOpenDisplay: can't initialize display!\n");
   exit(1);
  }

  // создаем базу данных для данного дисплея, в которую будут включены ресурсы
  // из командной строки
  XrmDatabase args = XtDatabase(DisId);
  // сливает вместе ресурсы дисплея, командной строки и файла ресурсов
  XrmCombineFileDatabase(XResourceFile, &args, False);
}

// получает корневой виджет
// функция заполняет необходимые ресурсы одних виджетов
// через известные другие (высота, ширина и т.д.)
void UpdateResources ( Widget targetW ) {

 // Имеем:
 Position headerY, vTitleX;
 Dimension headerH, vTitleW, vAxeW, legendH, plotBoxBorderW, hAxeH, footnoteH, Height, Width;
 Arg tmpX, tmpY, tmpH, tmpW;
 
 // получаем заданные параметры
 XtSetArg(tmpH, XtNheight, &Height);
 XtSetArg(tmpW, XtNwidth, &Width);
 XtVaGetValues(XtParent(targetW), tmpH, tmpW, NULL);

 XtSetArg(tmpY, XtNy, &headerY);
 XtSetArg(tmpH, XtNheight, &headerH);
 XtVaGetValues(XtNameToWidget(targetW, "header"), tmpY, tmpH, NULL);

 XtSetArg(tmpX, XtNx, &vTitleX);
 XtSetArg(tmpW, XtNwidth, &vTitleW);
 XtVaGetValues(XtNameToWidget(targetW, "vTitle"), tmpX, tmpW, NULL);

 XtSetArg(tmpW, XtNwidth, &vAxeW);
 XtVaGetValues(XtNameToWidget(targetW, "vAxe"), tmpW, NULL);

 XtSetArg(tmpH, XtNheight, &legendH);
 XtVaGetValues(XtNameToWidget(targetW, "legend"), tmpH, NULL);

 XtSetArg(tmpH, XtNheight, &hAxeH);
 XtVaGetValues(XtNameToWidget(targetW, "hAxe"), tmpH, NULL);

 XtSetArg(tmpH, XtNheight, &footnoteH);
 XtVaGetValues(XtNameToWidget(targetW, "footnote"), tmpH, NULL);

 XtSetArg(tmpW, XtNborderWidth, &plotBoxBorderW);
 XtVaGetValues(XtNameToWidget(targetW, "PlotBox"), tmpW, NULL);


 // устанавливаем другие параметры
 XtVaSetValues(XtNameToWidget(targetW, "header"), XtNx, vTitleW + vAxeW, XtNwidth, Width - vTitleW - vAxeW, NULL);

 XtVaSetValues(XtNameToWidget(targetW, "legend"), XtNx, vTitleW + vAxeW, XtNy, headerY + headerH, XtNwidth, Width - vTitleW - vAxeW, NULL);

 XtVaSetValues(XtNameToWidget(targetW, "footnote"), XtNx, vTitleW + vAxeW, XtNy, Height - footnoteH, XtNwidth, Width - vTitleW - vAxeW, NULL);

 XtVaSetValues(XtNameToWidget(targetW, "hAxe"), XtNx, vTitleW + vAxeW, XtNy, Height - footnoteH - hAxeH, XtNwidth, Width - vTitleW - vAxeW, NULL);

 XtVaSetValues(XtNameToWidget(targetW, "vTitle"), XtNy, headerY + headerH + legendH, XtNheight, Height - headerH - legendH - hAxeH - footnoteH, NULL);

 XtVaSetValues(XtNameToWidget(targetW, "vAxe"), XtNx, vTitleX + vTitleW, XtNy, headerY + headerH + legendH, XtNheight, Height - headerH - legendH - hAxeH - footnoteH, NULL);

 XtVaSetValues(XtNameToWidget(targetW, "PlotBox"), XtNx, vTitleW + vAxeW, XtNy, headerY + headerH + legendH, XtNwidth, Width - vTitleW - vAxeW - 2*plotBoxBorderW, XtNheight, Height - headerH - legendH - hAxeH - footnoteH - 2*plotBoxBorderW, NULL);

 if (Post) XtVaSetValues(XtParent(targetW), XtNmappedWhenManaged, False, NULL);
}

/*---------- Функция записи изображения в файл в формате PPM ---------------*/

void MakePPMImage ( Widget targetW, XtPointer tD )
{
 Dimension w, h;
 Display *disID = XtDisplay(targetW);
 Window winID = XtWindow(targetW);
 GC grC = XtGetGC(targetW, 0, NULL);
 XWindowAttributes att;
 XGetWindowAttributes(disID, winID, &att);
 w = att.width;
 h = att.height;
 
 int NumDepth, scrN = DefaultScreen(disID);
 Colormap clmp = DefaultColormap(disID, scrN);

 char * name;
       name = new char[MaxString+1];
       if ( !name ) { printf("Not enough memory!\n"); exit(1); }

       memset(name, 0, MaxString+1);

   if (!strcmp(OutputPath, "\0")) { if (next + 1 == ac) sprintf(name, "%s-%d%s", av[next], count, ".ppm");
        else sprintf(name, "%s--%s-%d%s", av[next], SelectFileName(av[ac-1]), count, ".ppm");
   } else { if (next + 1 == ac) sprintf(name, "%s/%s-%d%s", OutputPath, SelectFileName(av[next]), count, ".ppm");
             else sprintf(name, "%s/%s--%s-%d%s", OutputPath, SelectFileName(av[next]), SelectFileName(av[ac-1]), count, ".ppm");
           }

 FILE *o;
 if ( (o = fopen(name, "wt")) == NULL ) 
      { printf("\nError:\nCouldn't open file %s for writing!\n", name);
        exit(1);
       }
 fseek(o, 0, SEEK_SET);
 
 printf("Saving ppm image in file \"%s\" ... ", name);
 fflush(NULL);
 
 fprintf(o, "%s\n%d %d\n%d\n", "P6", w, h, 255);
 fclose(o);
       
 int out;
 if ( (out = open(name, O_APPEND | O_CREAT | O_WRONLY, 0644)) == -1 )
      { printf("\nError:\nCouldn't open file \"%s\" for writing!\n", name);
        exit(1);
       }
 lseek(out, 0, SEEK_SET);

 pict.init (disID, winID, 0, 0, w, h, clmp);

 for(int i=0;i<h;i++) for(int j=0;j<w;j++) {
   pict.GetRGB (i, j);
   write(out, pict.rgb,  pict.base * sizeof(unsigned char));
                                            }
 close(out);
 pict.close ();
  
 printf("- done.\n");
 
 delete(name);
}

/* to get pixel from the image */
int get_pixel (int x, int y) {
 return pict.get_pixel (x, y);
}

/*---------- Функция записи изображения в файл в формате GIF ---------------*/

void MakeGIFImage (Widget targetW, XtPointer tD) {
 Dimension w, h;
 Display *disID = XtDisplay(targetW);
 Window winID = XtWindow(targetW);
 GC grC = XtGetGC(targetW, 0, NULL);
 XWindowAttributes att;
 XGetWindowAttributes(disID, winID, &att);
 w = att.width;
 h = att.height;
 
 int NumDepth, scrN = DefaultScreen(disID);
 Colormap clmp = DefaultColormap(disID, scrN);

 char *name;
       name = new char[MaxString+1];
       if (!name) { printf("Not enough memory!\n"); exit(1); }

       memset(name, 0, MaxString+1);

   if (!strcmp(OutputPath, "\0")) { if (next + 1 == ac) sprintf(name, "%s-%d%s", av[next], count, ".gif");
        else sprintf(name, "%s--%s-%d%s", av[next], SelectFileName(av[ac-1]), count, ".gif");
   } else { if (next + 1 == ac) sprintf(name, "%s/%s-%d%s", OutputPath, SelectFileName(av[next]), count, ".gif");
             else sprintf(name, "%s/%s--%s-%d%s", OutputPath, SelectFileName(av[next]), SelectFileName(av[ac-1]), count, ".gif");
           }

 printf("Creating gif image in \"%s\" ... ", name);
 fflush(NULL);

 pict.init (disID, winID, 0, 0, w, h, clmp);

 /* create and set up the GIF-file */
 GIF_Create(name, w, h, pict.ncol, 8);
 pict.CreateColors();
 for (unsigned char i=0; i<pict.ncol; ++i) GIF_SetColor(i, pict.thecolors[i][0], pict.thecolors[i][1], pict.thecolors[i][2]);
 /* store the image, using the GetPixel function to extract pixel values */
 GIF_CompressImage(0, 0, -1, -1, get_pixel);
 /* finish it all and close the file */
 GIF_Close();

 pict.close();

 printf("- done.\n");
 
 delete(name);
}

/*--------- Функция записи выведенных данных в текстовый или бинарный файл ---------------------*/

void Save2File ( float * x, long left, long window, unsigned char form )
{
  FILE *txt;
  int out;
  long lg, wr, ttm, tmp = (block/aver) + (block%aver != 0 ? 1 : 0) - smooth + 1;
  struct stat inf;
  
  char * name;
         name = new char[MaxString+1];
         if ( !name ) { printf("Not enough memory!\n"); exit(1); }

  for (long j=next;j<ac;j++) {
   stat(av[j], &inf);
   for (long k=(long)floor((double)left/tmp);k<( (long)ceil((double)(left+window)/tmp) < iter ? (long)ceil((double)(left+window)/tmp) : iter );k++) {
   
    if (next + 1 == ac && noread == 1 && block == si) ;
      else { lg = (block-aver*(smooth != 1 ? smooth - 1: 0))*k;
             if (le+lg < (long)(inf.st_size/razm)) { wr = (lg + block > si ? si - lg : block);
	     
             ReadProc(j, le+lg, wr);
	     size = ( le+lg+wr > (long)(inf.st_size/razm) ? (long)(inf.st_size/razm) - le - lg : wr );
             if ( aver != 1 ) size = average(x, size);
             if ( smooth != 1 ) size = smoothing(x, size);
	     
					             } else size = 0;
            }
	    
   if (size != 0) { memset(name, 0, MaxString+1);

       if (!strcmp(OutputPath, "\0")) sprintf(name, "%s-%d%s", av[j], count, form == 0 ? ".txt" : ".bin");
        else sprintf(name, "%s/%s-%d%s", OutputPath, SelectFileName(av[j]), count, form == 0 ? ".txt" : ".bin");

       if (form == 0) { if ( (txt = fopen(name, (k == (long)floor((double)left/tmp) ? "wt" : "at") )) == NULL ) 
                         { printf("\nError:\nCouldn't open file \"%s\" for writing!\n", name);
                           exit(1);
                          }
                       } else { if ( (out = open(name, (k == (long)floor((double)left/tmp) ? O_TRUNC : O_APPEND) | O_CREAT | O_WRONLY, 0644)) == -1 )
                                 { printf("\nError:\nCouldn't open file \"%s\" for writing!\n", name);
                                   exit(1);
                                  }
		               }
  
       if (k == (long)floor((double)left/tmp)) { printf("Saving visual data in %s file \"%s\" ... ", form == 0 ? "text" : "binary", name); fflush(NULL); }
    
       ttm = (left-k*tmp+window < size ? left-k*tmp+window : size);
       for(long t=(left-k*tmp <= 0 ? 0 : left-k*tmp);t<(ttm >= tmp ? tmp : ttm);t++) {
         x[t] += (j-next)*Shift;
	 if (form == 0) fprintf(txt, "%ld %f\n", t + k*tmp, (x[t] > max[count] ? max[count] : (x[t] < min[count] ? min[count] : x[t])));
	  else write(out, (x[t] > max[count] ? &max[count] : (x[t] < min[count] ? &min[count] : &x[t])), sizeof(float));
                                                                          }
       if (form == 0) fclose(txt); else close(out);
                     }
                                                  }   printf("- done.\n");
                              }			      
   delete(name);
 }

/*=================================================================================


                 Г Р А Ф И Ч Е С К И Е     П О Д О К Н А      И

             Ф У Н К Ц И И      О Б Р А Б О Т К И     С О Б Ы Т И Й		 		                        


==================================================================================*/


/*------------------------ З А Г О Л О В О К --------------------------------

   Функция для вывода на экран содержания подокна заголовка

-----------------------------------------------------------------------------*/   

void WindowHeader ( Widget targetW, long a, bool ekr = true )
{
 typedef struct { char * prg, *file; } tit, *ptit;
 tit name;
  
 XtResource nameRes[] = { {"program", "Program", XtRString, sizeof(char *), XtOffset(ptit, prg),  XtRString, (void*)"default" },
                          {"file",    "File",    XtRString, sizeof(char *), XtOffset(ptit, file), XtRString, (void*)"default" } };
 XtGetApplicationResources(targetW, &name, nameRes, XtNumber(nameRes), NULL, 0);
 
 INIT_WIDGETS_PROPS     
 XFontStruct *cur_font = XQueryFont(disID, wData.font);
 fow = abs(cur_font->min_bounds.width);
 foa = cur_font->ascent;
 fod = cur_font->descent;
     
 if (ekr) XClearWindow(disID, winID);
    
 char prg[MaxString+1], file[MaxString+1], InfoString[MaxString+1];
 if (!strcmp(name.prg, "default")) sprintf(prg, "Programm: %s", av[0]);
  else sprintf(prg, name.prg, av[0]);
 if (!strcmp(name.file, "default")) sprintf(file, "File: %s", av[a]);
  else sprintf(file, name.file, av[a]);

 sprintf(InfoString, "%s   %s", prg, file);
 if (ekr) XDrawString(disID, winID, grC, 0, 3 + foa, InfoString, strlen(InfoString));
  else { if (h != 1) ps.string(att.x + 0, ps.getHeight() - 3 - foa, InfoString); }
 UNPLUG_WIDGETS_GC
}

/*-------------------------- Л Е Г Е Н Д А  ---------------------------------

   Функция для вывода на экран содержания подокна легенды (мини-хелпа)

-----------------------------------------------------------------------------*/

void Legend ( Widget targetW, bool ekr = true )
{
 typedef struct { char * keys; } tit, *ptit;
 tit name;
  
 XtResource nameRes[] = { {"keys",  "Keys",  XtRString, sizeof(char *), XtOffset(ptit, keys), XtRString, (void*)"Help: /q/Space/Esc/+/-/->/<-/PgUp/PgDn/Tab/i/g/t/b/p/l/MOUSE/" } };
 XtGetApplicationResources(targetW, &name, nameRes, XtNumber(nameRes), NULL, 0); 

 INIT_WIDGETS_PROPS
 if (ekr) XClearWindow(disID, winID);
    
 if (ekr) XDrawString(disID, winID, grC, 0, 3 + foa, name.keys, strlen(name.keys));
  else { if (h != 1) ps.string(att.x + 0, ps.getHeight() - att.y - 3 - foa, name.keys); }
 UNPLUG_WIDGETS_GC
}

/*---------------------   С Н О С К И ------------------------------------

  Функция для вывода на экран содержания подокна сносок

---------------------------------------------------------------------------*/  

void Footnote ( Widget targetW, XtPointer tD, bool ekr = true )
{
 typedef struct { char * first, * second; } tit, *ptit;
 tit name;
  
 XtResource nameRes[] = { {"first",  "First",  XtRString, sizeof(char *), XtOffset(ptit, first),  XtRString, (void*)"default" },
                          {"second", "Second", XtRString, sizeof(char *), XtOffset(ptit, second), XtRString, (void*)"default" } };
 XtGetApplicationResources(targetW, &name, nameRes, XtNumber(nameRes), NULL, 0);
 
 float * x;
 x = (float *)tD;
  
 long tmp = (block/aver) + (block%aver != 0 ? 1 : 0) - smooth + 1;
  
 INIT_WIDGETS_PROPS
 if (ekr) XClearWindow(disID, winID);

 char first[MaxString+1], second[MaxString+1];
 if (!strcmp(name.first, "default")) sprintf(first, "Pos: (%.5g , %.5g)", cur_x, cur_y);
  else sprintf(first, name.first);
    
 if (ekr) XDrawString(disID, winID, grC, 0, 3 + foa, first, strlen(first));
  else { if (h != 1) ps.string(att.x + 0, ps.getHeight() - att.y - 3 - foa, first); }

 if (!strcmp(name.second, "default")) {
  if (iter == 1 || ((tab == 0 ? lo : lo+okno) >= bll && (tab == 0 ? lo : lo+okno) <= blr)) 
    sprintf(second, "Left: %8ld   Window: %8ld   Y-value: %10.4g %s   Step: %8ld", lo, okno, (tab == 0 ? x[lo-bll] : x[lo-bll + okno]), (tab == 0 ? "(left) " : "(right)"), rate);
   else sprintf(second, "Left: %8ld   Window: %8ld   Step: %8ld", lo, okno, rate);
 } else sprintf(second, name.second);
   
 if (ekr) XDrawString(disID, winID, grC, 0, 2*(3 + foa) + fod, second, strlen(second));
  else { if (h != 1) ps.string(att.x + 0, ps.getHeight() - att.y - 2*(3 + foa) - fod, second); }
 UNPLUG_WIDGETS_GC
}

/*---------- З А Г О Л О В О К    В Е Р Т И К А Л Ь Н О Й     О С И ----------------------

    Функция для вывода на экран заголовка вертикальной оси

-----------------------------------------------------------------------------------------*/    

void vTitle ( Widget targetW, bool ekr = true )
{
 typedef struct { char * title; } tit, *ptit;
 tit name;
  
 XtResource nameRes[] = { {"name",  "Name",  XtRString, sizeof(char *), XtOffset(ptit, title), XtRString, (void*)"Y   |   a x i s" } };
 XtGetApplicationResources(targetW, &name, nameRes, XtNumber(nameRes), NULL, 0); 
  
 INIT_WIDGETS_PROPS      
 if (ekr) XClearWindow(disID, winID);

 char value[MaxString+1];
 int Nsp = 0, flag = 0, j = 0, len = 0;

  while ( name.title[j] != '\0') { if (name.title[j] == ' ' && flag == 0) { Nsp++; flag = 1; } else flag = 0;
                                   ++j; }
  Nsp++; flag = 0;

  if (!ekr) { ps.translate(att.x, ps.getHeight() - att.y - h); }

  if (ekr) {
   for (int i=0;i<Nsp;i++) {
     j = 0;
     memset(value, 0, MaxString+1);
     while ((name.title[len+j] != ' ' && name.title[len+j] != '\0') || flag == 1) { value[j] = name.title[len+j]; flag = 0; ++j; }
     if (name.title[len+j] == '\0') flag = 0; else { flag = 1; value[j] = '\0'; }
     len += strlen(value) + 1;
    XDrawString(disID, winID, grC, w - 3 - fow*strlen(value), h/2 - (Nsp*(foa+fod+1))/2 + foa + fod/2 + (foa + fod + 1)*i, value, strlen(value));
   }
  } else { ps.string(w-2-fod, h/2, name.title, true, 90); 
           ps.translate(-att.x, -ps.getHeight() + att.y + h);
          }
    
 UNPLUG_WIDGETS_GC
}

/*---------- В Е Р Т И К А Л Ь Н А Я    О С Ь ----------------------

    Функция для вывода на экран содержания подокна вертикальной оси

----------------------------------------------------------------------*/    

void VertAxe ( Widget targetW, bool ekr = true )
{
 typedef struct { float shift, scale; } par, *ppar;
                  
 par param;
 float def_scale = 1., def_shift = 0.;
  
 XtResource parRes[] = { {"scale", "Scale", XtRFloat,  sizeof(float),  XtOffset(ppar, scale), XtRFloat,  &def_scale },
			 {"shift", "Shift", XtRFloat,  sizeof(float),  XtOffset(ppar, shift), XtRFloat,  &def_shift } };
 XtGetApplicationResources(targetW, &param, parRes, XtNumber(parRes), NULL, 0); 

 INIT_WIDGETS_PROPS      
 if (ekr) XClearWindow(disID, winID);
 
 char value[MaxString+1];
 
 double step = (max[count] - min[count])/3;

 if (!ekr) { ps.translate(att.x, ps.getHeight() - att.y - h);
             ps.begin(w-5, 1, 1, 10);
           }
   
    sprintf(value, "%.3g", min[count]*param.scale + param.shift);
    if (ekr) { XDrawLine(disID, winID, grC, w-5, h-1, w, h-1);
               XDrawString(disID, winID, grC, w-8 - fow*(strlen(value)), h-2, value, strlen(value));
    } else { ps.lineto(w, 1);
             ps.string(w-8 - fow*(strlen(value)), 2, value);
            }
     sprintf(value, "%.3g", max[count]*param.scale + param.shift);
    if (ekr) XDrawLine(disID, winID, grC, w-5, 0, w, 0);
     else ps.line(w-5, h, w, h);
     for (int t=1;t<5;t++) if (ekr) XDrawLine(disID, winID, grC, w-2, t*(h/3/5), w, t*(h/3/5));
                            else ps.line(w-2, h - t*(h/3/5), w, h - t*(h/3/5));
    if (ekr) XDrawString(disID, winID, grC, w-8 - fow*(strlen(value)), foa, value, strlen(value));
     else ps.string(w-8 - fow*(strlen(value)), h - foa, value);
    
   for(int i=1;i<3;i++)
   {
    sprintf(value, "%.3g", (min[count] + i*step)*param.scale + param.shift);
    if (ekr) XDrawLine(disID, winID, grC, w-5, h - i*(h/3), w, h - i*(h/3));
     else ps.line(w-5, i*(h/3), w, i*(h/3));
     for (int t=1;t<5;t++) if (ekr) XDrawLine(disID, winID, grC, w-2, h - i*(h/3) + t*(h/3/5), w, h - i*(h/3) + t*(h/3/5));
                            else ps.line(w-2, i*(h/3) - t*(h/3/5), w, i*(h/3) - t*(h/3/5));
    if (ekr) XDrawString(disID, winID, grC, w-8 - fow*(strlen(value)), h-2 - i*(h/3), value, strlen(value));
     else ps.string(w-8 - fow*(strlen(value)), 2 + i*(h/3), value);
    }
    
 if (!ekr) { ps.end();
             ps.translate(-att.x, -ps.getHeight() + att.y + h);
            }
 UNPLUG_WIDGETS_GC
}

/*------------ Функция определения максимума среди всех файлов ------------------*/

void Vertic ( Widget targetW, XtPointer tD, int resize, bool ekr = true )
{
  float * x;
  x = (float *)tD;

  struct stat inf;
  long lg, wr, ttm, ttm1, tmp = (block/aver) + (block%aver != 0 ? 1 : 0) - smooth + 1;

  if (resize != 1) {  
  for (long j=next+cur_file; j<(Seq ? (next + cur_file + Group > ac ? ac : next + cur_file + Group) : ac); j++) {
   stat(av[j], &inf);

   for (long k=(long)floor((double)left/tmp); k<( (long)ceil((double)(left+window)/tmp) < iter ? (long)ceil((double)(left+window)/tmp) : iter ); k++) {

    if (next + 1 == ac && noread == 1 && block == si) ;
      else { lg = (block-aver*(smooth != 1 ? smooth - 1 : 0))*k;
             if (le+lg < (long)(inf.st_size/razm)) { wr = (lg + block > si ? si - lg : block);
                                                     bll = k * tmp;
						     ttm1 = (long)inf.st_size/razm/aver + (((long)inf.st_size/razm/aver)%aver == 0 ? 0 : 1) - smooth + 1;
	                                             blr = ( (k+1) * tmp < (left+window > ttm1 ? ttm1 : left+window) ? (k+1) * tmp - 1 : (left+window > ttm1 ? ttm1 : left+window) );

             ReadProc(j, le+lg, wr);
	     size = ( le+lg+wr > (long)(inf.st_size/razm) ? (long)(inf.st_size/razm) - le - lg : wr );
             if ( aver != 1 ) size = average(x, size);
             if ( smooth != 1 ) size = smoothing(x, size);
	     
					            } else { size = 0;
						             bll = -1; blr = -1;
						            }
	     if (next + 1 == ac) noread = 1;
            }

    ttm = (left-k*tmp+window < size ? left-k*tmp+window : size);
    for(long t=(left-k*tmp <= 0 ? 0 : left-k*tmp); t<(ttm >= tmp ? tmp : ttm); t++) {
      x[t] += (j-next-cur_file)*Shift;
      max[count] = (t==left%tmp && k== (long)floor((double)left/tmp) && j==next+cur_file ? x[t] : (x[t] > max[count] ? x[t] : max[count]));
      min[count] = (t==left%tmp && k== (long)floor((double)left/tmp) && j==next+cur_file ? x[t] : (x[t] < min[count] ? x[t] : min[count]));
    }
                                                         }
                              }

  switch (exflag) {
    case 1: min[count] = (minglob <= max[count] ? minglob : min[count]);
            break;    
    case 2: max[count] = (maxglob >= min[count] ? maxglob : max[count]);
            break;
    case 3: float tmp;
            if (maxglob < minglob) { tmp = maxglob; maxglob = minglob; minglob = tmp; }
	    tmp = min[count];
            min[count] = (minglob <= max[count] ? minglob : min[count]);
	    max[count] = (maxglob >= tmp ? maxglob : max[count]);
            break;
   default: break;
  }
                     } // resize != 1

  VertAxe(targetW, ekr);
}

/*---------- Г О Р И З О Н Т А Л Ь Н А Я    О С Ь ----------------------

    Функция для вывода на экран содержания подокна горизонтальной оси

----------------------------------------------------------------------*/

void HorizAxe ( Widget targetW, bool ekr = true )
{
 typedef struct { char * title;
                  float shift, scale; } tit, *ptit;
 tit name;
 float def_scale = 1., def_shift = 0.;
  
 XtResource nameRes[] = { {"name",  "Name",  XtRString, sizeof(char *), XtOffset(ptit, title), XtRString, (void*)"X-axis" },
                          {"scale", "Scale", XtRFloat,  sizeof(float),  XtOffset(ptit, scale), XtRFloat,  &def_scale },
			  {"shift", "Shift", XtRFloat,  sizeof(float),  XtOffset(ptit, shift), XtRFloat,  &def_shift } };
 XtGetApplicationResources(targetW, &name, nameRes, XtNumber(nameRes), NULL, 0); 
  
 INIT_WIDGETS_PROPS
 if (ekr) XClearWindow(disID, winID);
 
 char value[MaxString+1];
 double begin, step;

 begin = (double)left;
 step = ((double)window)/4.;

 if (!ekr) { ps.translate(att.x, ps.getHeight() - att.y - h);
             ps.begin(0, h+1, 1, 10);
           }
 
  sprintf(value, "%.2f", begin*name.scale + name.shift);
  if (ekr) XDrawLine(disID, winID, grC, 0, 0, 0, 5);
   else ps.lineto(0, h - 4);
  for (int t=1;t<5;t++) if (ekr) XDrawLine(disID, winID, grC, t*(w/4/5), 0, t*(w/4/5), 1);
                         else ps.line(t*(w/4/5), h+1, t*(w/4/5), h - 1);
  if (ekr) XDrawString(disID, winID, grC, 0, 5+3 + foa, value, strlen(value));
   else ps.string(0, h - 5 - 3 - foa, value);
  sprintf(value, "%.2f", (begin+window)*name.scale + name.shift);
  if (ekr) XDrawLine(disID, winID, grC, w-1, 0, w-1, 5);
   else ps.line(w-1, h+1, w-1, h - 4);
  if (ekr) XDrawString(disID, winID, grC, w-1 - fow*(strlen(value)), 5+3 + foa, value, strlen(value));
   else ps.string(w-1 - fow*(strlen(value)), h - 5 - 3 - foa, value);

  for (int i=1;i<4;i++)
   {
    sprintf(value, "%.2f", (begin + i*step)*name.scale + name.shift);
    if (ekr) XDrawLine(disID, winID, grC, i*(w/4), 0, i*(w/4), 5);
     else ps.line(i*(w/4), h+1, i*(w/4), h - 4);
    for (int t=1;t<5;t++) if (ekr) XDrawLine(disID, winID, grC, i*(w/4) + t*(w/4/5), 0, i*(w/4) + t*(w/4/5), 1);
                           else ps.line(i*(w/4) + t*(w/4/5), h+1, i*(w/4) + t*(w/4/5), h - 1);
    if (ekr) XDrawString(disID, winID, grC, i*(w/4) - (strlen(value)*fow)/2, 5+3 + foa, value, strlen(value));
     else ps.string(i*(w/4) - (strlen(value)*fow)/2, h - 5 - 3 - foa, value);
    }
    
  sprintf(value, name.title);
  if (ekr) XDrawString(disID, winID, grC, w/2 - (strlen(value)*fow)/2, 5 + 2*(3 + foa) + fod, value, strlen(value));
   else ps.string(w/2, h - 5 - 2*(3 + foa) - fod, value, true);
  
 if (!ekr) { ps.end();
             ps.translate(-att.x, -ps.getHeight() + att.y + h);
            }
  
 UNPLUG_WIDGETS_GC
}

/*------------  Д Е С Т Р У К Т О Р    О К О Н  --------------------

    Функция для корректного закрытия окон при выходе из программы

-----------------------------------------------------------------------*/    

void Destroy ( Widget targetW, XtPointer tD, XEvent *event, Boolean *cont )
{
  if (event->type == DestroyNotify ||
      event->type == KeyPress || event->type == KeyRelease)
  {
    *cont = False;
    delete (float *)tD;
    if (next + 1 != ac && (!Seq || (Seq && Group > 1))) { delete [] keepcur; if (NoCover == 1) { delete (keep); } }
    delete (type);
    delete (XBaseResourcesFile);
    delete (XResourceFile);
    delete (OutputPath);
    delete (mtstatus);
    XtDestroyWidget (targetW);
    XtDestroyApplicationContext (XtWidgetToApplicationContext(targetW));  
    exit(0);
  }
}                                      

/*---------- Обработчик событий для движения пойнтера внутри окна для рисования ------------------*/

void PointerPos ( Widget targetW,  XtPointer tD, XEvent *event, Boolean *cont )
{
 if (event->type == MotionNotify)
  {
    XWindowAttributes attr;
    XGetWindowAttributes(XtDisplay(targetW), XtWindow(targetW), &attr);

    cur_x = (float)( left + window * ((float)(event->xmotion.x + attr.border_width)) / ((float)pw) );
    cur_y = min[count] + (max[count]-min[count]) * ((float)(ph - event->xmotion.y)) / ((float)ph) ;
    Footnote(XtNameToWidget(XtParent(targetW), "footnote"), tD);
   }
 }                                                              

/*-----------  РАМКА - БЕГУНОК для увеличения/уменьшения размера
                    выводимых на экран данных -----------------------------*/ 

void MapBegunok ( Widget targetW )
{
  int sh;
  
  typedef struct { Pixel fg; } frame, *pFrame;
  frame fr;

  XtResource myRes[] = { {"zoomer", "Zoomer", XtRPixel, sizeof(Pixel), XtOffset(pFrame, fg), XtRString, (void*)"black"} };
  XtGetApplicationResources(targetW, &fr, myRes, XtNumber(myRes), NULL, 0); 

  Display *disID = XtDisplay(targetW);
  Window winID   = XtWindow(targetW);
  GC grC         = XtGetGC(targetW, 0, NULL);
  
  XSetForeground(disID, grC, fr.fg);
  XSetFunction(disID, grC, GXxor);      
  XSetLineAttributes(disID, grC, 1, LineOnOffDash, CapButt, JoinRound);  
  
  int lg = (int)((((float)(lo-left))*((float)pw))/(float)window), 
      rg = (int)((((float)(lo-left+okno))*((float)pw))/(float)window);

  XDrawLine(disID, winID, grC, lg, 0, lg, ph);  
  if ( rg == pw) sh = 1; else sh = 0;
  if ( lg != rg - sh) XDrawLine(disID, winID, grC, rg - sh, 0, rg - sh, ph);   
  XDrawLine(disID, winID, grC, lg, ph/2, rg - sh, ph/2);

  XtReleaseGC(targetW, grC);
 }

/*---------- Обработчик событий для движения РАМКИ - БЕГУНКА -------------------------*/

void MoveBegunok ( Widget targetW,  XtPointer tD, XEvent *event, Boolean *cont )
{
 if (event->type == MotionNotify)
  {   
    MapBegunok(targetW);
    
    long Pos = (long)(((float)(event->xmotion.x)/(float)pw)*window);
    if (Pos + okno > window) Pos = window - okno;
    if (Pos < 0) Pos = 0;
    lo = left + Pos;

    MapBegunok(targetW);
    Footnote(XtNameToWidget(XtParent(targetW), "footnote"), tD);
   }
 }                                                              

/*------------ Обработчик событий для изменения размера РАМКИ - БЕГУНКА ---------------*/
 
void ResizeBegunok ( Widget targetW, XtPointer tD, XEvent *event, Boolean *cont )
 {
  if (event->type == ButtonPress)
   {        
     XEvent ev;
     struct timespec delay;
     delay.tv_sec  = 0;
     delay.tv_nsec = 100000000;
     
        switch(event->xbutton.button){
	        case 2: do {
		             MapBegunok(targetW);
			     nanosleep(&delay, NULL);
	                     if((lo-left) + okno + rate > window) ; else okno += rate;
                             MapBegunok(targetW);
                             Footnote(XtNameToWidget(XtParent(targetW), "footnote"), tD);    
			     delay.tv_sec = 0;
			     delay.tv_nsec = 50000000;
		            } while (!XCheckTypedEvent(XtDisplay(targetW), ButtonRelease, &ev));
	                break;

	        case 3:  do {
		             MapBegunok(targetW);
			     nanosleep(&delay, NULL);
	                     if(okno - rate < min_okno) ; else okno -= rate;
                             MapBegunok(targetW);
                             Footnote(XtNameToWidget(XtParent(targetW), "footnote"), tD);    
			     delay.tv_sec = 0;
			     delay.tv_nsec = 50000000;
		             } while (!XCheckTypedEvent(XtDisplay(targetW), ButtonRelease, &ev));
	                 break;
                                              }
  }
 }                                                           

/*---------------- Функция для вывода на экран данных ------------------------------*/

void Plotdata ( Widget targetW, XtPointer tD, bool ekr = true )
{    
  float * x;
  x = (float *)tD;

  struct stat inf;
  long j, lg, wr, ttm, ttm1, tmp = (block/aver) + (block%aver != 0 ? 1 : 0) - smooth + 1;
  Boolean check;

  typedef struct { int Num; } colorNum, *pcolorNum;
  colorNum clN;
  int def_clN = 6;

  XtResource numRes[] = { {"colorNumber", "ColorNumber", XtRInt, sizeof(int), XtOffset(pcolorNum, Num), XtRInt, &def_clN } };
  XtGetApplicationResources(targetW, &clN, numRes, XtNumber(numRes), NULL, 0); 
  
  frame_col<max_clnnum> fr_col;
  typedef frame_col<max_clnnum> *pFrame_col;
  int clnnum = (clN.Num >= max_clnnum ? max_clnnum : clN.Num);

  XtResource colRes[clN.Num];
  for (int cv=0;cv<clN.Num;cv++) { colRes[cv].resource_name   = glue("color", cv + 1);
				   colRes[cv].resource_class  = glue("Color", cv + 1);
				   colRes[cv].resource_type   = XtRPixel;
				   colRes[cv].resource_size   = sizeof(Pixel);
				   colRes[cv].resource_offset = XtOffset(pFrame_col, fg[cv]);
				   colRes[cv].default_type    = XtRString;
				   colRes[cv].default_addr    = (void*)"black";
                                  }
  XtGetApplicationResources(targetW, &fr_col, colRes, XtNumber(colRes), NULL, 0); 
  INIT_WIDGETS_PROPS
  
  pw = w;
  ph = h;

   Position xcur, ycur, xcurPr, ycurPr, ycurPr2;
   double stpx, stpy;       
	   
      stpx = (double)w/((double)(window));
      stpy = (double)h/(double)(max[count] - min[count]);
      
  if (!ekr) {
    ps.box(att.x + (long)floor(0.5*(float)att.border_width), ps.getHeight() - att.y - h - (long)floor(1.5*(float)att.border_width), w + att.border_width, h + att.border_width, att.border_width, 10);
    ps.translate(att.x + att.border_width, ps.getHeight() - att.y - h - att.border_width);
  }

   for (long k=(long)floor((double)left/tmp); k<( (long)ceil((double)(left+window)/tmp) < iter ? (long)ceil((double)(left+window)/tmp) : iter ); k++) {
   
      for (long a=next+cur_file; a<(Seq ? (next + cur_file + Group > ac ? ac : next + cur_file + Group) : ac); a++) {   stat(av[a], &inf);
   
    if (next + 1 == ac && noread == 1 && block == si) ;
      else { lg = (block-aver*(smooth != 1 ? smooth - 1: 0))*k;
             if (le+lg < (long)(inf.st_size/razm)) { wr = (lg + block > si ? si - lg : block);
                                                     bll = k * tmp;
						     ttm1 = (long)inf.st_size/razm/aver + (((long)inf.st_size/razm/aver)%aver == 0 ? 0 : 1) - smooth + 1;
	                                             blr = ( (k+1) * tmp < (left+window > ttm1 ? ttm1 : left+window) ? (k+1) * tmp - 1 : (left+window > ttm1 ? ttm1 : left+window) );
	     
             ReadProc(a, le+lg, wr);
	     size = ( le+lg+wr > (long)(inf.st_size/razm) ? (long)(inf.st_size/razm) - le - lg : wr );
             if ( aver != 1 ) size = average(x, size);
             if ( smooth != 1 ) size = smoothing(x, size);
	     
                                                     } else { size = 0;
						              bll = -1; blr = -1;
						             }
						    
	     if (next + 1 == ac) noread = 1; else if (ekr) XSetForeground(disID, grC, fr_col.fg[(a-next-cur_file)%clN.Num]);
            }
	    
   if (k == (long)floor((double)left/tmp)) {
	    
      x[(left-k*tmp <= 0 ? 0 : left-k*tmp)] += (a-next-cur_file)*Shift;
      
      xcurPr  = (short int)(((k-(long)floor((double)left/tmp))*tmp) * stpx);           
      ycurPr  = (short int)(h - (x[(left-k*tmp <= 0 ? 0 : left-k*tmp)] - min[count]) * stpy);
      ycurPr2 = ycurPr;
      
     if (NoCover == 1 && next + 1 != ac && (!Seq || (Seq && Group > 1))) keep[(left-k*tmp <= 0 ? 0 : left-k*tmp)] = (a == next ? ycurPr : (ycurPr > keep[(left-k*tmp <= 0 ? 0 : left-k*tmp)] ? ycurPr : keep[(left-k*tmp <= 0 ? 0 : left-k*tmp)]));
     if (a == next + cur_file) if (ekr) XClearWindow(disID, winID);
     
     if (!ekr) ps.begin(xcurPr, h - ycurPr, 1, (a-next-cur_file)%(clN.Num < 10 ? clN.Num : 10));
     
                                             } else { if (next + 1 != ac && (!Seq || (Seq && Group > 1))) { xcurPr  = keepcur[a-next-cur_file][0];
					                                                                    ycurPr  = keepcur[a-next-cur_file][1];
						                                                            ycurPr2 = keepcur[a-next-cur_file][2];
					                                                                   }
					     }
					          
     if (size != 0 && next+1 != ac) if (ekr) WindowHeader(XtNameToWidget(XtParent(targetW), "header"), a);						 
  
      ttm = (left-k*tmp+window < size ? left-k*tmp+window : size);
	for (j=(left-k*tmp <= 0 ? 0 : left-k*tmp) + (k == (long)floor((double)left/tmp) ? 1 : 0);j<(ttm >= tmp ? tmp : ttm);j++)
	 {
	   x[j] += (a-next-cur_file)*Shift;
	   
	   xcur = (short int)((j + (k-(long)floor((double)left/tmp))*tmp - ( k == (long)floor((double)left/tmp) ? 0 : left%tmp ) - (left-k*tmp <= 0 ? 0 : left-k*tmp)) * stpx);
	   ycur = (short int)(h - (x[j] - min[count]) * stpy);

           check = ( (short int)( (j+1+(k-(long)floor((double)left/tmp))*tmp-(left-k*tmp <= 0 ? 0 : left-k*tmp))*stpx )  != xcurPr ||
                     j+1 >= (ttm >= tmp ? tmp : ttm) );
	      
	   if (NoCover == 1 && next + 1 != ac && a != next+cur_file)
	     {
	      if (xcur == xcurPr) { if ( ( Shift >= 0. && ycur <= keep[j] ) || ( Shift <  0. && ycur >= keep[j] ) )
				     { ycurPr  = ( ycur >  ycurPr  ? ycur : ycurPr  );
		                       ycurPr2 = ( ycur <= ycurPr2 ? ycur : ycurPr2 );
				       keep[j] = ycur;
		                      }
				       else { if (Shift >= 0. && ycurPr  < keep[j]) ycurPr  = keep[j];
					      if (Shift <  0. && ycurPr2 > keep[j]) ycurPr2 = keep[j];
					     }
					     
				    if ( check ) { if (ycurPr != ycurPr2) if (ekr) XDrawLine(disID, winID, grC, xcurPr, ycurPr, xcur, ycurPr2);
				                                           else { ps.moveto(xcurPr, (h - ycurPr < 0 ? 0 : (h - ycurPr > h ? h : h - ycurPr)));
									          ps.lineto(xcur, (h - ycurPr2 < 0 ? 0 : (h - ycurPr2 > h ? h : h - ycurPr2)));
										 }
					           ycurPr = ycur;
					          }
		                   } else { if ( ( Shift >= 0. && ycur <= keep[j] && ycurPr <= (j == (left-k*tmp <= 0 ? 0 : left-k*tmp) + (k == (long)floor((double)left/tmp) ? 1 : 0) ? keepcur[a-next-cur_file][3] : keep[j-1]) ) ||
				      	         ( Shift <  0. && ycur >= keep[j] && ycurPr >= (j == (left-k*tmp <= 0 ? 0 : left-k*tmp) + (k == (long)floor((double)left/tmp) ? 1 : 0) ? keepcur[a-next-cur_file][3] : keep[j-1]) ) ) 
					     { if (ekr) XDrawLine(disID, winID, grC, xcurPr, ycurPr, xcur, ycur);
					        else { ps.moveto(xcurPr, (h - ycurPr < 0 ? 0 : (h - ycurPr > h ? h : h - ycurPr)));
						       ps.lineto(xcur, (h - ycur < 0 ? 0 : (h - ycur > h ? h : h - ycur)));
						      }
	        			       keep[j] = ycur;                                              
	        			      }                                                             
	                                     xcurPr  = xcur;
	        			     ycurPr  = ycur;
	        			     ycurPr2 = ycur;		           			   			   
				           }
	      } else { if (NoCover == 1 && next + 1 != ac && (!Seq || (Seq && Group > 1))) keep[j] = ycur;
	       
	              if (xcur == xcurPr) { ycurPr  = ( ycur >  ycurPr  ? ycur : ycurPr  );
		                            ycurPr2 = ( ycur <= ycurPr2 ? ycur : ycurPr2 );

					    if ( check ) { if (ycurPr != ycurPr2) if (ekr) XDrawLine(disID, winID, grC, xcurPr, ycurPr, xcur, ycurPr2);
					                                           else { ps.moveto(xcurPr, (h - ycurPr < 0 ? 0 : (h - ycurPr > h ? h : h - ycurPr)));
										          ps.lineto(xcur, (h - ycurPr2 < 0 ? 0 : (h - ycurPr2 > h ? h : h - ycurPr2)));
											 }
					                   ycurPr = ycur;
					                  }
		                           } else { if (ekr) XDrawLine(disID, winID, grC, xcurPr, ycurPr, xcur, ycur);
					             else { ps.moveto(xcurPr, (h - ycurPr < 0 ? 0 : (h - ycurPr > h ? h : h - ycurPr)));
						            ps.lineto(xcur, (h - ycur < 0 ? 0 : (h - ycur > h ? h : h - ycur)));
							   }
					            xcurPr  = xcur;
						    ycurPr  = ycur;
						    ycurPr2 = ycur;
					           }
	             }
	  }
	    if (next + 1 != ac && (!Seq || (Seq && Group > 1))) { keepcur[a-next-cur_file][0] = xcurPr;
	                                                          keepcur[a-next-cur_file][1] = ycurPr;
	                                                          keepcur[a-next-cur_file][2] = ycurPr2;
				                                  if (NoCover == 1) keepcur[a-next-cur_file][3] = keep[j-1];
	                                                         }
                              }
                                                                      }
   if (!ekr) { ps.end();
               ps.translate(- att.x - att.border_width, -ps.getHeight() + att.y + h + att.border_width);
              }

   if (ekr) MapBegunok(targetW);
   UNPLUG_WIDGETS_GC   
}

/*---------- Функция перерисовки окна -----------------*/
// в качестве Widget должен передаваться корневой, т.е. coreW

void WindowRedraw (Widget targetW, XtPointer tD)
{
  WindowHeader (XtNameToWidget(targetW, "header"), next);
  Legend       (XtNameToWidget(targetW, "legend"));
  vTitle       (XtNameToWidget(targetW, "vTitle"));
  Vertic       (XtNameToWidget(targetW, "vAxe"), tD, 1);
  HorizAxe     (XtNameToWidget(targetW, "hAxe"));
  Plotdata     (XtNameToWidget(targetW, "PlotBox"), tD);
  Footnote     (XtNameToWidget(targetW, "footnote"), tD);
}

/*---------- Обработчик событий для обработки события Expose --------------------*/
					      
void WinExpose ( Widget targetW, XtPointer tD, XEvent *event, Boolean *cont )
{
  if (event->type == VisibilityNotify &&
      // перерисовываем только тогда, когда окно полностью открыто
      event->xvisibility.state == 0 &&
      auto_expose)
   WindowRedraw(targetW, tD);
} 

/*---------- Обработчик событий для изменения реальных размеров окна --------------------*/
					      
void WinResize ( Widget targetW, XtPointer tD, XEvent *event, Boolean *cont )
{
  if(event->type == ConfigureNotify)
  {
    *cont = False;
    Dimension Height = event->xconfigure.height;
    Dimension Width = event->xconfigure.width;
    Dimension hfoot, hxaxis;
    Widget tmpW;
    XWindowChanges values;
    XWindowAttributes attr;

    tmpW = XtNameToWidget(targetW, "header");
    XGetWindowAttributes(XtDisplay(tmpW), XtWindow(tmpW), &attr);
    values.width = Width - attr.x;
    XConfigureWindow(XtDisplay(tmpW), XtWindow(tmpW), CWWidth, &values);
    WindowHeader(tmpW, next);
    
    tmpW = XtNameToWidget(targetW, "legend");
    XGetWindowAttributes(XtDisplay(tmpW), XtWindow(tmpW), &attr);
    values.width = Width - attr.x;
    XConfigureWindow(XtDisplay(tmpW), XtWindow(tmpW), CWWidth, &values);
    Legend(tmpW);
    
    tmpW = XtNameToWidget(targetW, "footnote");
    XGetWindowAttributes(XtDisplay(tmpW), XtWindow(tmpW), &attr);
    values.y = Height - attr.height;
    values.width = Width - attr.x;
    XConfigureWindow(XtDisplay(tmpW), XtWindow(tmpW), CWY | CWWidth, &values);
    hfoot = attr.height;
    
    tmpW = XtNameToWidget(targetW, "hAxe");
    XGetWindowAttributes(XtDisplay(tmpW), XtWindow(tmpW), &attr);
    values.width = Width - attr.x;
    values.y = Height - hfoot - attr.height;
    XConfigureWindow(XtDisplay(tmpW), XtWindow(tmpW), CWY | CWWidth, &values);
    hxaxis = attr.height;
    
    tmpW = XtNameToWidget(targetW, "vTitle");
    XGetWindowAttributes(XtDisplay(tmpW), XtWindow(tmpW), &attr);
    values.height = Height - hfoot - hxaxis - attr.y;
    XConfigureWindow(XtDisplay(tmpW), XtWindow(tmpW), CWHeight, &values);
    vTitle(tmpW);
    
    tmpW = XtNameToWidget(targetW, "vAxe");
    XGetWindowAttributes(XtDisplay(tmpW), XtWindow(tmpW), &attr);
    values.height = Height - hfoot - hxaxis - attr.y;
    XConfigureWindow(XtDisplay(tmpW), XtWindow(tmpW), CWHeight, &values);
    
    Vertic(tmpW, tD, 1);
    HorizAxe(XtNameToWidget(targetW, "hAxe"));
            
    tmpW = XtNameToWidget(targetW, "PlotBox");
    XGetWindowAttributes(XtDisplay(tmpW), XtWindow(tmpW), &attr);
    values.width = Width - attr.x - 2*attr.border_width;
    values.height = Height - hfoot - hxaxis - attr.y - 2*attr.border_width;
    XConfigureWindow(XtDisplay(tmpW), XtWindow(tmpW), CWWidth | CWHeight, &values);    
    Plotdata(tmpW, tD);
    Footnote(XtNameToWidget(targetW, "footnote"), tD);
  }
}                                             

/*--------- Функция записи выведенных данных в Postscript ---------------------*/

void Save2PS (Widget targetW, XtPointer tD, bool is_land = false) {
  char * name;
         name = new char[MaxString+1];
         if ( !name ) { printf("Not enough memory!\n"); exit(1); }

  memset(name, 0, MaxString+1);
  
   if (!strcmp(OutputPath, "\0")) {
    if (next + 1 == ac) sprintf(name, "%s-%d%s", av[next], count, ".ps");
     else sprintf(name, "%s--%s-%d%s", av[next], SelectFileName(av[ac-1]), count, ".ps");
   } else { if (next + 1 == ac) sprintf(name, "%s/%s-%d%s", OutputPath, SelectFileName(av[next]), count, ".ps");
             else sprintf(name, "%s/%s--%s-%d%s", OutputPath, SelectFileName(av[next]), SelectFileName(av[ac-1]), count, ".ps");
           }
	   
   static int base_w, base_h, bw, fs;
   static Arg tmpW, tmpH, tmpBW;
   XtSetArg(tmpH, XtNheight, &base_h);
   XtSetArg(tmpW, XtNwidth, &base_w);
   XtSetArg(tmpBW, XtNborderWidth, &bw);
   XtVaGetValues(XtParent(targetW), tmpH, tmpW, NULL);
   XtVaGetValues(XtNameToWidget(targetW, "PlotBox"), tmpBW, NULL);
   

   typedef struct { Font  font; } wD, *twData;
   wD wData;
   XtResource tRes[] = { {XtNfont, XtCFont, XtRFont, sizeof(Font), XtOffset(twData, font), XtRString, (void*)"6x10"}};
   XtGetApplicationResources(XtNameToWidget(targetW, "header"), &wData, tRes, XtNumber(tRes), NULL, 0); \
   XFontStruct *cur_font = XQueryFont(XtDisplay(XtNameToWidget(targetW, "header")), wData.font);
   fs = cur_font->ascent + cur_font->descent;
   
   ps.open(name, base_w, base_h, bw, fs, is_land);
  
   if (!Post) { printf("Saving visual data in postscript (%s) file \"%s\" ... ", is_land ? "Landscape" : "Portrait", name); fflush(NULL); }
   
   WindowHeader(XtNameToWidget(targetW, "header"), next, false);
   Legend(XtNameToWidget(targetW, "legend"), false);
   vTitle(XtNameToWidget(targetW, "vTitle"), false);
   if (!Post) VertAxe(XtNameToWidget(targetW, "vAxe"), false);
    else Vertic(XtNameToWidget(targetW, "vAxe"), tD, 0, false);
   HorizAxe(XtNameToWidget(targetW, "hAxe"), false);
   Footnote(XtNameToWidget(targetW, "footnote"), tD, false);
   Plotdata(XtNameToWidget(targetW, "PlotBox"), tD, false);

   if (!Post) printf("- done.\n");
   ps.close();
   
   delete(name);
}

/*------------- Отработчик событий по нажатым на клавиатуре кнопкам -------------------*/

void Keyboard ( Widget targetW, XtPointer tD, XEvent *event, Boolean *cont )
{
if ( event->type == KeyPress || event->type == KeyRelease )
{
 KeySym a = XLookupKeysym(&event->xkey, 0);
 long Pos = lo - left; 
 switch (a) {
 /*   "q"   */
  case 0x71:   Destroy(targetW, tD, event, cont);    
	       break;
 /*   "+"   */
  case 0xffab: MapBegunok(XtNameToWidget(targetW, "PlotBox"));
               if((lo-left) + okno + rate > window) ; else okno += rate;		          
	       MapBegunok(XtNameToWidget(targetW, "PlotBox"));
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);
               break;
 /*   "+"   */            /*---------- На самом деле это код = ------------*/
  case 0x3d:   MapBegunok(XtNameToWidget(targetW, "PlotBox"));
               if((lo-left) + okno + rate > window) ; else okno += rate;		          
	       MapBegunok(XtNameToWidget(targetW, "PlotBox"));
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);
               break;
 /*   "-"   */
  case 0xffad: MapBegunok(XtNameToWidget(targetW, "PlotBox"));               
               if(okno - rate < min_okno) ; else okno -= rate;
	       MapBegunok(XtNameToWidget(targetW, "PlotBox"));
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);                                   
               break;
 /*   "-"   */
  case 0x2d:   MapBegunok(XtNameToWidget(targetW, "PlotBox"));               
               if(okno - rate < min_okno) ; else okno -= rate;
	       MapBegunok(XtNameToWidget(targetW, "PlotBox"));
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);                                   
               break;
 /* "Left"  */
  case 0xff51: MapBegunok(XtNameToWidget(targetW, "PlotBox"));                              
               Pos -= rate;                            
               if (Pos < 0) Pos = 0;
               lo = left + Pos;
               MapBegunok(XtNameToWidget(targetW, "PlotBox"));
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);                                   
               break;
 /* "Left"  */
  case 0xff96: MapBegunok(XtNameToWidget(targetW, "PlotBox"));                              
               Pos -= rate;                            
               if (Pos < 0) Pos = 0;
               lo = left + Pos;
               MapBegunok(XtNameToWidget(targetW, "PlotBox"));
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);                                   
               break;
 /* "Right" */
  case 0xff53: MapBegunok(XtNameToWidget(targetW, "PlotBox"));                              
               Pos += rate;
               if (Pos + okno > window) Pos = window - okno;                   
               lo = left + Pos;
               MapBegunok(XtNameToWidget(targetW, "PlotBox"));
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);                                   
               break;
 /* "Right" */
  case 0xff98: MapBegunok(XtNameToWidget(targetW, "PlotBox"));                              
               Pos += rate;
               if (Pos + okno > window) Pos = window - okno;                   
               lo = left + Pos;
               MapBegunok(XtNameToWidget(targetW, "PlotBox"));
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);                                   
               break;
 /* "PgUp"  */	       
  case 0xff55: if (rate*2 >= okno/2) ; else rate *= 2;	       
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);                                   
               break;
 /* "PgUp"  */	       
  case 0xff9a: if (rate*2 >= okno/2) ; else rate *= 2;	       
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);                                   
               break;
 /* "PgDn"  */
  case 0xff56: if (rate/2 < 1) ; else rate /= 2;	       
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);                                   
               break;
 /* "PgDn"  */
  case 0xff9b: if (rate/2 < 1) ; else rate /= 2;	       
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);                                   
               break;
 /*  "Tab"  */
  case 0xff09: if (tab == 0) tab = 1; else tab = 0;
               Footnote(XtNameToWidget(targetW, "footnote"), tD);  
               break;
 /*   "i"   */
  case 0x69:   MapBegunok(XtNameToWidget(targetW, "PlotBox"));
               MakePPMImage(targetW, tD);
	       MapBegunok(XtNameToWidget(targetW, "PlotBox"));
               break;	       	       
 /*   "g"   */
  case 0x67:   MapBegunok(XtNameToWidget(targetW, "PlotBox"));
               MakeGIFImage(targetW, tD);
	       MapBegunok(XtNameToWidget(targetW, "PlotBox"));
               break;	       	       
 /*   "t"   */
  case 0x74:   Save2File((float *)tD, left, window, 0);               
               break;	       	       
 /*   "b"   */
  case 0x62:   Save2File((float *)tD, left, window, 1);               
               break;	       	       
 /*   "p"   */
  case 0x70:   Save2PS(targetW, tD, false);               
               break;	       	       
 /*   "l"   */
  case 0x6c:   Save2PS(targetW, tD, true);               
               break;	       	       
 /* "Space" */
  case 0x20:   if ( count > zoom - 1 ) ;
	        else {
		if ( window != min_okno ) {
	       lp[count] = left;
	       wp[count] = window;
	       rp[count] = rate;
	       count++;
               window = okno;
	       left = lo;
               okno = window/2 > min_okno ? window/2 : window - 1;
               rate = (window/10 < 1 ? 1 : window/10);
	       Vertic  (XtNameToWidget(targetW, "vAxe"), tD, 0);
	       HorizAxe(XtNameToWidget(targetW, "hAxe"));
               Plotdata(XtNameToWidget(targetW, "PlotBox"), tD);
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);
		                            }
		      }
               break;
 /*  "Esc"  */
  case 0xff1b: if ( count < 1 ) ;
	        else {
	       max[count] = 0.;
	       min[count] = 0.;
	       count--;
	       lo = left;
	       okno = window;
	       left = lp[count];
	       window = wp[count];
	       rate = rp[count];
               lp[count] = 0;
	       wp[count] = 0;
	       rp[count] = 0;
	       if (max[count] == 0. && min[count] == 0.) Vertic (XtNameToWidget(targetW, "vAxe"), tD, 0);
	        else Vertic (XtNameToWidget(targetW, "vAxe"), tD, 1);
	       HorizAxe(XtNameToWidget(targetW, "hAxe"));
               Plotdata(XtNameToWidget(targetW, "PlotBox"), tD);
	       Footnote(XtNameToWidget(targetW, "footnote"), tD);
		       }
               break;
  /* "LCtrl" */
  case 0xffe3: if (event->xkey.state == ControlMask) auto_expose = !auto_expose;
                else WindowRedraw(targetW, tD);
               break;
  /* "RCtrl" */	       
  case 0xffe4: if (event->xkey.state == ControlMask) auto_expose = !auto_expose;
                else WindowRedraw(targetW, tD);
               break;
 /* "Enter" */
  case 0xff0d: if (Seq) {
                if (next + cur_file + Group < ac || Cycle) {
		 if (next + cur_file + Group < ac) cur_file += Group; else cur_file = 0;
		 for (int i=0; i<=count; ++i) { max[i] = 0.; min[i] = 0.; }
                 WindowHeader (XtNameToWidget(targetW, "header"), next + cur_file);
                 Vertic  (XtNameToWidget(targetW, "vAxe"), tD, 0);
                 Plotdata(XtNameToWidget(targetW, "PlotBox"), tD);
		}
		         } // if (Seq)
               break;
/* "BackSpace" */
  case 0xff08: if (Seq) {
                if (next + cur_file - Group >= next || Cycle) {
		 if (next + cur_file - Group >= next) cur_file -= Group; else cur_file = ac - next - ((ac-next)%Group == 0 ? Group : (ac-next)%Group);
		 for (int i=0; i<=count; ++i) { max[i] = 0.; min[i] = 0.; }
                 WindowHeader (XtNameToWidget(targetW, "header"), next + cur_file);
                 Vertic  (XtNameToWidget(targetW, "vAxe"), tD, 0);
                 Plotdata(XtNameToWidget(targetW, "PlotBox"), tD);
		}
		         } // if (Seq)
               break;	       
  default:     break;
            }
      }
 }                                                

/*-------------------------------------------------------------------
 Функция с заданным интервалом (timint) проверяет изменился ли
 файл или нет. Если да, то перечитывает его
--------------------------------------------------------------------*/
void ChecksFileModification (XtPointer tD, XtIntervalId *id) {
  struct stat inf;
  int c = 0;
  for (long a=next+cur_file; a<(Seq ? (next + cur_file + Group > ac ? ac : next + cur_file + Group) : ac); ++a) {
    if (stat(av[a], &inf) != 0 ) { perror("stat: "); exit(1); }
    if (inf.st_mtime != mtstatus[a-next]) {
     mtstatus[a-next] = inf.st_mtime;
     ++c;
    } // if
  } // for
  if (c != 0) {
    noread = 0;
    Vertic  (XtNameToWidget(coreW, "vAxe"), tD, 0);
    HorizAxe(XtNameToWidget(coreW, "hAxe"));
    Plotdata(XtNameToWidget(coreW, "PlotBox"), tD);
    Footnote(XtNameToWidget(coreW, "footnote"), tD);
  } // if
  XtAppAddTimeOut(appcont, timint, ChecksFileModification, tD);
}

/*----------------------------------------------------------------
 Функция с заданным интервалом (slide_time) последовательно
 меняет файлы - то есть это слайд-шоу
------------------------------------------------------------------*/
void SlideShow (XtPointer tD, XtIntervalId *id) {
  if (Cycle) {
    if (next + cur_file + Group < ac) cur_file += Group; else cur_file = 0;
  } else { // если не задан Cycle, то сначала крутим в одну сторону, а потом в другую
    if (dir && next + cur_file + Group < ac) cur_file += Group; else dir = false;
    if (!dir && next + cur_file - Group >= next) cur_file -= Group; else { if (!dir && next + cur_file + Group < ac) { cur_file += Group; } dir = true; }
  }
  
  for (int i=0; i<=count; ++i) { max[i] = 0.; min[i] = 0.; }
  WindowHeader (XtNameToWidget(coreW, "header"), next + cur_file);
  Vertic  (XtNameToWidget(coreW, "vAxe"), tD, 0);
  Plotdata(XtNameToWidget(coreW, "PlotBox"), tD);
  
  XtAppAddTimeOut(appcont, slide_time, SlideShow, tD);
}

/*--------------------- Функция MAIN в Г Р А Ф И К Е ----------------------------------------*/

void Plotting () {

 size = si/aver + (si%aver == 0 ? 0 : 1) - smooth + 1;

 window = size >= min_okno ? size : 0;
 if ( window == 0 ) { printf("\nError:\nSize of massive < 2!\n"); exit(1); }
 left = 0;
 lo = left;
 okno = window/2  > min_okno ? window/2 : window - 1;
 rate = (window/10 < 1 ? 1 : window/10);

 if (next + 1 != ac && (!Seq || (Seq && Group > 1))) {
     if (NoCover == 1) { keep = new short int[(block/aver) + (block%aver != 0 ? 1 : 0) - smooth + 1];
                         if ( !keep ) { printf("Not enough memory!\n"); exit(1); }
	                 memset(keep, 0, ( (block/aver) + (block%aver != 0 ? 1 : 0) - smooth + 1 )*sizeof(short int));
                        }
     keepcur = new short int* [(!Seq ? ac-next : Group)];
     for (long w=0; w<(!Seq ? ac-next : Group); w++) keepcur[w] = new short int[NoCover == 1 ? 4 : 3];					
     if ( !keepcur ) { printf("Not enough memory!\n"); exit(1); }
     for (long w=0; w<(!Seq ? ac-next : Group); w++) memset(keepcur[w], 0, (NoCover == 1 ? 4 : 3)*sizeof(short int));
 }

 Widget shellW  = XtVaAppCreateShell("See", "See", applicationShellWidgetClass, DisId, NULL);
        coreW   = XtVaCreateManagedWidget("mainBox",  compositeWidgetClass, shellW,    NULL);
 Widget headerW = XtVaCreateManagedWidget("header",   coreWidgetClass,      coreW,     NULL); 
 Widget legendW = XtVaCreateManagedWidget("legend",   coreWidgetClass,      coreW,     NULL); 
 Widget vTitleW = XtVaCreateManagedWidget("vTitle",   coreWidgetClass,      coreW,     NULL);
 Widget vaxeW   = XtVaCreateManagedWidget("vAxe",     coreWidgetClass,      coreW,     NULL);
 Widget haxeW   = XtVaCreateManagedWidget("hAxe",     coreWidgetClass,      coreW,     NULL);
 Widget plotW   = XtVaCreateManagedWidget("PlotBox",  coreWidgetClass,      coreW,     NULL);
 Widget footW   = XtVaCreateManagedWidget("footnote", coreWidgetClass,      coreW,     NULL);

 // определяем неизвестные параметры у виджетов через известные у других
 UpdateResources(coreW);
 
 XtRealizeWidget(shellW);

 // Перехватываем фокус клавиатурного ввода
 XWMHints wmh;
 wmh.flags = InputHint;
 wmh.input = True;
 XSetWMHints(DisId, XtWindow(shellW), &wmh);

 if (!Post) {
  WindowHeader (headerW, next);
  Legend       (legendW);
  vTitle       (vTitleW);
  Vertic       (vaxeW, m, 0);
  HorizAxe     (haxeW);
  Plotdata     (plotW, m);
  Footnote     (footW, m);
 }

 mtstatus = new unsigned long[ac-next];
 if (!mtstatus) { printf("Not enough memory!\n"); exit(1); }
 memset(mtstatus, 0, (ac-next)*sizeof(unsigned long));
 struct stat inf;
 for (long j=0; j<ac-next; ++j) {
  if (stat(av[j+next], &inf) != 0) { perror("stat: "); exit(1); }
  mtstatus[j] = inf.st_mtime;
 }
 if (timint >= 0) XtAppAddTimeOut(appcont, timint, ChecksFileModification, m);
 // устанавливаем слайд-шоу
 if (slide_time >= 0) XtAppAddTimeOut(appcont, slide_time, SlideShow, m);

 if (Post) Save2PS (coreW, m, true);

 if (!Post) {
  XtAddEventHandler(shellW,  StructureNotifyMask, False, Destroy,       m);
  XtAddEventHandler(coreW,   KeyPressMask,        False, Keyboard,      m);
  XtAddEventHandler(coreW,   StructureNotifyMask, False, WinResize,     m);
 
  // если сервер поддерживает backing_store, то не добавляем обработку события Expose
  if (!DoesBackingStore(XtScreen(shellW)))
   XtAddEventHandler(coreW, VisibilityChangeMask, False, WinExpose, m);
  
  XtAddEventHandler(plotW,   Button1MotionMask,   False, MoveBegunok,   m);
  XtAddEventHandler(plotW,   PointerMotionMask,   False, PointerPos,    m);
  XtAddEventHandler(plotW,   ButtonPressMask |
                             ButtonReleaseMask,   False, ResizeBegunok, m);   

  XtAppMainLoop(appcont);
 }
}

/*===================================== E N D ==================================================*/

