#ifndef _ZpWriteCompressFile_h__
#define _ZpWriteCompressFile_h__

#include "zpack.h"

namespace zp
{
	class Package;

	class ZpWriteCompressFile : public IWriteFile
	{
	public:
		ZpWriteCompressFile(Package* package, u64 offset, u32 originalsize, u32 chunkSize, u32 flag, u64 nameHash);
		virtual ~ZpWriteCompressFile();

		virtual u32 size() const;
		virtual u32 flag() const;
		virtual void seek(u32 pos);
		virtual u32 tell() const;
		virtual u32 write(const u8* buffer, u32 size);

	private:
		void seekInPackage();

	private:
		Package*	m_package;
		u64			m_offset;
		u32			m_flag;
		u32			m_originalsize;
		u32			m_compressSize;
		u32			m_chunkSize;
		u64			m_nameHash;
		u32			m_writePos;
	};
};

#endif // _ZpWriteCompressFile_h__
