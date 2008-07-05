from distutils.core import Extension
import numinclude
import sys

class NumarrayExtension(Extension):
    def __init__(self, module, sources=None, **keywords):
        
        if sources is None:
            sources = [module+"module.c"]
            
        if "include_dirs" not in keywords.keys():
            keywords["include_dirs"] = []
        keywords["include_dirs"].append(numinclude.include_dir)

        if "extra_compile_args" not in keywords.keys():
            keywords["extra_compile_args"] = []
        keywords["extra_compile_args"].extend(EXTRA_COMPILE_ARGS)
        
        if "extra_link_args" not in keywords.keys():
            keywords["extra_link_args"] = []
        keywords["extra_link_args"].extend(EXTRA_LINK_ARGS)
        
        Extension.__init__(self, module, sources, **keywords)


EXTRA_LINK_ARGS = []          # Link options for all platforms
EXTRA_COMPILE_ARGS = []       # Compile options for all platforms

if sys.platform == "osf1v5":
    EXTRA_COMPILE_ARGS.extend(["-ieee"])

if not hasattr(sys, 'version_info') or sys.version_info < (2,2,0,'final'):
    raise SystemExit, "Python 2.2 or later required to build numarray."

