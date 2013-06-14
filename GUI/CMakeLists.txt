INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
set( QT_USE_XML TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


FILE(GLOB SRC ui/*.cpp Peripherals/*.cpp Script/*.cpp MCU/*.cpp)
FILE(GLOB_RECURSE SRCR DockWidgets/*.cpp Breakpoints/*.cpp Tracking/*.cpp)
FILE(GLOB HEADERS ui/*.h Peripherals/*.h Script/*.h MCU/*.h)
FILE(GLOB_RECURSE HEADERSR DockWidgets/*.h Breakpoints/*.h Tracking/*.h)
FILE(GLOB FORMS ui/*.ui)
FILE(GLOB_RECURSE FORMSR DockWidgets/*.ui Tracking/*.ui)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS} ${HEADERSR})
QT4_WRAP_UI(FORMS_HEADERS ${FORMS} ${FORMSR})
# QT4_ADD_RESOURCES(helloworld_RESOURCES_RCC ${helloworld_RESOURCES})

ADD_EXECUTABLE(qsimkit ${SRC} ${SRCR} main.cpp
    ${HEADERS_MOC}
    ${FORMS_HEADERS})
TARGET_LINK_LIBRARIES(qsimkit ${QT_LIBRARIES} ${PYTHONQT_LIBRARIES})

ADD_SUBDIRECTORY(Peripherals)
ADD_SUBDIRECTORY(MCU)


FILE(GLOB PERIPHERAL_HEADERS Peripherals/Peripheral.h
	Peripherals/PeripheralInterface.h
	Peripherals/SimulationObject.h
	ui/ScreenObject.h
	)

FILE(GLOB PERIPHERAL_SRC Peripherals/SimulationObject.cpp
	ui/ScreenObject.cpp
	)

QT4_WRAP_CPP(PERIPHERAL_HEADERS_MOC ${PERIPHERAL_HEADERS})
ADD_LIBRARY(simkitperipheral SHARED ${PERIPHERAL_HEADERS_MOC} ${PERIPHERAL_SRC})
TARGET_LINK_LIBRARIES(simkitperipheral ${QT_LIBRARIES})

