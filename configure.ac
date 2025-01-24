#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(
    [ncount],
    [0.0.1],
    [https://github.com/mgualdron/ncount/issues],
    [ncount],
    [https://github.com/mgualdron/ncount])

AC_CONFIG_SRCDIR([src/ncount.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# AutoMake
AM_INIT_AUTOMAKE([1.11 subdir-objects])
AC_CONFIG_FILES([Makefile lib/Makefile])

# Checks for programs.
AC_PROG_CC
gl_EARLY
gl_INIT
AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for libraries.
# AC_CHECK_LIB([csv], [csv_parse], [LIBS="-l:libcsv.a $LIBS"] [AC_DEFINE([HAVE_LIBCSV], [1], [Define if csv_parse is found.])])

# Checks for header files.
# AC_CHECK_HEADERS([locale.h stdlib.h string.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memset setlocale strdup getline])

AC_OUTPUT

echo \
"-----------------------------------------------------------
Thank you for installing ${PACKAGE_NAME} version ${PACKAGE_VERSION}.

Installation directory prefix: '${prefix}'.
Compilation command: '${CC} ${CFLAGS} ${CPPFLAGS}'

To build, test, and install the program, type the following:

    make
    make check
    make install

------------------------------------------------------------"
