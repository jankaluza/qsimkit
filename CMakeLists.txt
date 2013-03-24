cmake_minimum_required(VERSION 2.6)
project(qsimkit)

set(cppunit_DIR "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(cppunit)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXml REQUIRED)

if(NOT CPPUNIT_FOUND AND CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARY)
	set(CCPUNIT_LIBRARIES ${CPPUNIT_LIBRARY})
	set(CPPUNIT_FOUND 1)
	message(STATUS "Using cppunit: ${CPPUNIT_INCLUDE_DIR} ${CPPUNIT_LIBRARIES}")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/adevs-2.6/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/msp430gcc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_COMPILER_IS_GNUCXX)
	ADD_DEFINITIONS(-O0)
	ADD_DEFINITIONS(-ggdb)
endif()

ADD_SUBDIRECTORY(3rdparty)
ADD_SUBDIRECTORY(CPU)
ADD_SUBDIRECTORY(GUI)

if (CPPUNIT_FOUND)
	ADD_SUBDIRECTORY(Tests)
endif()
