#include "GMFS.h"

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#undef min
	#undef max
#elif defined(__linux__)
	#include <dlfcn.h>
#else
#error Target operating system not supported
#endif

#include <cstdio>

typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);

enum
{
	IFACE_OK = 0,
	IFACE_FAILED
};

enum FileSystemSeek_t
{
	FILESYSTEM_SEEK_HEAD = SEEK_SET,
	FILESYSTEM_SEEK_CURRENT = SEEK_CUR,
	FILESYSTEM_SEEK_TAIL = SEEK_END,
};

class IBaseFileSystem
{
public:
	virtual int				Read(void* pOutput, int size, FileHandle_t file) = 0;
	virtual int				Write(void const* pInput, int size, FileHandle_t file) = 0;

	// if pathID is NULL, all paths will be searched for the file
	virtual FileHandle_t	Open(const char* pFileName, const char* pOptions, const char* pathID = 0) = 0;
	virtual void			Close(FileHandle_t file) = 0;


	virtual void			Seek(FileHandle_t file, int pos, FileSystemSeek_t seekType) = 0;
	virtual unsigned int	Tell(FileHandle_t file) = 0;
	virtual unsigned int	Size(FileHandle_t file) = 0;
	virtual unsigned int	Size(const char* pFileName, const char* pPathID = 0) = 0;

	virtual void			Flush(FileHandle_t file) = 0;
	virtual bool			Precache(const char* pFileName, const char* pPathID = 0) = 0;

	virtual bool			FileExists(const char* pFileName, const char* pPathID = 0) = 0;
	virtual bool			IsFileWritable(char const* pFileName, const char* pPathID = 0) = 0;
	virtual bool			SetFileWritable(char const* pFileName, bool writable, const char* pPathID = 0) = 0;

	virtual long			GetFileTime(const char* pFileName, const char* pPathID = 0) = 0;

	//--------------------------------------------------------
	// Reads/writes files to utlbuffers. Use this for optimal read performance when doing open/read/close
	//--------------------------------------------------------
	//virtual bool			ReadFile(const char* pFileName, const char* pPath, CUtlBuffer& buf, int nMaxBytes = 0, int nStartingByte = 0, FSAllocFunc_t pfnAlloc = NULL) = 0;
	//virtual bool			WriteFile(const char* pFileName, const char* pPath, CUtlBuffer& buf) = 0;
	//virtual bool			UnzipFile(const char* pFileName, const char* pPath, const char* pDestination) = 0;
};

static IBaseFileSystem* pFileSystem = nullptr;

#if defined(WIN32)
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
	pFileSystem = static_cast<IBaseFileSystem*>(createInterface("VBaseFileSystem011", &retcode));
	return (retcode == IFACE_OK && pFileSystem != nullptr) ? FILESYSTEM_STATUS::OK : FILESYSTEM_STATUS::CREATEINTERFACE_FAILED;
}
#elif defined(__linux__)
FILESYSTEM_STATUS FileSystem::LoadFileSystem()
{
	void* filesystem = dlopen("filesystem_stdio_client.so", RTLD_NOW | RTLD_NOLOAD);
	if (filesystem == nullptr) return FILESYSTEM_STATUS::MODULELOAD_FAILED;

	CreateInterfaceFn createInterface = reinterpret_cast<CreateInterfaceFn>(dlsym(
		filesystem,
		"CreateInterface"
	));
	if (createInterface == nullptr) return FILESYSTEM_STATUS::GETPROCADDR_FAILED;

	int retcode;
	pFileSystem = static_cast<IBaseFileSystem*>(createInterface("VBaseFileSystem011", &retcode));
	return (retcode == IFACE_OK && pFileSystem != nullptr) ? FILESYSTEM_STATUS::OK : FILESYSTEM_STATUS::CREATEINTERFACE_FAILED;
}
#endif

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

int FileSystem::Read(uint8_t* pData, uint32_t bytes, FileHandle_t pFile)
{
	if (pFileSystem == nullptr) return 0;
	return pFileSystem->Read(pData, bytes, pFile);
}

int FileSystem::Write(const uint8_t* pData, uint32_t bytes, FileHandle_t pFile)
{
	if (pFileSystem == nullptr) return 0;
	return pFileSystem->Write(pData, bytes, pFile);
}

void FileSystem::Close(FileHandle_t pFile)
{
	if (pFileSystem == nullptr) return;
	pFileSystem->Close(pFile);
}

