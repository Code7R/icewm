cmake_minimum_required(VERSION 2.6)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

PROJECT(ICEWM CXX C)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR})

set(PREFIX "/usr/local" CACHE STRING "Target file space")
# iiieh, neeing to add some guards, since having this nested construct
# interacts badly when changing a variable afterwards cmake also insists on
# overwritting that variables later :-( if not protected
IF(NOT DATADIR)
set(DATADIR "${PREFIX}/share" CACHE STRING "Shared data files directory")
ENDIF()
IF(NOT LIBDIR)
set(LIBDIR "${DATADIR}/icewm")
ENDIF()
IF(NOT DOCDIR)
set(DOCDIR "${DATADIR}/doc/icewm")
ENDIF()
IF(NOT LOCDIR)
set(LOCDIR "${DATADIR}/locale")
ENDIF()
IF(NOT CFGDIR)
set(CFGDIR "${PREFIX}/etc/icewm")
ENDIF()
IF(NOT ICEHELPIDX)
set(ICEHELPIDX "${DOCDIR}/icewm.html")
ENDIF()


add_subdirectory(src)
#add_subdirectory(lib)
