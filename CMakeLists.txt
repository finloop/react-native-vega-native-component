cmake_minimum_required(VERSION 3.22) # APMF needs CMP0095 (>= 3.16); 3.22 is safe

project(CustomViewDemo VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generated IDL bindings (from kepler_add_idl_import_library) land in the binary
# dir; make find_package / includes see them.
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

find_package(Apmf REQUIRED)
find_package(Kepler REQUIRED)

# Generate C++ bindings for the UI-Toolkit IDL. The packages are resolved from
# the IDL trees staged under apmf/iface/ (the importer searches the project dir;
# they are copied from the SDK's KeplerUI{View,React,Graphics}Interface packages
# because the react-native-vega in-app sysroot has no CMake config for them).
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

# Build the component as an in-process component library.
#
# DEPLOYMENT CONTRACT (the non-obvious part): an app-provided in-process
# component is dlopen'd at runtime by a filename derived from its component path
# ("/com.example.customview" -> "com.example.customview.so"), and its SONAME must
# match that filename or the APMF self-registration won't resolve. OUTPUT_NAME +
# empty PREFIX make CMake emit exactly "com.example.customview.so" with a matching
# auto-generated SONAME, installed flat into lib/. (Verified against Amazon's own
# @amazon-devices/vega-carousel, which ships com.amazon.keplerscript.carousel.so.)
kepler_add_library(customview SHARED
  kepler/ViewManagerFactory.cpp
  kepler/ColorBoxViewManager.cpp)
set_target_properties(customview PROPERTIES OUTPUT_NAME "com.example.customview" PREFIX "")
target_link_libraries(customview PRIVATE customview_idl)
install(TARGETS customview)
