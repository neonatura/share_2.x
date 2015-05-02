AUTOMAKE_OPTIONS = foreign
JAVAC_OPT=-d $(top_builddir)/src/java-share-lib
AM_GCJFLAGS = -Wno-all -fjni

INCLUDES = -I$(top_srcdir)/src/share-lib \
	-I$(top_srcdir)/src/share-lib/fs \
	-I$(top_srcdir)/src/share-lib/net \
	-I$(top_srcdir)/src/share-lib/mem \
	-I$(top_srcdir)/src/share-lib/sys

if LIBSHARE_JAVA
BUILT_SOURCES = libshare.jar
libshare_java_DATA = libshare.jar 
libshare_javadir = $(libdir)

noinst_PROGRAMS = test_libshare_java
test_libshare_java_SOURCES = libshare.jar test_libshare_java.java
test_libshare_java_LDFLAGS = --main=test_libshare_java -fjni
TESTS = test_libshare_java
endif

javasources = \
	$(top_srcdir)/src/share-lib/java/share_java.java \
	$(top_srcdir)/src/share-lib/java/share_javaJNI.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_fd_set.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_hostent.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_long.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_char.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shfs_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shjson_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shkey_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shlock_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shmeta_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shpeer_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_shpool_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_p_unsigned_char.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shbuf_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shfs_ino_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shfs_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shjson_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shkey_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shlock_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shmeta_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shpeer_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shpool_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shsig_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_shsize_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_size_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_sockaddr.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_socklen_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_ssize_t.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_timeval.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_unsigned_char.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_unsigned_int.java \
	$(top_srcdir)/src/share-lib/java/SWIGTYPE_p_void.java

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
	net/sharelib/SWIGTYPE_p_size_t.class \
	net/sharelib/SWIGTYPE_p_sockaddr.class \
	net/sharelib/SWIGTYPE_p_socklen_t.class \
	net/sharelib/SWIGTYPE_p_ssize_t.class \
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

api_sources = \
	$(srcdir)/net/sharelib/Share.java \
	$(srcdir)/net/sharelib/SHTime.java \
	$(srcdir)/net/sharelib/SHCRC64.java \
	$(srcdir)/net/sharelib/mem/SHBuffer.java \
	$(srcdir)/net/sharelib/mem/SHBufferInputStream.java \
	$(srcdir)/net/sharelib/mem/SHBufferOutputStream.java

api_classes = \
	net/sharelib/Share.class \
	net/sharelib/SHTime.class \
	net/sharelib/SHCRC64.class \
	net/sharelib/mem/SHBuffer.class \
	net/sharelib/mem/SHBufferInputStream.class \
	net/sharelib/mem/SHBufferOutputStream.class

libshare.so:
	$(LN_S) -f $(top_builddir)/src/share-lib/.libs/libshare.so

libshare.jar: libshare.so
	$(JAVAC) $(JAVAC_OPT) $(javasources) $(api_sources) test_libshare_java.java
	$(JAR) cf libshare.jar $(javaclasses) $(api_classes)

clean-local:
	rm -f net/sharelib/*.class net/sharelib/*/*.class libshare.jar libshare_java.so *.class

