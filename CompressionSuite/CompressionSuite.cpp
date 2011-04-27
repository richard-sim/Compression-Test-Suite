#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <memory.h>
#include <string.h>
#include <wchar.h>

#include "Doboz/Compressor.h"
#include "Doboz/Decompressor.h"

#include "Yappy/yappy.h"

#include "QuickLZ/quicklz.h"

#include "FastLZ/fastlz.h"

#include "LZF/lzf.h"

#include "Snappy/snappy.h"

#include "LZ4/LZ4.h"

//#include "winpthreads.h"
#include "TimerWin.h"

#include "CompressionSuite.h"


const unsigned int kYappyBlockSize = (1 << 16);


bool CompressionSuite::Compress(const char* inputFileName, const Algorithm algorithm, const char* outputFileName, CompressionStats* pStats)
{
	if (pStats)
	{
		pStats->UncompressedDataSize = 0;
		pStats->CompressedDataSize = 0;
		pStats->TemporaryBufferSize = 0;
		pStats->ElapsedTime = 0.0;
	}

	struct _stat st;
	_stat(inputFileName, &st);
	int uncompressedDataSize = st.st_size;
	if (pStats)
		pStats->UncompressedDataSize = uncompressedDataSize;

	char* pInputBuffer = new char[uncompressedDataSize];
	{
		FILE* f = fopen(inputFileName, "rb");
		if (!f)
		{
			delete[] pInputBuffer;
			return false;
		}

		fread(pInputBuffer, uncompressedDataSize, 1, f);

		fclose(f);
	}

	int temporaryBufferSize = 0;
	if (algorithm == ALG_DOBOZ)
	{
		temporaryBufferSize = (int)doboz::Compressor::getMaxCompressedSize(uncompressedDataSize);
	}
	else if (algorithm == ALG_YAPPY)
	{
		// A guess... the extra 2b per block is for the 'compressed block length' for decompression.
		// The extra 16 is because the sample code allocates 16 more per block, and we get memory
		// stomping otherwise. :/
		temporaryBufferSize = uncompressedDataSize + (uncompressedDataSize*2/kYappyBlockSize) + (uncompressedDataSize*16/kYappyBlockSize);
		// And this is because we still get stomping. :/
		temporaryBufferSize *= 2;
	}
	else if (algorithm == ALG_QUICKLZ)
	{
		temporaryBufferSize = uncompressedDataSize * 2;
	}
	else if (algorithm == ALG_FASTLZ)
	{
		// Must be no smaller than 66b, and at least 5% larger than uncompressed data
		temporaryBufferSize = 66 + uncompressedDataSize * 2;
	}
	else if (algorithm == ALG_LZF)
	{
		temporaryBufferSize = uncompressedDataSize * 2;
	}
	else if (algorithm == ALG_SNAPPY)
	{
		temporaryBufferSize = (int)snappy::MaxCompressedLength(uncompressedDataSize);
	}
	else if (algorithm == ALG_LZ4)
	{
		temporaryBufferSize = uncompressedDataSize * 2;
	}
	if (pStats)
		pStats->TemporaryBufferSize = temporaryBufferSize;

	char* pOutputBuffer = new char[temporaryBufferSize];
	size_t outputSize;

	double elapsedTime = 0.0;
	bool result = false;
	if (algorithm == ALG_DOBOZ)
	{
		doboz::Compressor compressor;
		Timer timer;

		timer.delta();
		result = (compressor.compress(pInputBuffer, uncompressedDataSize, pOutputBuffer, temporaryBufferSize, outputSize) == doboz::RESULT_OK);
		elapsedTime = timer.delta();
	}
	else if (algorithm == ALG_YAPPY)
	{
		Yappy_FillTables();
		unsigned char* pSrcData = (unsigned char*)pInputBuffer;
		unsigned char* pDstData = (unsigned char*)pOutputBuffer;

		Timer timer;

		timer.delta();
		//for (int offset=0; offset<uncompressedDataSize; offset+=kYappyBlockSize, pSrcData+=kYappyBlockSize)
		{
			unsigned int blockSize = uncompressedDataSize;//((uncompressedDataSize-offset) >= kYappyBlockSize) ? kYappyBlockSize : uncompressedDataSize-offset;
			
			unsigned int* pCompressedBlockSize = (unsigned int*)pDstData;
			pDstData += sizeof(unsigned int);

			unsigned char* pBlockEnd = Yappy_Compress(pSrcData, pDstData, blockSize);
			unsigned int compressedBlockSize = pBlockEnd - pDstData;
			*pCompressedBlockSize = static_cast<unsigned int>(compressedBlockSize);

			pDstData = pBlockEnd;
		}
		elapsedTime = timer.delta();

		outputSize = pDstData - (unsigned char*)pOutputBuffer;
		result = (outputSize <= (unsigned int)temporaryBufferSize) && (outputSize != 0);
	}
	else if (algorithm == ALG_QUICKLZ)
	{
		qlz_state_compress qlzState;
		memset(&qlzState, 0, sizeof(qlzState));
		Timer timer;

		timer.delta();
		outputSize = qlz_compress(pInputBuffer, pOutputBuffer, uncompressedDataSize, &qlzState);
		elapsedTime = timer.delta();

		result = (outputSize <= (unsigned int)temporaryBufferSize) && (outputSize != 0);
	}
	else if (algorithm == ALG_FASTLZ)
	{
		Timer timer;

		timer.delta();
		outputSize = fastlz_compress(pInputBuffer, uncompressedDataSize, pOutputBuffer);
		elapsedTime = timer.delta();

		result = (outputSize <= (unsigned int)temporaryBufferSize) && (outputSize != 0);
	}
	else if (algorithm == ALG_LZF)
	{
		Timer timer;

		timer.delta();
		outputSize = lzf_compress(pInputBuffer, uncompressedDataSize, pOutputBuffer, temporaryBufferSize);
		elapsedTime = timer.delta();

		result = (outputSize <= (unsigned int)temporaryBufferSize) && (outputSize != 0);
	}
	else if (algorithm == ALG_SNAPPY)
	{
		Timer timer;

		timer.delta();
		snappy::RawCompress(pInputBuffer, uncompressedDataSize, pOutputBuffer, &outputSize);
		elapsedTime = timer.delta();

		result = (outputSize <= (unsigned int)temporaryBufferSize) && (outputSize != 0);
	}
	else if (algorithm == ALG_LZ4)
	{
		Timer timer;

		timer.delta();
		outputSize = LZ4_compress(pInputBuffer, pOutputBuffer, uncompressedDataSize);
		elapsedTime = timer.delta();

		result = (outputSize <= (unsigned int)temporaryBufferSize) && (outputSize != 0);
	}

	if (pStats)
		pStats->ElapsedTime = elapsedTime;

	if (!result)
	{
		delete[] pOutputBuffer;
		delete[] pInputBuffer;
		return false;
	}

	int compressedDataSize = (int)outputSize;
	if (pStats)
		pStats->CompressedDataSize = compressedDataSize;

	FILE* of = fopen(outputFileName, "wb");
	if (!of)
	{
		delete[] pOutputBuffer;
		delete[] pInputBuffer;
		return false;
	}

	FileHeader header;
	header.Algorithm = algorithm;
	header.UncompressedDataSize = uncompressedDataSize;
	fwrite(&header, sizeof(header), 1, of);

	fwrite(pOutputBuffer, compressedDataSize, 1, of);

	fclose(of);
	delete[] pOutputBuffer;

	delete[] pInputBuffer;

	return true;
}


