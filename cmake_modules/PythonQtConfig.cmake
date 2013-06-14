find_path(PYTHONQT_INCLUDE_PATH
NAME PythonQt.h
PATHS /usr/include/PythonQt /usr/local/include/PythonQt PythonQt/src
)

find_library(PYTHONQT_LIBRARIES
NAMES PythonQt
PATH /usr/lib /usr/local/lib /usr/lib64 /usr/local/lib64 PythonQt/lib
)

find_library(PYTHONQTALL_LIBRARIES
NAMES PythonQt_QtAll
PATH /usr/lib /usr/local/lib /usr/lib64 /usr/local/lib64 PythonQt/lib
)

if(PYTHONQT_INCLUDE_PATH AND PYTHONQT_LIBRARIES AND PYTHONQTALL_LIBRARIES)
set(PythonQt_FOUND TRUE)
set(PYTHONQT_LIBRARIES ${PYTHONQT_LIBRARIES} ${PYTHONQTALL_LIBRARIES})
else()
message(FATAL_ERROR "Could not find PythonQt or PythonQt_QtAll")
set(PythonQt_FOUND FALSE)
endif()



