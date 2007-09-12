import os, genapi, sys

def move(s, d):
    try:
        os.remove(d);
    except:
        pass;
    os.rename(s, d);

def check_rebuild(source, target):
    if not os.path.exists(target):
        return 1
    targettime = os.path.getmtime(target)
    sourcetime = os.path.getmtime(source)
    return targettime <= sourcetime

def need_rebuild(sources, targets):
    for t in targets:
        for s in sources:
            if check_rebuild(s, t):
                return 1
    return 0
        
def main():
    functions="Include/numarray/libnumarray.fnc"
    
    body_files=["Src/libnumarray.ch"]
    
    extra_dependencies=["Include/numarray/genapis.py",
                        "Include/numarray/genapi.py"]

    outputs=["Include/numarray/libnumarray.h",
             "Src/libnumarraymodule.c"]
    
    header_includes=["arraybase.h",
                     "nummacro.h",
                     "numcomplex.h",
                     "ieeespecial.h",
                     "cfunc.h"]
    
    if (need_rebuild( [functions] + body_files + extra_dependencies, outputs)
        or "--check-dates" not in sys.argv):
        
        genapi.main(module="libnumarray",
                    qualified_module="numarray.libnumarray",
                    functions=functions,
                    header_includes=header_includes,
                    body_files=body_files)
        move("libnumarray.h", "Include/numarray/libnumarray.h")
        move("libnumarray.c", "Src/libnumarraymodule.c")

        genapi.main(module="libteacup",
                    qualified_module="numarray.libteacup",
                    functions = "Include/numarray/libteacup.fnc",
                    header_includes = [],
                    body_files=["Src/teacup.ch"]);
        move("libteacup.h","Include/numarray/libteacup.h");
        move("libteacup.c", "Src/libteacupmodule.c");

        genapi.main(module="libnumeric",
                    qualified_module="numarray.libnumeric",
                    functions = "Include/numarray/libnumeric.fnc",
                    header_includes = [],
                    body_files=["Src/libnumeric.ch"])
        move("libnumeric.h", "Include/numarray/libnumeric.h");
        move("libnumeric.c", "Src/libnumericmodule.c")
    
if __name__ == "__main__":
    main()
