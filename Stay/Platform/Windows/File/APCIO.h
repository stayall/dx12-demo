#pragma once

#include "../WinStd.h"
#include <fstream>

namespace stay::File
{

	enum FileAcceseType
	{
		ReadFile = GENERIC_READ,
		WriteFile = GENERIC_WRITE,
		RWFile = GENERIC_READ | GENERIC_WRITE
	};

	class IOStreamBase
	{
	public:
		IOStreamBase();
	
		virtual ~IOStreamBase();

		virtual DWORD AsycnRead(void* buffer, DWORD size) = 0;
		virtual DWORD AsycnWrite() = 0;
		DWORD GetFileSize();
		void Flush();
		void Cancel();

	protected:
		FileAcceseType m_acceseType;
		std::wstring m_filePath;
		OVERLAPPED m_overlappen;
		uint8_t* m_buffer;
		HANDLE m_file;
		HANDLE m_wait;
	};

	class APCIOSteam : public IOStreamBase
	{
		APCIOSteam() = default;
		APCIOSteam(LPCWSTR fileName, DWORD accese = FileAcceseType::RWFile, DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_DELETE, DWORD createoption = CREATE_NEW);

		virtual ~APCIOSteam() = default;

		virtual DWORD AsycnRead(void* buffer, DWORD size);
		virtual DWORD AsycnWrite() {};
	};

}