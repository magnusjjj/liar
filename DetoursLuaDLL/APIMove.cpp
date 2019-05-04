#include "APIMove.h"

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