CompressionSuite::Decompressor::Decompressor()
	: mpInputBuffer(NULL)
	, mCompressedDataSize(0)
{
	mHeader.Algorithm = ALG_Invalid;
	mHeader.UncompressedDataSize = 0;
}

CompressionSuite::Decompressor::~Decompressor()
{
	FreeCompressedData();
}

void CompressionSuite::Decompressor::FreeCompressedData()
{
	if (mpInputBuffer != NULL)
	{
		delete[] mpInputBuffer;
		mpInputBuffer = NULL;
	}

	mHeader.Algorithm = ALG_Invalid;
	mHeader.UncompressedDataSize = 0;
}

bool CompressionSuite::Decompressor::ReadCompressedData(const char* inputFileName)
{
	FILE* f = fopen(inputFileName, "rb");
	if (!f)
		return false;

	fseek(f, 0, SEEK_END);
	mCompressedDataSize = ftell(f) - sizeof(FileHeader);
	fseek(f, 0, SEEK_SET);

	int sizeHeader = fread(&mHeader, sizeof(mHeader), 1, f);

	mpInputBuffer = new char[mCompressedDataSize];
	int sizeRead = fread(mpInputBuffer, mCompressedDataSize, 1, f);

	fclose(f);

	return true;
}

bool CompressionSuite::Decompressor::QueryUncompressedDataSize(int* pUncompressedDataSize)
{
	if (mHeader.Algorithm == ALG_Invalid)
		return false;

	*pUncompressedDataSize = mHeader.UncompressedDataSize;

	// Some algorithms have a way to query the uncompressed data size; use it to verify our header
	if (mHeader.Algorithm == ALG_DOBOZ)
	{
		doboz::Decompressor decompressor;
		doboz::CompressionInfo compressionInfo;
		doboz::Result result = decompressor.getCompressionInfo(mpInputBuffer, mCompressedDataSize, compressionInfo);
		if (result != doboz::RESULT_OK)
			return false;

		assert(mHeader.UncompressedDataSize == (int)compressionInfo.uncompressedSize);
	}
	else if (mHeader.Algorithm == ALG_SNAPPY)
	{
		size_t computedSize = 0;
		if (!snappy::GetUncompressedLength(mpInputBuffer, mCompressedDataSize, &computedSize))
			return false;

		assert(mHeader.UncompressedDataSize == (int)computedSize);
	}

	return true;
}

