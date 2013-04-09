#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(libshare, 2.0.5, bburrell.brian@gmail.com)
AM_INIT_AUTOMAKE(libshare, 2.0.5) 
AC_CONFIG_SRCDIR([src/share-lib])
AC_CONFIG_HEADERS([config.h])
#AC_CONFIG_HEADER([src/share-lib/share_version.h])
AC_CONFIG_MACRO_DIR([m4])


# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_PROG_MKDIR_P
AC_PROG_LIBTOOL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/socket.h sys/time.h sys/resource.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t stdout])

# mmap() support
AC_FUNC_MMAP
if test x$ac_cv_func_mmap_fixed_mapped = xyes; then
  AC_CHECK_FUNCS(munmap msync)
fi

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_MEMCMP
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([dup2 getcwd gethostbyname gettimeofday memmove memset mkdir _mkdir socket strchr strdup strerror strrchr strtol inet_addr fopen fread fwrite fclose ftell stat fstat getrusage])

# scripts #
AC_CONFIG_FILES([scripts/shared], [chmod +x scripts/shared])
AC_CONFIG_FILES([scripts/shcat], [chmod +x scripts/shcat])
AC_CONFIG_FILES([scripts/shls], [chmod +x scripts/shls])
AC_CONFIG_FILES([scripts/shrm], [chmod +x scripts/shrm])
AC_CONFIG_FILES([scripts/shmkdir], [chmod +x scripts/shmkdir])
AC_CONFIG_FILES([scripts/shcp], [chmod +x scripts/shcp])
AC_CONFIG_FILES([scripts/shping], [chmod +x scripts/shping])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AM_CONDITIONAL([HAVE_DOXYGEN],
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/doxygen.conf])])

AC_CHECK_PROGS([JAVA], [java])
if test -z "$JAVA";
   then AC_MSG_WARN([Java not found - continuing without Java Compiler support])
fi
AM_CONDITIONAL([HAVE_JAVA], [test -n "$JAVA"])

AC_CHECK_PROGS([JAVAC], [javac])
if test -z "$JAVAC";
   then AC_MSG_WARN([Java Compiler not found - continuing without Java Compiler support])
fi
AM_CONDITIONAL([HAVE_JAVAC], [test -n "$JAVAC"])

AC_CHECK_PROGS([JAR], [jar])
if test -z "$JAR";
   then AC_MSG_WARN([Jar not found - continuing without Jar support])
fi
AM_CONDITIONAL([HAVE_JAR], [test -n "$JAR"])

AC_CHECK_PROGS([SWIG], [swig])
if test -z "$SWIG";
   then AC_MSG_WARN([Swig not found - continuing without Swig support])
fi
AM_CONDITIONAL([HAVE_SWIG], [test -n "$SWIG"])


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


AC_OUTPUT(Makefile src/share-lib/Makefile src/java-share-lib/Makefile src/share-util/Makefile scripts/Makefile testing/Makefile doc/Makefile examples/Makefile bin/Makefile)

