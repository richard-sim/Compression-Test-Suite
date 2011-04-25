#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

namespace CompressionInterop
{
	public enum class Algorithm
	{
		Doboz,
		Yappy,
		QuickLZ,
		FastLZ,
		LZF,
		Snappy,
		LZ4,
	};

	public ref class Stats
	{
	public:
		int		UncompressedDataSize;
		int		CompressedDataSize;
		int		TemporaryBufferSize;
		double	ElapsedTime;
	};

	public ref class Suite
	{
	public:
		bool Compress(String^ inputFileName, Algorithm algorithm, String^ outputFileName, Stats^% stats);

		bool Decompress(String^ inputFileName, [Out] array<Byte>^% uncompressedData, Stats^% stats);
	};
}
