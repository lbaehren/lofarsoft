AC_DEFUN([SWIN_DOWNLOAD],
[
  AC_PROVIDE([SWIN_DOWNLOAD])

  AC_PATH_PROG(WGET, wget, no)
  if test $WGET != no; then
    download=$WGET
  else
    AC_PATH_PROG(CURL, curl, no)
    if test $CURL != no; then
      download="$CURL -f -L -O"
    else
      AC_MSG_WARN([No download program was found])
      download="echo Cannot download"
    fi
  fi

  AC_SUBST(download)

])

