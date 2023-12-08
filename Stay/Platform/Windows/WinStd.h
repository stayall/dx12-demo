#ifndef _STATY_WIN_STANDAER_HEAD_FILE_H_
#define _STATY_WIN_STANDAER_HEAD_FILE_H_

#pragma warning(disable:4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable:4238) // nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable:4239) // A non-const reference may only be bound to an lvalue; assignment operator takes a reference to non-const
#pragma warning(disable:4324) // structure was padded due to __declspec(align())
#pragma warning(disable:26495) // 

#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN


#ifndef UNICODE
#define UNICODE
#endif  // !UNICODE

#include <windows.h>
#include <wrl.h>
#include <string>
#include <shellapi.h>
#include <sal.h>

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <dxgi.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#define CPU_GPU_VIRTUAL_ADDRESS_NULL      ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define CPU_GPU_VIRTUAL_ADDRESS_UNKNOWN   ((D3D12_GPU_VIRTUAL_ADDRESS)-1)

#define SAFE_RELEASE(ptr) if(ptr != nullptr)ptr->Release()

#endif // !_STATY_WIN_STANDAER_HEAD_FILE_H_

