#include "GMFS.h"

#include "filesystem.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef min
#undef max

static IFileSystem* pFileSystem = nullptr;

bool FileSystem::LoadFileSystem()
{
	CreateInterfaceFn createInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(
		GetModuleHandle("filesystem_stdio.dll"),
		"CreateInterface"
	));
	if (createInterface == nullptr) return false;

	int retcode;
	pFileSystem = static_cast<IFileSystem*>(createInterface("IFileSystem", &retcode));
	return retcode == IFACE_OK && pFileSystem != nullptr;
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

