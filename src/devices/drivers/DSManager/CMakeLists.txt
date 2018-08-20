## INC & SOURCE DIRS
set(DSManager_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(DSManager_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
  ${DSManager_SRC_DIR}/DSManagerClass.cpp 
  ${DSManager_SRC_DIR}/DSManager.cpp 
  ${DSManager_SRC_DIR}/DSManagerStateMachine.cpp 
  ${DSManager_SRC_DIR}/ClassFactory.cpp
)

file(GLOB THIS_HEADERS ${DSManager_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)


## Append srcs and heads to project srcs and heads
list (APPEND DSManager_SRCS ${THIS_SRCS})
list (APPEND DSManager_HEADERS ${THIS_HEADERS})
list (APPEND DSManager_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_DSManager_SRCS ${DSManager_SRC_DIR}/main.cpp)

set (DSManager_SRCS ${DSManager_SRCS} PARENT_SCOPE)
set (DSManager_HEADERS ${DSManager_HEADERS} PARENT_SCOPE)
set (DSManager_HEADERS_WPATH ${DSManager_HEADERS_WPATH} PARENT_SCOPE)
set (Main_DSManager_SRCS ${Main_DSManager_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