bool CompressionSuite::Decompressor::Decompress(char* pOutputBuffer, const int outputBufferSize, CompressionStats* pStats)
{
	assert(mHeader.Algorithm != ALG_Invalid);

	int uncompressedDataSize = mHeader.UncompressedDataSize;
	if (pStats)
	{
		pStats->CompressedDataSize = mCompressedDataSize;
		pStats->UncompressedDataSize = uncompressedDataSize;
	}

	if (uncompressedDataSize > outputBufferSize)
		return false;

	double elapsedTime = 0.0;
	bool result = false;
	if (mHeader.Algorithm == ALG_DOBOZ)
	{
		doboz::Decompressor decompressor;
		Timer timer;

		timer.delta();
		result = (decompressor.decompress(mpInputBuffer, mCompressedDataSize, pOutputBuffer, uncompressedDataSize) == doboz::RESULT_OK);
		elapsedTime = timer.delta();
	}
	else if (mHeader.Algorithm == ALG_YAPPY)
	{
		Timer timer;
		timer.delta();

		unsigned char* pSrcData = (unsigned char*)mpInputBuffer;
		unsigned char* pSrcDataEnd = (unsigned char*)mpInputBuffer + mCompressedDataSize;
		unsigned char* pDstData = (unsigned char*)pOutputBuffer;

		while (pSrcData < pSrcDataEnd)
		{
			unsigned int compressedBlockSize = *reinterpret_cast<unsigned int*>(pSrcData);
			pSrcData += sizeof(unsigned int);

			unsigned char* pSrcBlockEnd = pSrcData + compressedBlockSize;
			unsigned char* pDstBlockEnd = Yappy_UnCompress(pSrcData, pSrcBlockEnd, pDstData);

			pDstData = pDstBlockEnd;
			pSrcData = pSrcBlockEnd;
		};

		elapsedTime = timer.delta();

		result = ((pDstData == ((unsigned char*)pOutputBuffer + uncompressedDataSize)) &&
				  (pSrcData == pSrcDataEnd));
	}
	else if (mHeader.Algorithm == ALG_QUICKLZ)
	{
		qlz_state_decompress qlzState;
		memset(&qlzState, 0, sizeof(qlzState));
		Timer timer;

		timer.delta();
		unsigned int outSize = qlz_decompress(mpInputBuffer, pOutputBuffer, &qlzState);
		elapsedTime = timer.delta();

		result = (outSize == uncompressedDataSize);
	}
	else if (mHeader.Algorithm == ALG_FASTLZ)
	{
		Timer timer;

		timer.delta();
		int outSize = fastlz_decompress(mpInputBuffer, mCompressedDataSize, pOutputBuffer, uncompressedDataSize);
		elapsedTime = timer.delta();

		result = (outSize == uncompressedDataSize);
	}
	else if (mHeader.Algorithm == ALG_LZF)
	{
		Timer timer;

		timer.delta();
		int outSize = lzf_decompress(mpInputBuffer, mCompressedDataSize, pOutputBuffer, uncompressedDataSize);
		elapsedTime = timer.delta();

		result = (outSize == uncompressedDataSize);
	}
	else if (mHeader.Algorithm == ALG_SNAPPY)
	{
		Timer timer;

		timer.delta();
		result = snappy::RawUncompress(mpInputBuffer, mCompressedDataSize, pOutputBuffer);
		elapsedTime = timer.delta();
	}
	else if (mHeader.Algorithm == ALG_LZ4)
	{
		Timer timer;

		timer.delta();
		int outSize = LZ4_decode(mpInputBuffer, pOutputBuffer, mCompressedDataSize);
		elapsedTime = timer.delta();

		result = (outSize == uncompressedDataSize);
	}

	if (pStats)
		pStats->ElapsedTime = elapsedTime;

	FreeCompressedData();

	return result;
}
