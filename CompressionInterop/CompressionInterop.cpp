#include "stdafx.h"
#include <msclr/marshal.h>
#include "CompressionSuite.h"
#include "CompressionInterop.h"
#include <vector>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;
using namespace msclr::interop;


namespace CompressionInterop
{
	bool Suite::Compress(String^ inputFileName, Algorithm algorithm, String^ outputFileName, Stats^% stats)
	{
		msclr::interop::marshal_context^ inputContext = gcnew msclr::interop::marshal_context();
		const char* _inputFileName = inputContext->marshal_as<const char*>(inputFileName);

		msclr::interop::marshal_context^ outputContext = gcnew msclr::interop::marshal_context();
		const char* _outputFileName = outputContext->marshal_as<const char*>(outputFileName);

		CompressionSuite::CompressionStats _stats;
		bool result = CompressionSuite::Compress(_inputFileName, (CompressionSuite::Algorithm)algorithm, _outputFileName, &_stats);

		stats->UncompressedDataSize = _stats.UncompressedDataSize;
		stats->CompressedDataSize = _stats.CompressedDataSize;
		stats->TemporaryBufferSize = _stats.TemporaryBufferSize;
		stats->ElapsedTime = _stats.ElapsedTime;

		delete outputContext;
		delete inputContext;

		GC::Collect();

		return result;
	}

	bool Suite::Decompress(String^ inputFileName, [Out] array<Byte>^% uncompressedData, Stats^% stats)
	{
		msclr::interop::marshal_context^ inputContext = gcnew msclr::interop::marshal_context();
		const char* _inputFileName = inputContext->marshal_as<const char*>(inputFileName);

		CompressionSuite::Decompressor decompressor;

		if (!decompressor.ReadCompressedData(_inputFileName))
		{
			delete inputContext;
			return false;
		}
		delete inputContext;

		int uncompressedDataSize;
		if (!decompressor.QueryUncompressedDataSize(&uncompressedDataSize))
			return false;

		// Temporary allocation +16b as Yappy will potentially copy past the end
		// of the data due to its use of intrinsics for a fast 16b copy.
		// This also covers LZ4's requirement of +3b of possible overwrite.
		array<Byte>^ tmpUncompressedData = gcnew array<Byte>(uncompressedDataSize+16);
		System::Runtime::InteropServices::GCHandle h = System::Runtime::InteropServices::GCHandle::Alloc(tmpUncompressedData, System::Runtime::InteropServices::GCHandleType::Pinned);

		bool result = false;
		try
		{
			char* pUncompressedData = (char*)h.AddrOfPinnedObject().ToPointer();

			CompressionSuite::CompressionStats _stats;
			result = decompressor.Decompress(pUncompressedData, uncompressedDataSize, &_stats);

			stats->UncompressedDataSize = _stats.UncompressedDataSize;
			stats->CompressedDataSize = _stats.CompressedDataSize;
			stats->TemporaryBufferSize = _stats.TemporaryBufferSize;
			stats->ElapsedTime = _stats.ElapsedTime;
		}
		finally
		{
			h.Free();
		}

		uncompressedData = gcnew array<Byte>(uncompressedDataSize);
		array<Byte>::Copy(tmpUncompressedData, uncompressedData, uncompressedDataSize);

		GC::Collect();

		return result;
	}
}
