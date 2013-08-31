INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
set( QT_USE_XML TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


FILE(GLOB SRC ui/*.cpp Peripherals/*.cpp Script/*.cpp MCU/*.cpp)
FILE(GLOB_RECURSE SRCR DockWidgets/*.cpp Breakpoints/*.cpp Tracking/*.cpp Dwarf/*.cpp)
FILE(GLOB HEADERS ui/*.h Peripherals/*.h Script/*.h MCU/*.h)
FILE(GLOB_RECURSE HEADERSR DockWidgets/*.h Breakpoints/*.h Tracking/*.h Dwarf/*.h)
FILE(GLOB FORMS ui/*.ui)
FILE(GLOB_RECURSE FORMSR DockWidgets/*.ui Tracking/*.ui)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS} ${HEADERSR})
QT4_WRAP_UI(FORMS_HEADERS ${FORMS} ${FORMSR})
QT4_ADD_RESOURCES(RESOURCES icons/icons.qrc)

ADD_EXECUTABLE(qsimkit ${SRC} ${SRCR} main.cpp
    ${HEADERS_MOC}
    ${FORMS_HEADERS}
    ${RESOURCES})

IF(WIN32)
#c:/Python27/lib/python27.lib
	TARGET_LINK_LIBRARIES(qsimkit ${QT_LIBRARIES} ${PYTHONQT_LIBRARIES})
else()
	TARGET_LINK_LIBRARIES(qsimkit ${QT_LIBRARIES} ${PYTHONQT_LIBRARIES})
endif()

INSTALL(TARGETS qsimkit
	RUNTIME DESTINATION bin
	COMPONENT qsimkit
)

INSTALL(DIRECTORY Packages
	DESTINATION ${SHARE_INSTALL_DIR}/qsimkit
	COMPONENT msp430
)

if(WIN32)
INSTALL(FILES ${QT_BINARY_DIR}/QtGui4.dll
	${QT_BINARY_DIR}/QtCore4.dll
	${QT_BINARY_DIR}/QtXml4.dll
	${QT_BINARY_DIR}/QtSvg4.dll
	DESTINATION ${LIB_INSTALL_DIR}
	COMPONENT qsimkit
)
endif()
    
ADD_SUBDIRECTORY(Peripherals)
ADD_SUBDIRECTORY(MCU)

# simkitperipheral library

FILE(GLOB PERIPHERAL_HEADERS Peripherals/Peripheral.h
	Peripherals/PeripheralInterface.h
	Peripherals/SimulationObject.h
	ui/ScreenObject.h
	MCU/MCU.h
	MCU/Register.h
	MCU/RegisterSet.h
	MCU/Memory.h
	Tracking/PinHistory.h
	DockWidgets/Peripherals/MemoryItem.h
	Dwarf/DwarfDebugData.h
	Dwarf/DwarfLoader.h
	Dwarf/DwarfSubprogram.h
	Dwarf/DwarfLocation.h
	Dwarf/DwarfLocationList.h
	Dwarf/DwarfExpression.h
	Dwarf/DwarfVariable.h
	)

FILE(GLOB PERIPHERAL_SRC Peripherals/SimulationObject.cpp
	ui/ScreenObject.cpp
	Tracking/PinHistory.cpp
	DockWidgets/Peripherals/MemoryItem.cpp
	Dwarf/DwarfDebugData.cpp
	Dwarf/DwarfLoader.cpp
	Dwarf/DwarfSubprogram.cpp
	Dwarf/DwarfLocation.cpp
	Dwarf/DwarfLocationList.cpp
	Dwarf/DwarfExpression.cpp
	Dwarf/DwarfVariable.cpp
	MCU/VariableValueFormatter.cpp
	)

QT4_WRAP_CPP(PERIPHERAL_HEADERS_MOC ${PERIPHERAL_HEADERS})
IF(WIN32)
	ADD_LIBRARY(simkitperipheral ${PERIPHERAL_HEADERS_MOC} ${PERIPHERAL_SRC})
else()
	ADD_LIBRARY(simkitperipheral SHARED ${PERIPHERAL_HEADERS_MOC} ${PERIPHERAL_SRC})
endif()
TARGET_LINK_LIBRARIES(simkitperipheral ${QT_LIBRARIES})

INSTALL(TARGETS simkitperipheral
	DESTINATION ${LIB_INSTALL_DIR}
	COMPONENT qsimkit
)

