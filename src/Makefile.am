bin_PROGRAMS = ddtable_test
ddtable_test_SOURCES = ddtable_test.c
ddtable_test_CPPFLAGS = -I$(top_srcdir)/common -I$(top_srcdir)/include
ddtable_test_LDADD = ../libddtable/libddtable.la ../common/libspookyc.la

check_SCRIPTS = unittest.sh
TESTS = $(check_SCRIPTS)

unittest.sh:
	echo './ddtable_test == 0' > unittest.sh
	chmod +x unittest.sh

CLEANFILES = unittest.sh
