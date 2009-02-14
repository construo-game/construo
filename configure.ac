##  Construo - A wire-frame construction game
##  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
##
##  This program is free software; you can redistribute it and/or
##  modify it under the terms of the GNU General Public License
##  as published by the Free Software Foundation; either version 2
##  of the License, or (at your option) any later version.
## 
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##  
##  You should have received a copy of the GNU General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

# init automake/conf
AC_INIT(construo, 0.2.3-CVS)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR(src/construo.hpp)
AM_CONFIG_HEADER(config.h)

# check for some programs
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_INSTALL
AC_PATH_XTRA
AC_LANG(C++)

AC_MSG_CHECKING([whether we are enabling debug-mode])
AC_ARG_ENABLE([debug],
              AC_HELP_STRING([--enable-debug],
                             [enable debug support (default: no)]),,
              [enable_debug=no])
AC_MSG_RESULT($enable_debug)

if test x$enable_debug = xyes; then
  AC_DEFINE(DEBUG, 1, [Enable debuging])
  CXXFLAGS="-g -O0 -Wall -Werror"
fi

AC_MSG_CHECKING([whether we are enabling gprof profiling-support])
AC_ARG_ENABLE([gprof],
              AC_HELP_STRING([--enable-gprof],
                             [enable gprof profiling (default: no)]),,
              [enable_gprof=no])
AC_MSG_RESULT($enable_gprof)

if test x$enable_gprof = xyes; then
  CXXFLAGS="$CXXFLAGS -pg"
fi

AC_MSG_CHECKING([whether we are enabling FunctionCheck profiling])
AC_ARG_ENABLE([fcheck],
              AC_HELP_STRING([--enable-fcheck],
                             [enable fcheck profiling (default: no)]),,
              [enable_fcheck=no])
AC_MSG_RESULT($enable_fcheck)

if test x$enable_fcheck = xyes; then
  CXXFLAGS="`fc-config --cflags`"
  LIBS="$LIBS `fc-config --libs`"
fi

dnl Checking for the install prefix and then use it to define PINGUS_DATADIR
dnl PINGUS_EXPAND_DIR(pingus_datadir, "$datadir/games/$PACKAGE")
dnl AC_DEFINE_UNQUOTED(PINGUS_DATADIR, "$pingus_datadir", [Location of Pingus data files])
dnl AC_DEFINE(CONSTRUO_DATADIR, $datadir/games/$PACKAGE, [Location of Construo datafiles])
AC_DEFINE_DIR(DATADIR, datadir, [Location of datafiles])

AC_CHECK_LIB(z, gzopen)

dnl --------------------------------------
dnl -- Check for usable Display Targets --
dnl --------------------------------------
ConstruoTargets=""

dnl -------------------
dnl -- Check for X11 --
dnl -------------------
if test "$no_x" = "yes"; then
   echo ""
   echo "### Error: Couldn't find X11 header files, probally x11-devel packages"
   echo "###        not installed?"
   echo ""
else
    ConstruoTargets="$ConstruoTargets construo.x11${EXEEXT}"
    x11_LIBS="$x11_LIBS $X_LIBS $X_PRE_LIBS $X_EXTRA_LIBS -lX11"
    x11_CXXFLAGS="$CXXFLAGS $X_CFLAGS"

    AC_CHECK_LIB(Xext, XMissingExtension,
        [x11_LIBS="$x11_LIBS -lXext"
         AC_DEFINE(HAVE_LIBXEXT, 1, [Xext is available])],
         [], $x11_LIBS)

    AC_CHECK_LIB( Xxf86vm, XF86VidModeSwitchToMode,
      [x11_LIBS="$x11_LIBS -lXxf86vm"
       AC_DEFINE(HAVE_LIBXXF86VM, 1, [VidMode is available])],
       [], $x11_LIBS)

    AC_SUBST(x11_LIBS)
    AC_SUBST(x11_CXXFLAGS)
fi

dnl --------------------
dnl -- Check for Glut --
dnl --------------------

dnl On MacOSX $target gives: powerpc-apple-darwin6.6 and the define __apple__
dnl This doesn't seem entirily correct and there is a -framework GLUT
case "$target_os" in
     cygwin*)
        AC_CHECK_LIB(glut32,   main, 
                     [ConstruoTargets="$ConstruoTargets construo.glut${EXEEXT}"
                      AC_SUBST(glut_LIBS, "-lglut32 -lglu32 -lopengl32")
                      AC_SUBST(glut_CXXFLAGS, "")],
                     [AC_MSG_WARN([Couldn't detect glut])
                     ], [-lglu32 -lopengl32])
        ;;
     darwin*)
        MY_CHECK_GLUT_OSX([
        ConstruoTargets="$ConstruoTargets construo.glut"
        AC_SUBST(glut_LIBS, "-framework GLUT -framework OpenGL -framework Foundation")
        AC_SUBST(glut_CXXFLAGS, "-I/System/Library/Frameworks/GLUT.framework/Headers/")])
        ;;
     *)
        AC_CHECK_LIB(glut, glutMainLoop,
        [
          ConstruoTargets="$ConstruoTargets construo.glut"
          glut_LIBS="-lglut"
          AC_SUBST(glut_LIBS)
          AC_SUBST(glut_CXXFLAGS, "")
         ],
        [])
        ;;
esac

dnl ------------------------
dnl -- Check for FreeGlut --
dnl ------------------------
AC_CHECK_LIB(freeglut-1.3, glutMainLoop,
[
  ConstruoTargets="$ConstruoTargets construo.freeglut${EXEEXT}"
  AC_SUBST(freeglut_LIBS,"-lfreeglut-1.3")
dnl  echo "Freeglut: " $freeglut_LIBS
],
[])

if test -z "$ConstruoTargets"; then
   echo ""
   echo "ERROR: Neither X11 nor Glut could be detected, so Construo is not"
   echo "ERROR: buildable. You might need to install the -devel packages for"
   echo "ERROR: the libraries or check what else could have gone wrong."
   echo "ERROR: The file 'config.log' contains more details on the runed libary"
   echo "ERROR: checks and might help you to figure out what went wrong."
   echo ""
   exit 1
fi

AC_SUBST(ConstruoTargets)

AC_CONFIG_FILES(Makefile src/Makefile examples/Makefile)
AC_OUTPUT

echo ""
echo "   CXXFLAGS      = $CXXFLAGS"
echo "   LIBS          = $LIBS"
echo "   Debugging     = $enable_debug"
echo "   GProf         = $enable_gprof"
echo "   FunctionCheck = $enable_fcheck"
echo "   Targets       =$ConstruoTargets"
echo ""
echo "Now type 'make' and everything should work, if not, write a bug reports to:"
echo ""
echo "  Ingo Ruhnke <grumbel@gmx.de>"
echo ""

# EOF #
