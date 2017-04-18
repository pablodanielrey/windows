
#include <stdio.h>
#include <iostream>
#include <windows.h>

int wmain(int argc, wchar_t **argv) {
	std::wcout << "Esperando para ser Injectado";
	std::wcout << "Presione CTRL para salir";

	while (!(GetKeyState(VK_CONTROL) & 0x8000)) {
		Sleep(100);
	}

	std::wcout << "Saliendo del programa";
}