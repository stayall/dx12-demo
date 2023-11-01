#include "WinHelper.h"

namespace stay
{
	std::wstring CharacterConversion::MultiByteToWideChar(const LPCSTR str)
	{
		WCHAR* pwStr = nullptr;
		int size = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, nullptr, 0);
		pwStr = new WCHAR[size];
		::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, pwStr, size);
		
		std::wstring ws(pwStr);
		delete[] pwStr;

		return ws;
	}
	std::string CharacterConversion::WideCharToMultiByte(const LPCWSTR wstr)
	{
		int iSize;
		char* pszMultiByte;

		iSize = ::WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
		pszMultiByte = new char[iSize];
		::WideCharToMultiByte(CP_ACP, 0, wstr, -1, pszMultiByte, iSize, NULL, NULL);

		std::string errorInfo(pszMultiByte);
		delete[] pszMultiByte;

		return errorInfo;
	}
}
