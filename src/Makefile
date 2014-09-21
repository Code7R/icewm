VERSION = 1.3.7
HOSTOS  = Linux 2.6.34.7-61.fc13.x86_64
HOSTCPU = x86_64

LIBDIR = /usr/share/icewm
CFGDIR = /etc/icewm
LOCDIR = /usr/share/locale
DOCDIR = /usr/share/doc

################################################################################

CXX =           g++
HOSTCXX =       g++
LD =            g++
HOSTLD =        g++
EXEEXT =        

DEBUG =         
GCCDEP =        
DEFS =          -DHAVE_CONFIG_H \
                -DLIBDIR='"$(LIBDIR)"' \
                -DCFGDIR='"$(CFGDIR)"' \
                -DLOCDIR='"$(LOCDIR)"' \
                -DKDEDIR='"$(KDEDIR)"' \
                -DPACKAGE='"icewm"' \
                -DVERSION='"$(VERSION)"' \
                -DHOSTOS='"$(HOSTOS)"' \
                -DHOSTCPU='"$(HOSTCPU)"' \
                -DEXEEXT='"$(EXEEXT)"' \
                -DICEWMEXE='"icewm$(EXEEXT)"' \
                -DICEWMTRAYEXE='"icewmtray$(EXEEXT)"' \
                -DICEWMBGEXE='"icewmbg$(EXEEXT)"' \
                -DICESMEXE='"icewm-session$(EXEEXT)"' \
                -DICEHELPEXE='"icehelp$(EXEEXT)"' \
                -DICEHELPIDX='"$(DOCDIR)/icewm-$(VERSION)/icewm.html"'

CXXFLAGS =      -fpermissive -Wall -Wpointer-arith -Wwrite-strings -Woverloaded-virtual -W -fno-exceptions -fno-rtti -g -O2 $(DEBUG) $(DEFS) `pkg-config gdk-pixbuf-xlib-2.0 --cflags` \
	         -I/usr/include/freetype2   -pthread -I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include    # `fc-config --cflags`
LFLAGS =	 
LIBS =           `pkg-config gdk-pixbuf-xlib-2.0 --libs` 

CORE_LIBS =     -lXinerama  -lSM -lICE  -lX11   -lXrandr -lXrender -lXext -lXft -lXrender -lfontconfig -lfreetype -lX11   # `fc-config --libs`
IMAGE_LIBS =    -pthread -lgdk_pixbuf_xlib-2.0 -lgdk_pixbuf-2.0 -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lrt -lglib-2.0  
AUDIO_LIBS =    
GNOME1_LIBS =    
GNOME2_LIBS =    

################################################################################

libice_OBJS = ref.o \
        mstring.o \
	upath.o \
        yapp.o yxapp.o ytimer.o ywindow.o ypaint.o ypopup.o \
        yworker.o \
        misc.o ycursor.o ysocket.o \
        ypaths.o ypixbuf.o ylocale.o yarray.o ypipereader.o \
        yxembed.o yconfig.o yprefs.o \
        yfont.o yfontcore.o yfontxft.o \
        ypixmap.o \
	yimage.o \
        yimage_gdk.o yimage_imlib.o yimage_xpm.o \
        ytooltip.o # FIXME
        
libitk_OBJS = \
	ymenu.o ylabel.o yscrollview.o \
        ymenuitem.o yscrollbar.o ybutton.o ylistbox.o yinput.o \
        yicon.o \
        wmconfig.o # FIXME

genpref_OBJS = \
	genpref.o

icewm_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icewm_OBJS = \
        ymsgbox.o ydialog.o yurl.o \
        wmsession.o wmwinlist.o wmtaskbar.o wmwinmenu.o \
        wmdialog.o wmabout.o wmswitch.o wmstatus.o \
        wmoption.o wmaction.o \
        wmcontainer.o wmclient.o \
        wmmgr.o wmapp.o \
        wmframe.o wmbutton.o wmminiicon.o wmtitle.o movesize.o \
        themes.o decorate.o browse.o \
        wmprog.o \
        atasks.o aworkspaces.o amailbox.o aclock.o acpustatus.o \
	apppstatus.o aaddressbar.o objbar.o aapm.o atray.o ysmapp.o \
        yxtray.o \
        $(libitk_OBJS) $(libice_OBJS)

