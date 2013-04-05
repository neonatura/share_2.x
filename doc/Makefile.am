dist_man_MANS = man/man1/shnet.1

man/man1/shnet.1: helpfile.stamp

helpfile.stamp:
	mkdir -p man/man1
	$(HELP2MAN) --output=man/man1/shnet.1 --name='libshare command-line tool' $(top_builddir)/src/share-util/shnet
	echo Timestamp > helpfile.stamp

if HAVE_DOXYGEN
dist_man_MANS += \
	man/man3/libshare_buffer.3 \
	man/man3/libshare_fs.3 \
	man/man3/libshare_fs_inode.3 \
	man/man3/libshare_fs_mode.3 \
	man/man3/libshare_key.3 \
	man/man3/libshare_meta.3 \
	man/man3/libshare_socket.3  
man/man3/libshare_buffer.3: doxyfile.stamp
man/man3/libshare_fs.3: doxyfile.stamp
man/man3/libshare_fs_inode.3: doxyfile.stamp
man/man3/libshare_fs_mode.3: doxyfile.stamp
man/man3/libshare_key.3: doxyfile.stamp
man/man3/libshare_meta.3: doxyfile.stamp
man/man3/libshare_socket.3: doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) doxygen.conf
	echo Timestamp > doxyfile.stamp

else

doxyfile.stamp:
	echo Timestamp > doxyfile.stamp

endif

CLEANFILES = doxyfile.stamp helpfile.stamp

all-local: doxyfile.stamp helpfile.stamp
clean-local:
	rm -rf $(top_builddir)/doc/man

