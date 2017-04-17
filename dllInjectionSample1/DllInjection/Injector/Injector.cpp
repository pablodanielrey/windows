// Injector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"

/*
Thanks Zer0Mem0ry
https://youtu.be/IBwoVUR1gt8
*/

bool injectDynamicLibrary(DWORD processId, char *dllPath) {
	HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
	if (hTargetProcess) {
		// cargo la dir de LoadLibraryA en nuestro proceso. se supone que es la misma en todos los procesos.
		LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

		// aloco memoria en el proceso destino para guardar el path a la libreria y copio la ruta
		LPVOID lPath = VirtualAllocEx(hTargetProcess, 0, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (lPath == 0) {
			return false;
		}
		WriteProcessMemory(hTargetProcess, lPath, (LPVOID)dllPath, strlen(dllPath) + 1, 0);

		HANDLE remoteThread = CreateRemoteThread(hTargetProcess, 0, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, lPath, 0, 0);
		if (remoteThread == 0) {
			return false;
		}
		WaitForSingleObject(remoteThread, INFINITE);

		// libero recursos
		//VirtualFreeEx(hTargetProcess, lPath, strlen(dllPath) + 1, MEM_RELEASE);
		VirtualFreeEx(hTargetProcess, lPath, 0, MEM_RELEASE);
		CloseHandle(remoteThread);
		CloseHandle(hTargetProcess);

		return true;
	}
	return false;
}


int main()
{
	DWORD pId = 0;
	injectDynamicLibrary(pId, "DllInjection.dll");
    return 0;
}

