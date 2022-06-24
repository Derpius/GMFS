cmake_minimum_required (VERSION 3.20)

project("GMFS")

# Generate project
set(PREMAKE_TARGET vs2019)

execute_process(
	COMMAND premake5 ${PREMAKE_TARGET} --file=${PROJECT_SOURCE_DIR}/premake5.lua
	OUTPUT_VARIABLE PROCESS_OUTPUT
	RESULT_VARIABLE PROCESS_STATUS
)

if (NOT "${PROCESS_STATUS}" STREQUAL "0")
	message(STATUS "${PROCESS_OUTPUT}")
	message(FATAL_ERROR "premake generation failed with exit code ${PROCESS_STATUS}")
else ()
	message(VERBOSE "${PROCESS_OUTPUT}")
endif ()

if (WIN32)
	set(PREMAKE_PLATFORM "windows")
elseif (APPLE)
	set(PREMAKE_PLATFORM "macosx")
elseif (UNIX)
	set(PREMAKE_PLATFORM "linux")
else ()
	message(FATAL_ERROR "Unkown operating system")
endif ()

set(PREMAKE_PROJECT_DIR "${PROJECT_SOURCE_DIR}/build/premake/${PREMAKE_PLATFORM}/${PREMAKE_TARGET}")

# Build with MSBuild
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	set(MSBUILD_TARGET "Release")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
	set(MSBUILD_TARGET "ReleaseWithSymbols")
else ()
	message(FATAL_ERROR "Unsupported CMake build type: ${CMAKE_BUILD_TYPE}")
endif ()

if ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "x86")
	set(MSBUILD_PLATFORM "Win32")
	set(MSBUILD_FOLDER "x86/${MSBUILD_TARGET}")
elseif ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "x64")
	set(MSBUILD_PLATFORM "x64")
	set(MSBUILD_FOLDER "x86_64/${MSBUILD_TARGET}")
else ()
	message(WARNING "Unsupported CMake generator platform '${CMAKE_GENERATOR_PLATFORM}', defaulting to x64")
	set(MSBUILD_PLATFORM "x64")
	set(MSBUILD_FOLDER "x86_64/${MSBUILD_TARGET}")
endif ()

set(GMFS_LIBRARY ${PREMAKE_PROJECT_DIR}/${MSBUILD_FOLDER}/GMFS.lib)

add_custom_command(
	OUTPUT ${GMFS_LIBRARY}
	COMMAND msbuild ${PREMAKE_PROJECT_DIR}/GMFS.sln
		-target:gmcl_GMFS
		-property:Configuration=${MSBUILD_TARGET}
		-property:Platform=${MSBUILD_PLATFORM}
		-property:ConfigurationType=StaticLibrary
		-property:TargetName=GMFS
		-property:TargetExt=.lib
		-maxCpuCount
)

add_custom_target(
	GMFS ALL
	DEPENDS ${GMFS_LIBRARY}
)
