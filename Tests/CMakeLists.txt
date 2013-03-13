FILE(GLOB_RECURSE SRC_TEST *.cpp)

ADD_EXECUTABLE(simkit_test ${SRC_TEST})
set_target_properties(simkit_test PROPERTIES COMPILE_DEFINITIONS SIMKIT_TEST=1)

target_link_libraries(simkit_test simkit_cpu ${CPPUNIT_LIBRARY})
