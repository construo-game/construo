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
AC_INIT(construo, 0.2.0)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR(construo.hxx)
AM_CONFIG_HEADER(config.h)

# check for some programs
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_CC
AC_PROG_INSTALL
AC_PATH_XTRA

ConstruoTargets=""

if test -n "$X_DISPLAY_MISSING"; then
   echo ""
   echo "### Error: Couldn't find X11 header files, probally x11-devel packages"
   echo "###        not installed?"
   echo ""
else
    ConstruoTargets="$ConstruoTargets construo.x11"
    LIBS="$LIBS $X_LIBS $X_PRE_LIBS $X_EXTRA_LIBS -lX11"
    CXXFLAGS="$CXXFLAGS $X_CFLAGS"
fi

AC_CHECK_LIB(Xext, XMissingExtension)
AC_CHECK_LIB(Xxf86vm, XF86VidModeSwitchToMode)

AC_CHECK_LIB(glut, glutMainLoop,
[
ConstruoTargets="$ConstruoTargets construo.glut"
AC_SUBST(glut_LIBS,"-lglut")
],
[])

AC_CHECK_LIB(freeglut-1.3, glutMainLoop,
[
ConstruoTargets="$ConstruoTargets construo.freeglut"
AC_SUBST(freeglut_LIBS,"-lfreeglut-1.3")
echo "Freeglut: " $freeglut_LIBS
],
[], "-lXxf86vm")

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

AC_CONFIG_FILES(Makefile examples/Makefile)
AC_OUTPUT

echo ""
echo "   CXXFLAGS = $CXXFLAGS"
echo "   LIBS     = $LIBS"
echo "   Targets  = $ConstruoTargets"

# EOF #