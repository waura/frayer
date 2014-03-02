# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. Example variables are:
#   CPACK_GENERATOR                     - Generator used to create package
#   CPACK_INSTALL_CMAKE_PROJECTS        - For each project (path, name, component)
#   CPACK_CMAKE_GENERATOR               - CMake Generator used for the projects
#   CPACK_INSTALL_COMMANDS              - Extra commands to install components
#   CPACK_INSTALL_DIRECTORIES           - Extra directories to install
#   CPACK_PACKAGE_DESCRIPTION_FILE      - Description file for the package
#   CPACK_PACKAGE_DESCRIPTION_SUMMARY   - Summary of the package
#   CPACK_PACKAGE_EXECUTABLES           - List of pairs of executables and labels
#   CPACK_PACKAGE_FILE_NAME             - Name of the package generated
#   CPACK_PACKAGE_ICON                  - Icon used for the package
#   CPACK_PACKAGE_INSTALL_DIRECTORY     - Name of directory for the installer
#   CPACK_PACKAGE_NAME                  - Package project name
#   CPACK_PACKAGE_VENDOR                - Package project vendor
#   CPACK_PACKAGE_VERSION               - Package project version
#   CPACK_PACKAGE_VERSION_MAJOR         - Package project version (major)
#   CPACK_PACKAGE_VERSION_MINOR         - Package project version (minor)
#   CPACK_PACKAGE_VERSION_PATCH         - Package project version (patch)

# There are certain generator specific ones

# NSIS Generator:
#   CPACK_PACKAGE_INSTALL_REGISTRY_KEY  - Name of the registry key for the installer
#   CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS - Extra commands used during uninstall
#   CPACK_NSIS_EXTRA_INSTALL_COMMANDS   - Extra commands used during install


