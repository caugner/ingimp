## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DGIMP_APP_VERSION=\"$(GIMP_APP_VERSION)\"	\
	-DG_LOG_DOMAIN=\"Gimp-Core\"

INCLUDES = \
	-I$(top_builddir)	\
	-I$(top_srcdir)		\
	-I$(top_builddir)/app	\
	-I$(top_srcdir)/app	\
	$(GDK_PIXBUF_CFLAGS)	\
	$(GTK_CFLAGS) \
	$(LIBART_CFLAGS)	\
	$(GLIB_CFLAGS)		\
	-I$(includedir)

noinst_LIBRARIES = libappcore.a

libappcore_a_sources = \
	core-enums.h			\
	core-types.h			\
	gimp.c				\
	gimp.h				\
	gimp-contexts.c			\
	gimp-contexts.h			\
	gimp-documents.c		\
	gimp-documents.h		\
	gimp-edit.c			\
	gimp-edit.h			\
	gimp-gradients.c		\
	gimp-gradients.h		\
	gimp-gui.c			\
	gimp-gui.h			\
	gimp-modules.c			\
	gimp-modules.h			\
	gimp-parasites.c		\
	gimp-parasites.h		\
	gimp-templates.c		\
	gimp-templates.h		\
	gimp-transform-region.c		\
	gimp-transform-region.h		\
	gimp-transform-resize.c		\
	gimp-transform-resize.h		\
	gimp-transform-utils.c		\
	gimp-transform-utils.h		\
	gimp-units.c			\
	gimp-units.h			\
	gimp-user-install.c		\
	gimp-user-install.h		\
	gimp-utils.c			\
	gimp-utils.h			\
	gimparea.c			\
	gimparea.h			\
	gimpbrush.c			\
	gimpbrush.h			\
	gimpbrush-header.h		\
	gimpbrush-load.c		\
	gimpbrush-load.h		\
	gimpbrush-scale.c		\
	gimpbrush-scale.h		\
	gimpbrushclipboard.c		\
	gimpbrushclipboard.h		\
	gimpbrushgenerated.c		\
	gimpbrushgenerated.h		\
	gimpbrushgenerated-load.c	\
	gimpbrushgenerated-load.h	\
	gimpbrushgenerated-save.c	\
	gimpbrushgenerated-save.h	\
	gimpbrushpipe.c			\
	gimpbrushpipe.h			\
	gimpbrushpipe-load.c		\
	gimpbrushpipe-load.h		\
	gimpbuffer.c			\
	gimpbuffer.h			\
	gimpchannel.c			\
	gimpchannel.h			\
	gimpchannel-combine.c		\
	gimpchannel-combine.h		\
	gimpchannel-select.c		\
	gimpchannel-select.h		\
	gimpchannelpropundo.c		\
	gimpchannelpropundo.h		\
	gimpchannelundo.c		\
	gimpchannelundo.h		\
	gimpcontainer.c			\
	gimpcontainer.h			\
	gimpcontainer-filter.c		\
	gimpcontainer-filter.h		\
	gimpcontext.c			\
	gimpcontext.h			\
	gimpcoords.c			\
	gimpcoords.h			\
	gimpdashpattern.c		\
	gimpdashpattern.h		\
	gimpdata.c			\
	gimpdata.h			\
	gimpdatafactory.c		\
	gimpdatafactory.h		\
	gimpdocumentlist.c		\
	gimpdocumentlist.h		\
	gimpdrawable.c			\
	gimpdrawable.h			\
	gimpdrawable-blend.c		\
	gimpdrawable-blend.h		\
	gimpdrawable-bucket-fill.c	\
	gimpdrawable-bucket-fill.h	\
	gimpdrawable-combine.c		\
	gimpdrawable-combine.h		\
	gimpdrawable-convert.c		\
	gimpdrawable-convert.h		\
	gimpdrawable-desaturate.c	\
	gimpdrawable-desaturate.h	\
	gimpdrawable-equalize.c		\
	gimpdrawable-equalize.h		\
	gimpdrawable-foreground-extract.c\
	gimpdrawable-foreground-extract.h\
	gimpdrawable-histogram.c	\
	gimpdrawable-histogram.h	\
	gimpdrawable-invert.c		\
	gimpdrawable-invert.h		\
	gimpdrawable-levels.c		\
	gimpdrawable-levels.h		\
	gimpdrawable-offset.c		\
	gimpdrawable-offset.h		\
	gimpdrawable-preview.c		\
	gimpdrawable-preview.h		\
	gimpdrawable-stroke.c		\
	gimpdrawable-stroke.h		\
	gimpdrawable-transform.c	\
	gimpdrawable-transform.h	\
	gimpdrawablemodundo.c		\
	gimpdrawablemodundo.h		\
	gimpdrawableundo.c		\
	gimpdrawableundo.h		\
	gimpfloatingselundo.c		\
	gimpfloatingselundo.h		\
	gimpgradient.c			\
	gimpgradient.h			\
	gimpgradient-load.c		\
	gimpgradient-load.h		\
	gimpgradient-save.c		\
	gimpgradient-save.h		\
	gimpgrid.c			\
	gimpgrid.h			\
	gimpguide.c			\
	gimpguide.h			\
	gimpguideundo.c			\
	gimpguideundo.h			\
	gimpimage.c			\
	gimpimage.h			\
	gimpimage-arrange.c		\
	gimpimage-arrange.h		\
	gimpimage-colorhash.c		\
	gimpimage-colorhash.h		\
	gimpimage-colormap.c		\
	gimpimage-colormap.h		\
	gimpimage-contiguous-region.c	\
	gimpimage-contiguous-region.h	\
	gimpimage-convert.c		\
	gimpimage-convert.h		\
	gimpimage-convert-fsdither.h	\
	gimpimage-convert-data.h	\
	gimpimage-crop.c		\
	gimpimage-crop.h		\
	gimpimage-duplicate.c		\
	gimpimage-duplicate.h		\
	gimpimage-flip.c		\
	gimpimage-flip.h		\
	gimpimage-grid.h                \
	gimpimage-grid.c                \
	gimpimage-guides.c		\
	gimpimage-guides.h		\
	gimpimage-item-list.c		\
	gimpimage-item-list.h		\
	gimpimage-merge.c		\
	gimpimage-merge.h		\
	gimpimage-new.c			\
	gimpimage-new.h			\
	gimpimage-pick-color.c		\
	gimpimage-pick-color.h		\
	gimpimage-preview.c		\
	gimpimage-preview.h		\
	gimpimage-quick-mask.c		\
	gimpimage-quick-mask.h		\
	gimpimage-resize.c		\
	gimpimage-resize.h		\
	gimpimage-rotate.c		\
	gimpimage-rotate.h		\
	gimpimage-sample-points.c	\
	gimpimage-sample-points.h	\
	gimpimage-scale.c		\
	gimpimage-scale.h		\
	gimpimage-snap.c		\
	gimpimage-snap.h		\
	gimpimage-undo.c		\
	gimpimage-undo.h		\
	gimpimage-undo-push.c		\
	gimpimage-undo-push.h		\
	gimpimageundo.c			\
	gimpimageundo.h			\
	gimpimagefile.c			\
	gimpimagefile.h			\
	gimpimagemap.c			\
	gimpimagemap.h			\
	gimpinteraction-logger.c \
	gimpinteraction-logger.h \
	gimpitem.c			\
	gimpitem.h			\
	gimpitem-linked.c		\
	gimpitem-linked.h		\
	gimpitem-preview.c		\
	gimpitem-preview.h		\
	gimpitempropundo.c		\
	gimpitempropundo.h		\
	gimpitemundo.c			\
	gimpitemundo.h			\
	gimplayer.c			\
	gimplayer.h			\
	gimplayer-floating-sel.c	\
	gimplayer-floating-sel.h	\
	gimplayermask.c			\
	gimplayermask.h			\
	gimplayermaskpropundo.c		\
	gimplayermaskpropundo.h		\
	gimplayermaskundo.c		\
	gimplayermaskundo.h		\
	gimplayerpropundo.c		\
	gimplayerpropundo.h		\
	gimplayerundo.c			\
	gimplayerundo.h			\
	gimplist.c			\
	gimplist.h			\
	gimpmaskundo.c			\
	gimpmaskundo.h			\
	gimpobject.c			\
	gimpobject.h			\
	gimppaintinfo.c			\
	gimppaintinfo.h			\
	gimppattern.c			\
	gimppattern.h			\
	gimppattern-header.h		\
	gimppattern-load.c		\
	gimppattern-load.h		\
	gimppatternclipboard.c		\
	gimppatternclipboard.h		\
	gimppalette.c			\
	gimppalette.h			\
	gimppalette-import.c		\
	gimppalette-import.h		\
	gimppalette-load.c		\
	gimppalette-load.h		\
	gimppalette-save.c		\
	gimppalette-save.h		\
	gimpparamspecs.c		\
	gimpparamspecs.h		\
	gimpparamspecs-desc.c		\
	gimpparamspecs-desc.h		\
	gimpparasitelist.c		\
	gimpparasitelist.h		\
	gimppdbprogress.c		\
	gimppdbprogress.h		\
	gimppickable.c			\
	gimppickable.h			\
	gimppreviewcache.c		\
	gimppreviewcache.h		\
	gimpprogress.c			\
	gimpprogress.h			\
	gimpprojection.c		\
	gimpprojection.h		\
	gimpprojection-construct.c	\
	gimpprojection-construct.h	\
	gimpsamplepoint.c		\
	gimpsamplepoint.h		\
	gimpsamplepointundo.c		\
	gimpsamplepointundo.h		\
	gimpscanconvert.c		\
	gimpscanconvert.h		\
	gimpselection.c			\
	gimpselection.h			\
	gimpstrokedesc.c		\
	gimpstrokedesc.h		\
	gimpstrokeoptions.c		\
	gimpstrokeoptions.h		\
	gimpsubprogress.c		\
	gimpsubprogress.h		\
	gimptemplate.c			\
	gimptemplate.h			\
	gimptoolinfo.c			\
	gimptoolinfo.h			\
	gimptooloptions.c		\
	gimptooloptions.h		\
	gimptoolpresets.c		\
	gimptoolpresets.h		\
	gimpunit.c			\
	gimpunit.h			\
	gimpundo.c			\
	gimpundo.h			\
	gimpundostack.c			\
	gimpundostack.h			\
	gimpviewable.c			\
	gimpviewable.h

