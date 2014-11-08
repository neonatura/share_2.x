AUTOMAKE_OPTIONS = foreign
AM_GCJFLAGS = -Wno-all -Wno-unchecked -Wno-raw -v -fjni
#CFLAGS =
#LDFLAGS =
JAVAC_OPT=-d $(top_builddir)/src/java-share-lib

noinst_PROGRAMS = test_libshare test_libshare_static test_libshare_java
BUILT_SOURCES = libshare.jar

common_INCLUDES = \
  $(top_builddir)/src/share-lib/share.h \
  $(top_srcdir)/src/share-lib/shtime.h \
  $(top_srcdir)/src/share-lib/shkey.h \
  $(top_srcdir)/src/share-lib/shpref.h \
  $(top_srcdir)/src/share-lib/shbuf.h \
  $(top_srcdir)/src/share-lib/fs/shfs_adler.h \
  $(top_srcdir)/src/share-lib/fs/shfs.h \
  $(top_srcdir)/src/share-lib/fs/shfs_read.h \
  $(top_srcdir)/src/share-lib/fs/shfs_meta.h \
  $(top_srcdir)/src/share-lib/fs/shfs_time64.h \
  $(top_srcdir)/src/share-lib/fs/shfs_write.h \
  $(top_srcdir)/src/share-lib/fs/shfs_proc.h \
  $(top_srcdir)/src/share-lib/delta/xdelta3.h \
  $(top_srcdir)/src/share-lib/socket/write.h \
  $(top_srcdir)/src/share-lib/socket/read.h \
  $(top_srcdir)/src/share-lib/socket/bind.h \
  $(top_srcdir)/src/share-lib/socket/connect.h \
  $(top_srcdir)/src/share-lib/socket/shfcntl.h \
  $(top_srcdir)/src/share-lib/socket/gethost.h \
  $(top_srcdir)/src/share-lib/socket/close.h \
  $(top_srcdir)/src/share-lib/socket/socket.h \
  $(top_srcdir)/src/share-lib/socket/sockbuff.h \
  $(top_srcdir)/src/share-lib/socket/accept.h \
  $(top_srcdir)/src/share-lib/meta/shmeta.h \
  $(top_srcdir)/src/share-lib/meta/shmeta_value.h

# test_libshare
test_libshare_SOURCES = $(common_INCLUDES) test_libshare.c
test_libshare_LDADD = $(top_builddir)/src/share-lib/libshare.la

# test_libshare_static
test_libshare_static_SOURCES = $(common_INCLUDES) test_libshare.c
test_libshare_static_LDFLAGS = -static
test_libshare_static_LDADD = $(top_builddir)/src/share-lib/libshare.a

# test_libshare_java
libshare.jar: $(top_builddir)/src/java-share-lib/libshare.jar
	$(CP) $(top_builddir)/src/java-share-lib/libshare.jar .
	$(JAR) xf libshare.jar

test_libshare_java_SOURCES = libshare.jar test_libshare_java.java
test_libshare_java_LDFLAGS = --main=test_libshare_java -fjni

