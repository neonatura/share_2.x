#!/bin/bash
mkdir -p release/libshare/doc release/libshare/bin release/libshare/sbin release/libshare/lib
cd release
../configure --libdir=/h/src/sharelib/release/libshare/lib --bindir=/h/src/sharelib/release/libshare/bin --sbindir=/h/src/sharelib/release/libshare/sbin --docdir=/h/src/sharelib/release/libshare/doc
make
make install
tar -cpf libshare-`arch`.tar libshare
gzip -f libshare-`arch`.tar
