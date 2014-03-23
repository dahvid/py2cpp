
cmake_minimum_required (VERSION 2.6)
project (py2cpp)



SET(CMAKE_BUILD_TYPE Debug)
#SET(CMAKE_BUILD_TYPE RelWithDebInfo)



#SET(CMAKE_CXX_FLAGS "-fPIC")
IF (WIN32)
    #SET(CMAKE_C_FLAGS "/EHsc")
   # SET(WIN32 1)
ELSEIF(APPLE)
    SET(CMAKE_CXX_FLAGS "-fPIC  -std=c++0x -pthread -ldl")
ELSE()
    SET(CMAKE_CXX_FLAGS "-fPIC  -std=c++0x -pthread -ldl -lrt")
ENDIF (WIN32)




set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH  ${CMAKE_SOURCE_DIR}/bin)
set(PUBLIC_INCLUDES ${CMAKE_SOURCE_DIR}/include)


message("Building cmake files for project directory.", ${CMAKE_SOURCE_DIR})


IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(PYTHON_LIBRARIES /opt/local/lib/libpython2.6.dylib)
	set(PYTHON_INCLUDE_PATH /opt/local/Library/Frameworks/Python.framework/Versions/2.6/Headers)
ELSE()
	find_package(PythonLibs REQUIRED)
ENDIF()
message("found Python libraries "  ${PYTHON_LIBRARIES})
message("found Python includes "  ${PYTHON_INCLUDE_PATH})


find_package(Boost COMPONENTS  python REQUIRED)
message("found boost libraries:")
foreach(item ${Boost_LIBRARIES})
	message(${item})
endforeach()

message("found boost includes "  ${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${Boost_INCLUDE_DIRS},  ${PYTHON_INCLUDE_PATH})

add_subdirectory(test)





