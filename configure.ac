#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT(libshare, 2.26, support@neo-natura.com)
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE(libshare, 2.26) 
AC_CONFIG_SRCDIR([src/share-lib])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_HEADERS([src/share-lib/test_libshare.c])
AC_CONFIG_HEADERS([src/php-share-lib/libshare_php.i])
#AC_CONFIG_HEADERS([po/Makefile.in])
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST([with_kernel], [`uname -r`])
AC_SUBST([with_kerneldir], [/lib/modules/$with_kernel])
AC_PATH_KERNEL_SOURCE
AC_SUBST([KERNEL_SRC], [$with_kerneldir/build])
AC_SUBST([KERNEL_MOD], [$with_kerneldir/extra])
AM_CONDITIONAL([KERNEL_2_6_32], [test `uname -r|cut -d- -f1` = "2.6.32"])
AM_CONDITIONAL([KMOD_DIR], [test -f /lib/modules/`uname -r`/build])

LT_INIT

version_patch=26
AC_SUBST(version_patch)
version_info=2:26:0
AC_SUBST(version_info)

#VERSION_CUR=`$SED -ne 's/^#define LIBOAUTH_CUR *\([0-9]*\)/\1/p' ${srcdir}/src/oauth.h 2>/dev/null`
#VERSION_REV=`$SED -ne 's/^#define LIBOAUTH_REV *\([0-9]*\)/\1/p' ${srcdir}/src/oauth.h 2>/dev/null`
#VERSION_AGE=`$SED -ne 's/^#define LIBOAUTH_AGE *\([0-9]*\)/\1/p' ${srcdir}/src/oauth.h 2>/dev/null`
#VERSION_INFO=${VERSION_CUR}:${VERSION_REV}:${VERSION_AGE}
#AC_SUBST(VERSION_INFO)

#AC_LIB_LINKFLAGS([pthread])

# docs
AC_CONFIG_FILES([doc/RFC/RFC-ESTP-DRAFT.txt])
AC_CONFIG_FILES([doc/NEWS])

AC_CONFIG_FILES([bin/README.txt], [chmod -wx bin/README.txt])
AC_CONFIG_FILES([examples/README.txt], [chmod -wx examples/README.txt])
AC_CONFIG_FILES([src/java-share-lib/test_libshare_java.java])
AC_CONFIG_FILES([src/php-share-lib/test_libshare_php.php])
AC_CONFIG_FILES([src/sexe/test_sexe.lua])
AC_CONFIG_FILES([src/sexe/test_event.lua])
# php
AC_CONFIG_FILES([src/php-share-lib/class_share.php])
AC_CONFIG_FILES([src/php-share-lib/class_share_fs.php])
AC_CONFIG_FILES([src/php-share-lib/class_shinode.php])
AC_CONFIG_FILES([src/php-share-lib/class_shfile.php])
AC_CONFIG_FILES([src/php-share-lib/test_libshare_fs.php])
# sh
AC_CONFIG_FILES([src/java-share-lib/test_libshare_java.sh], [chmod a+rx src/java-share-lib/test_libshare_java.sh])
AC_CONFIG_FILES([src/php-share-lib/test_libshare_php.sh], [chmod a+rx src/php-share-lib/test_libshare_php.sh])
AC_CONFIG_FILES([src/php-share-lib/test_libshare_fs.sh], [chmod a+rx src/php-share-lib/test_libshare_fs.sh])
AC_CONFIG_FILES([src/sexe/test_sexe.sh], [chmod a+rx src/sexe/test_sexe.sh])
AC_CONFIG_FILES([src/sexe/test_event.sh], [chmod a+rx src/sexe/test_event.sh])
AC_CONFIG_FILES([release.sh], [chmod a+rwx release.sh])

AC_CONFIG_FILES([module/2.6.32/shnet/Makefile])
AC_CONFIG_FILES([module/2.6.32/shnet/shesp.c])
AC_CONFIG_FILES([module/2.6.32/shnet/shesp_csum.c])
AC_CONFIG_FILES([module/2.6.32/shnet/shesp_dict.c])
AC_CONFIG_FILES([module/2.6.32/shnet/shesp_ipv4.c])
AC_CONFIG_FILES([module/2.6.32/shnet/shesp.h])

AC_CONFIG_FILES([src/sexe/sexe.c])

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

CFLAGS="$CFLAGS -I/lib/jvm/java-*/include"
CPPFLAGS="$CPPFLAGS -I/usr/include/php/Zend -I/usr/include/php/TSRM -I/usr/include/php/main -I/usr/include/php"
AC_SUBST(CPPFLAGS)

