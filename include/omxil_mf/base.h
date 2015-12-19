
#ifndef OMX_MF_BASE_HPP__
#define OMX_MF_BASE_HPP__

#if defined(__linux__)
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#elif defined(_WINDOWS)
#define _WIN32_WINNT    _WIN32_WINNT_WINXP
#include <WinSDKVer.h>
//#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if defined(_WINDOWS)
#    ifdef __OMX_MF_EXPORTS
#        define OMX_MF_API __declspec(dllexport)
#    else
#        define OMX_MF_API __declspec(dllimport)
#    endif
#else
#    ifdef __OMX_MF_EXPORTS
#        define OMX_MF_API
#    else
#        define OMX_MF_API extern
#    endif
#endif

#if defined(_WINDOWS)
#    ifdef __OMX_MF_EXPORTS
#        define OMX_MF_API_CLASS __declspec(dllexport)
#    else
#        define OMX_MF_API_CLASS __declspec(dllimport)
#    endif
#else
#    ifdef __OMX_MF_EXPORTS
#        define OMX_MF_API_CLASS
#    else
#        define OMX_MF_API_CLASS
#    endif
#endif

#endif /* OMX_MF_BASE_H__ */
