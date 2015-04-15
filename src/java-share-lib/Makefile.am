AUTOMAKE_OPTIONS = foreign
JAVAC_OPT=-d $(top_builddir)/src/java-share-lib
AM_GCJFLAGS = -Wno-all -fjni

SWIG_JAVA_OPT = -java -package net.sharelib
SWIG_CONF = libshare_java.i

INCLUDES = -I$(top_srcdir)/src/share-lib
INCLUDES += -I$(top_srcdir)/src/share-lib/fs
INCLUDES += -I$(top_srcdir)/src/share-lib/net
INCLUDES += -I$(top_srcdir)/src/share-lib/mem


if LIBSHARE_JAVA
BUILT_SOURCES = libshare.jar
noinst_PROGRAMS = test_libshare_java
lib_LTLIBRARIES = libshare_java.la
libshare_java_la_LIBADD = $(top_builddir)/src/share-lib/libshare.la
libshare_java_la_SOURCES = $(swig_sources)
libshare_java_la_LDFLAGS = -no-undefined -version-info @version_info@
test_libshare_java_SOURCES = libshare.jar test_libshare_java.java
test_libshare_java_LDFLAGS = --main=test_libshare_java -fjni
TESTS = test_libshare_java
if HAVE_JAR
libshare_java_DATA = libshare.jar 
libshare_javadir = $(libdir)
endif
endif



javasources = \
	share_java.java \
	share_javaJNI.java \
	SWIGTYPE_p_fd_set.java \
	SWIGTYPE_p_hostent.java \
	SWIGTYPE_p_long.java \
	SWIGTYPE_p_p_char.java \
	SWIGTYPE_p_p_shfs_t.java \
	SWIGTYPE_p_p_shjson_t.java \
	SWIGTYPE_p_p_shkey_t.java \
	SWIGTYPE_p_p_shlock_t.java \
	SWIGTYPE_p_p_shmeta_t.java \
	SWIGTYPE_p_p_shpeer_t.java \
	SWIGTYPE_p_p_shpool_t.java \
	SWIGTYPE_p_p_unsigned_char.java \
	SWIGTYPE_p_shbuf_t.java \
	SWIGTYPE_p_shfs_ino_t.java \
	SWIGTYPE_p_shfs_t.java \
	SWIGTYPE_p_shjson_t.java \
	SWIGTYPE_p_shkey_t.java \
	SWIGTYPE_p_shlock_t.java \
	SWIGTYPE_p_shmeta_t.java \
	SWIGTYPE_p_shpeer_t.java \
	SWIGTYPE_p_shpool_t.java \
	SWIGTYPE_p_shsig_t.java \
	SWIGTYPE_p_shsize_t.java \
	SWIGTYPE_p_shtime_t.java \
	SWIGTYPE_p_size_t.java \
	SWIGTYPE_p_sockaddr.java \
	SWIGTYPE_p_socklen_t.java \
	SWIGTYPE_p_ssize_t.java \
	SWIGTYPE_p_time_t.java \
	SWIGTYPE_p_timeval.java \
	SWIGTYPE_p_unsigned_char.java \
	SWIGTYPE_p_unsigned_int.java \
	SWIGTYPE_p_void.java

javaclasses = \
	net/sharelib/share_java.class \
	net/sharelib/share_javaJNI.class \
	net/sharelib/SWIGTYPE_p_fd_set.class \
	net/sharelib/SWIGTYPE_p_hostent.class \
	net/sharelib/SWIGTYPE_p_long.class \
	net/sharelib/SWIGTYPE_p_p_char.class \
	net/sharelib/SWIGTYPE_p_p_shfs_t.class \
	net/sharelib/SWIGTYPE_p_p_shjson_t.class \
	net/sharelib/SWIGTYPE_p_p_shkey_t.class \
	net/sharelib/SWIGTYPE_p_p_shlock_t.class \
	net/sharelib/SWIGTYPE_p_p_shmeta_t.class \
	net/sharelib/SWIGTYPE_p_p_shpeer_t.class \
	net/sharelib/SWIGTYPE_p_p_shpool_t.class \
	net/sharelib/SWIGTYPE_p_p_unsigned_char.class \
	net/sharelib/SWIGTYPE_p_shbuf_t.class \
	net/sharelib/SWIGTYPE_p_shfs_ino_t.class \
	net/sharelib/SWIGTYPE_p_shfs_t.class \
	net/sharelib/SWIGTYPE_p_shjson_t.class \
	net/sharelib/SWIGTYPE_p_shkey_t.class \
	net/sharelib/SWIGTYPE_p_shlock_t.class \
	net/sharelib/SWIGTYPE_p_shmeta_t.class \
	net/sharelib/SWIGTYPE_p_shpeer_t.class \
	net/sharelib/SWIGTYPE_p_shpool_t.class \
	net/sharelib/SWIGTYPE_p_shsig_t.class \
	net/sharelib/SWIGTYPE_p_shsize_t.class \
	net/sharelib/SWIGTYPE_p_shtime_t.class \
	net/sharelib/SWIGTYPE_p_size_t.class \
	net/sharelib/SWIGTYPE_p_sockaddr.class \
	net/sharelib/SWIGTYPE_p_socklen_t.class \
	net/sharelib/SWIGTYPE_p_ssize_t.class \
	net/sharelib/SWIGTYPE_p_time_t.class \
	net/sharelib/SWIGTYPE_p_timeval.class \
	net/sharelib/SWIGTYPE_p_unsigned_char.class \
	net/sharelib/SWIGTYPE_p_unsigned_int.class \
	net/sharelib/SWIGTYPE_p_void.class

common_includes = \
  $(top_srcdir)/src/share-lib/share.h \
  $(top_srcdir)/src/share-lib/sherr.h \
  $(top_srcdir)/src/share-lib/share_base.h \
  $(top_srcdir)/src/share-lib/shpeer.h \
  $(top_srcdir)/src/share-lib/shtime.h \
  $(top_srcdir)/src/share-lib/shcrc.h \
  $(top_srcdir)/src/share-lib/mem/shmem.h \
  $(top_srcdir)/src/share-lib/fs/shfs.h \
  $(top_srcdir)/src/share-lib/shpref.h \
  $(top_srcdir)/src/share-lib/net/shnet.h

swig_sources = \
  libshare_java_wrap.c


if HAVE_SWIG
.src-stamp: libshare_java.i
	$(SWIG_PROG) $(SWIG_JAVA_OPT) $(SWIG_CONF)
	$(JAVAC) $(JAVAC_OPT) $(javasources) test_libshare_java.java
	echo > .src-stamp
else
.src-stamp: libshare_java.i
	$(CP) $(top_srcdir)/src/java-share-lib/prebuild/* $(builddir)
	$(JAVAC) $(JAVAC_OPT) $(javasources) test_libshare_java.java
	echo > .src-stamp
endif

libshare.jar: .src-stamp
	$(JAR) cf libshare.jar $(javaclasses)
	$(LN_S) -f .libs/libshare_java.so


#all-local: java-wrap

clean-local:
	rm -f .src-stamp

