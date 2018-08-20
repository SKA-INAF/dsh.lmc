## INC & SOURCE DIRS
set(FormulaConf_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(FormulaConf_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
  ${FormulaConf_SRC_DIR}/FormulaConfClass.cpp 
  ${FormulaConf_SRC_DIR}/FormulaConf.cpp 
  ${FormulaConf_SRC_DIR}/FormulaConfStateMachine.cpp 
  ${FormulaConf_SRC_DIR}/ClassFactory.cpp
	${FormulaConf_SRC_DIR}/FormulaConfDynAttrUtils.cpp
	${FormulaConf_SRC_DIR}/readthread.cpp
)

file(GLOB THIS_HEADERS ${FormulaConf_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)


## Append srcs and heads to project srcs and heads
list (APPEND FormulaConf_SRCS ${THIS_SRCS})
list (APPEND FormulaConf_HEADERS ${THIS_HEADERS})
list (APPEND FormulaConf_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_FormulaConf_SRCS ${FormulaConf_SRC_DIR}/main.cpp)

set (FormulaConf_SRCS ${FormulaConf_SRCS} PARENT_SCOPE)
set (FormulaConf_HEADERS ${FormulaConf_HEADERS} PARENT_SCOPE)
set (FormulaConf_HEADERS_WPATH ${FormulaConf_HEADERS_WPATH} PARENT_SCOPE)
set (Main_FormulaConf_SRCS ${Main_FormulaConf_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

