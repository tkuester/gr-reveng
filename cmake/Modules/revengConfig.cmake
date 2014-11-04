INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_REVENG reveng)

FIND_PATH(
    REVENG_INCLUDE_DIRS
    NAMES reveng/api.h
    HINTS $ENV{REVENG_DIR}/include
        ${PC_REVENG_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    REVENG_LIBRARIES
    NAMES gnuradio-reveng
    HINTS $ENV{REVENG_DIR}/lib
        ${PC_REVENG_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(REVENG DEFAULT_MSG REVENG_LIBRARIES REVENG_INCLUDE_DIRS)
MARK_AS_ADVANCED(REVENG_LIBRARIES REVENG_INCLUDE_DIRS)

