#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(libshare, 2.16, support@neo-natura.com)
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE(libshare, 2.16) 
AC_CONFIG_SRCDIR([src/share-lib])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_HEADERS([src/share-lib/test_libshare.c])
AC_CONFIG_HEADERS([src/java-share-lib/libshare_java.i])
AC_CONFIG_HEADERS([src/php-share-lib/libshare_php.i])
#AC_CONFIG_HEADERS([po/Makefile.in])
AC_CONFIG_MACRO_DIR([m4])

LT_INIT

version_patch=16
version_info=2:16:0
AC_SUBST(version_info)


#AC_LIB_LINKFLAGS([pthread])

AC_CONFIG_FILES([bin/README.txt], [chmod -wx bin/README.txt])
AC_CONFIG_FILES([examples/README.txt], [chmod -wx examples/README.txt])
AC_CONFIG_FILES([src/java-share-lib/test_libshare_java.java])
AC_CONFIG_FILES([src/php-share-lib/test_libshare_php.php])
# php
AC_CONFIG_FILES([src/php-share-lib/class_share.php])
AC_CONFIG_FILES([src/php-share-lib/class_share_fs.php])
AC_CONFIG_FILES([src/php-share-lib/class_shinode.php])
AC_CONFIG_FILES([src/php-share-lib/class_shfile.php])
AC_CONFIG_FILES([src/php-share-lib/test_libshare_fs.php])
#
AC_CONFIG_FILES([src/java-share-lib/test_libshare_java.sh], [chmod a+rx src/java-share-lib/test_libshare_java.sh])
AC_CONFIG_FILES([src/php-share-lib/test_libshare_php.sh], [chmod a+rx src/php-share-lib/test_libshare_php.sh])
AC_CONFIG_FILES([src/php-share-lib/test_libshare_fs.sh], [chmod a+rx src/php-share-lib/test_libshare_fs.sh])

# Architecture
AC_MSG_CHECKING([for supported operating system])
case "$host_os" in
  *linux*)
  opsys=linux
AC_DEFINE([LINUX], 1, [Define for the Linux operating system.])
  ;;
  sunos4*)
  opsys=sunos4
AC_DEFINE([SUNOS4], 1, [Define for the SunOS 4.x operating system.])
  ;;
  solaris2* | sysv[[45]]* | irix[[56]]*)
  opsys=svr4
  AC_DEFINE([SVR4], 1, [Define for the System V release 4 operating
      system or a derivative like Solaris 2.x or Irix 5.x.])
  case "$host_os" in
  sysv4.2uw*)
AC_DEFINE(UNIXWARE, 2, [Define for UnixWare systems.])
  ;;
  sysv5*)
AC_DEFINE(UNIXWARE, 7, [Define for UnixWare systems.])
  ;;
  esac
  ;;
  freebsd*)
  opsys=freebsd
AC_DEFINE([FREEBSD], 1, [Define for the FreeBSD operating system.])
  ;;
  *)
  AC_MSG_RESULT([NO!])
AC_MSG_ERROR([operating system $host_os is not supported by strace])
  ;;
  esac
  AC_MSG_RESULT($opsys)
AC_MSG_CHECKING([for supported architecture])
  case "$host_cpu" in
  bfin)
  arch=bfin
AC_DEFINE([BFIN], 1, [Define for the Blackfin architecture.])
  ;;
  i[[3456]]86|pentium)
  arch=i386
AC_DEFINE([I386], 1, [Define for the i386 architecture.])
  ;;
  ia64)
  arch=ia64
AC_DEFINE([IA64], 1, [Define for the IA64 architecture.])
  ;;
  m68k)
  arch=m68k
AC_DEFINE([M68K], 1, [Define for the m68k architecture.])
  ;;
  sparc64*)
  arch=sparc64
AC_DEFINE([SPARC64], 1, [Define for the SPARC64 architecture.])
  ;;
  sparc*)
  arch=sparc
AC_DEFINE([SPARC], 1, [Define for the SPARC architecture.])
  ;;
  mips*)
  arch=mips
AC_DEFINE([MIPS], 1, [Define for the MIPS architecture.])
  ;;
  alpha*)
  arch=alpha
AC_DEFINE([ALPHA], 1, [Define for the Alpha architecture.])
  ;;
  powerpc*)
  arch=powerpc
AC_DEFINE([POWERPC], 1, [Define for the PowerPC architecture.])
  ;;
  arm*)
  arch=arm
AC_DEFINE([ARM], 1, [Define for the ARM architecture.])
  ;;
  avr32*)
  arch=avr32
AC_DEFINE([AVR32], 1, [Define for the AVR32 architecture.])
  ;;
  s390)
  arch=s390
AC_DEFINE([S390], 1, [Define for the S390 architecture.])
  ;;
  s390x)
  arch=s390x
AC_DEFINE([S390X], 1, [Define for the S390x architecture.])
  ;;
  hppa*|parisc*)
  arch=hppa
AC_DEFINE([HPPA], 1, [Define for the HPPA architecture.])
  ;;
  sh64*)
  arch=sh64
AC_DEFINE([SH64], 1, [Define for the SH64 architecture.])
  ;;
  sh*)
  arch=sh
AC_DEFINE([SH], 1, [Define for the SH architecture.])
  ;;
  x86?64*)
  arch=x86_64
