#!/bin/sh

aclocal
automake --add-missing --force --copy
autoconf
autoheader

# EOF #
