#ifndef __COMPRESSION_SUITE_H__
#define __COMPRESSION_SUITE_H__

struct CompressionSuite
{
	enum Algorithm
	{
		ALG_DOBOZ,
		ALG_YAPPY,
		ALG_QUICKLZ,
		ALG_FASTLZ,
		ALG_LZF,
		ALG_SNAPPY,
		ALG_LZ4,
		ALG_ZLIB,
		ALG_LZHAM,
		ALG_LZMA,

		ALG_Count,
		ALG_Invalid=ALG_Count
	};

	struct CompressionStats
	{
		int		UncompressedDataSize;
		int		CompressedDataSize;
		int		TemporaryBufferSize;
		double	ElapsedTime;
	};

	struct FileHeader
	{
		Algorithm		Algorithm;
		int				UncompressedDataSize;
	};

	static bool Compress(const char* inputFileName, const Algorithm algorithm, const char* outputFileName, CompressionStats* pStats);

	class Decompressor
	{
	public:
		Decompressor();
		~Decompressor();

		bool ReadCompressedData(const char* inputFileName);
		bool QueryUncompressedDataSize(int* pUncompressedDataSize);
		bool Decompress(char* pOutputBuffer, const int outputBufferSize, CompressionStats* pStats);
		void FreeCompressedData();

	private:
		char*		mpInputBuffer;
		int			mCompressedDataSize;

		FileHeader	mHeader;
	};
};

#endif // __COMPRESSION_SUITE_H__