icesh_LIBS = \
	$(CORE_LIBS)
icesh_OBJS = \
	icesh.o misc.o

icewm-session_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icewm-session_OBJS = \
	icesm.o $(libice_OBJS)

icewmhint_LIBS = \
	$(CORE_LIBS)
icewmhint_OBJS = \
	icewmhint.o

icewmbg_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icewmbg_OBJS = \
	icewmbg.o $(libice_OBJS)

icesound_LIBS = \
	$(CORE_LIBS) $(AUDIO_LIBS)
icesound_OBJS = \
	icesound.o misc.o ycmdline.o

icewm-menu-gnome1_LIBS = \
	$(CORE_LIBS) $(GNOME1_LIBS)
icewm-menu-gnome1_OBJS = \
	gnome.o misc.o ycmdline.o yarray.o

icewm-menu-gnome2_LIBS = \
	$(CORE_LIBS) $(GNOME2_LIBS)
icewm-menu-gnome2_OBJS = \
	gnome2.o misc.o ycmdline.o yarray.o

icehelp_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icehelp_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) icehelp.o

iceclock_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) iceclock.o aclock.o
iceclock_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icebar_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) \
        wmtaskbar.o \
        wmprog.o browse.o themes.o wmaction.o \
	amailbox.o aclock.o acpustatus.o apppstatus.o aaddressbar.o objbar.o
icewmtray_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) yxtray.o icetray.o
icewmtray_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icesame_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) icesame.o
icesame_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icelist_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) icelist.o
icelist_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
iceview_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) iceview.o
iceview_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
iceicon_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) iceicon.o
iceicon_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icerun_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) icerun.o
icerun_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
iceskt_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) iceskt.o
testmenus_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS) 
testmenus_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) testmenus.o wmprog.o wmaction.o themes.o browse.o
testwinhints_OBJS= \
        testwinhints.o
testwinhints_LIBS = \
        $(CORE_LIBS) $(IMAGE_LIBS)

testnetwmhints_OBJS= \
        testnetwmhints.o
testnetwmhints_LIBS = \
        $(CORE_LIBS) $(IMAGE_LIBS)

testmap_OBJS = \
        testmap.o
testmap_LIBS = \
        $(CORE_LIBS) $(IMAGE_LIBS)

testlocale_OBJS = \
	testlocale.o ylocale.o misc.o
testarray_OBJS = \
	testarray.o yarray.o misc.o

################################################################################

APPLICATIONS = icewm icewm-session icesh icewmhint icewmbg icewmtray icehelp
TESTCASES = testarray testlocale testmap testmenus testnetwmhints testwinhints iceview icesame iceicon icerun icelist
OBJECTS =  $(icewm_OBJS) $(icewm-session_OBJS) $(icesh_OBJS) $(icewmhint_OBJS) $(icewmbg_OBJS) $(icewmtray_OBJS) $(icehelp_OBJS)  $(testarray_OBJS) $(testlocale_OBJS) $(testmap_OBJS) $(testmenus_OBJS) $(testnetwmhints_OBJS) $(testwinhints_OBJS) $(iceview_OBJS) $(icesame_OBJS) $(iceicon_OBJS) $(icerun_OBJS) $(icelist_OBJS)
BINARIES =  icewm$(EXEEXT) icewm-session$(EXEEXT) icesh$(EXEEXT) icewmhint$(EXEEXT) icewmbg$(EXEEXT) icewmtray$(EXEEXT) icehelp$(EXEEXT)  testarray$(EXEEXT) testlocale$(EXEEXT) testmap$(EXEEXT) testmenus$(EXEEXT) testnetwmhints$(EXEEXT) testwinhints$(EXEEXT) iceview$(EXEEXT) icesame$(EXEEXT) iceicon$(EXEEXT) icerun$(EXEEXT) icelist$(EXEEXT)

