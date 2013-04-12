

noinst_INCLUDES = \
	share.h \
	sherr.h \
	shpeer.h \
	shtime.h \
	shcrc.h \
	shpref.h \
	shfile.h \
	mem/shmem.h \
	mem/shmem_buf.h \
	mem/shmem_pool.h \
	mem/shmem_meta.h \
	mem/shmem_crypt.h \
	mem/shmem_key.h \
	mem/shmem_lock.h \
	fs/shfs.h \
	fs/shfs_meta.h \
	fs/shfs_journal.h \
	fs/shfs_read.h \
	fs/shfs_write.h \
	fs/shfs_partition.h \
	fs/shfs_inode.h \
	fs/shfs_dir.h \
	net/shnet.h \
	net/shnet_read.h \
	net/shnet_bind.h \
	net/shnet_connect.h \
	net/shnet_fcntl.h \
	net/shnet_gethost.h \
	net/shnet_close.h \
	net/shnet_socket.h \
	net/shnet_accept.h \
	net/shnet_write.h

share.h: .include-stamp
sherr.h: .include-stamp
shpeer.h: .include-stamp
shtime.h: .include-stamp
mem/shmem.h: .include-stamp
mem/shmem_buf.h: .include-stamp
mem/shmem_pool.h: .include-stamp
mem/shmem_meta.h: .include-stamp
mem/shmem_crypt.h: .include-stamp
mem/shmem_key.h: .include-stamp
mem/shmem_lock.h: .include-stamp
shcrc.h: .include-stamp
fs/shfs.h: .include-stamp
fs/shfs_meta.h: .include-stamp
fs/shfs_journal.h: .include-stamp
fs/shfs_read.h: .include-stamp
fs/shfs_write.h: .include-stamp
fs/shfs_partition.h: .include-stamp
fs/shfs_inode.h: .include-stamp
fs/shfs_dir.h: .include-stamp
shpref.h: .include-stamp
shfile.h: .include-stamp
net/shnet.h: .include-stamp
net/shnet_read.h: .include-stamp
net/shnet_bind.h: .include-stamp
net/shnet_connect.h: .include-stamp
net/shnet_fcntl.h: .include-stamp
net/shnet_gethost.h: .include-stamp
net/shnet_close.h: .include-stamp
net/shnet_socket.h: .include-stamp
net/shnet_accept.h: .include-stamp
net/shnet_write.h: .include-stamp


.include-stamp:
	ln -sf $(top_srcdir)/src/share-lib/share.h
	ln -sf $(top_srcdir)/src/share-lib/sherr.h
	ln -sf $(top_srcdir)/src/share-lib/shpeer.h
	ln -sf $(top_srcdir)/src/share-lib/shtime.h
	ln -sf $(top_srcdir)/src/share-lib/shcrc.h
	ln -sf $(top_srcdir)/src/share-lib/shpref.h
	ln -sf $(top_srcdir)/src/share-lib/shfile.h
	mkdir -p mem
	ln -sf $(top_srcdir)/src/share-lib/mem/shmem.h mem
	ln -sf $(top_srcdir)/src/share-lib/mem/shmem_buf.h mem
	ln -sf $(top_srcdir)/src/share-lib/mem/shmem_pool.h mem
	ln -sf $(top_srcdir)/src/share-lib/mem/shmem_meta.h mem
	ln -sf $(top_srcdir)/src/share-lib/mem/shmem_crypt.h mem
	ln -sf $(top_srcdir)/src/share-lib/mem/shmem_key.h mem
	ln -sf $(top_srcdir)/src/share-lib/mem/shmem_lock.h mem
	mkdir -p fs
	ln -sf $(top_srcdir)/src/share-lib/fs/shfs.h fs
	ln -sf $(top_srcdir)/src/share-lib/fs/shfs_meta.h fs
	ln -sf $(top_srcdir)/src/share-lib/fs/shfs_journal.h fs
	ln -sf $(top_srcdir)/src/share-lib/fs/shfs_read.h fs
	ln -sf $(top_srcdir)/src/share-lib/fs/shfs_write.h fs
	ln -sf $(top_srcdir)/src/share-lib/fs/shfs_partition.h fs
	ln -sf $(top_srcdir)/src/share-lib/fs/shfs_inode.h fs
	ln -sf $(top_srcdir)/src/share-lib/fs/shfs_dir.h fs
	mkdir -p net
	ln -sf $(top_srcdir)/src/share-lib/net/shnet.h net
	ln -sf $(top_srcdir)/src/share-lib/net/shnet_read.h net
	ln -sf $(top_srcdir)/src/share-lib/net/shnet_bind.h net
	ln -sf $(top_srcdir)/src/share-lib/net/shnet_connect.h net
	ln -sf $(top_srcdir)/src/share-lib/net/shnet_fcntl.h net
	ln -sf $(top_srcdir)/src/share-lib/net/shnet_gethost.h net
	ln -sf $(top_srcdir)/src/share-lib/net/shnet_close.h net
	ln -sf $(top_srcdir)/src/share-lib/net/shnet_socket.h net
	ln -sf $(top_srcdir)/src/share-lib/net/shnet_accept.h net
	ln -sf $(top_srcdir)/src/share-lib/net/shnet_write.h net
	echo Timestamp > .include-stamp

clean-local: 
	rm -f $(noinst_INCLUDES)

all-local: .include-stamp
