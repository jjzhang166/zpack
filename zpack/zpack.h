#ifndef __ZPACK_H__
#define __ZPACK_H__

#include <string>

//#if defined (_MSC_VER) && defined (UNICODE)
//	#define ZP_USE_WCHAR
//#endif

//#if defined (_MSC_VER)
	#define ZP_CASE_SENSITIVE	0
//#else
//	#define ZP_CASE_SENSITIVE	1
//#endif

namespace zp
{
#if defined (ZP_USE_WCHAR)
	typedef wchar_t Char;
	#ifndef _T
		#define _T(str) L##str
	#endif
	typedef std::wstring String;
	#define Fopen _wfopen
	#define Strcpy wcscpy_s
#else
	typedef char Char;
	#ifndef _T
		#define _T(str) str
	#endif
	typedef std::string String;
	#define Fopen fopen
	#define Strcpy strcpy
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

const u32 MAX_FILENAME_LEN = 1024;

const u32 OPEN_READONLY = 1;
const u32 OPEN_NO_FILENAME = 2;

const u32 PACK_UNICODE = 1;

const u32 FILE_DELETE = (1<<0);
const u32 FILE_COMPRESS = (1<<1);
//const u32 FILE_WRITING = (1<<2);

const u32 FILE_FLAG_USER0 = (1<<10);
const u32 FILE_FLAG_USER1 = (1<<11);

enum eResFormat
{
	eResFormatPVR = 0,
	eResFormatPVRTC4,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
* Version Sample: a.b.c
* a是主版本号, 对应mainVersion
* b,c是分级的次版本号，各占8位，由subVersion提取
*/

// VS默认的是8字节对齐，GCC默认的是4字节对齐，在IOS下, sizeof(PackageHeader)=140,而在android在win32是144
// 这里要强制WIN32和android也用4字节对齐，保证数据在不同平台的一致性
// 切记要用#pragma pack()恢复原来的对齐，不然可能导致其他奇怪的问题！！！！
#pragma pack(4)
struct PackageHeader
{
	u32	sign;
	u32	version;
	u8 mainVersion;				//主版本号
	u32 subVersion;				//次版本号
	u32 oldSubVersion;			//旧次版本号
	u32	headerSize;
	u32	fileCount;
	u64	fileEntryOffset;
	u64 filenameOffset;
	u32	allFileEntrySize;
	u32 allFilenameSize;
	u32 originFilenamesSize;	//filename size before compression
	u32 chunkSize;				//file compress unit
	u32	flag;
	u32 fileEntrySize;
	u8 resFormat;				//资源格式PVR/PVRTC4
	u32 unkown[1];
	u32 reserved[18];
	u32 getFileCount()
	{
		return fileCount;
	}
	void setFileCount(u32 v)
	{
		fileCount = v;
	}
};
#pragma pack()
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FileEntry
{
	u64	byteOffset;
	u64	nameHash;
	u32	packSize;	//size in package(may be compressed)
	u32 originSize;
	u32 flag;
	u32 chunkSize;	//can be different with chunkSize in package header
	u64 contentHash;
	u32 availableSize;
	u32 reserved;
};
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef bool (*Callback)(const Char* path, zp::u32 fileSize, void* param);

class IReadFile;
class IWriteFile;

///////////////////////////////////////////////////////////////////////////////////////////////////
class IPackage
{
public:
	virtual bool readonly() const = 0;

	virtual const Char* packageFilename() const = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	//readonly functions, not available when package is dirty
	//IFile will become unavailable after package is modified

	virtual bool hasFile(const Char* filename) const = 0;
	virtual IReadFile* openFile(const Char* filename) = 0;
	virtual void closeFile(IReadFile* file) = 0;

	virtual u32 getFileCount() const = 0;
	virtual bool getFileInfo(u32 index, Char* filenameBuffer, u32 filenameBufferSize, u32* fileSize = 0,
							u32* packSize = 0, u32* flag = 0, u32* availableSize = 0, u64* contentHash = 0) const = 0;
	virtual bool getFileInfo(const Char* filename, u32* fileSize = 0, u32* packSize = 0,
							u32* flag = 0, u32* availableSize = 0, u64* contentHash = 0) const = 0;

	///////////////////////////////////////////////////////////////////////////////////////////////
	//package manipulation fuctions, not available in read only mode

	//do not add same file more than once between flush() call
	//outFileSize	origin file size
	//outPackSize	size in package
	virtual bool addFile(const Char* filename, const Char* externalFilename, u32 fileSize, u32 flag,
						u32* outPackSize = 0, u32* outFlag = 0, u32 chunkSize = 0) = 0;

	virtual bool addFileStream(const Char* filename, const Char* exterFilename, u32 fileSize, u32 flag,
		u32* outPackSize = 0, u32* outFlag = 0, u32 chunkSize = 0) = 0;

	virtual bool addFileBuf(const Char* filename, const u8* fileBuf, u32 fileSize, u32 flag,
		u32* outPackSize = 0, u32* outFlag = 0, u32 chunkSize = 0) = 0;

	virtual IWriteFile* createFile(const Char* filename, u32 fileSize, u32 packSize,
									u32 chunkSize = 0, u32 flag = 0, u64 contentHash = 0) = 0;
	virtual IWriteFile* openFileToWrite(const Char* filename) = 0;
	virtual void closeFile(IWriteFile* file) = 0;

	//can not remove files added after last flush() call
	virtual bool removeFile(const Char* filename) = 0;

	//return true if there's any unsaved change of package
	virtual bool dirty() const = 0;

	//package file won't change before calling this function
	virtual void flush() = 0;

	virtual bool defrag(Callback callback, void* callbackParam) = 0;	//can be very slow, don't call this all the time

	virtual u32 getFileUserDataSize() const = 0;

	virtual bool writeFileUserData(const Char* filename, const u8* data, u32 dataLen) = 0;
	virtual bool readFileUserData(const Char* filename, u8* data, u32 dataLen) = 0;
	// add by [4/10/2013 Panruixing]
	virtual void setMainVersion(u8 version) = 0;
	virtual u8	 getMainVersion() const = 0;
	virtual void setSubVersion(u32 version) = 0;
	virtual u32	 getSubVersion() const = 0;
	virtual void setOldSubVersion(u32 version) = 0;
	virtual u32  getOldSubVersion() const = 0;
protected:
	virtual ~IPackage(){}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class IReadFile
{
public:
	virtual u32 size() const = 0;

	virtual u32 availableSize() const = 0;

	virtual u32 flag() const = 0;

	virtual void seek(u32 pos) = 0;

	virtual u32 tell() const = 0;

	virtual u32 read(u8* buffer, u32 size) = 0;

protected:
	virtual ~IReadFile(){}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class IWriteFile
{
public:
	virtual u32 size() const = 0;

	virtual u32 flag() const = 0;

	virtual void seek(u32 pos) = 0;

	virtual u32 tell() const = 0;

	virtual u32 write(const u8* buffer, u32 size) = 0;

protected:
	virtual ~IWriteFile(){}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

IPackage* create(const Char* filename, u32 chunkSize = 0x40000, u32 fileUserDataSize = 0);
IPackage* open(const Char* filename, u32 flag = OPEN_READONLY | OPEN_NO_FILENAME);
void close(IPackage* package);
bool  uncompressData(u8* dest, u32 *destLen, u8 *source, u32 sourceLen);
int  uncompressMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int *outLength, unsigned int outLenghtHint);
}

#endif
