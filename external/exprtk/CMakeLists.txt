## INC & SOURCE DIRS
set(Exprtk_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(Exprtk_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB THIS_HEADERS ${Exprtk_INC_DIR}/*.hpp)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)


## Append srcs and heads to project srcs and heads
list (APPEND Exprtk_HEADERS ${THIS_HEADERS})
list (APPEND Exprtk_HEADERS_WPATH ${THIS_HEADERS_WPATH})

set (Exprtk_HEADERS ${Exprtk_HEADERS} PARENT_SCOPE)
set (Exprtk_HEADERS_WPATH ${Exprtk_HEADERS_WPATH} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

