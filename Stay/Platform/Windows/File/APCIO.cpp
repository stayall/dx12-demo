#include <filesystem>

#include "APCIO.h"
#include "../WinError.h"

namespace stay::File
{
	IOStreamBase::IOStreamBase()
	{
		ZeroMemory(&m_overlappen, sizeof(OVERLAPPED));
		m_wait = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_wait == nullptr)
		{
			THROW_LASTEXCEPTION();
		}

		m_overlappen.hEvent = m_wait;
	}
	IOStreamBase::~IOStreamBase()
	{
		if (m_file != nullptr) CloseHandle(m_file);
		if (m_wait != nullptr) CloseHandle(m_wait);
	}
	DWORD IOStreamBase::GetFileSize()
	{
		if (m_file == nullptr)
		{
			return 0;
		}

		ASSERT(::GetFileType(m_file) == FILE_TYPE_DISK, "Just Support Disk File");

		DWORD len = ::GetFileSize(m_file, nullptr);
		if (len == INVALID_FILE_SIZE)
		{
			THROW_LASTEXCEPTION();
		}

		return len;

	}
	APCIOSteam::APCIOSteam(LPCWSTR fileName, DWORD accese, DWORD shareMode, DWORD createoption)
	{
		ASSERT(wcslen(fileName) <= MAX_PATH);

		struct _stat buffer;
		if (_wstat(fileName, &buffer) == -1)
		{
			THROW_LASTEXCEPTION();
		}

		
		m_file = CreateFileW(fileName, accese, shareMode, nullptr, createoption, FILE_FLAG_OVERLAPPED, nullptr);
		if (m_file == INVALID_HANDLE_VALUE)
		{
			THROW_LASTEXCEPTION();
		}

		m_filePath = fileName;
		m_acceseType = FileAcceseType(accese);
	}
	DWORD APCIOSteam::AsycnRead(void* buffer, DWORD size)
	{
		auto count = GetFileSize();
		buffer = new uint8_t[count];
		(size);
		//::ReadFile(m_file, buffer, size, nullptr, &m_overlappen);

		return 0;
	}
}
