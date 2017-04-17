// Injector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "tlhelp32.h"
#include "stdio.h"
#include "iostream"

/*
Zer0Mem0ry
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
			CloseHandle(hTargetProcess);
			return false;
		}
		WriteProcessMemory(hTargetProcess, lPath, (LPVOID)dllPath, strlen(dllPath) + 1, 0);

		HANDLE remoteThread = CreateRemoteThread(hTargetProcess, 0, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, lPath, 0, 0);
		if (remoteThread == 0) {
			CloseHandle(hTargetProcess);
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

DWORD getProcessByName(wchar_t *name) {

	HANDLE hpsn = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hpsn == INVALID_HANDLE_VALUE) {
		return 0;
	}
	__try {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hpsn, &pe32) == TRUE) {
			while (Process32Next(hpsn, &pe32) == TRUE) {
				std::wcout << "Proceso : " << pe32.szExeFile << "pid : " << pe32.th32ProcessID << L"\n";
				if (_wcsicmp(pe32.szExeFile, name) == 0) {
					DWORD hProcess = pe32.th32ProcessID;
					return hProcess;
				}
			}
		}

		return 0;
	}
	__finally {
		CloseHandle(hpsn);
	}

}


int wmain(int argc, wchar_t **argv)
{
	if (argc >= 2) {
		wchar_t *name = argv[1];
		std::wcout << "Buscando : " << name << "\n";
		printf("%d", getProcessByName(name));
		//injectDynamicLibrary(pId, "DllInjection.dll");
		return 0;
	}
	return 1;
}

