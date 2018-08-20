## INC & SOURCE DIRS
set(TestDevice_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TestDevice_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message (STATUS "TestDevice_INC_DIR: ${TestDevice_INC_DIR}")

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
  ${TestDevice_SRC_DIR}/TestDeviceClass.cpp 
  ${TestDevice_SRC_DIR}/TestDevice.cpp 
  ${TestDevice_SRC_DIR}/TestDeviceStateMachine.cpp 
  ${TestDevice_SRC_DIR}/ClassFactory.cpp
)

file(GLOB_RECURSE THIS_HEADERS ${TestDevice_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)

## Append srcs and heads to project srcs and heads
list (APPEND UNIT_TEST_SRCS ${THIS_SRCS})
list (APPEND UNIT_TEST_HEADERS ${THIS_HEADERS})
list (APPEND UNIT_TEST_HEADERS_WPATH ${THIS_HEADERS_WPATH})

set (UNIT_TEST_SRCS ${UNIT_TEST_SRCS} PARENT_SCOPE)
set (UNIT_TEST_HEADERS ${UNIT_TEST_HEADERS} PARENT_SCOPE)
set (UNIT_TEST_HEADERS_WPATH ${UNIT_TEST_HEADERS_WPATH} PARENT_SCOPE)

list (APPEND TEST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (TEST_INCLUDE_DIRS ${TEST_INCLUDE_DIRS} PARENT_SCOPE)

