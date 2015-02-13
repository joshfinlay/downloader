#                                               -*- Autoconf -*- Process 
# this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([downloader], [0.2], [josh@finlay.id.au])
AC_OUTPUT(Makefile src/Makefile)
AC_CONFIG_SRCDIR([src/downloader.c])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

PKG_CHECK_MODULES([libcurl], [libcurl])
PKG_CHECK_MODULES([ncurses], [ncurses])
# PKG_CHECK_MODULES([curses], [curses])

# PKG_CHECK_MODULES([libreadline], [libreadline])
# PKG_CHECK_MODULES([termcap], [termcap])

AC_OUTPUT

AC_MSG_NOTICE([])
AC_MSG_NOTICE([=================================])
AC_MSG_NOTICE([ Looks like configure went well! ])
AC_MSG_NOTICE([])
AC_MSG_NOTICE([ Please run make && make install ])
AC_MSG_NOTICE([=================================])

