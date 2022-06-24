#pragma once

#include <stdint.h>

typedef void* FileHandle_t;

namespace FileSystem
{
	bool LoadFileSystem();

	bool Exists(const char* file, const char* path = nullptr);
	FileHandle_t Open(const char* file, const char* mode, const char* path = nullptr);

	uint32_t Size(FileHandle_t pFile);
	void Read(uint8_t* pData, uint32_t bytes, FileHandle_t pFile);
	void Close(FileHandle_t pFile);
};
