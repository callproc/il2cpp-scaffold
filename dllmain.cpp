#define WIN32_LAZY_AND_MEAN

#include <Windows.h>
#include <thread>

#include <common/include/console.h>

#include "sdk/il2cpp.h"

void entry() {
	console::open("callproc/il2cpp-scaffold");

}

BOOL WINAPI DllMain(HMODULE module, DWORD reason, void*) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(module);
		std::thread(entry).detach();
	}
}