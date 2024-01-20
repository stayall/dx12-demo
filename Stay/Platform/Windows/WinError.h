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
		WindowException(const char* filePath, int line, const char* errorMsg);
		virtual const char* what() const noexcept;
		virtual const char* getType() const noexcept;
		virtual const std::string getOriginString() const noexcept;
		const std::string getErrorInfo() const noexcept;
		const int getErrorId() const noexcept;
		virtual ~WindowException() = default;


	private:
		HRESULT hr;
		std::string m_errorMsg;
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
			ErrorTraceW(__FILEW__, __LINE__, L##x, true);\
		}
#endif // !hr



#else
#ifndef HR
#define HR(x) x


#endif // !hr

#endif // 

}


#ifndef _RELEASE
inline void PrintSubMessage() {}
inline void PrintSubMessage(const char* format, ...)
{
	char buffer[256];
	va_list ap;
	va_start(ap, format);
	vsprintf_s(buffer, 256, format, ap);
	va_end(ap);
	OutputDebugStringA(buffer);
	OutputDebugStringA("   ");
}
#endif

#ifdef _DEBUG
#define OUTPUTSTR(format, ...)


#define THROW_LASTEXCEPTION() \
	throw stay::WindowException(__FILE__, __LINE__, GetLastError())

#define THROW_EXCEPTION_INFO(info) \
	throw stay::WindowException(__FILE__, __LINE__, info)

#define ASSERT(hr, ...)\
do {\
	if (!(bool)(hr)) \
	{\
		PrintSubMessage(#hr);\
		PrintSubMessage(__VA_ARGS__);\
		__debugbreak(); \
	}\
} while (0)

#define USER_ASSERT(hr, ...)\
do {\
	if (!(bool)(hr)) \
	{\
		PrintSubMessage(#hr);\
		PrintSubMessage(__VA_ARGS__);\
	THROW_EXCEPTION_INFO(#hr);\
		; \
	}\
} while (0)

#define ERRORSTR(e) e.what()
#define CHWON_EXCEPTION(hr) stay::WindowException(__FILE__, __LINE__, hr)

#define THROW_IF_FAILED(hr) \
	do {if (hr != S_OK) \
	{\
		auto e = CHWON_EXCEPTION(hr);\
		OutputDebugStringA(ERRORSTR(e));\
		throw e;\
	}}while(0)

#elif _RELEASE
#define THROW_IF_FAILED(hr) (void)(hr)
#define ASSERT(hr, ...) (void)(hr)
#define THROW_LASTEXCEPTION()
#endif // _DEBUG

#endif // !_STAY_WINDOWS_ERROR_H_

