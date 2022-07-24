#include "GMFS.h"

#include "filesystem.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min
#undef max

static IFileSystem* pFileSystem = nullptr;

FILESYSTEM_STATUS FileSystem::LoadFileSystem()
{
	HMODULE filesystem = GetModuleHandle(TEXT("filesystem_stdio.dll"));
	if (filesystem == nullptr) return FILESYSTEM_STATUS::MODULELOAD_FAILED;

	CreateInterfaceFn createInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(
		filesystem,
		"CreateInterface"
	));
	if (createInterface == nullptr) return FILESYSTEM_STATUS::GETPROCADDR_FAILED;

	int retcode;
	pFileSystem = static_cast<IFileSystem*>(createInterface("VFileSystem022", &retcode));
	return (retcode == IFACE_OK && pFileSystem != nullptr) ? FILESYSTEM_STATUS::OK : FILESYSTEM_STATUS::CREATEINTERFACE_FAILED;
}

bool FileSystem::Exists(const char* file, const char* path)
{
	if (pFileSystem == nullptr) return false;
	return pFileSystem->FileExists(file, path);
}

FileHandle_t FileSystem::Open(const char* file, const char* mode, const char* path)
{
	if (pFileSystem == nullptr) return nullptr;
	return pFileSystem->Open(file, mode, path);
}

uint32_t FileSystem::Size(FileHandle_t pFile)
{
	if (pFileSystem == nullptr) return 0;
	return pFileSystem->Size(pFile);
}

void FileSystem::Read(uint8_t* pData, uint32_t bytes, FileHandle_t pFile)
{
	if (pFileSystem == nullptr) return;
	pFileSystem->Read(pData, bytes, pFile);
}

void FileSystem::Close(FileHandle_t pFile)
{
	if (pFileSystem == nullptr) return;
	pFileSystem->Close(pFile);
}

