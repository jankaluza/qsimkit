cmake_minimum_required(VERSION 2.6)
project(qsimkit)

if(LIB_INSTALL_DIR)
else()
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif()

set(cppunit_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(cppunit)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXml REQUIRED)

set(Python_ADDITIONAL_VERSIONS 2.7 2.6 2.5 2.4 2.3 2.2 2.1 2.0)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})

# Find PythonQt
set(PythonQt_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(PythonQt)


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

ADD_DEFINITIONS(-DMODULES_DIR="${LIB_INSTALL_DIR}/qsimkit")
ADD_DEFINITIONS(-DPACKAGES="${CMAKE_INSTALL_PREFIX}/share/qsimkit")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/adevs-2.6/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_COMPILER_IS_GNUCXX)
	ADD_DEFINITIONS(-O0)
	ADD_DEFINITIONS(-ggdb)
endif()

ADD_SUBDIRECTORY(GUI)

if (CPPUNIT_FOUND)
	ADD_SUBDIRECTORY(Tests)
endif()


# CPACK

set(CPACK_PACKAGE_NAME "QSimKit")
set(CPACK_PACKAGE_VENDOR "qsimkit.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "QSimKit - MSP430 Simulator")
set(CPACK_PACKAGE_VERSION "0.0.1")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "QSimKit")

set(CPACK_COMPONENTS_ALL qsimkit msp430 button lcd led)

# This must always be last!
include(CPack)



