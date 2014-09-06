AUTOMAKE_OPTIONS = foreign
MAINTAINERCLEANFILES = all-local $(java_wrap_sources) Makefile.in

if HAVE_SWIG
SWIG_PROG=swig
BUILT_SOURCES = jshare_wrap.c
else
SWIG_PROG=true
endif

common_includes = \
	$(top_srcdir)/src/share-lib/share.h \
	$(top_srcdir)/src/share-lib/sherr.h \
	$(top_srcdir)/src/share-lib/share_base.h \
	$(top_srcdir)/src/share-lib/shpeer.h \
	$(top_srcdir)/src/share-lib/shtime.h \
	$(top_srcdir)/src/share-lib/mem/shmem.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-internal.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-list.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-main.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3-test.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/shpref.h \
  $(top_srcdir)/src/share-lib/net/shnet.h


wrap_java_sources = \
	jshare.java \
	jshareJNI.java \
	SWIGTYPE_p_p_char.java \
	SWIGTYPE_p_p_shbuf_t.java \
	SWIGTYPE_p_p_shkey_t.java \
	SWIGTYPE_p_p_shmeta_t.java \
	SWIGTYPE_p_shbuf_t.java \
	SWIGTYPE_p_shkey_t.java \
	SWIGTYPE_p_shmeta_t.java \
	SWIGTYPE_p_shtime_t.java \
	SWIGTYPE_p_size_t.java \
	SWIGTYPE_p_ssize_t.java \
	SWIGTYPE_p_sockaddr.java \
	SWIGTYPE_p_socklen_t.java \
	SWIGTYPE_p_hostent.java \
	SWIGTYPE_p_void.java

SWIG_CONF = $(top_srcdir)/wrap/java/jshare.i

SWIG_JAVA_OPT = -java -package libshare -outdir "$(top_builddir)/wrap/java/"

jshare_wrap.c: swig-stamp $(common_includes)
jshare.java: swig-stamp

swig-stamp:
	$(SWIG_PROG) $(SWIG_JAVA_OPT) $(SWIG_CONF)
	echo Timestamp > swig-stamp

all-local: swig-stamp 

clean-local:
	rm -f swig-stamp 


