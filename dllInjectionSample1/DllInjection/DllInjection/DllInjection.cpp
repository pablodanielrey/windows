// DllInjection.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "windows.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		MessageBoxA(0, "Hola mundoooo!!!", 0, 0);
	}
	else if (ul_reason_for_call == DLL_THREAD_ATTACH) {
		MessageBoxA(0, "Hola mundoooo!!!", 0, 0);
	}

	return TRUE;
}

