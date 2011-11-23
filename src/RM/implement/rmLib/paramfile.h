/*
 *  This file is part of libcxxsupport.
 *
 *  libcxxsupport is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  libcxxsupport is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libcxxsupport; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 *  libcxxsupport is being developed at the Max-Planck-Institut fuer Astrophysik
 *  and financially supported by the Deutsches Zentrum fuer Luft- und Raumfahrt
 *  (DLR).
 */

/*
 *  Class for parsing parameter files
 *
 *  Copyright (C) 2003, 2004, 2005, 2008, 2009, 2010 Max-Planck-Society
 *  Authors: Martin Reinecke, Reinhard Hell
 */

#include <map>
#include <set>
#include <string>
#include <cstring>
#include <iostream>
#define tsize uint
using namespace std;
class paramfile
  {
  private:

    typedef std::map<string,string> params_type;
    params_type params;
    mutable std::set<std::string> read_params;
    bool verbose;

    std::string get_valstr(const std::string &key) const
      {
      params_type::const_iterator loc=params.find(key);
      if (loc!=params.end()) return loc->second;
      cerr <<  "Cannot find the key '" << key << "'." << endl ;
      throw ("missing Parameter") ;
      }

    bool param_unread (const std::string &key) const
      { return (read_params.find(key)==read_params.end()); }

  string trim (const string &orig)
  {
  string::size_type p1=orig.find_first_not_of(" \t");
  if (p1==string::npos) return "";
  string::size_type p2=orig.find_last_not_of(" \t");
  return orig.substr(p1,p2-p1+1);
  }

  public:
  void parse_file (const string &filename, map<string,string> &dict)
  {
    int lineno=0;
    dict.clear();
    ifstream inp(filename.c_str());
    //   planck_assert (inp,"Could not open parameter file '"+filename+"'.");
    while (inp)
      {
      string line;
      getline(inp, line);
      ++lineno;
      // remove potential carriage returns at the end of the line
      line=line.substr(0,line.find("\r"));
      line=line.substr(0,line.find("#"));
      line=trim(line);
      if (line.size()>0)
        {
        string::size_type eqpos=line.find("=");
        if (eqpos!=string::npos)
          {
          string key=trim(line.substr(0,eqpos)),
               value=trim(line.substr(eqpos+1,string::npos));
          if (key=="")
            cerr << "Warning: empty key in '" << filename << "', line "
               << lineno << endl;
          else
            {
            if (dict.find(key)!=dict.end())
            cerr << "Warning: key '" << key << "' multiply defined in '"
                 << filename << "', line " << lineno << endl;
            dict[key]=value;
            }
          }
        else
          cerr << "Warning: unrecognized format in '" << filename << "', line "
             << lineno << ":\n" << line << endl;
        }
      }
    }
    paramfile (const std::string &filename, bool verbose_=true)
      : verbose(verbose_)
      { parse_file (filename, params); }

    paramfile (const params_type &par, bool verbose_=true)
      : params (par), verbose(verbose_)
      {}

    ~paramfile ()
      {
      if (verbose)
        for (params_type::const_iterator loc=params.begin();
             loc!=params.end(); ++loc)
          if (param_unread(loc->first))
            std::cout << "Parser warning: unused parameter "
                      << loc->first << std::endl;
      }

    void setVerbosity (bool verbose_)
      { verbose = verbose_; }
    
  
   void stringToData (const string &x, string &value)
   { value = trim(x); }

 void stringToData (const string &x, double &value)
  {
    istringstream strstrm(x);
    strstrm >> value;
  }
 
 void stringToData (const string &x, int &value)
  {
    istringstream strstrm(x);
    strstrm >> value;
  }
      
  void stringToData (const string &x, bool &value)
  {
    const char *x2 = x.c_str();
    const char *fval[] = {"F","f","n","N","false",".false.","FALSE",".FALSE." };
    const char *tval[] = {"T","t","y","Y","true",".true.","TRUE",".TRUE." };
    for (tsize i=0; i< sizeof(fval)/sizeof(fval[0]); ++i)
      if (strcmp(x2,fval[i])==0) { value=false; return; }
    for (tsize i=0; i< sizeof(tval)/sizeof(tval[0]); ++i)
      if (strcmp(x2,tval[i])==0) { value=true; return; }
    throw "conversion error in stringToData<bool>";
  }

    bool param_present(const std::string &key) const
      { return (params.find(key)!=params.end()); }

      
    string find (const std::string &key) 
      {
      string result;
      result = get_valstr(key);
      if (verbose && param_unread(key))
        std::cout << "Parser: " << key << " = " << result << std::endl;
      read_params.insert(key);
      return result;
      }

    int findInt(const std::string &key)  {
       string val = find(key) ;
       int res ;
       stringToData(val,res) ;
       return res ;
    }

    double findDouble(const std::string &key)  {
       string val = find(key) ;
       double res ;
       stringToData(val,res) ;
       return res ;
    }

    const params_type &getParams() const
      { return params; }

  };