# Checks for header files.
AC_CHECK_HEADERS(fcntl.h linux/fd.h memory.h net/errno.h \
  sgtty.h string.h iconv.h fnmatch.h \
  sys/param.h sys/device.h sys/gentape.h \
  sys/inet.h sys/io/trioctl.h sys/ioctl.h \
  sys/mtio.h sys/time.h sys/tprintf.h sys/tape.h \
unistd.h locale.h arpa/inet.h limits.h netdb.h netinet/in.h stddef.h stdint.h stdio.h stdlib.h sys/socket.h sys/resource.h unistd.h, wctype.h wchar.h)
AC_CHECK_HEADERS(math.h)
AC_CHECK_HEADERS(signal.h)
AC_CHECK_HEADERS(pthread.h)
AC_CHECK_HEADERS(sys/resource.h)
AC_CHECK_HEADERS(sys/select.h)
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(wtype.h)
AC_CHECK_HEADERS(wchar.h)
AC_CHECK_HEADERS(wcchar.h)
AC_CHECK_HEADERS(utime.h)
AC_CHECK_HEADERS(pwd.h)
AC_CHECK_HEADERS(paths.h)
 

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
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_CHECK_TYPES([ptrdiff_t stdout fpos_t])
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)

gt_TYPE_SSIZE_T
gl_INIT

