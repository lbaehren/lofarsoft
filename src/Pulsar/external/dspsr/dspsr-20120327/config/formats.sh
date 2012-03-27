#!/bin/sh

available_formats=`cd Kernel/Formats > /dev/null && echo */Makefile.am | sed -e 's|/Makefile.am||g'`

echo Available formats:

#
# Create an AC_DEFINE and AM_CONDITIONAL for each available format
#

cat > config/bootstrap.m4 <<EOF

dnl DO NOT EDIT - this file is generated by config/formats.sh during bootstrap

AC_DEFUN([DSPSR_FORMATS],
[
  DSPSR_SELECTED_FORMATS([$available_formats])

EOF

for format in $available_formats; do

  echo $format

  cat >> config/bootstrap.m4 <<EOF

  DSPSR_TEST_FORMAT([${format}])

  if test \$${format}_selected = 1; then
    AC_DEFINE([HAVE_${format}], [1],
              [Define to 1 to use the ${format} file format])
  fi

  AM_CONDITIONAL([HAVE_${format}], [test \$${format}_selected = 1])

EOF

done

echo "])" >> config/bootstrap.m4

#
# Conditionally add SUBDIRS for each available format
#

echo "# Generated by bootstrap; do not edit" > config/Makefile.bootstrap
echo "SUBDIRS =" >> config/Makefile.bootstrap
echo "libFormats_la_LIBADD =" >> config/Makefile.bootstrap

for format in $available_formats; do

  echo "if HAVE_${format}" >> config/Makefile.bootstrap
  echo "  SUBDIRS += ${format}" >> config/Makefile.bootstrap
  echo "  libFormats_la_LIBADD += ${format}/lib${format}.la" >> config/Makefile.bootstrap
  echo "endif" >> config/Makefile.bootstrap

done

echo "SUBDIRS += ." >> config/Makefile.bootstrap

