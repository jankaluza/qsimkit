cmake_minimum_required(VERSION 2.6)
project(qsimkit)

if(LIB_INSTALL_DIR)
else()
	if(WIN32)
		set(LIB_INSTALL_DIR bin)
	else()
		set(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)
	endif()
endif()

if(SHARE_INSTALL_DIR)
else()
	if(WIN32)
		set(SHARE_INSTALL_DIR bin)
	else()
		set(SHARE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share)
	endif()
endif()

set(cppunit_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(cppunit)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXml REQUIRED)

set(Python_ADDITIONAL_VERSIONS 2.7 2.6 2.5 2.4 2.3 2.2 2.1 2.0)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})

# Find PythonQt
set(PythonQt_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(PythonQt QUIET)


if (PYTHONQT_INCLUDE_PATH AND PYTHONQT_LIBRARIES)
	include_directories(${PYTHONQT_INCLUDE_PATH})
	message(STATUS "Using PythonQt: ${PYTHONQT_LIBRARIES} ${PYTHONQT_INCLUDE_PATH}")
else()
	if (QT_VERSION_MINOR GREATER 7)
		message(STATUS "Using bundled PythonQt for Qt-4.8")
		ADD_SUBDIRECTORY(3rdparty/pythonqt21-qt48)
		include_directories(3rdparty/pythonqt21-qt48/)
	else()
		message(STATUS "Using bundled PythonQt for Qt-4.6.2")
		ADD_SUBDIRECTORY(3rdparty/pythonqt21-qt462)
		include_directories(3rdparty/pythonqt21-qt462)
	endif()
	set(PYTHONQT_LIBRARIES PythonQt)
endif()

if(NOT CPPUNIT_FOUND AND CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARY)
	set(CCPUNIT_LIBRARIES ${CPPUNIT_LIBRARY})
	set(CPPUNIT_FOUND 1)
	message(STATUS "Using cppunit: ${CPPUNIT_INCLUDE_DIR} ${CPPUNIT_LIBRARIES}")
endif()

if (CPPUNIT_FOUND)
    message(STATUS "Using cppunit:: ${CPPUNIT_INCLUDE_DIR} ${CPPUNIT_LIBRARIES}")
    include_directories(${CPPUNIT_INCLUDE_DIR})
endif()

ADD_DEFINITIONS(-DMODULES_DIR="${LIB_INSTALL_DIR}/qsimkit")
ADD_DEFINITIONS(-DPACKAGES="${SHARE_INSTALL_DIR}/qsimkit")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/adevs-2.6/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_COMPILER_IS_GNUCXX)
	ADD_DEFINITIONS(-O0)
	ADD_DEFINITIONS(-ggdb)
endif()

ADD_SUBDIRECTORY(QSimKit)

if (CPPUNIT_FOUND)
	ADD_SUBDIRECTORY(Tests)
endif()


# CPACK

set(CPACK_PACKAGE_NAME "QSimKit")
set(CPACK_PACKAGE_VENDOR "qsimkit.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QSimKit - MSP430 Simulator")
set(CPACK_PACKAGE_VERSION "0.0.2")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "2")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "QSimKit")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

set(CPACK_COMPONENTS_ALL qsimkit msp430 button lcd led oscillator sd)

set(CPACK_COMPONENT_QSIMKIT_DISPLAY_NAME "QSimKit")
set(CPACK_COMPONENT_QSIMKIT_REQUIRED 1)
set(CPACK_COMPONENT_MSP430_DISPLAY_NAME "MSP430")
set(CPACK_COMPONENT_BUTTON_DISPLAY_NAME "Button")
set(CPACK_COMPONENT_LCD_DISPLAY_NAME "LCD")
set(CPACK_COMPONENT_LED_DISPLAY_NAME "LED")
set(CPACK_COMPONENT_OSCILLATOR_DISPLAY_NAME "Crystal Oscillator")
set(CPACK_COMPONENT_SD_DISPLAY_NAME "SD Card")

set(CPACK_COMPONENT_MSP430_GROUP "MCUs")
set(CPACK_COMPONENT_BUTTON_GROUP "Peripherals")
set(CPACK_COMPONENT_LCD_GROUP "Peripherals")
set(CPACK_COMPONENT_LED_GROUP "Peripherals")
set(CPACK_COMPONENT_OSCILLATOR_GROUP "Peripherals")
set(CPACK_COMPONENT_SD_GROUP "Peripherals")

SET(CPACK_NSIS_CREATE_ICONS "
CreateShortCut '\$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\QSimKit.lnk' '$INSTDIR\\\\bin\\\\qsimkit.exe'
"
)
SET(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '\$DESKTOP\\\\QSimKit.lnk' '$INSTDIR\\\\bin\\\\qsimkit.exe'")
SET(CPACK_NSIS_DELETE_ICONS "
Delete '\$SMPROGRAMS\\\\$MUI_TEMP\\\\QSimKit.lnk'
")
SET(CPACK_NSIS_DELETE_ICONS_EXTRA "Delete '\$DESKTOP\\\\QSimKit.lnk'")

# This must always be last!
include(CPack)



