PROJECT_GENERATOR_VERSION = 3

include("./garrysmod_common")

CreateWorkspace({name = "GMFS", abi_compatible = true, path = "build/premake/" .. os.target() .. "/" .. _ACTION})

CreateProject({serverside = false, source_path = "source"})
IncludeLuaShared()
IncludeScanning()
IncludeDetouring()
IncludeHelpersExtended()
IncludeSDKCommon()
IncludeSDKTier0()
IncludeSDKTier1()
