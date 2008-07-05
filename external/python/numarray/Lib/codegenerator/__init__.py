def GenAll():
  from convcode import generate_conv_code
  from basecode import all_types

  generate_conv_code("Src/_convmodule.c")

  from bytescode import generate_bytes_code
  generate_bytes_code("Src/_bytesmodule.c")

  from sortcode import generate_sort_code
  generate_sort_code("Src/_sortmodule.c")

  from ufunccode import generate_ufunc_code
  for t in all_types():
    source = "Src/_ufunc%smodule.c" % (t,)
    generate_ufunc_code(source, t)

# =================================================================

from ufunccode import UfuncModule, make_stub

if __name__ == "__main__":
  GenAll()

