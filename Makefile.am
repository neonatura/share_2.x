

ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = foreign
SUBDIRS = src/share-lib src/share-util scripts doc 

#lib_PROGRAMS = share
#bin_PROGRAMS = shnet
#noinst_PROGRAMS = shareregtest sharedecode
#
#common_SOURCES = \
#	share-lib/share.h \
#	share-lib/key/shkey.h \
#	share-lib/meta/shmeta.h \
#	share-lib/meta/shmeta_value.h \
#	share-lib/buffer/shbuf.h \
#	share-lib/shfs/shfs_meta.h \
#	share-lib/shfs/shfs_read.h \
#	share-lib/shfs/shfs_write.h \
#	share-lib/shfs/shfs_adler.h \
#	share-lib/shfs/shfs_time64.h \
#	share-lib/socket/sockbuff.h \
#	share-lib/socket/socket.h \
#	share-lib/socket/connect.h \
#	share-lib/socket/shfcntl.h \
#	share-lib/socket/gethost.h
#
#share_SOURCES = $(common_SOURCES) \
#	share-lib/share.c \
#	share-lib/key/shkey.c \
#	share-lib/fs/shfs_read.c \
#	share-lib/fs/shfs.c \
#	share-lib/fs/shfs_time64.c \
#	share-lib/fs/shfs_meta.c \
#	share-lib/fs/shfs_write.c \
#	share-lib/fs/shfs_proc.c \
#	share-lib/fs/shfs_adler.c \
#	share-lib/delta/share.c \
#	share-lib/socket/shfcntl.c \
#	share-lib/socket/connect.c \
#	share-lib/socket/accept.c \
#	share-lib/socket/sockbuff.c \
#	share-lib/socket/gethost.c \
#	share-lib/socket/write.c \
#	share-lib/socket/read.c \
#	share-lib/socket/socket.c \
#	share-lib/socket/close.c \
#	share-lib/socket/bind.c \
#	share-lib/buffer/shbuf.c \
#	share-lib/svn/shsvn_hash.c.orig \
#	share-lib/svn/shsvn.c.orig \
#	share-lib/meta/shmeta.c \
#	share-lib/meta/shmeta_value.c
#
#shnet_SOURCES = $(common_SOURCES) \
#	share-util/shnet_server.h \
#	share-util/shnet_file.h \
#	share-util/shnet.h \
#	share-util/shnet_test.h \
#	share-util/shnet_ping.h \
#	share-util/shnet_scan.h \
#	share-util/shnet_test.c \
#	share-util/shnet_server.c \
#	share-util/shnet_ping.c \
#	share-util/shnet_scan.c \
#	share-util/shnet_file.c \
#	share-util/shnet.c
#
#shareregtest_SOURCES = $(common_SOURCES) \
#	testing/cmp.h \
#	testing/delta.h \
#	testing/file.h \
#	testing/modify.h \
#	testing/random.h \
#	testing/regtest.cc \
#	testing/regtest_c.c \
#	testing/segment.h \
#	testing/sizes.h \
#	testing/test.h
#
## Note: for extra sanity checks, enable -Wconversion. Note there
## are a lot of false positives.
#WFLAGS = -Winline -Wnested-externs -Wredundant-decls \
#				 -Wwrite-strings -Wcast-align -Wcast-qual -Wshadow
#
#C_WFLAGS = $(WFLAGS) -pedantic -std=c99
#CXX_WFLAGS = $(WFLAGS)
#
#common_CFLAGS = \
#	      -DGENERIC_ENCODE_TABLES=0 \
#	      -DREGRESSION_TEST=1 \
#	      -DSECONDARY_DJW=1 \
#	      -DSECONDARY_FGK=1 \
#	      -DXD3_POSIX=1 \
#	      -DXD3_USE_LARGEFILE64=1 \
#	      -DXD3_MAIN=1
#
#if DEBUG_SYMBOLS
#  common_CFLAGS += -g
#endif
#
## For additional debugging, add -DXD3_DEBUG=1, 2, 3, ...
#share_CFLAGS = $(C_WFLAGS) $(common_CFLAGS) -DXD3_DEBUG=0
#share_LDADD = -lm
#
#sharedecode_CFLAGS = \
#	$(C_WFLAGS) \
#	-DGENERIC_ENCODE_TABLES=0 \
#	-DREGRESSION_TEST=0 \
#	-DSECONDARY_DJW=0 \
#	-DSECONDARY_FGK=0 \
#	-DSECONDARY_LZMA=0 \
#	-DXD3_USE_LARGEFILE64=1 \
#	-DXD3_MAIN=1 \
#	-DXD3_ENCODER=0 \
#	-DXD3_STDIO=1 \
#	-DEXTERNAL_COMPRESSION=0 \
#	-DVCDIFF_TOOLS=0
#
#shareregtest_CXXFLAGS = \
#	$(CXX_WFLAGS) $(common_CFLAGS) -DNOT_MAIN=1 -DXD3_DEBUG=1
#shareregtest_CFLAGS = \
#	$(C_WFLAGS) $(common_CFLAGS) -DNOT_MAIN=1 -DXD3_DEBUG=1
#shareregtest_LDADD = -lm
#
#man1_MANS = share.1
#
#EXTRA_DIST = 
##	draft-korn-vcdiff.txt \
##	README \
##	examples/Makefile \
##	examples/README \
##	examples/checksum_test.cc \
##	examples/compare_test.c \
##	examples/encode_decode_test.c \
##	examples/small_page_test.c \
##	examples/speed_test.c \
##	examples/test.h \
##	examples/iOS/share-ios-test/share-ios-test.xcodeproj/project.pbxproj \
##	examples/iOS/share-ios-test/share-ios-test/Xd3iOSAppDelegate.h \
##	examples/iOS/share-ios-test/share-ios-test/Xd3iOSAppDelegate.m \
##	examples/iOS/share-ios-test/share-ios-test/Xd3iOSViewController.h \
##	examples/iOS/share-ios-test/share-ios-test/Xd3iOSViewController.m \
##	examples/iOS/share-ios-test/share-ios-test/en.lproj/InfoPlist.strings \
##	examples/iOS/share-ios-test/share-ios-test/en.lproj/MainStoryboard_iPad.storyboard \
##	examples/iOS/share-ios-test/share-ios-test/en.lproj/MainStoryboard_iPhone.storyboard \
##	examples/iOS/share-ios-test/share-ios-test/file_v1.bin \
##	examples/iOS/share-ios-test/share-ios-test/file_v1_to_v2.bin \
##	examples/iOS/share-ios-test/share-ios-test/file_v2.bin \
##	examples/iOS/share-ios-test/share-ios-test/main.m \
##	examples/iOS/share-ios-test/share-ios-test/share-ios-test-Info.plist \
##	examples/iOS/share-ios-test/share-ios-test/share-ios-test-Prefix.pch \
##	Makefile.mingw \
##	Makefile.orig \
##	testing/share-regtest.py \
##	testing/share-test.py \
##	share.1 \
##	share.i \
##	share.vcxproj \
##	share.wxi \
##	share.wxs
#
#
