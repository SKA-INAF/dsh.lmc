##add_subdirectory(DishMonitor)
add_subdirectory(LMCMonitor)

# DishMonitor
##set (DishMonitor_SRCS ${DishMonitor_SRCS} PARENT_SCOPE)
##set (DishMonitor_HEADERS ${DishMonitor_HEADERS} PARENT_SCOPE)
##set (DishMonitor_HEADERS_WPATH ${DishMonitor_HEADERS_WPATH} PARENT_SCOPE)
##set (Main_DishMonitor_SRCS ${Main_DishMonitor_SRCS} PARENT_SCOPE)

# LMCMonitor
set (LMCMonitor_SRCS ${LMCMonitor_SRCS} PARENT_SCOPE)
set (LMCMonitor_HEADERS ${LMCMonitor_HEADERS} PARENT_SCOPE)
set (LMCMonitor_HEADERS_WPATH ${LMCMonitor_HEADERS_WPATH} PARENT_SCOPE)
set (Main_LMCMonitor_SRCS ${Main_LMCMonitor_SRCS} PARENT_SCOPE)

set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

