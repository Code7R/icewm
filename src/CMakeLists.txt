cmake_minimum_required(VERSION 2.6)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

PROJECT(ICEWM CXX)

INCLUDE(CheckIncludeFiles) 
INCLUDE(CheckLibraryExists)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckTypeSize)
INCLUDE(CheckIncludeFileCXX)
INCLUDE(FindPkgConfig)
INCLUDE(CheckCXXSourceCompiles)

SET(CXXFLAGS_COMMON " -pthread -g -O2 -Wall -Wextra -Wno-unused-parameter -D_FILE_OFFSET_BITS=64 ${CMAKE_CXX_FLAGS}")
# same for linker flags, imported from LDFLAGS environment var
SET(LDFLAGS_COMMON "${CMAKE_EXE_LINKER_FLAGS}")
#message("Initial CXX flags: ${ACNG_CXXFLAGS_COMMON}")
#message("Initial linker flags: ${LDFLAGS_MIN}")

# apply to test calls
SET(CMAKE_REQUIRED_FLAGS ${CXXFLAGS_COMMON})
SET(CMAKE_EXE_LINKER_FLAGS ${LDFLAGS_COMMON})


include_directories(${PROJECT_SOURCE_DIR})

find_library(CONFIG_LIBICONV "iconv")
#option(NO_CONFIGURE_MENUES "Define to disable configurable menu support" off)
option(CONFIG_XPM "Legacy XPM image loader" off)
option(CONFIG_IMLIB "(Deprecated) IMLIB image loader" off)
option(CONFIG_GDK_PIXBUF_XLIB "LIBGDKPIXBUF based image loader" on)
option(CONFIG_I18N "Define to enable internationalization" on)
option(CONFIG_SESSION "Define to enable X session management" on)

CHECK_LIBRARY_EXISTS (X11 XInternAtoms "" HAVE_XINTERNATOMS)

set(PREFIX "/usr/local" CACHE STRING "Target file space")
set(DATADIR "${PREFIX}/share" CACHE STRING "Shared data files directory")
set(LIBDIR "${DATADIR}/icewm")
set(DOCDIR "${DATADIR}/doc/icewm")
set(LOCDIR "${DATADIR}/locale")
set(CFGDIR "${PREFIX}/etc/icewm")

#set(CONFIG_UNICODE_SET "" cache string "Your iconv unicode set in machine endian encoding (e.g. WCHAR_T, UCS-4-INTERNAL, UCS-4LE, UCS-4BE)")

# the only used ones...
# for x in `cat funclist` ; do grep $x src/* lib/* && echo $x >> exlist ; done 
# perl -e 'print "CHECK_FUNCTION_EXISTS($_ HAVE_".uc($_).")\n" for @ARGV' `cat exlist`
# perl -e 'print "#cmakedefine HAVE_".uc($_)."\n" for @ARGV' `cat exlist`
CHECK_FUNCTION_EXISTS(dup2 HAVE_DUP2)
CHECK_FUNCTION_EXISTS(getcwd HAVE_GETCWD)
CHECK_FUNCTION_EXISTS(gethostbyname HAVE_GETHOSTBYNAME)
CHECK_FUNCTION_EXISTS(gethostname HAVE_GETHOSTNAME)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_FUNCTION_EXISTS(mblen HAVE_MBLEN)
CHECK_FUNCTION_EXISTS(memchr HAVE_MEMCHR)
CHECK_FUNCTION_EXISTS(memmove HAVE_MEMMOVE)
CHECK_FUNCTION_EXISTS(memset HAVE_MEMSET)
CHECK_FUNCTION_EXISTS(mkdir HAVE_MKDIR)
CHECK_FUNCTION_EXISTS(nl_langinfo HAVE_NL_LANGINFO)
CHECK_FUNCTION_EXISTS(putenv HAVE_PUTENV)
CHECK_FUNCTION_EXISTS(select HAVE_SELECT)
CHECK_FUNCTION_EXISTS(setlocale HAVE_SETLOCALE)
CHECK_FUNCTION_EXISTS(socket HAVE_SOCKET)
CHECK_FUNCTION_EXISTS(strcasecmp HAVE_STRCASECMP)
CHECK_FUNCTION_EXISTS(strchr HAVE_STRCHR)
CHECK_FUNCTION_EXISTS(strcspn HAVE_STRCSPN)
CHECK_FUNCTION_EXISTS(strdup HAVE_STRDUP)
CHECK_FUNCTION_EXISTS(strerror HAVE_STRERROR)
CHECK_FUNCTION_EXISTS(strncasecmp HAVE_STRNCASECMP)
CHECK_FUNCTION_EXISTS(strrchr HAVE_STRRCHR)
CHECK_FUNCTION_EXISTS(strspn HAVE_STRSPN)
CHECK_FUNCTION_EXISTS(strstr HAVE_STRSTR)
CHECK_FUNCTION_EXISTS(strtol HAVE_STRTOL)
CHECK_FUNCTION_EXISTS(sysinfo HAVE_SYSINFO)
CHECK_FUNCTION_EXISTS(uname HAVE_UNAME)
CHECK_FUNCTION_EXISTS(abort HAVE_ABORT)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_FUNCTION_EXISTS(putenv HAVE_PUTENV)
CHECK_FUNCTION_EXISTS(select HAVE_SELECT)
CHECK_FUNCTION_EXISTS(socket HAVE_SOCKET)
CHECK_FUNCTION_EXISTS(strtol HAVE_STRTOL)
CHECK_FUNCTION_EXISTS(basename HAVE_BASENAME)
CHECK_FUNCTION_EXISTS(sysctlbyname HAVE_SYSCTLBYNAME)
CHECK_FUNCTION_EXISTS("getloadavg" HAVE_GETLOADAVG)

