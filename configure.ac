#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(libshare, 2.1.0, bburrell.brian@gmail.com)
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE(libshare, 2.1.0) 
AC_CONFIG_SRCDIR([src/share-lib])
AC_CONFIG_HEADERS([config.h])
#AC_CONFIG_HEADERS([po/Makefile.in])
AC_CONFIG_MACRO_DIR([m4])

LT_INIT

AC_LIB_LINKFLAGS([pthread])

AC_CONFIG_FILES([src/java-share-lib/test_libshare_java], [chmod +x src/java-share-lib/test_libshare_java])
AC_CONFIG_FILES([bin/README.txt], [chmod -wx bin/README.txt])

# Checks for programs.
AC_PROG_CC
gl_EARLY

AC_PROG_CXX
AC_EXEEXT
AC_PROG_YACC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_LIBTOOL
AC_PROG_YACC
AM_PROG_CC_C_O

AC_SYS_LARGEFILE
AC_ISC_POSIX
AC_C_INLINE

# Checks for header files.
AC_CHECK_HEADERS(fcntl.h linux/fd.h memory.h net/errno.h \
  sgtty.h string.h \
  sys/param.h sys/device.h sys/gentape.h \
  sys/inet.h sys/io/trioctl.h \
  sys/mtio.h sys/time.h sys/tprintf.h sys/tape.h \
  unistd.h locale.h arpa/inet.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h sys/socket.h sys/resources.h unistd.h, wctype.h wchar.h utime.h signal.h pthread.h wchar.h sys/select.h)

#AC_CHECK_HEADERS([sys/buf.h], [], [],
#[#if HAVE_SYS_PARAM_H
##include <sys/param.h>
##endif])

AC_HEADER_SYS_WAIT
AC_HEADER_DIRENT
AC_HEADER_MAJOR
AC_HEADER_STAT
AC_HEADER_STDC

AC_CHECK_LIB([m], [fabs])
AC_CHECK_LIB([pthread], [pthread_self])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t stdout])

gt_TYPE_SSIZE_T
gl_INIT

AC_CHECK_DECLS([getgrgid],,, [#include <grp.h>])
AC_CHECK_DECLS([getpwuid],,, [#include <pwd.h>])
AC_CHECK_DECLS([time],,, [#include <time.h>])

AC_REPLACE_FUNCS(waitpid)


# mmap() support
AC_FUNC_MMAP
if test x$ac_cv_func_mmap_fixed_mapped = xyes; then
  AC_CHECK_FUNCS(munmap msync)
fi

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_MEMCMP
AC_FUNC_STRFTIME
AC_FUNC_ALLOCA
AC_CHECK_FUNCS([getcwd gethostbyname gettimeofday memmove memset mkdir _mkdir socket strchr strdup strerror strrchr strtol inet_addr ftell stat fstat getrusage fopen fclose fread fwrite pthread_mutex_lock fchmod fchown fsync lstat mkfifo readlink symlink kill ftruncate memchr regcomp select setenv setlocale strtoumax open close read write socket ioctl fcntl connect bind accept listen pthread_mutex_unlock calloc gettid pthread_self pthread_mutex_init pthread_mutex_destroy])

AC_CHECK_MEMBERS([struct stat.st_rdev])

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

AM_CONDITIONAL([LIBUNISTRING_COMPILE_UNIWIDTH_WIDTH], [false])

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

## Gettext.
#AM_GNU_GETTEXT([external], [need-formatstring-macros])
#AM_GNU_GETTEXT_VERSION([0.16])

## Iconv
#AM_ICONV
#AC_CHECK_HEADERS(iconv.h)
#AC_CHECK_TYPE(iconv_t,:,
#              AC_DEFINE(iconv_t, int,
#                        [Conversion descriptor type]),
#              [
##ifdef HAVE_ICONV_H
## include <iconv.h>
##endif
#])



AC_OUTPUT(Makefile wrap/java/Makefile wrap/php/Makefile src/gnu/Makefile src/share-lib/Makefile src/java-share-lib/Makefile src/share-util/Makefile scripts/Makefile testing/Makefile doc/Makefile examples/Makefile include/Makefile bin/Makefile)