################################################################################

all:	base
base:	 icewm$(EXEEXT) icewm-session$(EXEEXT) icesh$(EXEEXT) icewmhint$(EXEEXT) icewmbg$(EXEEXT) icewmtray$(EXEEXT) icehelp$(EXEEXT) genpref$(EXEEXT) ../lib/preferences
tests:	 testarray$(EXEEXT) testlocale$(EXEEXT) testmap$(EXEEXT) testmenus$(EXEEXT) testnetwmhints$(EXEEXT) testwinhints$(EXEEXT) iceview$(EXEEXT) icesame$(EXEEXT) iceicon$(EXEEXT) icerun$(EXEEXT) icelist$(EXEEXT)
clean:
	rm -f $(BINARIES) genpref$(EXEEXT) *.o *.d *~

.PHONY: all base tests clean

################################################################################

%.o: %.cc
	@echo "  CXX     " $@
	@$(CXX) $(CXXFLAGS) $(GCCDEP) -c $<

$(BINARIES):
	@echo "  LD      " $@
	@$(LD) -o $@ $($(@:$(EXEEXT)=)_OBJS) $(LFLAGS) $($(@:$(EXEEXT)=)_LFLAGS) $(LIBS) $($(@:$(EXEEXT)=)_LIBS)

genpref.o: genpref.cc
	@echo "  HOSTCXX " $@
	@$(HOSTCXX) $(CXXFLAGS) $(GCCDEP) -c $<
	
genpref$(EXEEXT):
	@echo "  HOSTLD  " $@
	@$(HOSTLD) -o $@ $(genpref_OBJS)

################################################################################

gnome.o: gnome.cc
	@echo "  CXX     " $@
	@$(CXX) $(CXXFLAGS)   $(GCCDEP) -c $<

gnome2.o: gnome2.cc
	@echo "  CXX     " $@
	@$(CXX) $(CXXFLAGS)   $(GCCDEP) -c $<

################################################################################

#libice.so: $(libice_OBJS)
#	-@rm -f $@
#	ld -shared -o $@ $(libice_OBJS)

wmabout.o: ../VERSION

../lib/preferences: genpref$(EXEEXT)
	@echo "  GENPREF " $@
	@./genpref$(EXEEXT) >../lib/preferences

genpref$(EXEEXT): $(genpref_OBJS)

################################################################################

check:	all tests
	./icewm$(EXEEXT) --help >/dev/null
	./testarray$(EXEEXT)
	./testlocale$(EXEEXT)

################################################################################
#END

icewm$(EXEEXT): $(icewm_OBJS)
icewm-session$(EXEEXT): $(icewm-session_OBJS)
icesh$(EXEEXT): $(icesh_OBJS)
icewmhint$(EXEEXT): $(icewmhint_OBJS)
icewmbg$(EXEEXT): $(icewmbg_OBJS)
icewmtray$(EXEEXT): $(icewmtray_OBJS)
icehelp$(EXEEXT): $(icehelp_OBJS)
testarray$(EXEEXT): $(testarray_OBJS)
testlocale$(EXEEXT): $(testlocale_OBJS)
testmap$(EXEEXT): $(testmap_OBJS)
testmenus$(EXEEXT): $(testmenus_OBJS)
testnetwmhints$(EXEEXT): $(testnetwmhints_OBJS)
testwinhints$(EXEEXT): $(testwinhints_OBJS)
iceview$(EXEEXT): $(iceview_OBJS)
icesame$(EXEEXT): $(icesame_OBJS)
iceicon$(EXEEXT): $(iceicon_OBJS)
icerun$(EXEEXT): $(icerun_OBJS)
icelist$(EXEEXT): $(icelist_OBJS)
