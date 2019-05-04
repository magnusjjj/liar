//////////////////////////////////////////////////////////////////////////////
//
//  Detours Test Program (simple.cpp of simple.dll)
//
//  Microsoft Research Detours Package
//
//  Copyright (c) Microsoft Corporation.  All rights reserved.
//
//  This DLL will detour the Windows SleepEx API so that TimedSleep function
//  gets called instead.  TimedSleepEx records the before and after times, and
//  calls the real SleepEx API through the TrueSleepEx function pointer.
//

//#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include "detours.h"
#include <string>
#include "APIMove.h"
#include "thatluashit.h"

//#pragma warning( disable : 4996;)

std::wstring filestoredirect[] = { L"GameConfig.txt", L"CmdSeq.wc", L"" };
thatluashit* lua;

LSTATUS WINAPI RedirectedRegCreateKeyExA(
	HKEY                        hKey,
	LPCSTR                      lpSubKey,
	DWORD                       Reserved,
	LPSTR                       lpClass,
	DWORD                       dwOptions,
	REGSAM                      samDesired,
	CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	PHKEY                       phkResult,
	LPDWORD                     lpdwDisposition
) {
	if (strstr(lpSubKey, "Hammer") != NULL)
	{
		std::string changy = std::string(lpSubKey);
		changy = changy.replace(changy.find("Hammer"), sizeof("HammerTuxieLauncher") - 1, "HammerTuxieLauncher");
		//return TrueCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		return TrueRegCreateKeyExA(hKey, changy.c_str(), Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	}
	else {
		return TrueRegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	}
	//return TrueRegCreateKeyExA(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

HANDLE WINAPI RedirectedFindFirstFileExW(
	LPCWSTR             lpFileName,
	FINDEX_INFO_LEVELS fInfoLevelId,
	LPVOID             lpFindFileData,
	FINDEX_SEARCH_OPS  fSearchOp,
	LPVOID             lpSearchFilter,
	DWORD              dwAdditionalFlags
)
{
	lua->getLock();
	lua->setFunctionName("file_create");
	lua->pushlpcwstr(lpFileName);
	lua->executeFunction(1, 1);
	LPCWSTR filenameredirected = lua->poplpcwstr();
	lua->releaseLock();

	HANDLE ret = TrueFindFirstFileExW(
		filenameredirected,
		fInfoLevelId,
		lpFindFileData,
		fSearchOp,
		lpSearchFilter,
		dwAdditionalFlags
	);

	free((void *)filenameredirected);
	return ret;
}

HANDLE WINAPI RedirectedCreateFileA(LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) {
	lua->getLock();
	lua->setFunctionName("file_create");
	lua->pushlpcstr(lpFileName);
	lua->executeFunction(1, 1);
	LPCSTR filenameredirected = lua->poplpcstr();
	lua->releaseLock();

	HANDLE ret = TrueCreateFileA(filenameredirected, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	free((void *)filenameredirected);
	return ret;
}

HANDLE WINAPI RedirectedCreateFileW(LPCWSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) {
	lua->getLock();
	lua->setFunctionName("file_create");
	lua->pushlpcwstr(lpFileName);
	lua->executeFunction(1, 1);
	LPCWSTR filenameredirected = lua->poplpcwstr();
	lua->releaseLock();
	HANDLE ret = TrueCreateFileW(filenameredirected, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	free((void *)filenameredirected);
	return ret;
}

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
		DetourAttach(&(PVOID&)TrueRegCreateKeyExA, RedirectedRegCreateKeyExA);
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
		DetourDetach(&(PVOID&)TrueRegCreateKeyExA, RedirectedRegCreateKeyExA);
		error = DetourTransactionCommit();
		fflush(stdout);
	}
	return TRUE;
}

//
///////////////////////////////////////////////////////////////// End of File.
