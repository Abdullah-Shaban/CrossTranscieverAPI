INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_XCVR xcvr)

FIND_PATH(
    XCVR_INCLUDE_DIRS
    NAMES xcvr/api.h
    HINTS $ENV{XCVR_DIR}/include
        ${PC_XCVR_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    XCVR_LIBRARIES
    NAMES gnuradio-xcvr
    HINTS $ENV{XCVR_DIR}/lib
        ${PC_XCVR_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(XCVR DEFAULT_MSG XCVR_LIBRARIES XCVR_INCLUDE_DIRS)
MARK_AS_ADVANCED(XCVR_LIBRARIES XCVR_INCLUDE_DIRS)

