AUTOMAKE_OPTIONS = foreign
MAINTAINERCLEANFILES = all-local $(javasources) Makefile.in
JAVA = java
JAVAC_OPT=-d $(top_builddir)/src/java-share-lib

javasources = \
	$(top_builddir)/wrap/java/jshare.java \
	$(top_builddir)/wrap/java/jshareJNI.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_fd_set.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_hostent.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_long.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_p_char.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_p_shbuf_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_p_shkey_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_p_shlock_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_p_shmeta_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_p_shpool_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_p_uint8_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_shbuf_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_shkey_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_shlock_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_shmeta_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_shpool_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_shtime_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_size_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_sockaddr.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_socklen_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_ssize_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_timeval.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_uint32_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_uint8_t.java \
	$(top_builddir)/wrap/java/SWIGTYPE_p_void.java

javaclasses = \
	libshare/jshare.class \
	libshare/jshareJNI.class \
	libshare/SWIGTYPE_p_fd_set.class \
	libshare/SWIGTYPE_p_hostent.class \
	libshare/SWIGTYPE_p_long.class \
	libshare/SWIGTYPE_p_p_char.class \
	libshare/SWIGTYPE_p_p_shbuf_t.class \
	libshare/SWIGTYPE_p_p_shkey_t.class \
	libshare/SWIGTYPE_p_p_shlock_t.class \
	libshare/SWIGTYPE_p_p_shmeta_t.class \
	libshare/SWIGTYPE_p_p_shpool_t.class \
	libshare/SWIGTYPE_p_p_uint8_t.class \
	libshare/SWIGTYPE_p_shbuf_t.class \
	libshare/SWIGTYPE_p_shkey_t.class \
	libshare/SWIGTYPE_p_shlock_t.class \
	libshare/SWIGTYPE_p_shmeta_t.class \
	libshare/SWIGTYPE_p_shpool_t.class \
	libshare/SWIGTYPE_p_shtime_t.class \
	libshare/SWIGTYPE_p_size_t.class \
	libshare/SWIGTYPE_p_sockaddr.class \
	libshare/SWIGTYPE_p_socklen_t.class \
	libshare/SWIGTYPE_p_ssize_t.class \
	libshare/SWIGTYPE_p_timeval.class \
	libshare/SWIGTYPE_p_uint32_t.class \
	libshare/SWIGTYPE_p_uint8_t.class \
	libshare/SWIGTYPE_p_void.class


if HAVE_JAR
libshare_java_DATA = libshare.jar 
libshare_javadir = $(libdir)
endif


java-wrap: $(javasources)
	$(JAVAC) $(JAVAC_OPT) $(javasources)

libshare.jar: java-wrap
	jar cf libshare.jar $(javaclasses)

test_libshare_java.class: $(top_srcdir)/src/java-share-lib/test_libshare_java.java
	$(JAVAC) $(JAVAC_OPT) -cp . $(top_srcdir)/src/java-share-lib/test_libshare_java.java

run: test_libshare_java.class
	$(JAVA) -Djava.library.path=$(top_builddir)/src/share-lib/.libs test_libshare_java

all-local: java-wrap test_libshare_java.class

clean-local:
	rm -f test_libshare_java.class $(javaclasses) 


