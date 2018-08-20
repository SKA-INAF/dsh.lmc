## INC & SOURCE DIRS
set(ElementLogger_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ElementLogger_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
  ${ElementLogger_SRC_DIR}/ElementLoggerClass.cpp 
  ${ElementLogger_SRC_DIR}/ElementLogger.cpp 
  ${ElementLogger_SRC_DIR}/ElementLoggerStateMachine.cpp 
  ${ElementLogger_SRC_DIR}/ClassFactory.cpp
)

file(GLOB THIS_HEADERS ${ElementLogger_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)

## Append srcs and heads to project srcs and heads
list (APPEND ElementLogger_SRCS ${THIS_SRCS})
list (APPEND ElementLogger_HEADERS ${THIS_HEADERS})
list (APPEND ElementLogger_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_ElementLogger_SRCS ${ElementLogger_SRC_DIR}/main.cpp)

set (ElementLogger_SRCS ${ElementLogger_SRCS} PARENT_SCOPE)
set (ElementLogger_HEADERS ${ElementLogger_HEADERS} PARENT_SCOPE)
set (ElementLogger_HEADERS_WPATH ${ElementLogger_HEADERS_WPATH} PARENT_SCOPE)
set (Main_ElementLogger_SRCS ${Main_ElementLogger_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

