## Process this file with automake to produce Makefile.in

libgimpui = $(top_builddir)/libgimp/libgimpui-$(GIMP_API_VERSION).la
libgimpconfig = $(top_builddir)/libgimpconfig/libgimpconfig-$(GIMP_API_VERSION).la
libgimpwidgets = $(top_builddir)/libgimpwidgets/libgimpwidgets-$(GIMP_API_VERSION).la
libgimp = $(top_builddir)/libgimp/libgimp-$(GIMP_API_VERSION).la
libgimpcolor = $(top_builddir)/libgimpcolor/libgimpcolor-$(GIMP_API_VERSION).la
libgimpbase = $(top_builddir)/libgimpbase/libgimpbase-$(GIMP_API_VERSION).la
libgimpmath = $(top_builddir)/libgimpmath/libgimpmath-$(GIMP_API_VERSION).la

if OS_WIN32
mwindows = -mwindows
endif

AM_LDFLAGS = $(mwindows)

libexecdir = $(gimpplugindir)/plug-ins

libexec_PROGRAMS = uri

if HAVE_GNOMEVFS
backend_sources = uri-backend-gnomevfs.c
backend_cflags = $(URI_GNOME_VFS_CFLAGS)
backend_libs = $(URI_GNOME_VFS_LIBS)
else
if HAVE_LIBCURL
backend_sources = uri-backend-libcurl.c
backend_cflags = $(URI_LIBCURL_CFLAGS)
backend_libs = $(URI_LIBCURL_LIBS)
else
backend_sources = uri-backend-ingimp.c
backend_cflags = 
backend_libs = 
endif
endif

uri_SOURCES = \
	uri.c			\
	uri-backend.h		\
	$(backend_sources)

INCLUDES = \
	-I$(top_srcdir)		\
	$(backend_cflags)	\
	$(GTK_CFLAGS)		\
	-I$(includedir)

LDADD = \
	$(libgimpui)		\
	$(libgimpwidgets)	\
	$(libgimpconfig)	\
	$(libgimpmath)		\
	$(libgimp)		\
	$(libgimpcolor)		\
	$(libgimpbase)		\
	$(backend_libs)		\
	$(GTK_LIBS)		\
	$(RT_LIBS)		\
	$(INTLLIBS)