SET(CPACK_ALL_INSTALL_TYPES "Full")
SET(CPACK_BINARY_BUNDLE "")
SET(CPACK_BINARY_CYGWIN "")
SET(CPACK_BINARY_DEB "")
SET(CPACK_BINARY_DRAGNDROP "")
SET(CPACK_BINARY_NSIS "")
SET(CPACK_BINARY_OSXX11 "")
SET(CPACK_BINARY_PACKAGEMAKER "")
SET(CPACK_BINARY_RPM "")
SET(CPACK_BINARY_STGZ "")
SET(CPACK_BINARY_TBZ2 "")
SET(CPACK_BINARY_TGZ "")
SET(CPACK_BINARY_TZ "")
SET(CPACK_BINARY_ZIP "")
SET(CPACK_CMAKE_GENERATOR "Visual Studio 10 Win64")
SET(CPACK_COMPONENTS_ALL "main;src;Unspecified")
SET(CPACK_COMPONENTS_ALL_SET_BY_USER "TRUE")
SET(CPACK_COMPONENT_MAIN_INSTALL_TYPES "Full")
SET(CPACK_COMPONENT_SRC_INSTALL_TYPES "Full")
SET(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
SET(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
SET(CPACK_COMPONENT_main_DISPLAY_NAME "Binaries and the Documentation")
SET(CPACK_COMPONENT_src_DISPLAY_NAME "The source code")
SET(CPACK_GENERATOR "ZIP")
SET(CPACK_IGNORE_FILES "/\\.svn/;/autom4te.cache/;/build/;/interfaces/;/swig_python/;/octave/;/gtest/;~$;\\.aux$;\\.bbl$;\\.blg$;\\.idx$;\\.ilg$;\\.ind$;\\.log$;\\.toc$;\\.out$;\\.pyc$;\\.pyo$;\\.vcproj$;/1$;")
SET(CPACK_INSTALLED_DIRECTORIES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0;/")
SET(CPACK_INSTALL_CMAKE_PROJECTS "")
SET(CPACK_INSTALL_PREFIX "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/install")
SET(CPACK_MODULE_PATH "")
SET(CPACK_NSIS_CONTACT "")
SET(CPACK_NSIS_DISPLAY_NAME "Open Source Computer Vision Library")
SET(CPACK_NSIS_DISPLAY_NAME_SET "TRUE")
SET(CPACK_NSIS_HELP_LINK "http:\\\\opencv.willowgarage.com")
SET(CPACK_NSIS_INSTALLED_ICON_NAME "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0\\doc\\opencv.ico")
SET(CPACK_NSIS_INSTALLER_ICON_CODE "")
SET(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
SET(CPACK_NSIS_MENU_LINKS "http://opencv.willowgarage.com;Start Page;doc\\opencv2refman.pdf;The OpenCV Reference Manual;doc\\opencv_tutorials.pdf;The OpenCV Tutorials for Beginners;CMakeLists.txt;The Build Script (open with CMake);samples\\c;C Samples;samples\\cpp;C++ Samples;samples\\python;Python Samples")
SET(CPACK_NSIS_MODIFY_PATH "ON")
SET(CPACK_NSIS_MUI_ICON "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0\\doc\\opencv.ico")
SET(CPACK_NSIS_MUI_UNIICON "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0\\doc\\opencv.ico")
SET(CPACK_NSIS_PACKAGE_NAME "OpenCV 2.3.0rc")
SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\opencv.willowgarage.com")
SET(CPACK_OUTPUT_CONFIG_FILE "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/CPackConfig.cmake")
SET(CPACK_PACKAGE_DEFAULT_LOCATION "/")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "C:/Program Files (x86)/CMake 2.8/share/cmake-2.8/Templates/CPack.GenericDescription.txt")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OpenCV SDK for Visual Studio 10 Win64 is an All-In-One package for developing computer vision applications")
SET(CPACK_PACKAGE_EXECUTABLES "")
SET(CPACK_PACKAGE_FILE_NAME "OpenCV-2.3.0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "OpenCV2.3")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "OpenCV 2.3.0")
SET(CPACK_PACKAGE_NAME "OpenCV")
SET(CPACK_PACKAGE_RELOCATABLE "true")
SET(CPACK_PACKAGE_VENDOR "OpenCV project opencvlibrary.sourceforge.net")
SET(CPACK_PACKAGE_VERSION "2.3.0")
SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_RESOURCE_FILE_LICENSE "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/doc/license.txt")
SET(CPACK_RESOURCE_FILE_README "C:/Program Files (x86)/CMake 2.8/share/cmake-2.8/Templates/CPack.GenericDescription.txt")
SET(CPACK_RESOURCE_FILE_WELCOME "C:/Program Files (x86)/CMake 2.8/share/cmake-2.8/Templates/CPack.GenericWelcome.txt")
SET(CPACK_SET_DESTDIR "OFF")
SET(CPACK_SOURCE_CYGWIN "")
SET(CPACK_SOURCE_GENERATOR "ZIP")
SET(CPACK_SOURCE_IGNORE_FILES "/\\.svn/;/autom4te.cache/;/build/;/interfaces/;/swig_python/;/octave/;/gtest/;~$;\\.aux$;\\.bbl$;\\.blg$;\\.idx$;\\.ilg$;\\.ind$;\\.log$;\\.toc$;\\.out$;\\.pyc$;\\.pyo$;\\.vcproj$;/1$;")
SET(CPACK_SOURCE_INSTALLED_DIRECTORIES "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0;/")
SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "D:/Documents/visual studio 2010/Projects/frayer/3rdparty/OpenCV-2.3.0/CPackSourceConfig.cmake")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "OpenCV-2.3.0")
SET(CPACK_SOURCE_TBZ2 "")
SET(CPACK_SOURCE_TGZ "")
SET(CPACK_SOURCE_TOPLEVEL_TAG "win64-Source")
SET(CPACK_SOURCE_TZ "")
SET(CPACK_SOURCE_ZIP "")
SET(CPACK_STRIP_FILES "")
SET(CPACK_SYSTEM_NAME "win64")
SET(CPACK_TOPLEVEL_TAG "win64-Source")
