Why Androgenizer
================

Android has its own GNU make-based build system. However, most developers
would not be happy to add an additional build system to their open source
project (most likely using autotools), as doing it would likely lead to the
build systems diverging at some point. Androgenizer was created to avoid this
situation.

Instead of adding an Android.mk file for each module of your project, you can
now add a single top-level Android.mk for running configure automatically as a
pre-build phase. This completes the autoconf/automake/configure step and
then calls make in the relevant directories to create an Android.mk.

Each Makefile.am from the modules you want to compile on Android gets a small
fragment to generate an Android.mk file using androgenizer. In this call to
androgenizer, SOURCES, CFLAGS, LDFLAGS, LIBADD etc. are provided and used to 
automatically generate the corresponding Android.mk. Since the file is
generated using automake variables, the two builds will remain in sync.

NDK builds and system builds
============================

Sometimes the Android.mk file contents depend on whether the project is
being built with the NDK or as part of a full Android OS distribution, in
the external/ directory. The latter is referred to as a system build.
Androgenizer takes the difference into account.

Androgenizer detects a system build by the ANDROID_BUILD_TOP environment
variable, and if ANDROID_BUILD_TOP is not set or is empty, assumes an
NDK build.

Parameters
==========

At this point, having read the Android build system documentation would be a
good thing. If you have the Android code, look at
build/core/build-system.html. For convenience, this is mirrored at:
http://people.collabora.com/~arun/android/build-system.html

androgenizer takes the following parameters:

-:PROJECT should be called first, and once.

-:SUBDIR adds an -include, expects <project>_TOP variable to be defined

Path substitution for -I statements
-:ABS_TOP sets the absolute path to the source directory
-:REL_TOP sets the relative path to the source directory
Should probably always be used as such:
	-:REL_TOP $(top_srcdir) -:ABS_TOP $(abs_top_srcdir)

Module instantiation:
-:STATIC  creates a new module that will close with BUILD_STATIC_LIBRARY
-:SHARED  creates a new module that will close with BUILD_SHARED_LIBRARY
-:EXECUTABLE creates a new module that will close with BUILD_EXECUTABLE
-:HOST_STATIC  creates a new module that will close with BUILD_HOST_STATIC_LIBRARY
-:HOST_SHARED  creates a new module that will close with BUILD_HOST_SHARED_LIBRARY
-:HOST_EXECUTABLE creates a new module that will close with BUILD_HOST_EXECUTABLE
Multiple modules can be instantiated with a single command line

Adding resources to modules (a module must be declared first!):
-:SOURCES followed by any number of source files

-:CFLAGS followed by any number of flags to pass to the C compiler
-:CXXFLAGS followed by any number of flags to pass to the C++ compiler
-:CPPFLAGS followed by any number of C pre-processor flags

	CPPFLAGS will be passed to both the C and C++ compiler
	CFLAGS will also be passed to *both* the C and C++ compiler
	CXXFLAGS will be passed to just the C++ compiler

	Android uses a different convention - don't be surprised when your
	CXXFLAGS end up in LOCAL_CPPFLAGS in an Android.mk
	There is no way to pass flags to just the C compiler in	the Android
	build system.

	All -I flags in any of CFLAGS, CPPFLAGS or CXXFLAGS will be emitted
	into LOCAL_C_INCLUDES without the "-I".

	some flags are silently removed: -Werror -pthread

-:LDFLAGS followed by any number of linker directives to be processed...
	-l<foo> will be added as lib<foo> to LOCAL_SHARED_LIBRARIES
	-L and -R will be silently removed
	-pthread and -lpthread will be silently removed
	-lrt will be silently removed (rt is a built-in library in bionic)
	-no-undefined will be silently removed
	-dlopen, -version-info, and the word following those
	(the option argument) will be silently removed
	Of plain file arguments, only *.a and *.la files are kept, all others
	are silently dropped.

-:LIBFILTER_STATIC followed by a list of libs (no lib prefix, or extension)
	These libs will be added to LOCAL_STATIC_LIBRARIES instead of
	LOCAL_SHARED_LIBRARIES. Note, that this is just a filter,
	you also need the lib in -:LDFLAGS for it to be linked at all.

-:LIBFILTER_WHOLE followed by a list of libs (no lib prefix, or extension)
	These libs will be added to LOCAL_WHOLE_STATIC_LIBRARIES.
	This is just a filter, see -:LIBFILTER_STATIC.

-:TAGS  must be followed by any number of: optional user eng tests

-:HEADERS followed by any number of headers for LOCAL_COPY_HEADERS

-:HEADER_TARGET sets LOCAL_COPY_HEADERS_TO
	may be followed by multiple strings, but only the last is kept

-:PASSTHROUGH followed by any number of strings to be dumped directly into
	the current module.  eg LOCAL_ARM_MODE:=arm

-:END optional... might go away in the future, was probably a dumb idea.
	ends the current module, but so does starting a new one...

Example
=======

From the gstreamer build for the main libgstreamer-0.10 library.

Android.mk: Makefile.am
	androgenizer -:PROJECT gstreamer \
		     # `-- the name of the project
	 -:SHARED libgstreamer-@GST_MAJORMINOR@ \
	 -:TAGS eng debug \
         -:REL_TOP $(top_srcdir) -:ABS_TOP $(abs_top_srcdir) \
	 -:SOURCES $(libgstreamer_@GST_MAJORMINOR@_la_SOURCES) \
	           $(nodist_libgstreamer_@GST_MAJORMINOR@_la_SOURCES) \
	 -:CFLAGS $(DEFS) $(libgstreamer_@GST_MAJORMINOR@_la_CFLAGS) \
	 -:LDFLAGS $(libgstreamer_@GST_MAJORMINOR@_la_LDFLAGS) \
	           $(libgstreamer_@GST_MAJORMINOR@_la_LIBADD) \
	           -ldl \
	 -:SUBDIR gst/parse \
	 -:HEADER_TARGET gstreamer-@GST_MAJORMINOR@/gst \
	 -:HEADERS $(libgstreamer_@GST_MAJORMINOR@include_HEADERS) \
	 -:LIBFILTER_STATIC gstparse \
	 -:PASSTHROUGH LOCAL_ARM_MODE:=arm \
	> $@
