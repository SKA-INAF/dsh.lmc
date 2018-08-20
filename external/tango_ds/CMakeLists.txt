add_subdirectory(AlarmSystem)
add_subdirectory(FormulaConf)

# Elettra Alarm System
set (AlarmSystem_SRCS ${AlarmSystem_SRCS} PARENT_SCOPE)
set (AlarmSystem_HEADERS ${AlarmSystem_HEADERS} PARENT_SCOPE)
set (AlarmSystem_HEADERS_WPATH ${AlarmSystem_HEADERS_WPATH} PARENT_SCOPE)
set (Main_AlarmSystem_SRCS ${Main_AlarmSystem_SRCS} PARENT_SCOPE)

# Formula Conf
set (FormulaConf_SRCS ${FormulaConf_SRCS} PARENT_SCOPE)
set (FormulaConf_HEADERS ${FormulaConf_HEADERS} PARENT_SCOPE)
set (FormulaConf_HEADERS_WPATH ${FormulaConf_HEADERS_WPATH} PARENT_SCOPE)
set (Main_FormulaConf_SRCS ${Main_FormulaConf_SRCS} PARENT_SCOPE)

set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

