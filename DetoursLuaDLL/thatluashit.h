#pragma once
#include <Windows.h>
extern "C" {
	#include <lauxlib.h>
	#include <lua.h>
	#include <lualib.h>
}

class thatluashit
{
private:
	HANDLE hLuaMutex;
	lua_State *L;
public:
	thatluashit(const char* filename);
	~thatluashit();
	void setFunctionName(const char * function_name);
	void executeFunction(int argumentnumber, int returnnumber);
	void pushlpcwstr(LPCWSTR str);
	void pushlpcstr(LPCSTR str);
	LPCWSTR poplpcwstr();
	LPCSTR poplpcstr();
	void getLock();
	void releaseLock();
};

