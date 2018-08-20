## INC & SOURCE DIRS
set(HdbEventSubscriber_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(HdbEventSubscriber_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

## GET SOURCES and HEADERS
set(THIS_SRCS ${THIS_SRCS} 
	${HdbEventSubscriber_SRC_DIR}/HdbEventSubscriberClass.cpp 
  ${HdbEventSubscriber_SRC_DIR}/HdbEventSubscriber.cpp 
  ${HdbEventSubscriber_SRC_DIR}/HdbEventSubscriberStateMachine.cpp 
  ${HdbEventSubscriber_SRC_DIR}/ClassFactory.cpp
	${HdbEventSubscriber_SRC_DIR}/HdbEventSubscriberDynAttrUtils.cpp
	${HdbEventSubscriber_SRC_DIR}/HdbDevice.cpp	
	${HdbEventSubscriber_SRC_DIR}/CheckPeriodicThread.cpp
	${HdbEventSubscriber_SRC_DIR}/PollerThread.cpp
	${HdbEventSubscriber_SRC_DIR}/PushThread.cpp
	${HdbEventSubscriber_SRC_DIR}/StatsThread.cpp
	${HdbEventSubscriber_SRC_DIR}/SubscribeThread.cpp	
)

file(GLOB THIS_HEADERS ${HdbEventSubscriber_INC_DIR}/*.h)

set(THIS_HEADERS_WPATH "")
foreach (_headerFile ${THIS_HEADERS})
	get_filename_component(_basefilename ${_headerFile} NAME)
  list (APPEND THIS_HEADERS_WPATH ${_basefilename})
endforeach()
list(REMOVE_DUPLICATES THIS_HEADERS_WPATH)


## Append srcs and heads to project srcs and heads
list (APPEND HdbEventSubscriber_SRCS ${THIS_SRCS})
list (APPEND HdbEventSubscriber_HEADERS ${THIS_HEADERS})
list (APPEND HdbEventSubscriber_HEADERS_WPATH ${THIS_HEADERS_WPATH})
list (APPEND Main_HdbEventSubscriber_SRCS ${HdbEventSubscriber_SRC_DIR}/main.cpp)

set (HdbEventSubscriber_SRCS ${HdbEventSubscriber_SRCS} PARENT_SCOPE)
set (HdbEventSubscriber_HEADERS ${HdbEventSubscriber_HEADERS} PARENT_SCOPE)
set (HdbEventSubscriber_HEADERS_WPATH ${HdbEventSubscriber_HEADERS_WPATH} PARENT_SCOPE)
set (Main_HdbEventSubscriber_SRCS ${Main_HdbEventSubscriber_SRCS} PARENT_SCOPE)

list (APPEND INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

