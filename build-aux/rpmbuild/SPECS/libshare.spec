Name:           libshare
Version:        2.26
Release:        1%{?dist}
Summary:        The share library suite.

Group:          System Environment/Libraries
License:        GPLv3+
URL:            http://www.sharelib.net/
Source0:        http://www.sharelib.net/release/libshare-2.26.tar.gz

#BuildRequires:  gcc
#Requires:       info 

%description


%package        devel
Summary:        Development files for %{name}
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description    devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.


%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'


%check
make check


%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%doc
%{_libdir}/*.so.*
%{_libdir}/*.jar
%{_bindir}/readsexe
%{_bindir}/shar
%{_bindir}/shattr
%{_bindir}/shcat
%{_bindir}/shcp
%{_bindir}/shdelta
%{_bindir}/shdiff
%{_bindir}/shinfo
%{_bindir}/shln
%{_bindir}/shlog
%{_bindir}/shls
%{_bindir}/shpasswd
%{_bindir}/shpatch
%{_bindir}/shpref
%{_bindir}/shrev
%{_bindir}/shrm
%{_bindir}/shstat
%{_bindir}/sx
%{_bindir}/sxc
%{_bindir}/sxsh
%{_bindir}/unshar
%{_docdir}/libshare/libshare_html.tar.xz
%{_sbindir}/shared
%{_sbindir}/shfsyncd
%{_sbindir}/shlogd
%{_mandir}/man1/readsexe.1.gz
%{_mandir}/man1/shar.1.gz
%{_mandir}/man1/shattr.1.gz
%{_mandir}/man1/shcat.1.gz
%{_mandir}/man1/shcp.1.gz
%{_mandir}/man1/shdelta.1.gz
%{_mandir}/man1/shdiff.1.gz
%{_mandir}/man1/shinfo.1.gz
%{_mandir}/man1/shln.1.gz
%{_mandir}/man1/shls.1.gz
%{_mandir}/man1/shpasswd.1.gz
%{_mandir}/man1/shpatch.1.gz
%{_mandir}/man1/shpref.1.gz
%{_mandir}/man1/shrev.1.gz
%{_mandir}/man1/shrm.1.gz
%{_mandir}/man1/shstat.1.gz
%{_mandir}/man1/sx.1.gz
%{_mandir}/man1/sxc.1.gz
%{_mandir}/man1/sxsh.1.gz
%{_mandir}/man1/unshar.1.gz
%{_mandir}/man3/ashkey_num.3.gz
%{_mandir}/man3/ashkey_str.3.gz
%{_mandir}/man3/libshare.3.gz
%{_mandir}/man3/libshare_fs.3.gz
%{_mandir}/man3/libshare_mem.3.gz
%{_mandir}/man3/libshare_net.3.gz
%{_mandir}/man3/shbuf_cat.3.gz
%{_mandir}/man3/shbuf_catstr.3.gz
%{_mandir}/man3/shbuf_clear.3.gz
%{_mandir}/man3/shbuf_free.3.gz
%{_mandir}/man3/shbuf_init.3.gz
%{_mandir}/man3/shbuf_size.3.gz
%{_mandir}/man3/shbuf_trim.3.gz
%{_mandir}/man3/shcrypt_ashdecode.3.gz
%{_mandir}/man3/shcrypt_ashencode.3.gz
%{_mandir}/man3/shcrypt_shdecode.3.gz
%{_mandir}/man3/shcrypt_shdecode_str.3.gz
%{_mandir}/man3/shcrypt_shencode.3.gz
%{_mandir}/man3/shcrypt_shencode_str.3.gz
%{_mandir}/man3/shfs_link.3.gz
%{_mandir}/man3/shfs_link_find.3.gz
%{_mandir}/man3/shfs_sig_verify.3.gz
%{_mandir}/man3/shfs_unlink.3.gz
%{_mandir}/man3/shkey_bin.3.gz
%{_mandir}/man3/shkey_free.3.gz
%{_mandir}/man3/shkey_num.3.gz
%{_mandir}/man3/shkey_print.3.gz
%{_mandir}/man3/shkey_str.3.gz
%{_mandir}/man3/shkey_uniq.3.gz
%{_mandir}/man3/shlock_close.3.gz
%{_mandir}/man3/shlock_open.3.gz
%{_mandir}/man3/shlock_tryopen.3.gz
%{_mandir}/man3/shmeta_free.3.gz
%{_mandir}/man3/shmeta_get.3.gz
%{_mandir}/man3/shmeta_get_str.3.gz
%{_mandir}/man3/shmeta_get_void.3.gz
%{_mandir}/man3/shmeta_init.3.gz
%{_mandir}/man3/shmeta_print.3.gz
%{_mandir}/man3/shmeta_set.3.gz
%{_mandir}/man3/shmeta_set_str.3.gz
%{_mandir}/man3/shmeta_set_void.3.gz
%{_mandir}/man3/shmeta_unset_str.3.gz
%{_mandir}/man3/shmeta_unset_void.3.gz
%{_mandir}/man3/shmsg_read.3.gz
%{_mandir}/man3/shmsg_write.3.gz
%{_mandir}/man3/shmsgctl.3.gz
%{_mandir}/man3/shmsgget.3.gz
%{_mandir}/man3/shmsgrcv.3.gz
%{_mandir}/man3/shmsgsnd.3.gz
%{_mandir}/man3/shconnect.3.gz
%{_mandir}/man3/shconnect_host.3.gz
%{_mandir}/man3/shconnect_peer.3.gz
%{_mandir}/man3/shclose.3.gz
%{_mandir}/man3/shnet_accept.3.gz
%{_mandir}/man3/shnet_bind.3.gz
%{_mandir}/man3/shnet_bindsk.3.gz
%{_mandir}/man3/shnet_fcntl.3.gz
%{_mandir}/man3/shnet_gethostbyname.3.gz
%{_mandir}/man3/shnet_peer.3.gz
%{_mandir}/man3/shnet_read.3.gz
%{_mandir}/man3/shnet_select.3.gz
%{_mandir}/man3/shnet_sk.3.gz
%{_mandir}/man3/shnet_socket.3.gz
%{_mandir}/man3/shnet_verify.3.gz
%{_mandir}/man3/shnet_write.3.gz
%{_mandir}/man3/shpool_free.3.gz
%{_mandir}/man3/shpool_get.3.gz
%{_mandir}/man3/shpool_get_index.3.gz
%{_mandir}/man3/shpool_grow.3.gz
%{_mandir}/man3/shpool_init.3.gz
%{_mandir}/man3/shpool_put.3.gz
%{_mandir}/man3/shpool_size.3.gz


%files devel
%defattr(-,root,root,-)
%doc
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/*.a


%changelog
* May 08 2015 Neo Natura - 2.26
- Initial RPM release version of this package.

