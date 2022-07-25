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

if ("${ARCHITECTURE}" STREQUAL "x86")
	set(MSBUILD_PLATFORM "Win32")
	set(MSBUILD_FOLDER "x86/${MSBUILD_TARGET}")
	set(DLL_SUFFIX "32")
elseif ("${ARCHITECTURE}" STREQUAL "x64")
	set(MSBUILD_PLATFORM "x64")
	set(MSBUILD_FOLDER "x86_64/${MSBUILD_TARGET}")
	set(DLL_SUFFIX "64")
else ()
	message(WARNING "ARCHITECTURE cache variable not set to a valid value (x86, x64), defaulting to x64")
	set(MSBUILD_PLATFORM "x64")
	set(MSBUILD_FOLDER "x86_64/${MSBUILD_TARGET}")
endif ()

set(LIBRARY "${PREMAKE_PROJECT_DIR}/${MSBUILD_FOLDER}/gmcl_GMFS_win${DLL_SUFFIX}.lib")

set(SRCMIN_LIBS "${PROJECT_SOURCE_DIR}/garrysmod_common/sourcesdk-minimal/projects/${PREMAKE_PLATFORM}/${PREMAKE_TARGET}/${MSBUILD_FOLDER}")
if ("${ARCHITECTURE}" STREQUAL "x86")
	set(SRCMIN_PRECOMP_LIBS "${PROJECT_SOURCE_DIR}/garrysmod_common/sourcesdk-minimal/lib/public")
elseif ("${ARCHITECTURE}" STREQUAL "x64")
	set(SRCMIN_PRECOMP_LIBS "${PROJECT_SOURCE_DIR}/garrysmod_common/sourcesdk-minimal/lib/public/x64")
else ()
	set(SRCMIN_PRECOMP_LIBS "${PROJECT_SOURCE_DIR}/garrysmod_common/sourcesdk-minimal/lib/public/x64")
endif ()

add_custom_command(
	OUTPUT "${SRCMIN_LIBS}/tier1.lib" "${LIBRARY}"
	COMMAND msbuild ${PREMAKE_PROJECT_DIR}/GMFS.sln
		-target:Rebuild
		-property:Configuration=${MSBUILD_TARGET}
		-property:Platform=${MSBUILD_PLATFORM}
		-property:ConfigurationType=StaticLibrary
		-property:TargetExt=.lib
		-property:WholeProgramOptimization=false
		-maxCpuCount
)

add_custom_target(
	GMFS ALL
	DEPENDS "${SRCMIN_LIBS}/tier1.lib" "${LIBRARY}"
)

set(
	GMFS_LIBS

	"${LIBRARY}"

	"${SRCMIN_PRECOMP_LIBS}/tier0.lib"
	"${SRCMIN_PRECOMP_LIBS}/vstdlib.lib"
	"${SRCMIN_LIBS}/tier1.lib"

	PARENT_SCOPE
)
