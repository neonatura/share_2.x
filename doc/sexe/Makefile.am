if HAVE_DOXYGEN

html: doxyfile.stamp

dist_man_MANS = \
	man/man1/sxsh.1 \
	man/man1/sx.1 \
	man/man1/sxc.1 \
	man/man1/readsexe.1

doxyfile.stamp:
	mkdir -p html man/man1
	$(DOXYGEN) doxygen.conf
	$(HELP2MAN) --output=man/man1/sxsh.1 --name='sexe command-line interpreter' $(top_builddir)/src/sexe/shell/sxsh
	$(HELP2MAN) --output=man/man1/readsexe.1 --name='sexe bytecode symbol list' $(top_builddir)/src/sexe/readsexe
	$(HELP2MAN) --output=man/man1/sx.1 --name='sexe bytecode interpreter' $(top_builddir)/src/sexe/sx
	$(HELP2MAN) --output=man/man1/sxc.1 --name='sexe bytecode compiler' $(top_builddir)/src/sexe/sxc
	echo Timestamp > doxyfile.stamp

#	$(HELP2MAN) --output=man/man1/sxshd.1 --name='sexe command-line interpreter daemon' $(top_builddir)/src/sexe/shell/sxshd

man/man1/sxsh.1: doxyfile.stamp
man/man1/readsexe.1: doxyfile.stamp
man/man1/sx.1: doxyfile.stamp
man/man1/sxc.1: doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
	rm -rf $(builddir)/html doxyfile.stamp

endif
