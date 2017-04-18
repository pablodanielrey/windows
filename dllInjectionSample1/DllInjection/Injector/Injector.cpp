// Injector.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "tlhelp32.h"
#include "stdio.h"
#include "iostream"
#include "string"

/*
Zer0Mem0ry
https://youtu.be/IBwoVUR1gt8
*/

bool injectDynamicLibrary(DWORD processId, const wchar_t *dllPath) {
	HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
	if (hTargetProcess) {
		// cargo la dir de LoadLibraryA en nuestro proceso. se supone que es la misma en todos los procesos.
		LPVOID loadLibAddr = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
		std::wcout << "Ubicación de LoadLibrary == " << loadLibAddr << "\n";

		size_t strlen = (lstrlenW(dllPath) + 1) * sizeof(wchar_t);
		std::wcout << "Longitud a reservar de memoria : " << strlen << "\n";
		
		// aloco memoria en el proceso destino para guardar el path a la libreria y copio la ruta
		LPVOID lPath = VirtualAllocEx(hTargetProcess, 0, strlen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (lPath == 0) {
			CloseHandle(hTargetProcess);
			return false;
		}
		std::wcout << "Proceso : " << hTargetProcess << " Ubicación alocada : " << lPath << "\n";
		WriteProcessMemory(hTargetProcess, lPath, (LPVOID)dllPath, strlen, 0);

		HANDLE remoteThread = CreateRemoteThread(hTargetProcess, 0, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, lPath, 0, 0);
		if (remoteThread == 0) {
			CloseHandle(hTargetProcess);
			return false;
		}
		std::wcout << "Remote Thread : " << remoteThread << "\n";
		DWORD ret = WaitForSingleObject(remoteThread, INFINITE);
		std::wcout << "Retorno del thread : " << ret << "\n";

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
		const wchar_t *dllPath = L"C:\\Users\\user\\Source\\Repos\\windows\\dllInjectionSample1\\DllInjection\\x64\\Debug\\DllInjection.dll";
		wchar_t *name = argv[1];
		std::wcout << L"Buscando : " << name << "\n";
		DWORD pId = getProcessByName(name);
		std::wcout << L"Injectando dll en el proceso con id " << pId << "\n";
		if (injectDynamicLibrary(pId, dllPath)) {
			std::wcout << L"Librería injectada\n";
		}
		return 0;
	}
	return 1;
}

