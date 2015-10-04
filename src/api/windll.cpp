#ifdef _WINDOWS

/*
 * NOTE: Some macros must be defined in project options of Visual Studio.
 *       - _CRT_SECURE_NO_WARNINGS
 *         To use strncpy().
 *       - NOMINMAX
 *         To use std::min(), std::max().
 */

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif
