FILE(GLOB_RECURSE SRC_TEST *.cpp)

ADD_EXECUTABLE(simkit_test ${SRC_TEST})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../QSimKit/MCU/MSP430)
set_target_properties(simkit_test PROPERTIES COMPILE_DEFINITIONS SIMKIT_TEST=1)

target_link_libraries(simkit_test msp430 simkitperipheral ${CPPUNIT_LIBRARY})
