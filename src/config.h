/* src/config.h.  Generated from config.h.in by configure.  */
/* src/config.h.in.  Generated from configure.in by autoheader.  */

/* Address bar */
#define CONFIG_ADDRESSBAR 1

/* APM status applet */
#define CONFIG_APPLET_APM 1

/* LCD clock applet */
#define CONFIG_APPLET_CLOCK 1

/* CPU status applet */
#define CONFIG_APPLET_CPU_STATUS 1

/* Mailbox applet */
#define CONFIG_APPLET_MAILBOX 1

/* Network status applet */
#define CONFIG_APPLET_NET_STATUS 1

/* Define to enable X11 core conts. */
/* #undef CONFIG_COREFONTS */

/* Define to use gdk_pixbuf_xlib for image rendering */
#define CONFIG_GDK_PIXBUF_XLIB 1

/* Define to make IceWM more GNOME-friendly */
/* #undef CONFIG_GNOME_MENUS */

/* Define to enable gradient support. */
#define CONFIG_GRADIENTS 1

/* Define to enable GUI events support. */
/* #undef CONFIG_GUIEVENTS */

/* Define to enable internationalization */
#define CONFIG_I18N 1

/* Define when using libiconv */
/* #undef CONFIG_LIBICONV */

/* define how to query the current locale's codeset */
#define CONFIG_NL_CODESETS CODESET, _NL_CTYPE_CODESET_NAME, 0

/* Define to support the X session managment protocol */
#define CONFIG_SESSION 1

/* Define to enable X shape extension */
#define CONFIG_SHAPE 1

/* Define to allow transparent frame borders. */
#define CONFIG_SHAPED_DECORATION 1

/* Taskbar */
#define CONFIG_TASKBAR 1

/* Tooltips */
#define CONFIG_TOOLTIP 1

/* Window tray */
#define CONFIG_TRAY 1

/* preferred Unicode set */
/* #undef CONFIG_UNICODE_SET */

/* OS/2 like window list */
#define CONFIG_WINLIST 1

/* Window menu */
#define CONFIG_WINMENU 1

/* Define to enable x86 assembly code. */
/* #undef CONFIG_X86_ASM */

/* Define to enable XFreeType support. */
#define CONFIG_XFREETYPE 2

/* Define to enable XRANDR extension */
#define CONFIG_XRANDR 1

/* Define if you want to debug IceWM */
/* #undef DEBUG */

/* Define to enable ESD support. */
/* #undef ENABLE_ESD */

/* Define to enable internationalized message */
#define ENABLE_NLS 1

/* Define to enable OSS support. */
/* #undef ENABLE_OSS */

/* Define to enable YIFF support. */
/* #undef ENABLE_YIFF */

/* GNOME1 hints */
#define GNOME1_HINTS 1

/* Define to 1 if you have the `basename' function. */
#define HAVE_BASENAME 1

/* define if nl_langinfo supports CODESET */
/* #undef HAVE_CODESET */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define to 1 if you have the <esd.h> header file. */
/* #undef HAVE_ESD_H */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* getloadavg() is available */
#define HAVE_GETLOADAVG2 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the `iconv' function. */
#define HAVE_ICONV 1

/* Define to 1 if you have the `iconv_close' function. */
#define HAVE_ICONV_CLOSE 1

/* Define to 1 if you have the <iconv.h> header file. */
#define HAVE_ICONV_H 1

/* Define to 1 if you have the `iconv_open' function. */
#define HAVE_ICONV_OPEN 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <kstat.h> header file. */
/* #undef HAVE_KSTAT_H */

/* Define to 1 if you have the <langinfo.h> header file. */
#define HAVE_LANGINFO_H 1

/* Define to 1 if you have the `esd' library (-lesd). */
/* #undef HAVE_LIBESD */

/* Define to 1 if you have the <libgen.h> header file. */
#define HAVE_LIBGEN_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <linux/tasks.h> header file. */
/* #undef HAVE_LINUX_TASKS_H */

/* Define to 1 if you have the <linux/threads.h> header file. */
/* #undef HAVE_LINUX_THREADS_H */

/* Define to 1 if you have the <machine/apmvar.h> header file. */
/* #undef HAVE_MACHINE_APMVAR_H */

/* Define to 1 if you have the <machine/apm_bios.h> header file. */
/* #undef HAVE_MACHINE_APM_BIOS_H */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* define if have old kstat (Solaris only?) */
/* #undef HAVE_OLD_KSTAT */

/* Define to 1 if you have the `putenv' function. */
#define HAVE_PUTENV 1

/* Define to 1 if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strftime' function. */
#define HAVE_STRFTIME 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtol' function. */
#define HAVE_STRTOL 1

/* Define to 1 if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if you have the `sysctlbyname' function. */
/* #undef HAVE_SYSCTLBYNAME */

/* kern.cp_time MIB item is available */
/* #undef HAVE_SYSCTL_CP_TIME */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/dkstat.h> header file. */
/* #undef HAVE_SYS_DKSTAT_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/sysctl.h> header file. */
#define HAVE_SYS_SYSCTL_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <uvm/uvm_param.h> header file. */
/* #undef HAVE_UVM_UVM_PARAM_H */

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to 1 if you have the <X11/Xft/Xft.h> header file. */
/* #undef HAVE_X11_XFT_XFT_H */

/* Define to enable XInternAtoms */
#define HAVE_XINTERNATOMS 1

/* Define to 1 if you have the <Y2/Y.h> header file. */
/* #undef HAVE_Y2_Y_H */

/* define if nl_langinfo supports _NL_CTYPE_CODESET_NAME */
/* #undef HAVE__NL_CTYPE_CODESET_NAME */

/* Lite version */
/* #undef LITE */

/* Define to disable preferences support. */
/* #undef NO_CONFIGURE */

/* Define to disable configurable menu support. */
/* #undef NO_CONFIGURE_MENUS */

/* Define to disable keybinding support. */
/* #undef NO_KEYBIND */

/* Define to disable configurable window options support. */
/* #undef NO_WINDOW_OPTIONS */

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define to the type of arg 1 for `select'. */
#define SELECT_TYPE_ARG1 int

/* Define to the type of args 2, 3 and 4 for `select'. */
#define SELECT_TYPE_ARG234 (fd_set *)

/* Define to the type of arg 5 for `select'. */
#define SELECT_TYPE_ARG5 (struct timeval *)

/* The size of `char', as computed by sizeof. */
#define SIZEOF_CHAR 1

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* wmspec hints */
#define WMSPEC_HINTS 1

/* Define to enable Xinerama support */
#define XINERAMA 1

/* Define to 1 if the X Window System is missing or not being used. */
/* #undef X_DISPLAY_MISSING */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */
