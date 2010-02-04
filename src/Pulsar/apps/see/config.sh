#!/bin/sh
#
# configure script
#
rm -f config.h
touch .check.h

#
# check for <sys/time.h>
#
cat <<EOF >.chech.h
include <sys/time.h>
EOF
if gcc -E .check.h
 then
  echo '#define HAVE_SYS_TIME_H 1' >> config.h
fi

#
# check for <sys/utsname.h>
# 
cat <<EOF >.chech.h
include <sys/utsname.h>
EOF
if gcc -E .check.h
 then
  echo '#define HAVE_SYS_UTSNAME_H 1' >> config.h
fi

#
# check for <sys/sysinfo.h>
#
cat <<EOF >.chech.h
include <sys/sysinfo.h>
EOF
if gcc -E .check.h
 then
  echo '#define HAVE_SYS_SYSINFO_H 1' >> config.h
fi

#
# check for <linux/version.h>
#
cat <<EOF >.chech.h
include <linux/version.h>
EOF
if gcc -E .check.h
 then
  echo '#define HAVE_LINUX_VERSION_H 1' >> config.h
fi

