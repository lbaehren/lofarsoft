#ifndef _PS_HPP
#define _PS_HPP

/*---------------
 Class 'postscript' to print to ps-file
------------*/
#include <iostream>
#include <fstream>

class postscript {
 std::ofstream dev;
 bool is_land;
 int x_offset;
 int y_offset;
 int pwidth;  /* portrait width */
 int pheight; /* portrait height */
 int lwidth;  /* landscape width */
 int lheight; /* landscape height */
 float xscale;
 float yscale;

 int width, height, borderWidth;
 int fontsize;

 void init(const char* s, int w, int h, int bw, int fs, bool isl = false) {

  is_land = isl;
  width = w;
  height = h;
  borderWidth = bw;
  fontsize = fs;
  x_offset = 50;
  y_offset = 50;
  /* portrait */
  pwidth = 491;
  pheight = 395;
  /* landscape */
  lwidth = 504;
  lheight =720;
  /* scaling */
  if (is_land) xscale = (float)lwidth / (float)height; else xscale = (float)pwidth / (float)width;
  if (is_land) yscale = (float)lheight / (float)width; else yscale = (float)pheight / (float)height;
  
  dev.open(s);
  dev << "%!PS-Adobe-2.0" << std::endl;
  dev << "%%Title: " << s << std::endl;
  dev << "%%Creator: see (ASC Pulsar Group)" << std::endl;
  if (is_land) {
   dev << "%%BoundingBox: " << y_offset << " " << x_offset << " " << (lwidth+y_offset) << " " << (lheight+x_offset) << std::endl;
  } else {
   dev << "%%BoundingBox: " << x_offset << " " << y_offset << " " << (pwidth+x_offset) << " " << (pheight+y_offset) << std::endl;
  }
  dev << "%%Orientation: " << (is_land ? "Landscape" : "Portrait") << std::endl;
  dev << "%%EndComments" << std::endl;
  dev << "/L { lineto } def" << std::endl;
  dev << "/M { moveto } def" << std::endl;
  dev << "/R { rmoveto } def" << std::endl;
  dev << "/V { rlineto } def" << std::endl;
  dev << "/Cshow { currentpoint stroke M dup stringwidth pop -2 div 0 R show } def" << std::endl;
  dev << "/Lshow { currentpoint stroke M 0 0 R show } def" << std::endl;
  dev << "/Rshow { currentpoint stroke M dup stringwidth pop neg 0 R show } def" << std::endl;
  dev << "/userlinewidth 0.500 def" << std::endl;
  dev << "/Color false def" << std::endl;
  dev << "/Solid false def" << std::endl;
  dev << "/dl { 1 mul } def" << std::endl;
  dev << "/DL { Color {setrgbcolor Solid {pop []} if 0 setdash } {pop pop pop Solid {pop []} if 0 setdash} ifelse } def" << std::endl;
  dev << "/BL { stroke userlinewidth 2 mul setlinewidth } def" << std::endl;
  dev << "/AL { stroke userlinewidth 2 div setlinewidth } def" << std::endl;
  dev << "/UL { dup 0.500 mul /userlinewidth exch def 1 mul /udl exch def } def" << std::endl;
  dev << "/PL { stroke userlinewidth setlinewidth } def" << std::endl;
  dev << "/LT10 { BL [] 0 0 0 DL } def" << std::endl;
  dev << "/LT9 { AL [1 udl mul 2 udl mul] 0 setdash 0 0 0 setrgbcolor } def" << std::endl;
  dev << "/LT0 { PL [] 1 0 0 DL } def" << std::endl;
  dev << "/LT1 { PL [4 dl 2 dl] 0 1 0 DL } def" << std::endl;
  dev << "/LT2 { PL [2 dl 3 dl] 0 0 1 DL } def" << std::endl;
  dev << "/LT3 { PL [1 dl 1.5 dl] 1 0 1 DL } def" << std::endl;
  dev << "/LT4 { PL [5 dl 2 dl 1 dl 2 dl] 0 1 1 DL } def" << std::endl;
  dev << "/LT5 { PL [4 dl 3 dl 1 dl 3 dl] 1 1 0 DL } def" << std::endl;
  dev << "/LT6 { PL [2 dl 2 dl 2 dl 4 dl] 0 0 0 DL } def" << std::endl;
  dev << "/LT7 { PL [2 dl 2 dl 2 dl 2 dl 2 dl 4 dl] 1 0.3 0 DL } def" << std::endl;
  dev << "/LT8 { PL [2 dl 2 dl 2 dl 2 dl 2 dl 2 dl 2 dl 4 dl] 0.5 0.5 0.5 DL } def" << std::endl;  
  dev << "/box { dup neg 3 1 roll 0 2 1 roll V 0 V 0 2 1 roll V } def" << std::endl;
  dev << "/ArialRus findfont " << fontsize << " scalefont setfont" << std::endl;
  if (is_land) dev << y_offset << " " << x_offset << " translate" << std::endl;
   else dev << x_offset << " " << y_offset << " translate" << std::endl; 
  dev << xscale << " " << yscale << " scale" << std::endl;
  
 };

public:
 postscript() {};
 postscript(const char* s, int w, int h, int bw, int fs, bool isl = false) {
  init (s, w, h, bw, fs, isl);
 };
 
