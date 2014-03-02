# Install script for directory: D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/doc" TYPE FILE FILES
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/haartraining.htm"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/check_docs_whitelist.txt"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/CMakeLists.txt"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/license.txt"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/packaging.txt"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/opencv.jpg"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/acircles_pattern.png"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/opencv-logo.png"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/opencv-logo2.png"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/pattern.png"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/opencv2refman.pdf"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/opencv_cheatsheet.pdf"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/opencv_tutorials.pdf"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/opencv_user.pdf"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/doc/vidsurv" TYPE FILE FILES
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/vidsurv/Blob_Tracking_Modules.doc"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/vidsurv/Blob_Tracking_Tests.doc"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/vidsurv/TestSeq.doc"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

