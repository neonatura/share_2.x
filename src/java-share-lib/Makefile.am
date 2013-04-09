AUTOMAKE_OPTIONS = foreign
MAINTAINERCLEANFILES = all-local $(javasources) Makefile.in
TESTNAME = test
JAVA = java
JAVAC_OPT=-d $(top_builddir)/src/java-share-lib

if HAVE_SWIG
SWIG_PROG=swig
else
SWIG_PROG=false
endif

javasources = \
	jshare.java \
	jshareJNI.java \
	SWIGTYPE_p_p_char.java \
	SWIGTYPE_p_p_shbuf_t.java \
	SWIGTYPE_p_p_shfs_journal_t.java \
	SWIGTYPE_p_p_shfs_t.java \
	SWIGTYPE_p_p_shkey_t.java \
	SWIGTYPE_p_p_shmeta_t.java \
	SWIGTYPE_p_shbuf_t.java \
	SWIGTYPE_p_shfs_hdr_t.java \
	SWIGTYPE_p_shfs_ino_t.java \
	SWIGTYPE_p_shfs_journal_t.java \
	SWIGTYPE_p_shfs_t.java \
	SWIGTYPE_p_shkey_t.java \
	SWIGTYPE_p_shmeta_t.java \
	SWIGTYPE_p_shtime_t.java \
	SWIGTYPE_p_size_t.java \
	SWIGTYPE_p_ssize_t.java \
	SWIGTYPE_p_sockaddr.java \
	SWIGTYPE_p_socklen_t.java \
	SWIGTYPE_p_hostent.java \
	SWIGTYPE_p_void.java



javaclasses = \
	libshare/jshare.class \
	libshare/jshareJNI.class \
	libshare/SWIGTYPE_p_p_char.class \
	libshare/SWIGTYPE_p_p_shbuf_t.class \
	libshare/SWIGTYPE_p_p_shfs_journal_t.class \
	libshare/SWIGTYPE_p_p_shfs_t.class \
	libshare/SWIGTYPE_p_p_shkey_t.class \
	libshare/SWIGTYPE_p_p_shmeta_t.class \
	libshare/SWIGTYPE_p_shbuf_t.class \
	libshare/SWIGTYPE_p_shfs_hdr_t.class \
	libshare/SWIGTYPE_p_shfs_ino_t.class \
	libshare/SWIGTYPE_p_shfs_journal_t.class \
	libshare/SWIGTYPE_p_shfs_t.class \
	libshare/SWIGTYPE_p_shkey_t.class \
	libshare/SWIGTYPE_p_shmeta_t.class \
	libshare/SWIGTYPE_p_shtime_t.class \
	libshare/SWIGTYPE_p_size_t.class \
	libshare/SWIGTYPE_p_ssize_t.class \
	libshare/SWIGTYPE_p_sockaddr.class \
	libshare/SWIGTYPE_p_socklen_t.class \
	libshare/SWIGTYPE_p_hostent.class \
	libshare/SWIGTYPE_p_void.class



BUILT_SOURCES = jshare_wrap.c
SWIG_CONF = $(top_srcdir)/src/java-share-lib/jshare.i

SWIG_JAVA_OPT = -java -package libshare -outdir "$(top_builddir)/src/java-share-lib/"


if HAVE_JAR
libshare_java_DATA = libshare.jar 
libshare_javadir = $(libdir)
endif



jshare_wrap.c: swig-stamp
jshare.java: swig-stamp

jshare_wrap.c: swig-stamp

swig-stamp:
	$(SWIG_PROG) $(SWIG_JAVA_OPT) $(SWIG_CONF)
	echo Timestamp > swig-stamp

java-wrap: swig-stamp
	$(JAVAC) $(JAVAC_OPT) $(javasources)

libshare.jar: $(javasources) java-wrap
	jar cf libshare.jar $(javaclasses)

test: swig-stamp
	$(JAVAC) $(JAVAC_OPT) -cp . $(top_srcdir)/src/java-share-lib/test.java

run:
	$(JAVA) -Djava.library.path=$(top_builddir)/src/share-lib/.libs $(TESTNAME)

all-local: swig-stamp java-wrap $(TESTNAME)

clean-local:
	rm -f swig-stamp $(TESTNAME).class $(javaclasses) 