# perl -e 'do { s/\.|\//_/g; $_=uc $_;  print "#cmakedefine HAVE_$_\n"} for @ARGV' `cat headersDa`
# perl -e 'do {my $orig=$_; s/\.|\//_/g; $_=uc $_; print "CHECK_INCLUDE_FILE_CXX($orig HAVE_$_)\n"} for @ARGV' `cat headersDa`
CHECK_INCLUDE_FILE_CXX(fcntl.h HAVE_FCNTL_H)
CHECK_INCLUDE_FILE_CXX(langinfo.h HAVE_LANGINFO_H)
CHECK_INCLUDE_FILE_CXX(libintl.h HAVE_LIBINTL_H)
CHECK_INCLUDE_FILE_CXX(limits.h HAVE_LIMITS_H)
CHECK_INCLUDE_FILE_CXX(locale.h HAVE_LOCALE_H)
CHECK_INCLUDE_FILE_CXX(netdb.h HAVE_NETDB_H)
CHECK_INCLUDE_FILE_CXX(netinet/in.h HAVE_NETINET_IN_H)
CHECK_INCLUDE_FILE_CXX(stddef.h HAVE_STDDEF_H)
CHECK_INCLUDE_FILE_CXX(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILE_CXX(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILE_CXX(sys/file.h HAVE_SYS_FILE_H)
CHECK_INCLUDE_FILE_CXX(sys/ioctl.h HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILE_CXX(sys/param.h HAVE_SYS_PARAM_H)
CHECK_INCLUDE_FILE_CXX(sys/socket.h HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILE_CXX(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE_CXX(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILE_CXX(wchar.h HAVE_WCHAR_H)
CHECK_INCLUDE_FILE_CXX(sched.h HAVE_SCHED_H)
CHECK_INCLUDE_FILE_CXX(sys/dkstat.h HAVE_SYS_DKSTAT_H)
CHECK_INCLUDE_FILE_CXX(sys/param.h HAVE_SYS_PARAM_H)
CHECK_INCLUDE_FILE_CXX(sys/sysctl.h HAVE_SYS_SYSCTL_H)
CHECK_INCLUDE_FILE_CXX(uvm/uvm_param.h HAVE_UVM_UVM_PARAM_H)
CHECK_INCLUDE_FILE_CXX(libgen.h HAVE_LIBGEN_H)
CHECK_INCLUDE_FILE_CXX(machine/apmvar.h HAVE_MACHINE_APMVAR_H)
CHECK_INCLUDE_FILE_CXX(machine/apm_bios.h HAVE_MACHINE_APM_BIOS_H)
CHECK_INCLUDE_FILE_CXX(execinfo.h HAVE_EXECINFO_H)

CHECK_TYPE_SIZE(char SIZEOF_CHAR)
CHECK_TYPE_SIZE(long SIZEOF_LONG)
CHECK_TYPE_SIZE(int SIZEOF_INT)
CHECK_TYPE_SIZE(short SIZEOF_SHORT)

#########################################################
# fiting flags to options and available system features #
#########################################################

pkg_check_modules(fontconfig fontconfig)
string(REPLACE ";" " " fontconfig_LDFLAGS "${fontconfig_LDFLAGS}")
string(REPLACE ";" " " fontconfig_CFLAGS "${fontconfig_CFLAGS}")
pkg_check_modules(xext xext)
string(REPLACE ";" " " xext_LDFLAGS "${xext_LDFLAGS}")
string(REPLACE ";" " " xext_CFLAGS "${xext_CFLAGS}")
pkg_check_modules(x11 x11)
string(REPLACE ";" " " x11_LDFLAGS "${x11_LDFLAGS}")
string(REPLACE ";" " " x11_CFLAGS "${x11_CFLAGS}")
if(CONFIG_GDK_PIXBUF_XLIB)
pkg_check_modules(pixbuf gdk-pixbuf-xlib-2.0)
string(REPLACE ";" " " pixbuf_LDFLAGS "${pixbuf_LDFLAGS}")
string(REPLACE ";" " " pixbuf_CFLAGS "${pixbuf_CFLAGS}")
endif()

IF(CONFIG_SESSION)
   LIST(APPEND sm_LIBS SM)
   LIST(APPEND sm_LIBS ICE)
ENDIF()

option(CONFIG_SHAPE "Define to enable transparent frame	decoration (titlebar borders), requests X shape extension." on)
if(CONFIG_SHAPE AND NOT xext_FOUND)
   message(WARNING "Xext library not found, disabling CONFIG_SHAPE")
   set(CONFIG_SHAPE off)
endif()

option(CONFIG_ANTIALIASING "CONFIG_ANTIALIASING" on)
option(CONFIG_GRADIENTS "CONFIG_GRADIENTS" on)
if(CONFIG_GRADIENTS)
   set(CONFIG_ANTIALIASING on)
endif()

option(CONFIG_XRANDR "Define to enable XRANDR extension" on)
if(CONFIG_XRANDR)
pkg_check_modules(xrandr xrandr)
IF(NOT xrandr_FOUND)
   message(WARNING "Xrandr library not found, disabling CONFIG_XRANDR")
   set(CONFIG_XRANDR off)
ELSE()
   string(REPLACE ";" " " xrandr_LDFLAGS "${xrandr_LDFLAGS}")
   string(REPLACE ";" " " xrandr_CFLAGS "${xrandr_CFLAGS}")
ENDIF()
endif()

option(CONFIG_XFREETYPE "Define to enable XFT support" on)
if(CONFIG_XFREETYPE)
   set(CONFIG_XFREETYPE_VALUE 2)
   pkg_check_modules(xft xft>=2.0)
   IF(NOT xft_FOUND)
      set(CONFIG_XFREETYPE_VALUE 1)
      pkg_check_modules(xft xft)
   ENDIF()
   IF(NOT xft_FOUND)
      message(WARNING "xft library not found, disabling CONFIG_xft")
      set(CONFIG_XFREETYPE off)
   ELSE()
      string(REPLACE ";" " " xft_LDFLAGS "${xft_LDFLAGS}")
      string(REPLACE ";" " " xft_CFLAGS "${xft_CFLAGS}")
   ENDIF()
endif()

option(CONFIG_COREFONTS "Define to enable X11 core fonts" off)
option(CONFIG_PDA "Define to enable PDA operation (no window hiding)" off)




#######################################
# some extra sugar where available    #
#######################################


SET(CMAKE_REQUIRED_FLAGS ${CXXFLAGS_COMMON})
SET(CMAKE_EXE_LINKER_FLAGS "${LDFLAGS_COMMON}  -Wl,--as-needed ")
CHECK_CXX_SOURCE_COMPILES("int main() {return 0;}" USE_WLASNEEDED)
if(USE_WLASNEEDED)
   SET(LDFLAGS_COMMON ${CMAKE_EXE_LINKER_FLAGS})
else(USE_WLASNEEDED)
   SET(CMAKE_EXE_LINKER_FLAGS "${LDFLAGS_COMMON}")
endif(USE_WLASNEEDED)


#######################################
# all checks done, save configuration #
#######################################

CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/src/config.cmake.h.in" "${CMAKE_BINARY_DIR}/config.h")

SET(ICE_COMMON_SRCS mstring.cc upath.cc yapp.cc yxapp.cc ytimer.cc
 ywindow.cc ypaint.cc ypopup.cc misc.cc ycursor.cc ysocket.cc ypaths.cc
 ypixbuf.cc ylocale.cc yarray.cc ypipereader.cc yxembed.cc yconfig.cc
 yprefs.cc yfont.cc yfontcore.cc yfontxft.cc ypixmap.cc yimage.cc
 yimage_gdk.cc yimage_imlib.cc yimage_xpm.cc ytooltip.cc)

SET(ITK_SRCS ymenu.cc ylabel.cc yscrollview.cc ymenuitem.cc yscrollbar.cc ybutton.cc ylistbox.cc yinput.cc globit.c yicon.cc wmconfig.cc ref.cc
   ydialog.cc ymsgbox.cc    )

SET(ICEWM_SRCS ${ICE_COMMON_SRCS} ${ITK_SRCS}
   wmsession.cc wmwinlist.cc wmtaskbar.cc
 wmwinmenu.cc wmdialog.cc wmabout.cc wmswitch.cc wmstatus.cc
 wmoption.cc wmaction.cc wmcontainer.cc wmclient.cc wmmgr.cc
 wmapp.cc wmframe.cc wmbutton.cc wmminiicon.cc wmtitle.cc
 movesize.cc themes.cc decorate.cc browse.cc wmprog.cc atasks.cc
 aworkspaces.cc amailbox.cc aclock.cc acpustatus.cc apppstatus.cc
 aaddressbar.cc objbar.cc aapm.cc atray.cc ysmapp.cc yxtray.cc)

ADD_EXECUTABLE(icewm${EXEEXT} ${ICEWM_SRCS})
SET_TARGET_PROPERTIES(icewm${EXEEXT} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_COMMON} ${fontconfig_CFLAGS} ${x11_CFLAGS} ${xext_CFLAGS} ${pixbuf_CFLAGS} ${xft_CFLAGS} ${xrandr_CFLAGS}")
set(icewm_ld_extra ${lsd_LDFLAGS} ${pixbuf_LDFLAGS} ${fontconfig_LDFLAGS} ${xext_LDFLAGS} ${x11_LDFLAGS} ${xft_LDFLAGS} ${xrandr_LDFLAGS})
TARGET_LINK_LIBRARIES(icewm${EXEEXT} ${sm_LIBS} ${icewm_ld_extra})

#######################################
# print information and some warnings #
#######################################


# the compiler used for C++ files 
#MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} )
# if the compiler is a variant of gcc, this should be set to 1 
#MESSAGE( STATUS "CMAKE_COMPILER_IS_GNUCC: " ${CMAKE_COMPILER_IS_GNUCC} )
# if the compiler is a variant of g++, this should be set to 1 
#MESSAGE( STATUS "CMAKE_COMPILER_IS_GNUCXX : " ${CMAKE_COMPILER_IS_GNUCXX} )

#MESSAGE( STATUS "Target filespace: " ${PREFIX})
#MESSAGE( STATUS "DATADIR: " ${DATADIR})
#option(DATADIR "Shared data files directory" "${PREFIX}/share")
#option(LIBDIR "Package data directory" "${DATADIR}/icewm")

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
foreach (_variableName PREFIX DATADIR LIBDIR LOCDIR DOCDIR CFGDIR
      CONFIG_XPM
CONFIG_IMLIB 
CONFIG_GDK_PIXBUF_XLIB
      )
    message(STATUS "${_variableName} => ${${_variableName}}")
endforeach()

IF(CONFIG_SESSION)
   find_library(HAVE_LIBSM "SM")
   IF(NOT HAVE_LIBSM)
      message(WARNING "libSM.so not found, session management probably won't work")
   ENDIF()
   find_library(HAVE_LIBICE "ICE")
   IF(NOT HAVE_LIBICE)
      message(WARNING "libICE.so not found, session management probably won't work")
   ENDIF()
ENDIF()
