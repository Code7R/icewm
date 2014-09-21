srcdir =        .
top_srcdir =    ..

prefix  =       /usr
datadir =       ${prefix}/share

PACKAGE =	icewm
LOCDIR =        /usr/share/locale

DESTDIR = 

SOURCES =       $(top_srcdir)/src/aapm.cc $(top_srcdir)/src/acpustatus.cc $(top_srcdir)/src/amailbox.cc $(top_srcdir)/src/apppstatus.cc $(top_srcdir)/src/aworkspaces.cc $(top_srcdir)/src/icehelp.cc $(top_srcdir)/src/icelist.cc $(top_srcdir)/src/icesame.cc $(top_srcdir)/src/icesh.cc $(top_srcdir)/src/iceskt.cc $(top_srcdir)/src/icesound.cc $(top_srcdir)/src/iceview.cc $(top_srcdir)/src/icewmbg.cc $(top_srcdir)/src/icewmhint.cc $(top_srcdir)/src/misc.cc $(top_srcdir)/src/movesize.cc $(top_srcdir)/src/testnetwmhints.cc $(top_srcdir)/src/testwinhints.cc $(top_srcdir)/src/themes.cc $(top_srcdir)/src/wmabout.cc $(top_srcdir)/src/wmapp.cc $(top_srcdir)/src/wmconfig.cc $(top_srcdir)/src/wmdialog.cc $(top_srcdir)/src/wmframe.cc $(top_srcdir)/src/wmoption.cc $(top_srcdir)/src/wmprog.cc $(top_srcdir)/src/wmsession.cc $(top_srcdir)/src/wmstatus.cc $(top_srcdir)/src/wmtaskbar.cc $(top_srcdir)/src/wmwinlist.cc $(top_srcdir)/src/wmwinmenu.cc $(top_srcdir)/src/yapp.cc $(top_srcdir)/src/ybutton.cc $(top_srcdir)/src/ycmdline.cc $(top_srcdir)/src/yconfig.cc $(top_srcdir)/src/ycursor.cc $(top_srcdir)/src/yfontcore.cc $(top_srcdir)/src/yfontxft.cc $(top_srcdir)/src/yicon.cc $(top_srcdir)/src/yimage.cc $(top_srcdir)/src/yinput.cc $(top_srcdir)/src/ylocale.cc $(top_srcdir)/src/ymenuitem.cc $(top_srcdir)/src/ymsgbox.cc $(top_srcdir)/src/ypaths.cc $(top_srcdir)/src/ypixbuf.cc $(top_srcdir)/src/ysmapp.cc $(top_srcdir)/src/yurl.cc $(top_srcdir)/src/yxapp.cc
POFILES =	be.po bg.po ca.po cs.po da.po de.po el.po en.po es.po fi.po fr.po hr.po hu.po id.po it.po ja.po ko.po lt.po lv.po mk.po nb.po nl.po pl.po pt_BR.po ro.po ru.po sk.po sl.po sv.po tr.po uk.po vi.po zh_CN.po zh_TW.po
POXFILES =	be.pox bg.pox ca.pox cs.pox da.pox de.pox el.pox en.pox es.pox fi.pox fr.pox hr.pox hu.pox id.pox it.pox ja.pox ko.pox lt.pox lv.pox mk.pox nb.pox nl.pox pl.pox pt_BR.pox ro.pox ru.pox sk.pox sl.pox sv.pox tr.pox uk.pox vi.pox zh_CN.pox zh_TW.pox
MOFILES =	be.mo bg.mo ca.mo cs.mo da.mo de.mo el.mo en.mo es.mo fi.mo fr.mo hr.mo hu.mo id.mo it.mo ja.mo ko.mo lt.mo lv.mo mk.mo nb.mo nl.mo pl.mo pt_BR.mo ro.mo ru.mo sk.mo sl.mo sv.mo tr.mo uk.mo vi.mo zh_CN.mo zh_TW.mo

INSTALL =       /usr/bin/install -c
INSTALLDIR =    /usr/bin/install -c -m 755 -d
INSTALLLIB =    ${INSTALL} -m 644
XGETTEXT =	/usr/bin/xgettext
MSGMERGE =	/usr/bin/msgmerge
MSGFMT =	/usr/bin/msgfmt

.SUFFIXES:
.SUFFIXES: .po .mo

all: $(MOFILES)

install: all
	@echo "Installing message catalogues in $(DESTDIR)$(LOCDIR)"
	@for catalog in $(MOFILES); do \
	     lang=`echo $${catalog} | sed -e 's/\.mo//'` ; \
	     msgdir="$(DESTDIR)$(LOCDIR)/$${lang}/LC_MESSAGES"; \
	     echo "Installing language: $${lang}" ; \
	     $(INSTALLDIR) "$${msgdir}"; \
	     $(INSTALLLIB) "$${catalog}" "$${msgdir}/$(PACKAGE).mo"; \
	done

clean:
	rm -f  $(MOFILES) *~


# Merge existing translations and new code
merge: $(POXFILES)

# POTFILES.in lists files containing translatable strings
POTFILES.in: $(SOURCES)
	echo $(SOURCES) | tr ' ' '\n' > $@

# $(PACKAGE).pot is a template file for translations
$(PACKAGE).pot: POTFILES.in
	$(XGETTEXT) --default-domain=$(PACKAGE) --directory=../src \
	--add-comments --keyword=_ --keyword=N_ --files-from=POTFILES.in && \
	test ! -f $(PACKAGE).po || \
	( rm -f ./$(PACKAGE).pot && \
	  mv $(PACKAGE).po ./$(PACKAGE).pot )

# create new translations
%.pox: %.po $(PACKAGE).pot
	$(MSGMERGE) --no-location --output-file $@ $< $(PACKAGE).pot

# convert portable into machine objects
.po.mo:
	$(MSGFMT) -o $@ $<


report.html: *.po Makefile
	@(echo "<h1>National Language Support Status Report</h1>"; \
	  date; echo "<p>"; \
	  for catalog in *.po; do \
	    echo -n "<li>$${catalog}"; \
	    sed -ne's|^.*"Last-Translator[^:]*:\(.*\)<.*$$| by\1<ul>|p' \
                "$${catalog}"; \
		echo -n ' <li>'; \
		LC_ALL=en $(MSGFMT) -o /dev/null "$${catalog}" 2>&1 |\
		sed -e's|, |<li>|g' -e's|\.$$||'; \
		echo '</ul>'; \
	  done ) >$@
	@cat $@

#upload-report: report.html
#	scp report.html massel@icewm.sf.net:icesf/libphp/nls.html

stats:
	for x in *.po ; do echo -n "$$x: " ; $(MSGFMT) --statistics $$x 2>&1 ; done

update: merge
	for x in *.pox ; do cp -af $$x $${x%%pox}po ; done
