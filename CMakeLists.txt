cmake_minimum_required(VERSION 2.8.12)
include(ExternalProject)
project(mqblynk CXX)

# you fucking what? having to pass cmake args to make this do anything sane?
# INSTALL_DIR is ignored.
# BUILD_IN_SOURCE is to work around bugs in jsonpath's cmake :(
ExternalProject_Add(jsonpath_ext
	INSTALL_DIR blahfix
	SOURCE_DIR jsonpath
	BUILD_IN_SOURCE 1
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/install
)

ExternalProject_Get_Property(jsonpath_ext install_dir)

add_library(jsonpath UNKNOWN IMPORTED)
set_property(TARGET jsonpath
	PROPERTY IMPORTED_LOCATION ${install_dir}/lib/libjsonpath.so
)
add_dependencies(jsonpath jsonpath_ext)

ADD_DEFINITIONS(-Os -g3 -Wall -Werror --std=c++11 -Wmissing-declarations
-Wno-error=unused-variable -ffunction-sections -D_GNU_SOURCE -DLINUX)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-Wl,--gc-sections")

find_library(json NAMES json-c json)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(JSONC json-c json)
INCLUDE_DIRECTORIES(${JSONC_INCLUDE_DIRS} blynk blynk/linux)

set(BLYNK_SOURCE "blynk/linux/BlynkDebug.cpp"
"blynk/utility/BlynkHandlers.cpp")

add_executable(mqblynk src/main.cpp src/mq.cpp src/TopicMaps.cpp ${BLYNK_SOURCE})
target_link_libraries(mqblynk jsonpath ${json} mosquittopp)
