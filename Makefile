#  Copyright 2013 Brian Burrell 
#
#  This file is part of the Share Library.
#  (https://github.com/briburrell/share)
#        
#  The Share Library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version. 
#
#  The Share Library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with The Share Library.  If not, see <http://www.gnu.org/licenses/>.

all:
	make -C share-lib 
	make -C share-util

install: include/share.h
	make -C share-lib install
	make -C share-util install
	@mkdir -p /usr/local/share
	@cp -fr include/* /usr/local/share

clean:
	make -C share-util clean
	make -C share-lib clean
	rm -rf include
	rm -rf doc/doxygen/html

doc: include/share.h doc/doxygen/html
	echo Generated API documentation in 'doc/doxygen/html'.

doc/doxygen/html:
	doxygen doc/doxygen.conf

include/share.h:
	@mkdir -p include
	@cp -f share-lib/share.h include
	@mkdir -p include/key
	@cp -f share-lib/key/shkey.h include/key
	@mkdir -p include/svn
	@cp -f share-lib/svn/shsvn.h include/svn
	@cp -f share-lib/svn/shsvn_hash.h include/svn
	@mkdir -p include/fs
	@cp -f share-lib/fs/shfs.h include/fs
	@cp -f share-lib/fs/shfs_meta.h include/fs
	@cp -f share-lib/fs/shfs_read.h include/fs
	@cp -f share-lib/fs/shfs_write.h include/fs
	@cp -f share-lib/fs/shfs_adler.h include/fs
	@cp -f share-lib/fs/shfs_time64.h include/fs
	@mkdir -p include/socket
	echo Generated 'include/' directory.
	@cp -f share-lib/socket/sockbuff.h include/socket
	@cp -f share-lib/socket/socket.h include/socket
	@cp -f share-lib/socket/connect.h include/socket
	@cp -f share-lib/socket/gethost.h include/socket
	
