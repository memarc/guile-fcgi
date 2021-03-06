dnl aclocal.m4 generated automatically by aclocal 1.4

dnl Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
dnl This file is free software; the Free Software Foundation
dnl gives unlimited permission to copy and/or distribute it,
dnl with or without modifications, as long as this notice is preserved.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY, to the extent permitted by law; without
dnl even the implied warranty of MERCHANTABILITY or FITNESS FOR A
dnl PARTICULAR PURPOSE.

dnl   Automake macros for working with Guile.
dnl   
dnl   	Copyright (C) 1998 Free Software Foundation, Inc.
dnl   
dnl   This program is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2, or (at your option)
dnl   any later version.
dnl   
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl   
dnl   You should have received a copy of the GNU General Public License
dnl   along with this software; see the file COPYING.  If not, write to
dnl   the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
dnl   Boston, MA 02111-1307 USA
dnl   
dnl   As a special exception, the Free Software Foundation gives permission
dnl   for additional uses of the text contained in its release of GUILE.
dnl   
dnl   The exception is that, if you link the GUILE library with other files
dnl   to produce an executable, this does not by itself cause the
dnl   resulting executable to be covered by the GNU General Public License.
dnl   Your use of that executable is in no way restricted on account of
dnl   linking the GUILE library code into it.
dnl   
dnl   This exception does not however invalidate any other reasons why
dnl   the executable file might be covered by the GNU General Public License.
dnl   
dnl   This exception applies only to the code released by the
dnl   Free Software Foundation under the name GUILE.  If you copy
dnl   code from other Free Software Foundation releases into a copy of
dnl   GUILE, as the General Public License permits, the exception does
dnl   not apply to the code that you add in this way.  To avoid misleading
dnl   anyone as to the status of such modified files, you must delete
dnl   this exception notice from them.
dnl   
dnl   If you write modifications of your own for GUILE, it is your choice
dnl   whether to permit this exception to apply to your modifications.
dnl   If you do not wish that, delete this exception notice.


dnl   GUILE_FLAGS --- set flags for compiling and linking with Guile
dnl 
dnl   This macro runs the `guile-config' script, installed with Guile,
dnl   to find out where Guile's header files and libraries are
dnl   installed.  It sets two variables, marked for substitution, as
dnl   by AC_SUBST.
dnl
dnl	GUILE_CFLAGS --- flags to pass to a C or C++ compiler to build
dnl		code that uses Guile header files.  This is almost
dnl		always just a -I flag.
dnl
dnl     GUILE_LDFLAGS --- flags to pass to the linker to link a
dnl		program against Guile.  This includes `-lguile' for
dnl		the Guile library itself, any libraries that Guile
dnl		itself requires (like -lqthreads), and so on.  It may
dnl		also include a -L flag to tell the compiler where to
dnl		find the libraries.

AC_DEFUN([GUILE_FLAGS],[
  ## First, let's just see if we can find Guile at all.
  AC_MSG_CHECKING(for Guile)
  guile-config link > /dev/null || {
    echo "configure: cannot find guile-config; is Guile installed?" 1>&2
    exit 1
  }
  GUILE_CFLAGS="`guile-config compile`"
  GUILE_LDFLAGS="`guile-config link`"
  AC_SUBST(GUILE_CFLAGS)
  AC_SUBST(GUILE_LDFLAGS)
  AC_MSG_RESULT(yes)
])