 void open(const char* s, int w, int h, int bw, int fs, bool isl = false) {
  init (s, w, h, bw, fs, isl);
 };

 virtual ~postscript () {};
 
 void close () {
  dev << "showpage" << std::endl;
  dev.close();
 };
 
 void translate(int x0, int y0) {
  if (is_land) dev << -y0 << " " << x0 << " translate" << std::endl;
   else dev << x0 << " " << y0 << " translate" << std::endl;
 };

 void begin(int x0, int y0, int lw = 1, unsigned short lt = 0) {
  dev << lw << ".000 UL" << std::endl;
  dev << "LT" << lt << std::endl;
  dev << "newpath" << std::endl;
  if (is_land) dev << (height - y0) << " " << x0 << " M" << std::endl;
   else dev << x0 << " " << y0 << " M" << std::endl;
 };
 
 void end () {
  dev << "stroke" << std::endl;
 };

 void moveto(int x, int y) {
  if (is_land) dev << (height - y) << " " << x << " M" << std::endl;
   else dev << x << " " << y << " M" << std::endl;
 };
 
 void rmoveto(int x, int y) {
  if (is_land) dev << -y << " " << x << " R" << std::endl;
   else dev << x << " " << y << " R" << std::endl;
 };
 
 void lineto(int x, int y) {
  if (is_land) dev << (height - y) << " " << x << " L" << std::endl;
   else dev << x << " " << y << " L" << std::endl;
 };
 
 void rlineto(int w, int h) {
  if (is_land) dev << -h << " " << w << " V" << std::endl;
   else dev << w << " " << h << " V" << std::endl;
 };
 
 void line(int x1, int y1, int x2, int y2) {
  if (is_land) {
   dev << (height - y1) << " " << x1 << " M" << std::endl;
   dev << (height - y2) << " " << x2 << " L" << std::endl;
  } else {
   dev << x1 << " " << y1 << " M" << std::endl;
   dev << x2 << " " << y2 << " L" << std::endl;
  }
 };
 
 void box(int x0, int y0, int w, int h, int lw = 1, unsigned short lt = 0) {
  dev << lw << ".000 UL" << std::endl;
  dev << "LT" << lt << std::endl;
  dev << "newpath" << std::endl;
  if (is_land) {
   dev << (height-y0) << " " << x0 << " M" << std::endl;
   dev << -h << " " << w << " box" << std::endl;
  } else {
   dev << x0 << " " << y0 << " M" << std::endl;
   dev << w << " " << h << " box" << std::endl;
  }
  dev << "closepath" << std::endl;
  dev << "stroke" << std::endl;
 };

 void string(int x0, int y0, const char* s, bool center = false, int angle = 0) {
  if (is_land) dev << (height - y0) << " " << x0 << " M" << std::endl;
   else dev << x0 << " " << y0 << " M" << std::endl;
  if (is_land) angle += 90;
  if (angle != 0) dev << angle << " rotate" << std::endl;
  dev << "(" << s << ") " << (center ? "Cs" : "s") << "how" << std::endl;
  if (angle != 0) dev << -angle << " rotate" << std::endl;
 };

 int getWidth() { return width; };
 int getHeight() { return height; };
 int getBorderWidth() { return borderWidth; };
 
};

#endif // #ifndef _PS_HPP
