ACLOCAL_AMFLAGS = -I m4 --install
AM_CFLAGS = -g -O2 -std=gnu99 -Wall -Wextra
SUBDIRS = lib

noinst_LIBRARIES = build/libutil.a
build_libutil_a_SOURCES = src/util/dbg.h src/util/csv.c src/util/csv.h
build_libutil_a_CPPFLAGS = -I$(top_srcdir)/src -DNDEBUG

dist_man_MANS = man/ncount.1

bin_PROGRAMS = bin/ncount
bin_ncount_SOURCES = src/ncount.c
bin_ncount_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/lib -DNDEBUG
bin_ncount_LDADD = build/libutil.a lib/libgnu.a

# check_PROGRAMS = tests/darray_tests
# tests_darray_tests_SOURCES = tests/darray_tests.c tests/minunit.h
# tests_darray_tests_CPPFLAGS = -I$(top_srcdir)/src -DNDEBUG
# tests_darray_tests_LDADD = build/libutil.a
# TESTS = $(check_PROGRAMS)

EXTRA_DIST = m4/NOTES m4/gnulib-cache.m4