dnl   FCGI_FLAGS --- set flags for compiling and linking with libfcgi
dnl 
dnl   This macro looks for the fcgiapp.h and libfcgi.a files installed
dnl   with FCGI DevKit.  It sets two variables, marked for substitution, as
dnl   by AC_SUBST.
dnl
dnl	FCGI_CFLAGS --- flags to pass to a C or C++ compiler to build
dnl		code that uses libfcgi header files.  This is almost
dnl		always just a -I flag.
dnl
dnl     FCGI_LDFLAGS --- flags to pass to the linker to link a
dnl		program against libfcgi.  This includes `-lfcgi' for
dnl		the fcgi library itself. It may also include a -L 
dnl             flag to tell the compiler where to find the libraries

AC_DEFUN([FCGI_FLAGS],[
  AC_ARG_WITH(fcgi,
[  --with-fcgi=DIR        look for libfcgi includes in DIR/include and
                             libfcgi.a in DIR/lib (default=/usr/local)
                             See also --with-libfcgi-includes and 
                             --with-libfcgi-lib below],
   [AC_MSG_CHECKING("$withval/include/fcgiapp.h exists")
    if test ! -f "$withval/include/fcgiapp.h" ; then
      AC_MSG_RESULT(no)
      AC_ERROR("$withval/include/fcgiapp.h does not exist.")
    else
      AC_MSG_RESULT(yes)
    fi
    AC_MSG_CHECKING("$withval/include/fcgi_stdio.h exists")
    if test ! -f "$withval/include/fcgi_stdio.h" ; then
      AC_MSG_RESULT(no)
      AC_ERROR("$withval/include/fcgi_stdio.h does not exist.")
    else
      AC_MSG_RESULT(yes)
    fi
    AC_MSG_CHECKING("$withval/lib/libfcgi.a exists")
    if test ! -f "$withval/lib/libfcgi.a" ; then
      AC_MSG_RESULT(no)
      AC_ERROR("$withval/include/libfcgi.a does not exist.")
    else
      AC_MSG_RESULT(yes)
    fi
    FCGI_CFLAGS="-I$withval/include"
    FCGI_LDFLAGS="$withval/lib/libfcgi.a"
   ],[
    AC_ARG_WITH(fcgi-includes,
[  --with-fcgi-includes=DIR    look for libfcgi includes in DIR ],[
       if test ! -f "$withval/fcgiapp.h" ; then
        AC_ERROR("$withval/fcgiapp.h does not exist.")
       fi
       if test ! -f "$withval/fcgi_stdio.h" ; then
        AC_ERROR("$withval/fcgi_stdio.h does not exist.")
       fi
       FCGI_CFLAGS="-I$withval"],[
       AC_CHECK_HEADERS(fcgiapp.h)
       if test ! "$ac_cv_header_fcgiapp_h" = yes ; then
         AC_ERROR("Cannot build without fcgiapp.h. Use --with-fcgi-includes ?")
       fi
       AC_CHECK_HEADERS(fcgi_stdio.h)
       if test ! "$ac_cv_header_fcgi_stdio_h" = yes ; then
         AC_ERROR("Cannot build without fcgi_stdio.h. Use --with-fcgi-includes ?")
       fi
    ])
    AC_ARG_WITH(fcgi-lib,
[  --with-fcgi-lib=DIR    look for libfcgi libraries in DIR ],[
       AC_MSG_CHECKING("$withval/libfcgi.a exists")
       if test ! -f "$withval/libfcgi.a" ; then
        AC_MSG_RESULT(no)
        AC_ERROR("$withval/libfcgi.a does not exist.")
       else
        AC_MSG_RESULT(yes)
       fi
       FCGI_LDFLAGS="-L$withval -lfcgi"],[
	   echo "ac_include: FCGI_LDFLAGS=$FCGI_LDFLAGS"],[
       AC_CHECK_LIB(fcgi,FCGX_Accept)
       if test ! "$ac_cv_lib_fcgi_FCGX_Accept" = yes ; then
         AC_ERROR("Cannot build without libfcgi. Use --with-fcgi-lib ?")
       fi
       FCGI_LDFLAGS="-lfcgi"
    ])
  ])
  AC_SUBST(FCGI_CFLAGS)
  AC_SUBST(FCGI_LDFLAGS)
])


# Do all the work for Automake.  This macro actually does too much --
# some checks are only needed if your package does certain things.
# But this isn't really a big deal.

# serial 1

dnl Usage:
dnl AM_INIT_AUTOMAKE(package,version, [no-define])

AC_DEFUN(AM_INIT_AUTOMAKE,
[AC_REQUIRE([AC_PROG_INSTALL])
PACKAGE=[$1]
AC_SUBST(PACKAGE)
VERSION=[$2]
AC_SUBST(VERSION)
dnl test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" && test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi
ifelse([$3],,
AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package]))
AC_REQUIRE([AM_SANITY_CHECK])
AC_REQUIRE([AC_ARG_PROGRAM])
dnl FIXME This is truly gross.
missing_dir=`cd $ac_aux_dir && pwd`
AM_MISSING_PROG(ACLOCAL, aclocal, $missing_dir)
AM_MISSING_PROG(AUTOCONF, autoconf, $missing_dir)
AM_MISSING_PROG(AUTOMAKE, automake, $missing_dir)
AM_MISSING_PROG(AUTOHEADER, autoheader, $missing_dir)
AM_MISSING_PROG(MAKEINFO, makeinfo, $missing_dir)
AC_REQUIRE([AC_PROG_MAKE_SET])])

#
# Check to make sure that the build environment is sane.
#

AC_DEFUN(AM_SANITY_CHECK,
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftestfile
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftestfile 2> /dev/null`
   if test "[$]*" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftestfile`
   fi
   if test "[$]*" != "X $srcdir/configure conftestfile" \
      && test "[$]*" != "X conftestfile $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "[$]2" = conftestfile
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
rm -f conftest*
AC_MSG_RESULT(yes)])

dnl AM_MISSING_PROG(NAME, PROGRAM, DIRECTORY)
dnl The program must properly implement --version.
AC_DEFUN(AM_MISSING_PROG,
[AC_MSG_CHECKING(for working $2)
# Run test in a subshell; some versions of sh will print an error if
# an executable is not found, even if stderr is redirected.
# Redirect stdin to placate older versions of autoconf.  Sigh.
if ($2 --version) < /dev/null > /dev/null 2>&1; then
   $1=$2
   AC_MSG_RESULT(found)
else
   $1="$3/missing $2"
   AC_MSG_RESULT(missing)
fi
AC_SUBST($1)])

