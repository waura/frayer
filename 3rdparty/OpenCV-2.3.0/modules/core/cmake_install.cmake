# Install script for directory: D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/core

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/install")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_core230d.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Release/opencv_core230.lib")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_core230d.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Release/opencv_core230.dll")
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/opencv2/core" TYPE FILE FILES
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/core/include/opencv2/core/core.hpp"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/core/include/opencv2/core/core_c.h"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/core/include/opencv2/core/eigen.hpp"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/core/include/opencv2/core/internal.hpp"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/core/include/opencv2/core/mat.hpp"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/core/include/opencv2/core/operations.hpp"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/core/include/opencv2/core/types_c.h"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/core/include/opencv2/core/version.hpp"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/core/include/opencv2/core/wimage.hpp"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

