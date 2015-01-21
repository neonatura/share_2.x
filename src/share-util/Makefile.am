
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
	sharetool_info.h \
	file/file_cat.h \
	file/file_import.h \
	file/file_list.h \
	file/file_copy.h \
	file/file_link.h \
	file/file_unlink.h \
	file/file_attr.h \
	file/file_delta.h \
	file/file_diff.h \
	file/file_rev.h \
	file/file_info.h \
	info/info_table.h


common_SOURCES = \
	sharetool.c \
	sharetool_pref.c \
	sharetool_file.c \
	sharetool_rev.c \
	sharetool_info.c \
	file/file_cat.c \
	file/file_list.c \
	file/file_copy.c \
	file/file_link.c \
	file/file_unlink.c \
	file/file_attr.c \
	file/file_delta.c \
	file/file_diff.c \
	file/file_rev.c \
	file/file_info.c \
	info/info_table.c

bin_PROGRAMS = shpref shinfo \
	shdiff shdelta shpatch \
	shstat shls shcat shcp shln shrm shar unshar shattr shrev

noinst_PROGRAMS = shutil_static \
	static/shpref \
	static/shinfo \
	static/shstat \
	static/shls \
	static/shdiff \
	static/shdelta \
	static/shpatch \
	static/shar \
	static/unshar \
	static/shattr \
	static/shcat \
	static/shcp \
	static/shrm \
	static/shln \
	static/shrev

shpref_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shpref_LDADD = $(top_builddir)/src/share-lib/libshare.la

shdiff_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shdiff_LDADD = $(top_builddir)/src/share-lib/libshare.la

shdelta_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shdelta_LDADD = $(top_builddir)/src/share-lib/libshare.la

shpatch_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shpatch_LDADD = $(top_builddir)/src/share-lib/libshare.la

shstat_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shstat_LDADD = $(top_builddir)/src/share-lib/libshare.la

shinfo_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shinfo_LDADD = $(top_builddir)/src/share-lib/libshare.la

shls_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shls_LDADD = $(top_builddir)/src/share-lib/libshare.la

shutil_static_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shutil_static_LDADD = $(top_builddir)/src/share-lib/libshare.a

shrm_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shrm_LDADD = $(top_builddir)/src/share-lib/libshare.la

shln_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shln_LDADD = $(top_builddir)/src/share-lib/libshare.la

shar_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shar_LDADD = $(top_builddir)/src/share-lib/libshare.la

unshar_SOURCES = $(common_INCLUDES) $(common_SOURCES)
unshar_LDADD = $(top_builddir)/src/share-lib/libshare.la

shrev_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shrev_LDADD = $(top_builddir)/src/share-lib/libshare.la

shcat_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shcat_LDADD = $(top_builddir)/src/share-lib/libshare.la

shcp_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shcp_LDADD = $(top_builddir)/src/share-lib/libshare.la

shattr_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shattr_LDADD = $(top_builddir)/src/share-lib/libshare.la

dist_docdir = $(datadir)/doc/@PACKAGE@

if DEBUG
AM_CFLAGS = -g3 -O0 \
	-I$(top_srcdir)/src/share-lib \
	-DSHARELIB "-DDOC_DIR=\"$(dist_docdir)\""
AM_CXXFLAGS = -g3 -O0
else
AM_CFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops \
	-I$(top_srcdir)/src/share-lib -DSHARELIB
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

static/shdelta: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shdelta

static/shpatch: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shpatch

static/shstat: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shstat

static/shinfo: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shinfo

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

static/shar: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shar

static/unshar: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/unshar

static/shrm: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shrm

static/shrev: shutil_static
	$(MKDIR_P) static
	$(CP) shutil_static static/shrev


