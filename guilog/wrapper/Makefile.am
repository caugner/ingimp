## Makefile.am for gimp/guilog/wrapper

SUBDIRS = images

wrapperdir = $(libdir)/ingimp

wrapper_SCRIPTS = \
	ingimp \
	About.py \
	Config.py \
	ConsentDialog.py \
	FirstTimeHandler.py \
	ingimp-helper.py \
	Launcher.py \
	Main.py	\
	Participation.py \
	Pictoviewer.py \
	SplashScreen.py \
	Uploader.py \
	UploadDialog.py	\
	UserPrefs.py \
	Utils.py

EXTRA_DIST = $(wrapper_SCRIPTS)

edit = sed \
	-e 's|@datadir[@]|$(datadir)|g' \
	-e 's|@bindir[@]|$(bindir)|g' \
	-e 's|@libdir[@]|$(libdir)|g'

ingimp UploadDialog.py ConsentDialog.py: Makefile
	rm -f $@ $@.tmp
	$(edit) '$(srcdir)/$@.in' >$@.tmp
	chmod +x $@.tmp
	chmod a-w $@.tmp
	mv $@.tmp $@

# For the following, we don't need to add $(EXEEXT) to the LN_S ingimp line
# because building the executable is currently a manual process on Windows
install-data-hook:
	rm -f $(DESTDIR)$(bindir)/ingimp$(EXEEXT)
	rm -f $(DESTDIR)$(bindir)/ingimp-helper$(EXEEXT)
	$(LN_S) $(libdir)/ingimp/ingimp $(DESTDIR)$(bindir)/ingimp$(EXEEXT)
	$(LN_S) $(libdir)/ingimp/ingimp-helper.py $(DESTDIR)$(bindir)/ingimp-helper$(EXEEXT)
