#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ddtable], [1.0], [pr273@cornell.edu])
AM_INIT_AUTOMAKE
LT_PREREQ([2.2])
LT_INIT([dlopen])
AC_CONFIG_SRCDIR([src/ddtable_test.c]) 
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_LANG([C])
AC_PROG_CC([gcc llvm cc])
AC_PROG_CC_C_O
# AC_PROG_RANLIB - Not needed since we're using libtool
AC_PROG_INSTALL

# Sets flags
CFLAGS="-std=c99 -O3 -Wall -Wextra -Wpedantic"

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h memory.h stddef.h stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset memcmp])

AC_CONFIG_FILES([Makefile
                 common/Makefile
                 include/Makefile
                 libddtable/Makefile
                 src/Makefile])
AC_OUTPUT
