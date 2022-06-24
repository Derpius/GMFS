#include "GMFS.h"

#include "filesystem.h"
#include "GarrysMod/InterfacePointers.hpp"

static IFileSystem* pFileSystem = nullptr;

bool FileSystem::LoadFileSystem()
{
	pFileSystem = InterfacePointers::FileSystem();
	return pFileSystem == nullptr;
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

