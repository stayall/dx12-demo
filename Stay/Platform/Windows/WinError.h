#ifndef _STAY_WINDOWS_ERROR_H_
#define _STAY_WINDOWS_ERROR_H_

#include "WinStd.h"
#include "Exception.h"

namespace stay
{


	class WindowException : public Exception
	{
	public:
		static std::string TranslateError(HRESULT hr);

	public:
		WindowException(const char* filePath, int line, HRESULT hr);
		virtual const char* what() const noexcept;
		virtual const char* getType() const noexcept;
		virtual const std::string getOriginString() const noexcept;
		const std::string getErrorInfo() const noexcept;
		const int getErrorId() const noexcept;
		virtual ~WindowException() = default;


	private:
		HRESULT hr;
	};

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

}

#define THROW_LASTEXCEPTION() \
	throw stay::WindowException(__FILE__, __LINE__, GetLastError())
#define CHWON_EXCEPTION(hr) stay::WindowException(__FILE__, __LINE__, hr)
#define THROW_IF_FAILED(hr) \
	do {if (hr != S_OK) \
	{\
		throw CHWON_EXCEPTION(hr);\
	}}while(0)

#endif // !_STAY_WINDOWS_ERROR_H_
