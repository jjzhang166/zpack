#include "zpack.h"
#include "zpPackage.h"
#include "zpFile.h"
#include <fstream>
#include "zlib/zlibEx.h"
using namespace std;

namespace zp
{

///////////////////////////////////////////////////////////////////////////////////////////////////
IPackage* open(const Char* filename, u32 flag)
{
	Package* package = new Package(filename, 
									(flag & OPEN_READONLY) != 0,
									(flag & OPEN_NO_FILENAME) == 0);
	if (!package->valid())
	{
		delete package;
		package = NULL;
	}
	return package;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void close(IPackage* package)
{
	delete static_cast<Package*>(package);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
IPackage* create(const Char* filename, u32 chunkSize, u32 fileUserDataSize)
{
	fstream stream;
	locale loc = locale::global(locale(""));
	stream.open(filename, ios_base::out | ios_base::trunc | ios_base::binary);
	locale::global(loc);
	if (!stream.is_open())
	{
		return NULL;
	}
	u64 iPackageHeader = sizeof(PackageHeader);
	PackageHeader header;
	header.sign = PACKAGE_FILE_SIGN;
	header.version = CURRENT_VERSION;
	header.mainVersion = 0;
	header.oldSubVersion = 0;
	header.subVersion = 0;
	header.headerSize = iPackageHeader;
	header.setfileCount(0);
	header.fileEntryOffset = iPackageHeader;
	header.filenameOffset = iPackageHeader;
	header.allFileEntrySize = 0;
	header.allFilenameSize = 0;
	header.originFilenamesSize = 0;
	header.chunkSize = chunkSize;
	header.resFormat = 0;
#ifdef ZP_USE_WCHAR
	header.flag = PACK_UNICODE;
#else
	header.flag = 0;
#endif
	header.fileEntrySize = sizeof(FileEntry) + fileUserDataSize;
	memset(header.reserved, 0, sizeof(header.reserved));

	stream.write((char*)&header, sizeof(header));
	stream.close();

	return open(filename, 0);
}
bool uncompressData(u8* dest, u32 *destLen, u8 *source, u32 sourceLen)
{
	return (Z_OK == uncompress(dest, destLen, source, sourceLen));
}

#define BUFFER_INC_FACTOR (2)
int uncompressMemoryWithHint( unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int *outLength, unsigned int outLenghtHint )
{
	/* ret value */
	int err = Z_OK;

	int bufferSize = outLenghtHint;
	*out = new unsigned char[bufferSize];

	z_stream d_stream; /* decompression stream */    
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = in;
	d_stream.avail_in = inLength;
	d_stream.next_out = *out;
	d_stream.avail_out = bufferSize;

	/* window size to hold 256k */
	if( (err = inflateInit3(&d_stream, 15 + 32)) != Z_OK )
		return err;

	for (;;) 
	{
		err = inflate1(&d_stream, Z_NO_FLUSH);

		if (err == Z_STREAM_END)
		{
			break;
		}

		switch (err) 
		{
		case Z_NEED_DICT:
			err = Z_DATA_ERROR;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			inflateEndEx(&d_stream);
			return err;
		}

		// not enough memory ?
		if (err != Z_STREAM_END) 
		{
			delete [] *out;
			*out = new unsigned char[bufferSize * BUFFER_INC_FACTOR];

			/* not enough memory, ouch */
			if (! *out ) 
			{
				//CCLOG("cocos2d: ZipUtils: realloc failed");
				inflateEndEx(&d_stream);
				return Z_MEM_ERROR;
			}

			d_stream.next_out = *out + bufferSize;
			d_stream.avail_out = bufferSize;
			bufferSize *= BUFFER_INC_FACTOR;
		}
	}

	*outLength = bufferSize - d_stream.avail_out;
	err = inflateEndEx(&d_stream);
	return err;
}
}