libappcore_a_built_sources = \
	core-enums.c	\
	gimpmarshal.c	\
	gimpmarshal.h

libappcore_a_extra_sources = \
	gimpmarshal.list

libappcore_a_SOURCES = $(libappcore_a_built_sources) $(libappcore_a_sources)

EXTRA_DIST = \
	makefile.msc			\
	$(libappcore_a_extra_sources)

#
# rules to generate built sources
#
# setup autogeneration dependencies
gen_sources = xgen-gmh xgen-gmc xgen-cec
CLEANFILES = $(gen_sources)

gimpmarshal.h: $(srcdir)/gimpmarshal.list
	$(GLIB_GENMARSHAL) --prefix=gimp_marshal $(srcdir)/gimpmarshal.list --header >> xgen-gmh \
	&& (cmp -s xgen-gmh $(@F) || cp xgen-gmh $(@F)) \
	&& rm -f xgen-gmh xgen-gmh~

gimpmarshal.c: gimpmarshal.h
	echo "#include \"gimpmarshal.h\"" >> xgen-gmc \
	&& $(GLIB_GENMARSHAL) --prefix=gimp_marshal $(srcdir)/gimpmarshal.list --body >> xgen-gmc \
	&& cp xgen-gmc $(@F) \
	&& rm -f xgen-gmc xgen-gmc~

core-enums.c: $(srcdir)/core-enums.h $(GIMP_MKENUMS)
	$(GIMP_MKENUMS) \
		--fhead "#include \"config.h\"\n#include <glib-object.h>\n#include \"libgimpbase/gimpbase.h\"\n#include \"core-enums.h\"\n#include \"gimp-intl.h\"" \
		--fprod "\n/* enumerations from \"@filename@\" */" \
		--vhead "GType\n@enum_name@_get_type (void)\n{\n  static const G@Type@Value values[] =\n  {" \
		--vprod "    { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
		--vtail "    { 0, NULL, NULL }\n  };\n" \
		--dhead "  static const Gimp@Type@Desc descs[] =\n  {" \
		--dprod "    { @VALUENAME@, @valuedesc@, @valuehelp@ }," \
		--dtail "    { 0, NULL, NULL }\n  };\n\n  static GType type = 0;\n\n  if (! type)\n    {\n      type = g_@type@_register_static (\"@EnumName@\", values);\n      gimp_@type@_set_value_descriptions (type, descs);\n    }\n\n  return type;\n}\n" \
		$(srcdir)/core-enums.h > xgen-cec \
	&& cp xgen-cec $(@F) \
	&& rm -f xgen-cec
