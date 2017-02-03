# Notes
# You've got to make sure everything in the support dir is executable
# So do a chmod -R 755 in the support dir. Note this chmod seems to only
# work when done in cygwin... Go figure...
# Need to actually build zlib-1.2.3 to get the dev libs
# Need gw32c lib
# This one plug-in, wmf, will bomb out at start-up unless the
# following DLLs are renamed to the following names:
# freetype-6.dll, jpeg-62.dll, libpng.dll, zlib-1.dll
# Why it looks for these specific names, I do not know...
# Rename xmlparse.dll to libexpat.dll
# After doing an install, will need to copy all the dll's to the bin directory
# Will also need to copy glib/gtk/pango stuff in etc and lib to corresponding
# gimp directory
# Need to include libpng12.dll or it won't load images (copy libpng13.dll to libpng12.dll)
# I needed to compile lcms 1.17 from source in msys to make 2.4 compile. Also need to remove the "cdecl" from lcms.h file
# Need to add:
#include <ft2build.h>
# to wmf.c in plug-ins/common before any libwmf includes
#Also see this:
#Since my last post, I found that if I include -lintl in the link for libgimbase-2.0.dll, it works.
#
#I'm guessing GLIB_LIBS needs to include -lintl and -liconv so, I edited glib-2.0.pc and made the Libs line look like this:
#
#Libs: -L${libdir} -lglib-2.0 -lintl -liconv
#
#I then re-ran configure. Now GLIB_LIBS and other vars (GTK_LIBS, GDK_PIXBUFS_LIBS, etc) include -lintl -liconv 
#
#From: http://gug.sunsite.dk/forum/?threadid=3986
#
SUPPORT_DIR=/c/data/ingimp-2.4-support/dev
GLIB_LIB_DIR=${SUPPORT_DIR}/lib/glib-2.0
GTK_LIB_DIR=${SUPPORT_DIR}/lib/gtk-2.0
PANGO_LIB_DIR=${SUPPORT_DIR}/lib/pango
EXTRA_PATHS=${SUPPORT_DIR}/bin:${SUPPORT_DIR}/lib:${GLIB_LIB_DIR}:${GTK_LIB_DIR}:${GTK_LIB_DIR}/2.4.0/engines:${GTK_LIB_DIR}/2.4.0/immodules:${GTK_LIB_DIR}/2.4.0/loaders:${PANGO_LIB_DIR}:${PANGO_LIB_DIR}/1.5.0/modules
export PATH=$EXTRA_PATHS:$PATH:/c/cygwin/bin
export LIBRARY_PATH=$LIBRARY_PATH:$EXTRA_PATHS
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$EXTRA_PATHS
# Needed for pkg-config to be able to find its .pc files
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:${SUPPORT_DIR}/lib/pkgconfig
export C_INCLUDE_PATH=${C_INCLUDE_PATH}:${SUPPORT_DIR}/include:${SUPPORT_DIR}/include/freetype2
