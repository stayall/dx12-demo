#ifndef _STAY_WINDOWS_ERROR_H_
#define _STAY_WINDOWS_ERROR_H_

#include "WinStd.h"

HRESULT WINAPI ErrorTraceW(
	_In_z_ const WCHAR* strFile,
	_In_ DWORD dwLine,
	_In_ HRESULT hr,
	_In_opt_ const WCHAR* strMsg,
	_In_ bool bPopMsgBox);

#if defined(_DEBUG) || defined(DEBUG)
#ifndef HR
#define HR(x) \
		HRESULT hr = x; \
		if (FAILED(hr))\
		{\
			ErrorTraceW(__FILEW__, __LINE__, L#x, true);\
		}
#endif // !hr
#else
#ifndef HR
#define HR(x) x


#endif // !hr

#endif // 



#endif // !_STAY_WINDOWS_ERROR_H_
