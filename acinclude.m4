AC_DEFUN([AC_DEFINE_DIR], [
  test "x$prefix" = xNONE && prefix="$ac_default_prefix"
  test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'
  ac_define_dir=`eval echo [$]$2`
  ac_define_dir=`eval echo [$]ac_define_dir`
  ifelse($3, ,
    AC_DEFINE_UNQUOTED($1, "$ac_define_dir"),
    AC_DEFINE_UNQUOTED($1, "$ac_define_dir", $3))
])

AC_DEFUN([MY_CHECK_GLUT_OSX], [
 AC_MSG_CHECKING([for glut])
 save_libs="$LIBS"
 LIBS="-I/System/Library/Frameworks/GLUT.framework/Headers/ -framework GLUT -framework OpenGL -framework Foundation"
 AC_TRY_LINK_FUNC(glutMainLoop,
        [glut_available=yes
        $1],
        [glut_available=no])
 LIBS="$save_libs"
 AC_MSG_RESULT([$glut_available])
])

dnl EOF dnl
