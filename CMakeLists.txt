cmake_minimum_required(VERSION 3.22) # APMF needs CMP0095 (>= 3.16); 3.22 is safe

project(CustomViewDemo VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Generated IDL bindings (from kepler_add_idl_import_library) land in the binary
# dir; make find_package / includes see them.
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

find_package(Apmf REQUIRED)
find_package(Kepler REQUIRED)

# Generate C++ bindings for the UI-Toolkit IDL. The importer (apmfc) resolves
# these packages from the build sysroot, where the SDK stages its
# KeplerUI{View,React,Graphics}Interface IDL trees under
# .../usr/include/apmf/iface/ — no local copy needed. (find_package has no CMake
# config for them, but the IDL importer doesn't use one; it searches the
# sysroot include path.)
#   .view     -> IView, IViewModule
#   .react    -> IViewManager(Factory), IPropsDelegate, ILayoutDelegate, IJsObject, ...
#   .graphics -> Color, Size
kepler_add_idl_import_library(
  customview_idl
  PACKAGES
  com.amazon.kepler.uitoolkit.view
  com.amazon.kepler.uitoolkit.react
  com.amazon.kepler.uitoolkit.graphics
)

# Build the component
kepler_add_library(customview SHARED
  kepler/ViewManagerFactory.cpp
  kepler/ColorBoxViewManager.cpp)
set_target_properties(customview PROPERTIES OUTPUT_NAME "com.example.customview" PREFIX "")
target_link_libraries(customview PRIVATE customview_idl)
install(TARGETS customview)
