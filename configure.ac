#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(libshare, 2.0.4, bburrell.brian@gmail.com)
AM_INIT_AUTOMAKE(libshare, 2.0.4) 
AC_CONFIG_SRCDIR([src/share-lib])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_HEADER([src/share-lib/share_version.h])
AC_CONFIG_MACRO_DIR([m4])



# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL


# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h stddef.h stdint.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_STRUCT_TM
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_MEMCMP
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([dup2 getcwd gethostbyname gettimeofday memmove memset mkdir socket strchr strdup strerror strrchr strtol inet_addr])

# scripts #
AC_CONFIG_FILES([scripts/shnetd], [chmod +x scripts/shnetd])
AC_CONFIG_FILES([scripts/shcat], [chmod +x scripts/shcat])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AM_CONDITIONAL([HAVE_DOXYGEN],
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/doxygen.conf])])

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: yes]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac], [debug=true])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")


AC_PATH_PROG(HELP2MAN, help2man, @echo WARNING: skipped: help2man )

AM_PROG_CC_C_O

AC_OUTPUT(Makefile src/share-lib/Makefile src/share-util/Makefile scripts/Makefile testing/Makefile doc/Makefile)
