#ifndef _OPT_HPP
#define _OPT_HPP

#include <string>
#include <list>
#include <iterator>
#include <algorithm>

#ifndef MaxString
 #define MaxString 255
#endif

class CommandLineOptions {
 int argN;
 list<string> argV;
 list<string> removed_option;
 char** presentation;

 typedef struct { string option;
                  unsigned char kind;
		 } SelOpt;
 list<SelOpt> selected_opt;

public:
 CommandLineOptions(int ac, char** av, char** opt, int* ok, const int size) : argN(ac) {
 
  for (int i=0; i<ac; ++i) argV.push_back(av[i]);

  SelOpt tmp;
  for (int i=0; i<size; ++i) { tmp.option = opt[i]; tmp.kind = ok[i];
                               selected_opt.push_back(tmp);
			      }
  presentation = new char*[ac];
  for (int i=0; i<ac; ++i) presentation[i] = new char[MaxString+1];
 };
 
 ~CommandLineOptions() {
   delete [] presentation;
   selected_opt.clear();
   argV.clear();
   removed_option.clear();
 };

 // updates options
 void update(int ac, char** av) {
  argN = ac;
  argV.clear();
  for (int i=0; i<ac; ++i) argV.push_back(av[i]);
 };

 // remove selected options from the list
 char** remove() {
  list<string> nargV;
  list<string>::iterator it = argV.begin();
  list<SelOpt>::const_iterator jt;
  for (; it!=argV.end(); ++it) {
   for (jt=selected_opt.begin(); jt!=selected_opt.end(); ++jt)
    if (*it == jt->option) {
      removed_option.push_back(*it);
      if (jt->kind == 1) { removed_option.push_back(*(++it));
                           --argN;
		          }
     --argN;
     break;
    }
  if (jt == selected_opt.end()) nargV.push_back(*it);
}

   argV.clear();
   copy(nargV.begin(), nargV.end(), back_inserter(argV));
   nargV.clear();
   it = argV.begin();   
   for (int i=0; i<argN; ++i, ++it) {
    memset(presentation[i], 0, MaxString+1);
    strcpy(presentation[i], (*it).c_str());
   }
   return presentation;
 };

 // return the number of arguments
 int getN() { return argN; };

 // add selected options to the list
 char** add() {
   copy(removed_option.begin(), removed_option.end(), back_inserter(argV));
   argN += removed_option.size();
   
   delete [] presentation;
   presentation = new char*[argN];
   for (int i=0; i<argN; ++i) presentation[i] = new char[MaxString+1];
      
   list<string>::iterator it = argV.begin();
   for (int i=0; i<argN; ++i, ++it) {
    memset(presentation[i], 0, MaxString+1);
    strcpy(presentation[i], (*it).c_str());
   }
   return presentation;
 };
 
};

#endif // #ifndef _OPT_HPP
