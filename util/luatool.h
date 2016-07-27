#ifndef LUATOOL_H
#define LUATOOL_H

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "common_define.h"
#include <iostream>

namespace cgserver{

    class LuaTool{
    public:
        LuaTool();
        ~LuaTool();
	bool load(const std::string &f);

	//Get top value at lua stack, then pop it
	bool getValue(double &ret);
	bool getValue(long int &ret);
	bool getValue(std::string &str);

	//Push value to top stack of lua
	void pushValue(long int in);
	void pushValue(double in);	
	void pushValue(const std::string &in);
	void pushValue(const char *in);

	void popValue(int wanted = 1);
	void clearLuaStack();	

	bool exeFunc(int iCount, int oCount);
	//void funcName(void)
	bool exeFunc(const std::string &funcName);

	//<nResult> funcName(param1, param2)
	template <typename T1, typename T2>
	bool exeFunc(const std::string &funcName, T1 &x, T2 &y, int nResult = 1) {
	    lua_getglobal(_L, funcName.c_str());	
	    pushValue(x);
	    pushValue(y);
	    return exeFunc(2, nResult);
	}

	//<nResult> funcName(param1)
	template <typename T1>
	bool exeFunc(const std::string &funcName, T1 &x, int nResult = 1) {
	    lua_getglobal(_L, funcName.c_str());	
	    pushValue(x);
	    return exeFunc(1, nResult);
	}
	
	bool debug();

    private:
	lua_State *_L;
    };

    DF_SHARED_PTR(LuaTool);
}
#endif