AC_DEFINE([X86_64], 1, [Define for the AMD x86-64 architecture.])
  ;;
  cris|crisv10)
  arch=crisv10
AC_DEFINE([CRISV10], 1, [Define for the CRISv10 architecture.])
  ;;
  crisv32)
  arch=crisv32
AC_DEFINE([CRISV32], 1, [Define for the CRISv32 architecture.])
  ;;
  tile*)
  arch=tile
AC_DEFINE([TILE], 1, [Define for the Tile architecture])
  ;;
  *)
  AC_MSG_RESULT([NO!])
AC_MSG_ERROR([architecture $host_cpu is not supported by strace])
  ;;
  esac
AC_MSG_RESULT($arch)
AC_SUBST(opsys)
AC_SUBST(arch)
AM_CONDITIONAL([LINUX], [test x$opsys = xlinux])
AM_CONDITIONAL([I386], [test x$arch = xi386])
AM_CONDITIONAL([X86_64], [test x$arch = xx86_64])
AM_CONDITIONAL([SUNOS4], [test x$opsys = xsunos4])
AM_CONDITIONAL([SVR4], [test x$opsys = xsvr4])
AM_CONDITIONAL([FREEBSD], [test x$opsys = xfreebsd])

#if test "x$opsys" = "xlinux" 
#then
#bindir = /usr/bin
#if test "x$arch" = "x86_64" 
#then
#libdir = /usr/lib64
#phplibdir = /usr/lib64/php/modules
#else
#libdir = /usr/lib
#phplibdir = /usr/lib/php/modules
#fi
#fi


# Checks for programs.
AC_PROG_CC
gl_EARLY

AC_PROG_CXX
AM_PROG_AS
AC_EXEEXT
AC_PROG_YACC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_LIBTOOL
AC_PROG_YACC
AM_PROG_CC_C_O
AM_PROG_GCJ

AC_SYS_LARGEFILE
AC_ISC_POSIX
AC_C_INLINE

CPPFLAGS="$CPPFLAGS -I/usr/include/php/Zend -I/usr/include/php/TSRM -I/usr/include/php/main -I/usr/include/php"
AC_SUBST(CPPFLAGS)

# Checks for header files.
AC_CHECK_HEADERS(fcntl.h linux/fd.h memory.h net/errno.h \
  sgtty.h string.h \
  sys/param.h sys/device.h sys/gentape.h \
  sys/inet.h sys/io/trioctl.h \
  sys/mtio.h sys/time.h sys/tprintf.h sys/tape.h \
  unistd.h locale.h arpa/inet.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h sys/socket.h sys/resource.h unistd.h, wctype.h wchar.h utime.h signal.h pthread.h wchar.h sys/select.h)
 

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
#AC_CONFIG_FILES([scripts/shared], [chmod +x scripts/shared])
AC_CONFIG_FILES([scripts/shcat], [chmod +x scripts/shcat])
AC_CONFIG_FILES([scripts/shin], [chmod +x scripts/shin])
AC_CONFIG_FILES([scripts/shls], [chmod +x scripts/shls])
AC_CONFIG_FILES([scripts/shpref], [chmod +x scripts/shpref])
AC_CONFIG_FILES([scripts/shrm], [chmod +x scripts/shrm])
AC_CONFIG_FILES([scripts/shmkdir], [chmod +x scripts/shmkdir])
AC_CONFIG_FILES([scripts/shcp], [chmod +x scripts/shcp])
AC_CONFIG_FILES([scripts/shping], [chmod +x scripts/shping])


# options
AC_ARG_ENABLE([php],
  AS_HELP_STRING([--disable-php], [Disable php bootstrap compile.]))
AM_CONDITIONAL(LIBSHARE_PHP, [test "x$enable_php" != "xno"])
AS_IF([test "x$enable_php" != "xno"], [AC_CHECK_HEADER([zend.h],
    AC_MSG_RESULT([PHP bootstrap compile enabled]),
    AC_MSG_FAILURE([Install php development package or configure --disable-php]))])

AC_ARG_ENABLE([java],
  AS_HELP_STRING([--disable-java], [Disable java bootstrap compile.]))
AM_CONDITIONAL(LIBSHARE_JAVA, [test "x$enable_java" != "xno"])

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: yes]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac], [debug=true])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AC_SUBST(OPENSSL_DIR, "openssl")
AC_ARG_WITH([openssl],
  [AS_HELP_STRING([--with-openssl], [location of openssl library])],
  [OPENSSL_DIR=$withval])
AM_CONDITIONAL(OPENSSL_DIR, [test "x$with_openssl" != "xno"])




AC_CHECK_PROGS([CP], [cp])

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

# swig
AC_CHECK_PROG(SWIG_PROG, [swig], [swig], [true])
AM_CONDITIONAL(HAVE_SWIG, [test x"$SWIG_PROG" = x"swig"])

# help2man
AC_PATH_PROG(HELP2MAN, help2man, @echo WARNING: skipped: help2man )


AC_OUTPUT(Makefile depend/Makefile src/gnu/Makefile src/share-lib/Makefile src/java-share-lib/Makefile src/php-share-lib/Makefile src/share-util/Makefile src/share-daemon/Makefile src/share-log/Makefile src/share-coin/Makefile scripts/Makefile doc/Makefile examples/Makefile include/Makefile bin/Makefile)

