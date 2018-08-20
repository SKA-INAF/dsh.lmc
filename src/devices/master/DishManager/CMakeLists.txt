## INC & SOURCE DIRS
set(DishManager_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(DishManager_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
  ${DishManager_SRC_DIR}/DishManagerClass.cpp 
  ${DishManager_SRC_DIR}/DishManager.cpp 
  ${DishManager_SRC_DIR}/DishManagerStateMachine.cpp 
  ${DishManager_SRC_DIR}/ClassFactory.cpp
	${DishManager_SRC_DIR}/ConfigureTask.cpp
)

file(GLOB THIS_HEADERS ${DishManager_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)

## Append srcs and heads to project srcs and heads
list (APPEND DishManager_SRCS ${THIS_SRCS})
list (APPEND DishManager_HEADERS ${THIS_HEADERS})
list (APPEND DishManager_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_DishManager_SRCS ${DishManager_SRC_DIR}/main.cpp)

set (DishManager_SRCS ${DishManager_SRCS} PARENT_SCOPE)
set (DishManager_HEADERS ${DishManager_HEADERS} PARENT_SCOPE)
set (DishManager_HEADERS_WPATH ${DishManager_HEADERS_WPATH} PARENT_SCOPE)
set (Main_DishManager_SRCS ${Main_DishManager_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

