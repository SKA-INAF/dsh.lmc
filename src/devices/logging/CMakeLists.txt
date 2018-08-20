add_subdirectory(ElementLogger)
add_subdirectory(LogPrinter)

# ElementLogger
set (ElementLogger_SRCS ${ElementLogger_SRCS} PARENT_SCOPE)
set (ElementLogger_HEADERS ${ElementLogger_HEADERS} PARENT_SCOPE)
set (ElementLogger_HEADERS_WPATH ${ElementLogger_HEADERS_WPATH} PARENT_SCOPE)
set (Main_ElementLogger_SRCS ${Main_ElementLogger_SRCS} PARENT_SCOPE)

# LogPrinter
set(LogPrinterExe ${LogPrinterExe} PARENT_SCOPE)

set (INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

