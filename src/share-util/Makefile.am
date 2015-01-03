
AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS =

common_INCLUDES = \
	$(top_builddir)/config.h \
	$(top_srcdir)/src/share-lib/share.h \
	$(top_srcdir)/src/share-lib/mem/shmem.h \
	$(top_srcdir)/src/share-lib/sherr.h \
	$(top_srcdir)/src/share-lib/share_base.h \
	$(top_srcdir)/src/share-lib/shpeer.h \
	$(top_srcdir)/src/share-lib/shtime.h \
	$(top_srcdir)/src/share-lib/shpref.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/shfile.h \
	$(top_srcdir)/src/share-lib/fs/shfs.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3.h \
	$(top_srcdir)/src/share-lib/net/shnet.h \
	sharetool.h \
	sharetool_pref.h \
	sharetool_file.h \
	sharetool_rev.h \
	file/file_cat.h \
	file/file_import.h \
	file/file_list.h \
	file/file_mkdir.h \
	file/file_unlink.h \
	file/file_attr.h \
	file/file_delta.h \
	file/file_rev.h


common_SOURCES = \
	sharetool.c \
	sharetool_pref.c \
	sharetool_file.c \
	sharetool_rev.c \
	file/file_cat.c \
	file/file_list.c \
	file/file_mkdir.c \
	file/file_unlink.c \
	file/file_attr.c \
	file/file_delta.c \
	file/file_rev.c

bin_PROGRAMS = shpref shdiff shpatch \
	shls shcat shcp shln shmkdir shrm shattr shrev

noinst_PROGRAMS = shutil_static \
	static/shpref \
	static/shdiff \
	static/shpatch \
	static/shls \
	static/shattr \
	static/shcat \
	static/shcp \
	static/shln \
	static/shmkdir \
	static/shrm \
	static/shrev

shpref_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shpref_LDADD = $(top_builddir)/src/share-lib/libshare.la

shdiff_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shdiff_LDADD = $(top_builddir)/src/share-lib/libshare.la

shpatch_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shpatch_LDADD = $(top_builddir)/src/share-lib/libshare.la

shls_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shls_LDADD = $(top_builddir)/src/share-lib/libshare.la

shutil_static_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shutil_static_LDADD = $(top_builddir)/src/share-lib/libshare.a

shrm_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shrm_LDADD = $(top_builddir)/src/share-lib/libshare.la

shrev_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shrev_LDADD = $(top_builddir)/src/share-lib/libshare.la

shcat_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shcat_LDADD = $(top_builddir)/src/share-lib/libshare.la

shcp_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shcp_LDADD = $(top_builddir)/src/share-lib/libshare.la

shln_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shln_LDADD = $(top_builddir)/src/share-lib/libshare.la

shmkdir_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shmkdir_LDADD = $(top_builddir)/src/share-lib/libshare.la

shattr_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shattr_LDADD = $(top_builddir)/src/share-lib/libshare.la


if DEBUG
AM_CFLAGS = -g3 -O0 \
	-I$(top_srcdir)/src/share-lib \
	-DSHARELIB \
	-I$(top_srcdir)/src/share-lib/net \
	-I$(top_srcdir)/src/share-lib/fs \
	-I$(top_srcdir)/src/share-lib/mem
AM_CXXFLAGS = -g3 -O0
else
AM_CFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops \
	-I$(top_srcdir)/src/share-lib \
	-DSHARELIB \
	-I$(top_srcdir)/src/share-lib/net \
	-I$(top_srcdir)/src/share-lib/fs \
	-I$(top_srcdir)/src/share-lib/mem
AM_CXXFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops
endif

static/shpref: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shpref

static/shdiff: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shdiff

static/shpatch: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shpatch

static/shls: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shls

static/shattr: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shattr

static/shcat: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shcat

static/shcp: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shcp

static/shln: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shln

static/shmkdir: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shmkdir

static/shrm: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shrm

static/shrev: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shrev


