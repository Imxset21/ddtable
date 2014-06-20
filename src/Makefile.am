bin_PROGRAMS = test_ddtable_create.bin test_ddtable_hash.bin

# Testing table creation & deletion
test_ddtable_create_bin_SOURCES = test_ddtable_create.c
test_ddtable_create_bin_CPPFLAGS = -I$(top_srcdir)/common -I$(top_srcdir)/include
test_ddtable_create_bin_LDADD = ../libddtable/libddtable.la ../common/libddtablehashes.la

# Testing table hashing for random variables
test_ddtable_hash_bin_SOURCES = test_ddtable_hash.c
test_ddtable_hash_bin_CPPFLAGS = -I$(top_srcdir)/common -I$(top_srcdir)/include
test_ddtable_hash_bin_LDADD = ../libddtable/libddtable.la ../common/libddtablehashes.la

# Create test scripts
check_SCRIPTS = test_ddtable_create.sh test_ddtable_hash.sh
TESTS = $(check_SCRIPTS)

$(check_SCRIPTS): %.sh : %.bin
	echo './$< == 0' > $@
	chmod +x $@

CLEANFILES = $(check_SCRIPTS)
