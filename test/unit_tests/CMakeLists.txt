
add_subdirectory(test_device)
add_subdirectory(env)
add_subdirectory(utils)

##  UNIT TEST SOURCES ###
set (UNIT_TEST_SRCS ${UNIT_TEST_SRCS} PARENT_SCOPE)
set (UNIT_TEST_HEADERS ${UNIT_TEST_HEADERS} PARENT_SCOPE)
set (UNIT_TEST_HEADERS_WPATH ${UNIT_TEST_HEADERS_WPATH} PARENT_SCOPE)
#######################

## UNIT TEST MAIN ##
list (APPEND UNIT_TEST_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/main.cc)
set (UNIT_TEST_MAIN ${UNIT_TEST_MAIN} PARENT_SCOPE)
#####################

set (TEST_INCLUDE_DIRS ${TEST_INCLUDE_DIRS} PARENT_SCOPE)

