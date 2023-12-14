#pragma once

#include "../win.h"
#include <fstream>

namespace stay::File
{

	enum FileAcceseType
	{
		ReadFile = GENERIC_READ,
		WriteFile = GENERIC_WRITE,
		RWFile = GENERIC_READ | GENERIC_WRITE
	};

	class AsycnIOStream
	{
	public:
		AsycnIOStream() { m_overlappen = {}; };
		virtual ~AsycnIOStream() = default;

		virtual DWORD AsycnRead() {};
		virtual DWORD AsycnWrite() {};
		DWORD GetFileSize();
		void Flush();
		void Cancel();

	protected:
		OVERLAPPED m_overlappen;

		HANDLE m_file;
	};

}