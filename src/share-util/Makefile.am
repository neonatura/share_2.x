
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
	sharetool_server.h \
	sharetool_test.h \
	sharetool_ping.h \
	sharetool_pref.h \
	sharetool_scan.h \
	file/file_cat.h \
	file/file_import.h \
	file/file_list.h \
	file/file_mkdir.h \
	file/file_unlink.h


common_SOURCES = \
	sharetool.c \
	sharetool_test.c \
	sharetool_server.c \
	sharetool_ping.c \
	sharetool_pref.c \
	sharetool_scan.c \
	file/file_cat.c \
	file/file_import.c \
	file/file_list.c \
	file/file_mkdir.c \
	file/file_unlink.c

bin_PROGRAMS = shping shpref shdiff \
	shls shcat shcp shmkdir shrm shcp shin shout

noinst_PROGRAMS = shutil_static
#bin_SCRIPTS = shdiff shpref shfpref 

shping_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shping_LDADD = $(top_builddir)/src/share-lib/libshare.la

shpref_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shpref_LDADD = $(top_builddir)/src/share-lib/libshare.la

shdiff_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shdiff_LDADD = $(top_builddir)/src/share-lib/libshare.la

shls_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shls_LDADD = $(top_builddir)/src/share-lib/libshare.la

shutil_static_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shutil_static_LDADD = $(top_builddir)/src/share-lib/libshare.a

shrm_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shrm_LDADD = $(top_builddir)/src/share-lib/libshare.la

shcat_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shcat_LDADD = $(top_builddir)/src/share-lib/libshare.la

shcp_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shcp_LDADD = $(top_builddir)/src/share-lib/libshare.la

shmkdir_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shmkdir_LDADD = $(top_builddir)/src/share-lib/libshare.la

shin_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shin_LDADD = $(top_builddir)/src/share-lib/libshare.la

shout_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shout_LDADD = $(top_builddir)/src/share-lib/libshare.la


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



