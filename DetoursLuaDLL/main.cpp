//////////////////////////////////////////////////////////////////////////////
//
//  Detours liar package. Written by Magnus Johnsson, licensed under the MIT license

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include "detours.h"
#include <string>
#include "APIMove.h"
#include "thatluashit.h"

//#pragma warning( disable : 4996;)



BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	LONG error;
	(void)hinst;
	(void)reserved;

	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	if (dwReason == DLL_PROCESS_ATTACH) {
		DetourRestoreAfterWith();
		freopen("freopen.out", "w", stdout);
		freopen("freopenerr.out", "w", stderr);
		printf("simple" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
			" Starting.\n");
		fflush(stdout);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		lua = new thatluashit("redirect.lua");
		
		fflush(stdout);

		DetourAttach(&(PVOID&)TrueCreateFileA, RedirectedCreateFileA);
		DetourAttach(&(PVOID&)TrueCreateFileW, RedirectedCreateFileW);
		DetourAttach(&(PVOID&)TrueFindFirstFileExW, RedirectedFindFirstFileExW);
		error = DetourTransactionCommit();

		if (error == NO_ERROR) {
			printf("simple" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
				" Detoured SleepEx().\n");
		}
		else {
			printf("simple" DETOURS_STRINGIFY(DETOURS_BITS) ".dll:"
				" Error detouring SleepEx(): %d\n", error);
		}
	}
	else if (dwReason == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)TrueCreateFileA, RedirectedCreateFileA);
		DetourDetach(&(PVOID&)TrueCreateFileW, RedirectedCreateFileW);
		DetourDetach(&(PVOID&)TrueFindFirstFileExW, RedirectedFindFirstFileExW);
		error = DetourTransactionCommit();
		fflush(stdout);
	}
	return TRUE;
}

//
///////////////////////////////////////////////////////////////// End of File.
