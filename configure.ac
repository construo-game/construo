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
AC_CONFIG_SRCDIR(construo.hxx)
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)

# check for some programs
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_CC
AC_PROG_INSTALL
AC_PATH_XTRA

ConstruoTargets=""


if test ! -n X_DISPLAY_MISSING; then
   echo ""
   echo "### Error: Couldn't find X11 header files, probally x11-devel packages"
   echo "###        not installed?"
   echo ""
   exit 1
else
    ConstruoTargets="$ConstruoTargets construo.x11"
    LIBS="$LIBS $X_LIBS $X_PRE_LIBS $X_EXTRA_LIBS -lX11"
    CXXFLAGS="$CXXFLAGS $X_CFLAGS"
fi

AC_CHECK_LIB(glut, glutMainLoop,
[
ConstruoTargets="$ConstruoTargets construo.glut"
LIBS="$LIBS -lglut"
])

AC_SUBST(ConstruoTargets)

AC_CONFIG_FILES(Makefile examples/Makefile)
AC_OUTPUT

echo ""
echo "   CXXFLAGS = $CXXFLAGS"
echo "   LIBS     = $LIBS"
echo "   Targets  = $ConstruoTargets"

# EOF #