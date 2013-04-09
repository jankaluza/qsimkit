INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
set( QT_USE_XML TRUE)
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


FILE(GLOB SRC ui/*.cpp Peripherals/*.cpp Peripherals/MSP430/*.cpp Script/*.cpp)
FILE(GLOB_RECURSE SRCR DockWidgets/*.cpp Breakpoints/*.cpp History/*.cpp)
FILE(GLOB HEADERS ui/*.h Peripherals/*.h Peripherals/MSP430/*.h Script/*.h)
FILE(GLOB_RECURSE HEADERSR DockWidgets/*.h Breakpoints/*.h History/*.h)
FILE(GLOB FORMS ui/*.ui)
FILE(GLOB_RECURSE FORMSR DockWidgets/*.ui History/*.ui)

QT4_WRAP_CPP(HEADERS_MOC ${HEADERS} ${HEADERSR})
QT4_WRAP_UI(FORMS_HEADERS ${FORMS} ${FORMSR})
# QT4_ADD_RESOURCES(helloworld_RESOURCES_RCC ${helloworld_RESOURCES})

ADD_EXECUTABLE(qsimkit ${SRC} ${SRCR} main.cpp
    ${HEADERS_MOC}
    ${FORMS_HEADERS})
TARGET_LINK_LIBRARIES(qsimkit ${QT_LIBRARIES} ${PYTHONQT_LIBRARIES} simkit_cpu)

ADD_SUBDIRECTORY(Peripherals)
