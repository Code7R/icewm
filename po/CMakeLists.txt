cmake_minimum_required(VERSION 2.6)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

PROJECT(ICEWM CXX)

# not handy... :-(
#INCLUDE(FindGettext) 

# source files, read-only!
FILE(GLOB SRCS "${CMAKE_SOURCE_DIR}/src/*.cc")
foreach(_src ${SRCS})
	get_filename_component(_src "${_src}" NAME)
	LIST(APPEND SRCSRELATIVE "src/${_src}")
endforeach()
FILE(GLOB POS "${CMAKE_CURRENT_SOURCE_DIR}/*.po")
foreach(_src ${POS})
	get_filename_component(_src "${_src}" NAME)
	string(REPLACE ${PACKAGE} "" _src ${_src})
	string(REPLACE ".po" "" _src ${_src})
	LIST(APPEND LANGUAGES ${_src})
endforeach()
message("langs: ${LANGUAGES}")
SET(XGETTEXT_CMD xgettext -language=CXX --keyword=_ --keyword=N_  -s --package-name=${PACKAGE} --package-version=${VERSION})
SET(_potFile "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE}.pot")

ADD_CUSTOM_COMMAND(OUTPUT ${_potFile}
	COMMAND ${XGETTEXT_CMD} -o "${_potFile}" ${SRCSRELATIVE}
	DEPENDS ${SRCS}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "Extract translatable messages to ${_potFile}")
ADD_CUSTOM_TARGET(update_pot DEPENDS ${_potFile})


ADD_CUSTOM_TARGET(translations)

foreach(_lang ${LANGUAGES})
	SET(_tempPO "${CMAKE_CURRENT_BINARY_DIR}/${_lang}.po")
	ADD_CUSTOM_COMMAND(OUTPUT ${_tempPO}
	COMMAND msgmerge -o ${_tempPO} -v -s ${CMAKE_CURRENT_SOURCE_DIR}/${_lang}.po ${_potFile}
	DEPENDS "${_potFile}"
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "Updated: ${_tempPO}")
ADD_CUSTOM_TARGET(${_lang}.po DEPENDS ${_tempPO})

	SET(_tempPO "${CMAKE_CURRENT_BINARY_DIR}/${_lang}.po")
SET(_tempMO "${CMAKE_CURRENT_BINARY_DIR}/${_lang}.mo")
ADD_CUSTOM_COMMAND(OUTPUT ${_tempMO}
	COMMAND msgfmt -o ${_tempMO} -v ${_tempPO}
	DEPENDS ${_tempPO}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	COMMENT "Compiled: ${_tempPO}")
ADD_CUSTOM_TARGET(${_lang}.mo DEPENDS ${_tempMO})
install(FILES ${_tempMO} DESTINATION ${LOCDIR}/${_lang}/LC_MESSAGES/)
add_dependencies(translations  ${_lang}.mo)
endforeach()
