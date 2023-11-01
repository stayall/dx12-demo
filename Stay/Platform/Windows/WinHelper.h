#ifndef _STAY_WINDOWS_HELPER_H_
#define _STAY_WINDOWS_HELPER_H_

#include "WinStd.h"
#include <string>

namespace stay
{
	class CharacterConversion
	{
	public:
		static std::wstring MultiByteToWideChar(const LPCSTR str);
		static std::string WideCharToMultiByte(const LPCWSTR wstr);
	};
}

#endif // !_STAY_WINDOWS_HELPER_H_

