## INC & SOURCE DIRS
set(LMCStarter_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(LMCStarter_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
  ${LMCStarter_SRC_DIR}/LMCStarterClass.cpp 
  ${LMCStarter_SRC_DIR}/LMCStarter.cpp 
  ${LMCStarter_SRC_DIR}/LMCStarterStateMachine.cpp 
  ${LMCStarter_SRC_DIR}/ClassFactory.cpp
	${LMCStarter_SRC_DIR}/CheckProcessUtil.cpp
	${LMCStarter_SRC_DIR}/PingThread.cpp
	${LMCStarter_SRC_DIR}/StarterService.cpp
	${LMCStarter_SRC_DIR}/StarterUtil.cpp
	${LMCStarter_SRC_DIR}/StartProcessThread.cpp
)

file(GLOB THIS_HEADERS ${LMCStarter_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)

## Append srcs and heads to project srcs and heads
list (APPEND LMCStarter_SRCS ${THIS_SRCS})
list (APPEND LMCStarter_HEADERS ${THIS_HEADERS})
list (APPEND LMCStarter_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_LMCStarter_SRCS ${LMCStarter_SRC_DIR}/main.cpp)

set (LMCStarter_SRCS ${LMCStarter_SRCS} PARENT_SCOPE)
set (LMCStarter_HEADERS ${LMCStarter_HEADERS} PARENT_SCOPE)
set (LMCStarter_HEADERS_WPATH ${LMCStarter_HEADERS_WPATH} PARENT_SCOPE)
set (Main_LMCStarter_SRCS ${Main_LMCStarter_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

