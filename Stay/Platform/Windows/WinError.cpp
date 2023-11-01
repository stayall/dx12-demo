#include "WinError.h"
#include <sstream>

namespace stay
{

	_Use_decl_annotations_
		HRESULT __stdcall ErrorTraceW(const WCHAR* strFile, DWORD dwLine, HRESULT hr, const WCHAR* strMsg, bool bPopMsgBox)
	{
		WCHAR strBufferFile[MAX_PATH];
		WCHAR strBufferLine[128];
		WCHAR strBufferError[300];
		WCHAR strBufferMsg[1024];
		WCHAR strBufferHR[40];
		WCHAR strBuffer[3000];

		swprintf_s(strBufferLine, 128, L"%lu", dwLine);
		if (strFile)
		{
			swprintf_s(strBuffer, 3000, L"%ls(%ls): ", strFile, strBufferLine);
			OutputDebugStringW(strBuffer);
		}

		size_t nMsgLen = (strMsg) ? wcsnlen_s(strMsg, 1024) : 0;
		if (nMsgLen > 0)
		{
			OutputDebugStringW(strMsg);
			OutputDebugStringW(L" ");
		}
		// Windows SDK 8.0起DirectX的错误信息已经集成进错误码中，可以通过FormatMessageW获取错误信息字符串
		// 不需要分配字符串内存
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			strBufferError, 256, nullptr);

		WCHAR* errorStr = wcsrchr(strBufferError, L'\r');
		if (errorStr)
		{
			errorStr[0] = L'\0';	// 擦除FormatMessageW带来的换行符(把\r\n的\r置换为\0即可)
		}

		swprintf_s(strBufferHR, 40, L" (0x%0.8x)", hr);
		wcscat_s(strBufferError, strBufferHR);
		swprintf_s(strBuffer, 3000, L"错误码含义：%ls", strBufferError);
		OutputDebugStringW(strBuffer);

		OutputDebugStringW(L"\n");

		if (bPopMsgBox)
		{
			wcscpy_s(strBufferFile, MAX_PATH, L"");
			if (strFile)
				wcscpy_s(strBufferFile, MAX_PATH, strFile);

			wcscpy_s(strBufferMsg, 1024, L"");
			if (nMsgLen > 0)
				swprintf_s(strBufferMsg, 1024, L"当前调用：%ls\n", strMsg);

			swprintf_s(strBuffer, 3000, L"文件名：%ls\n行号：%ls\n错误码含义：%ls\n%ls",
				strBufferFile, strBufferLine, strBufferError, strBufferMsg);

			int nResult = MessageBoxW(GetForegroundWindow(), strBuffer, L"错误", MB_YESNO | MB_ICONERROR);
			if (nResult == IDYES)
				DebugBreak();
		}

		return hr;
	}

	std::string WindowException::TranslateError(HRESULT hr)
	{
		const DWORD wSize = 128;
		WCHAR* strError = L"";

		DWORD size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
			nullptr,
			hr,
			MAKELANGID(LANG_USER_DEFAULT, LANG_SYSTEM_DEFAULT),
			(LPTSTR)&strError,
			0,
			nullptr);
		if (size == 0)
		{
			char strBuffer[wSize];
			sprintf_s(strBuffer, wSize, "Format message failed with 0x%x\n", GetLastError());

			return strBuffer;
		}

		int iSize;
		char* pszMultiByte;

		iSize = WideCharToMultiByte(CP_ACP, 0, strError, -1, NULL, 0, NULL, NULL);
		pszMultiByte = new char[iSize];
		WideCharToMultiByte(CP_ACP, 0, strError, -1, pszMultiByte, iSize, NULL, NULL);

		std::string errorInfo(pszMultiByte);
		delete[] pszMultiByte;

		LocalFree(strError);
		return errorInfo;
	}

	WindowException::WindowException(const char* filePath, int line, HRESULT hr)
		: Exception(filePath, line)
	{
		this->hr = hr;
	}

	const char* WindowException::what() const noexcept
	{
		std::stringstream ss;
		ss << getOriginString();
		whatBuffer = ss.str();

		return whatBuffer.c_str();
	}

	const char* WindowException::getType() const noexcept
	{
		return "Window Exception";
	}

	const std::string WindowException::getOriginString() const noexcept
	{
		std::stringstream ss;
		ss << Exception::getOriginString()
			<< "ErrorCode: " << getErrorId() << std::endl
			<< "ErrorInfo: " << getErrorInfo() << std::endl;

		return ss.str();
	}

	const std::string WindowException::getErrorInfo() const noexcept
	{
		return TranslateError(hr);
	}

	const int WindowException::getErrorId() const noexcept
	{
		return hr;
	}

}