AC_CHECK_DECLS([getgrgid],,, [#include <grp.h>])
AC_CHECK_DECLS([getpwuid],,, [#include <pwd.h>])
AC_CHECK_DECLS([time],,, [#include <time.h>])
AC_CHECK_DECLS([isblank],,,[#include <ctype.h>])

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
AC_CHECK_FUNCS([getcwd gethostbyname gettimeofday memmove memset mkdir _mkdir socket strchr strdup strerror strrchr atoi strtol strtoll inet_addr ftell stat fstat getrusage fopen fclose fread fwrite pthread_mutex_lock fchmod fchown fsync lstat mkfifo readlink symlink kill ftruncate memchr regcomp select setenv setlocale strtoumax open close read write socket ioctl fcntl connect bind accept listen pthread_mutex_unlock calloc gettid pthread_self pthread_mutex_init pthread_mutex_destroy strtod killpg sysconf bsearch isalpha getservbyname getpwnam getpwuid getlogin_r getspnam])
# for refclock
AC_CHECK_FUNCS([nice plock pututline pututxline readlink rtprio])
AC_CHECK_FUNCS(htonll ntohll)

AC_CHECK_MEMBERS([struct stat.st_rdev])


# initd scripts #
AC_CONFIG_FILES([scripts/initd/shared], [chmod +x scripts/initd/shared])
AC_CONFIG_FILES([scripts/initd/shlogd], [chmod +x scripts/initd/shlogd])
AC_CONFIG_FILES([scripts/initd/shfsyncd], [chmod +x scripts/initd/shfsyncd])

AC_CHECK_DECL([_PATH_BSHELL],,AC_DEFINE_UNQUOTED([_PATH_BSHELL], "/bin/sh", [Define to system shell path]),[
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
])
AC_CHECK_DECL([_PATH_TTY],,AC_DEFINE_UNQUOTED([_PATH_TTY], "/dev/tty", [Define to tty device node path]),[
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
])
AC_CHECK_DECL([_PATH_DEVNULL],,AC_DEFINE_UNQUOTED([_PATH_DEVNULL], "/dev/null", [Define to devnull device node path]),[
#ifdef HAVE_PATHS_H
#include <paths.h>
#endif
])

AM_CONDITIONAL(HAVE_INITD_DIR, [test -d /etc/rc.d])
INITD_DIR="/etc/rc.d"
AC_SUBST(INITD_DIR)

# options
AC_ARG_ENABLE([shlogd],
  AS_HELP_STRING([--disable-shlogd], [Disable the libshare logging daemon.]))
AM_CONDITIONAL(HAVE_LIBSHARE_LOG_DAEMON, [test "x$enable_shlogd" != "xno"])

#AC_ARG_ENABLE([shcoind],
#  AS_HELP_STRING([--disable-shcoind], [Disable the USDe currency daemon.]))
#AM_CONDITIONAL(HAVE_LIBSHARE_COIN_DAEMON, [test "x$enable_shcoind" != "xno"])

AC_ARG_ENABLE([shfsyncd],
  AS_HELP_STRING([--disable-shfsyncd], [Disable the file-sync daemon.]))
AM_CONDITIONAL(HAVE_LIBSHARE_COIN_DAEMON, [test "x$enable_shfsyncd" != "xno"])

#AC_ARG_ENABLE([shauthd],
#  AS_HELP_STRING([--disable-shauthd], [Disable the libshare OAuth daemon.]))
#AM_CONDITIONAL(HAVE_LIBSHARE_AUTH_DAEMON, [test "x$enable_shauthd" != "xno"])

AC_ARG_ENABLE([php],
  AS_HELP_STRING([--disable-php], [Disable php bootstrap compile.]))
AM_CONDITIONAL(LIBSHARE_PHP, [test "x$enable_php" != "xno"])
AS_IF([test "x$enable_php" != "xno"], [AC_CHECK_HEADER([zend.h],
    AC_MSG_RESULT([PHP bootstrap compile enabled]),
    AC_MSG_FAILURE([Install php development package or configure --disable-php\n (centos) yum install php php-devel]))])
# NOTE: ubuntu: ln -sf /usr/include/php5 /usr/include/php

AC_ARG_ENABLE([java],
  AS_HELP_STRING([--disable-java], [Disable java bootstrap compile.]))
AM_CONDITIONAL(LIBSHARE_JAVA, [test "x$enable_java" != "xno"])
AS_IF([test "x$enable_java" != "xno"], [AC_CHECK_HEADER([jni.h],
    AC_MSG_RESULT([JAVA bootstrap compile enabled]),
    AC_MSG_FAILURE([Install java development package or configure --disable-java\n (ubuntu) apt-get install default-jdk; apt-get install gcj-jdk\n (centos) yum install java-1.8.0-openjdk java-1.8.0-openjdk-devel java-1.5.0-gcj libgcj libgcj-devel]))])

AC_ARG_ENABLE([magtek],
  AS_HELP_STRING([--disable-magtek], [Disable MagTek Mini Swipe Reader Device]))
AM_CONDITIONAL(USE_MAGTEK_DEVICE, [test "x$enable_magtek" != "xno"])
if test "x$enable_magtek" != "xno" ; then
  AC_DEFINE_UNQUOTED([USE_MAGTEK_DEVICE], 1,[Define to enable MagTek Mini Swipe Reader Device])
fi

AC_ARG_ENABLE([ztex],
  AS_HELP_STRING([--disable-ztex], [Disable ZTex FPGA 1.15y Device]))
AM_CONDITIONAL(USE_ZTEX_DEVICE, [test "x$enable_ztex" != "xno"])
if test "x$enable_ztex" != "xno" ; then
  AC_DEFINE_UNQUOTED([USE_ZTEX_DEVICE], 1, [Define to enable ZTex FPGA 1.15y Device])
fi

#AC_ARG_ENABLE(libcurl, AC_HELP_STRING([--disable-libcurl],[do not use libcurl]))

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac], [debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AC_CHECK_PROGS([CP], [cp])

AC_CHECK_PROGS([HTML2TEXT], [html2text])
AM_CONDITIONAL([HAVE_HTML2TEXT], [test -n "$HTML2TEXT"])
AC_SUBST([HTML2TEXT], [test -n "$HTML2TEXT"])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([LIBUNISTRING_COMPILE_UNIWIDTH_WIDTH], [false])
AM_CONDITIONAL([HAVE_DOXYGEN],
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([doc/libshare/doxygen.conf doc/sexe/doxygen.conf doc/share-util/doxygen.conf doc/shared/doxygen.conf doc/java-share-lib/doxygen.conf doc/php-share-lib/doxygen.conf])])

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

# swig - generate java stubs to access base C libshare library.
AC_CHECK_PROG(SWIG_PROG, [swig], [swig], [true])
if test -z "$SWIG_PROG";
   then AC_MSG_WARN([SWIG not found - continuing without SWIG support\n (ubuntu) apt-get install swig\n (centos) yum install swig])
fi
AM_CONDITIONAL(HAVE_SWIG, [test x"$SWIG_PROG" = x"swig"])

# help2man
AC_PATH_PROG(HELP2MAN, help2man, @echo WARNING: skipped: help2man )
AM_CONDITIONAL(HAVE_HELP2MAN, [test x"$HELP2MAN" = x"help2man"])

#AC_CHECK_LIB([curl], [curl_global_init], HAVE_CURL=1)
#AM_CONDITIONAL(HAVE_CURL, test $HAVE_CURL)

# configure/verify share-daemon USB I/O access
AC_ARG_ENABLE([usb],
  AS_HELP_STRING([--disable-usb], [Disable all usb I/O access.]))
AS_IF([test "x$enable_usb" != "xno"], 
  AC_CHECK_LIB([usb-1.0], [libusb_close], HAVE_LIBUSB=1),
    AC_MSG_RESULT([USB bootstrap compile enabled]),
    AC_MSG_FAILURE([Install usb development package or configure --disable-usb\n (centos) yum install usb usb-devel]))
AM_CONDITIONAL(HAVE_LIBUSB, test $HAVE_LIBUSB) 
if test $HAVE_LIBUSB; then
  AC_DEFINE_UNQUOTED([HAVE_LIBUSB], 1, [Define to enable libusb I/O access.])
fi

AC_OUTPUT(Makefile src/gnu/Makefile src/share-lib/java/Makefile src/share-lib/Makefile src/sexe/Makefile src/sexe/shell/Makefile src/java-share-lib/Makefile src/php-share-lib/Makefile src/share-util/Makefile src/share-daemon/Makefile src/share-log/Makefile src/share-fsync-daemon/Makefile doc/libshare/Makefile doc/sexe/Makefile doc/share-util/Makefile doc/shared/Makefile doc/java-share-lib/Makefile doc/php-share-lib/Makefile examples/Makefile include/Makefile module/Makefile bin/Makefile)

