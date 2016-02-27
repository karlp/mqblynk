cmake_minimum_required(VERSION 2.8.12)
project(mqblynk)

# Offer the user the choice of overriding the installation directories
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")

include(ExternalProject)
# you fucking what? having to pass cmake args to make this do anything sane?
# INSTALL_DIR is ignored.
# BUILD_IN_SOURCE is to work around bugs in jsonpath's cmake :(
ExternalProject_Add(jsonpath_ext
	INSTALL_DIR blahfix
	SOURCE_DIR jsonpath
	BUILD_IN_SOURCE 1
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/contrib
)

ExternalProject_Get_Property(jsonpath_ext install_dir)
message("jsonpaths install dir is ${install_dir}")

add_library(jsonpath UNKNOWN IMPORTED)
set_property(TARGET jsonpath
	PROPERTY IMPORTED_LOCATION contrib/lib/libjsonpath.so
)
add_dependencies(jsonpath jsonpath_ext)

ADD_DEFINITIONS(-Os -g3 -Wall -Wextra --std=c++11 -Wmissing-declarations
-Wno-error=unused-variable -ffunction-sections -D_GNU_SOURCE -DLINUX)

SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "-Wl,--gc-sections")

# pseudo find_package for mosquittopp
find_library(MOSQUITTOPP_LIBRARIES NAMES mosquittopp
	PATHS /home/karlp/.local
	PATH_SUFFIXES lib
)
find_path(MOSQUITTOPP_INCLUDE_DIRS NAMES mosquittopp.h
	PATHS /home/karlp/.local
	PATH_SUFFIXES include
)

INCLUDE(FindPkgConfig)
pkg_search_module(JSONC REQUIRED json-c json)

set(BLYNK_SOURCE "blynk/linux/BlynkDebug.cpp"
"blynk/utility/BlynkHandlers.cpp")

INCLUDE_DIRECTORIES(
	contrib/include
	${JSONC_INCLUDE_DIRS}
	blynk blynk/linux
	${MOSQUITTOPP_INCLUDE_DIRS}
)

add_executable(mqblynk src/main.cpp src/mq.cpp src/TopicMaps.cpp ${BLYNK_SOURCE})
target_link_libraries(mqblynk jsonpath ${JSONC_LIBRARIES} ${MOSQUITTOPP_LIBRARIES})

INSTALL(TARGETS mqblynk RUNTIME DESTINATION ${INSTALL_BIN_DIR})

