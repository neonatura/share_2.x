
AUTOMAKE_OPTIONS = foreign
CFLAGS = 
LDFLAGS = -lboost_system -lboost_filesystem -lboost_program_options -lboost_thread-mt -ldb_cxx -lz -ldl -lpthread

common_INCLUDES = \
	$(top_builddir)/config.h \
	$(top_srcdir)/src/share-lib/share.h \
	$(top_srcdir)/src/share-lib/mem/shmem_buf.h \
	$(top_srcdir)/src/share-lib/mem/shmem_pool.h \
	$(top_srcdir)/src/share-lib/mem/shmem_meta.h \
	$(top_srcdir)/src/share-lib/mem/shmem_crypt.h \
	$(top_srcdir)/src/share-lib/mem/shmem_key.h \
	$(top_srcdir)/src/share-lib/mem/shmem_lock.h \
	$(top_srcdir)/src/share-lib/sherr.h \
	$(top_srcdir)/src/share-lib/share_macro.h \
	$(top_srcdir)/src/share-lib/shpeer.h \
	$(top_srcdir)/src/share-lib/shtime.h \
	$(top_srcdir)/src/share-lib/shpref.h \
	$(top_srcdir)/src/share-lib/shcrc.h \
	$(top_srcdir)/src/share-lib/shfile.h \
	$(top_srcdir)/src/share-lib/fs/shfs.h \
	$(top_srcdir)/src/share-lib/fs/shfs_partition.h \
	$(top_srcdir)/src/share-lib/fs/shfs_inode.h \
	$(top_srcdir)/src/share-lib/fs/shfs_link.h \
	$(top_srcdir)/src/share-lib/fs/shfs_dir.h \
	$(top_srcdir)/src/share-lib/fs/shfs_file.h \
	$(top_srcdir)/src/share-lib/fs/shfs_cache.h \
	$(top_srcdir)/src/share-lib/fs/shfs_read.h \
	$(top_srcdir)/src/share-lib/fs/shfs_meta.h \
	$(top_srcdir)/src/share-lib/fs/shfs_write.h \
	$(top_srcdir)/src/share-lib/fs/shfs_proc.h \
	$(top_srcdir)/src/share-lib/fs/shfs_rev.h \
	$(top_srcdir)/src/share-lib/fs/shfs_aux.h \
	$(top_srcdir)/src/share-lib/delta/xdelta3.h \
	$(top_srcdir)/src/share-lib/net/shnet.h \
	$(top_srcdir)/src/share-lib/net/shnet_write.h \
	$(top_srcdir)/src/share-lib/net/shnet_read.h \
	$(top_srcdir)/src/share-lib/net/shnet_bind.h \
	$(top_srcdir)/src/share-lib/net/shnet_connect.h \
	$(top_srcdir)/src/share-lib/net/shnet_fcntl.h \
	$(top_srcdir)/src/share-lib/net/shnet_gethost.h \
	$(top_srcdir)/src/share-lib/net/shnet_close.h \
	$(top_srcdir)/src/share-lib/net/shnet_socket.h \
	$(top_srcdir)/src/share-lib/net/shnet_accept.h \
	$(top_srcdir)/src/share-lib/net/shnet_write.h \
	server/addrman.h \
	server/alert.h \
	server/allocators.h \
	server/base58.h \
	server/bignum.h \
	server/bitcoinrpc.h \
	server/checkpoints.h \
	server/compat.h \
	server/crypter.h \
	server/db.h \
	server/init.h \
	server/irc.h \
	server/kernel.h \
	server/key.h \
	server/keystore.h \
	server/main.h \
	server/mruset.h \
	server/netbase.h \
	server/net.h \
	server/pbkdf2.h \
	server/protocol.h \
	server/script.h \
	server/scrypt_mine.h \
	server/serialize.h \
	server/strlcpy.h \
	server/sync.h \
	server/ui_interface.h \
	server/uint256.h \
	server/util.h \
	server/version.h \
	server/walletdb.h \
	server/wallet.h \
	shcgbd_version.h \
	shcgbd_clientversion.h \
	stratum/stratum_message.h \
	stratum/stratum_protocol.h \
	stratum/stratum_user.h \
	stratum/stratum_task.h \
	shcgbd_daemon.h \
	shcgbd_block.h \
	shcgbd.h


common_SOURCES = \
	server/scrypt-x86_64.S \
	server/scrypt-x86.S \
	server/addrman.cpp \
	server/alert.cpp \
	server/bitcoinrpc.cpp \
	server/checkpoints.cpp \
	server/crypter.cpp \
	server/db.cpp \
	server/init.cpp \
	server/irc.cpp \
	server/kernel.cpp \
	server/key.cpp \
	server/keystore.cpp \
	server/main.cpp \
	server/netbase.cpp \
	server/net.cpp \
	server/noui.cpp \
	server/pbkdf2.cpp \
	server/protocol.cpp \
	server/rpcblockchain.cpp \
	server/rpcdump.cpp \
	server/rpcmining.cpp \
	server/rpcnet.cpp \
	server/rpcrawtransaction.cpp \
	server/rpcwallet.cpp \
	server/script.cpp \
	server/scrypt_mine.cpp \
	server/sync.cpp \
	server/util.cpp \
	server/version.cpp \
	server/wallet.cpp \
	server/walletdb.cpp \
	stratum/stratum_message.c \
	stratum/stratum_protocol.c \
	stratum/stratum_user.c \
	stratum/stratum_task.c \
	shcgbd_daemon.c \
	shcgbd_block.c \
	shcgbd.c

bin_PROGRAMS = shcgbd

shcgbd_SOURCES = $(common_INCLUDES) $(common_SOURCES)
shcgbd_LDADD = $(top_builddir)/src/share-lib/libshare.a \
	$(top_srcdir)/depend/openssl-1.0.1g/libssl.a \
	$(top_srcdir)/depend/openssl-1.0.1g/libcrypto.a

if DEBUG
AM_CFLAGS = -g3 -O0 \
	-I$(top_srcdir)/src/share-lib \
	-Isrc/share-lib 
AM_CXXFLAGS = -g3 -O0
else
AM_CFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops \
	-I$(top_srcdir)/src/share-lib \
	-Isrc/share-lib 
AM_CXXFLAGS = \
	-fexpensive-optimizations -ffast-math \
	-fomit-frame-pointer -fstrength-reduce -fthread-jumps -funroll-loops
endif

#man1_MANS = share.1
#
#EXTRA_DIST = \
#	shcat

##	testing/share-test.py \
##	share.1 \
##	share.i \
##	share.vcxproj \
##	share.wxi \
##	share.wxs
#
#
