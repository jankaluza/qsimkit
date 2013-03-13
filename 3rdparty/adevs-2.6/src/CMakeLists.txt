FILE(GLOB SRC *.cpp)

ADD_LIBRARY(simkit_adevs STATIC ${SRC})

# target_link_libraries(spectrum2_swiften_backend transport ${Boost_LIBRARIES} ${SWIFTEN_LIBRARY} ${LOG4CXX_LIBRARIES})

