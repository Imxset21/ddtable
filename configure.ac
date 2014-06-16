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

# Checks for (optional) libraries.
have_math=no
AC_SEARCH_LIBS([exp], [m], [have_math=yes])
have_random=no
AC_SEARCH_LIBS([rand], [random], [have_random=yes])

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h memory.h stddef.h stdint.h stdlib.h string.h])

# Checks for (optional) header files.
if test "x${have_math}" = xyes; then
   AC_CHECK_HEADERS([math.h], [], [have_math=no])
fi

if test "x${have_math}" = xno; then
   AC_MSG_WARN([
   -------------------------------------
   Unable to find math.h on this system.
   Building tests without math.h.
   -------------------------------------])
fi

# Checks for compiler characteristics.
AC_PROG_CC_C99
AC_C_INLINE
AC_C_CONST
AC_C_RESTRICT

# Checks for types/typedefs.
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MEMCMP
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
                 common/Makefile
                 include/Makefile
                 libddtable/Makefile
                 src/Makefile])
AC_OUTPUT
