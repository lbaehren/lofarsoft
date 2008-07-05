import sys, os

from f2py_tests import fwrite

def subprocess_result(library, behavior, kind):
    input = os.popen("python f2py_tests.py %s %s %s" %
                     (library,behavior,kind), "r", -1)
    return input.read()
    
def main():
    library = "Numeric"
    behavior = "standard"
    expected = {}
    for kind in kinds:
        expected[kind] = subprocess_result(library, behavior, kind)
        if expected[kind].find("error") > 0:
            fwrite("Numeric", "standard", kind,"ref count bad")
            fwrite("Got:")
            fwrite(expected[kind])
            fwrite("*"*65)
        else:
            fwrite("Numeric", "standard", kind,"ref count ok")
        # print expected[kind]
    library = "numarray"
    for behavior in ["standard","byteswapped","misaligned","strided"]:
        for kind in kinds:
            got = subprocess_result(library, behavior, kind)
            if got != expected[kind]:
                fwrite("Unexpected result for", repr(behavior), "in test", repr(kind))
                fwrite("Expected:")
                fwrite(str(expected[kind]))
                fwrite("Got:")
                fwrite(got)
                fwrite("*"*65)
            else:
                fwrite("numarray", behavior, kind, "ok")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        kinds = ["in_f","in_c","in_comma_out_f","in_comma_out_c",
                 "inout_f","inout_c"]  # ,"out_f","out_c"]
    else:
        kinds = sys.argv[1:]
        
    main()
