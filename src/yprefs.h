#ifndef __YPREFS_H
#define __YPREFS_H

#include "ylib.h"
#include "yconfig.h"

XIV(bool, dontRotateMenuPointer,                true)
XIV(bool, fontPreferFreetype,                   true)
XIV(bool, menuMouseTracking,                    false)
XIV(bool, replayMenuCancelClick,                false)
XIV(bool, showPopupsAbovePointer,               false)
XIV(bool, showEllipsis,                         true)
#ifdef CONFIG_I18N
XIV(bool, multiByte,                            true)
#endif
XIV(bool, modSuperIsCtrlAlt,                    true)
XIV(bool, doubleBuffer,                         true)
XIV(bool, xrrDisable,                           false)
XIV(int, xineramaPrimaryScreen,                 0)
XIV(int, MenuActivateDelay,                     40)
XIV(int, SubmenuActivateDelay,                  300)
XIV(int, DelayFuzziness,                        10)
XIV(int, ClickMotionDistance,                   4)
XIV(int, ClickMotionDelay,                      200)
XIV(int, MultiClickTime,                        400)
XIV(int, autoScrollStartDelay,                  500)
XIV(int, autoScrollDelay,                       60)
XIV(int, ToolTipDelay,                          500)
XIV(int, ToolTipTime,                           0)

///#warning "move this one back to WM"
XIV(bool, grabRootWindow,                       true)

// standard Debian paths and some extras
XSV(const char *, iconPath,                     "/usr/share/icons/hicolor:/usr/share/icons/oxygen:/usr/share/icons/oxygen/base:/usr/share/icons/gnome:/usr/share/icons/Adwaita:/usr/share/icons/Bluecurve:/usr/share/icons/HighContrast:/usr/share/icons/Humanity:/usr/share/icons/KDE_Classic:/usr/share/icons/Tango:/usr/share/icons/nuoveXT2:/usr/share/icons/locolor:/usr/share/icons:/usr/share/pixmaps")

XSV(const char *, themeName,                    CONFIG_DEFAULT_THEME)
XSV(const char *, xineramaPrimaryScreenName,    0)

enum WMLook {
    lookWin95  = 1 << 0,
    lookMotif  = 1 << 1,
    lookWarp3  = 1 << 2,
    lookWarp4  = 1 << 3,
    lookNice   = 1 << 4,
    lookPixmap = 1 << 5,
    lookMetal  = 1 << 6,
    lookGtk    = 1 << 7,
    lookFlat   = 1 << 8,
};
#define LOOK(looks)             (((looks) & wmLook) != 0)
#define CONFIG_DEFAULT_LOOK     lookNice
XIV(WMLook, wmLook,                             CONFIG_DEFAULT_LOOK)

XSV(const char *, clrToolTip,                   "rgb:E0/E0/00")
XSV(const char *, clrToolTipText,               "rgb:00/00/00")
XFV(const char *, toolTipFontName,              FONT(120), "sans-serif:size=11")

#endif

// vim: set sw=4 ts=4 et:
