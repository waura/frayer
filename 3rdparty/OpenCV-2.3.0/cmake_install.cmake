# Install script for directory: D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0

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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/install" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/win-install/OpenCVConfig.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/install/include" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/cvconfig.h")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/README")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "src")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/cmake_uninstall.cmake.in"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/OpenCV.mk.in"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/opencv.pc.cmake.in"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/OpenCVConfig.cmake.in"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/Package.cmake.in"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/cmake_uninstall.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/CPackConfig.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/CPackSourceConfig.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/cvconfig.h.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/OpenCVConfig.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/OpenCVFindIPP.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/OpenCVFindLATEX.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/OpenCVFindOpenEXR.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/OpenCVFindOpenNI.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/OpenCVFindPkgConfig.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/OpenCVModule.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/OpenCVPCHSupport.cmake"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/CMakeLists.txt"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "src")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/win-install/OpenCVConfig.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "src")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/include/CMakeLists.txt")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "src")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_calib3d230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_calib3d230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_core230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_core230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_contrib230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_contrib230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_features2d230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_features2d230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_ffmpeg230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_ffmpeg230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_flann230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_flann230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_gpu230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_gpu230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_highgui230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_highgui230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_imgproc230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_imgproc230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_legacy230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_legacy230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_ml230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_ml230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_objdetect230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_objdetect230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/bin/Debug/opencv_video230d.dll")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/lib/Debug/opencv_video230d.lib")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE DIRECTORY FILES
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/data"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/samples"
    REGEX "/\\.svn$" EXCLUDE)
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "main")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "src")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." TYPE DIRECTORY FILES
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/3rdparty"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/android"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules"
    "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc"
    REGEX "/\\.svn$" EXCLUDE)
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "src")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/include/cmake_install.cmake")
  INCLUDE("D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/modules/cmake_install.cmake")
  INCLUDE("D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/cmake_install.cmake")
  INCLUDE("D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/data/cmake_install.cmake")
  INCLUDE("D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/3rdparty/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
