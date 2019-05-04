#include "thatluashit.h"

#pragma warning(disable:4996)

thatluashit* lua;

thatluashit::thatluashit(const char* filename)
{
	this->L = luaL_newstate();
	luaL_openlibs(this->L);
	int error = luaL_dofile(this->L, filename);
	if (error) {
		printf("Thars an error bob");
	}
	else {
		printf("Its pretty chill");
	}
	this->hLuaMutex = CreateMutex(NULL, FALSE, NULL);
}


thatluashit::~thatluashit()
{
	lua_close(L);
	CloseHandle(this->hLuaMutex);
}

void thatluashit::setFunctionName(const char* function_name) {
	lua_getglobal(L, function_name);
}

void thatluashit::executeFunction(int argumentnumber, int returnnumber) {
	if (lua_pcall(this->L, argumentnumber, returnnumber, 0))
	{
		printf("error running function: %s", lua_tostring(L, -1));
	}
}

void thatluashit::pushlpcwstr(LPCWSTR str) {
	size_t len = wcslen(str);
	size_t maxUtf8len = 4 * len + 1;
	char *utf8string = new char[maxUtf8len];
	WideCharToMultiByte(CP_UTF8, 0, str, len + 1, utf8string, maxUtf8len, NULL, NULL);
	lua_pushstring(this->L, utf8string);
	delete[] utf8string;
}

void thatluashit::pushlpcstr(LPCSTR str) {
	lua_pushstring(this->L, str);
}

LPCWSTR thatluashit::poplpcwstr() {
	const char* instr = lua_tostring(L, -1);
	char* my_copy = (char*)malloc(strlen(instr) + 1);
	strcpy(my_copy, instr);
	lua_pop(this->L, 1);
	//MultiByteToWideChar(CP_UTF8, 0, my_copy, strlen(instr)+1, )


	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, my_copy, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[wchars_num];
	MultiByteToWideChar(CP_UTF8, 0, my_copy, -1, wstr, wchars_num);
	free(my_copy);
	return wstr;
}

LPCSTR thatluashit::poplpcstr() {
	const char* instr = lua_tostring(L, -1);
	char* my_copy = (char*)malloc(strlen(instr) + 1);
	strcpy(my_copy, instr);
	lua_pop(this->L, 1);
	return my_copy;
}


void thatluashit::getLock()
{
	// TODO: Check for abandoned mutex?
	DWORD dwWaitResult = WaitForSingleObject(this->hLuaMutex, INFINITE);
}

void thatluashit::releaseLock()
{
	ReleaseMutex(this->hLuaMutex);
}