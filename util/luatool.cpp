#include "luatool.h"

namespace cgserver{
    LuaTool::LuaTool(){
	_L = luaL_newstate();
	if (_L == NULL) {
	    std::cout << "NULL lua state" << std::endl;
	    return;
	}
	luaL_openlibs(_L);
    }
    
    LuaTool::~LuaTool(){
	if (_L) {
	    lua_close(_L);
	    _L = NULL;
	}
    }

    bool LuaTool::load(const std::string &f){
	if (_L == NULL) {
	    return false;
	}
	int bRet = luaL_dofile(_L, f.c_str());// do not use lua_loadfile!
	if(bRet) {
	    std::cout << "Load file error:"<< bRet<< std::endl;
	    return false;
	}
	std::cout << "Load success" << std::endl;
	return true;
    }
    
    bool LuaTool::getValue(double &ret) {
	if (!lua_isnumber(_L, -1)) {
	    return false;
	}
	ret = lua_tonumber(_L, -1);
	return true;
    }

    bool LuaTool::getValue(long int &ret) {
	ret = lua_tointeger(_L, -1);
	return true;
    }    
    
    bool LuaTool::getValue(std::string &str) {
	size_t len;
	const char *src = lua_tolstring(_L, -1, &len);
	str.assign(src);
	return true;
    }

    void LuaTool::pushValue(long int in) {
	lua_pushinteger(_L, in);
    }

    void LuaTool::pushValue(double in) {
	lua_pushnumber(_L, in);
    }

    void LuaTool::pushValue(const std::string &in) {
	lua_pushstring(_L, in.c_str());
    }

    void LuaTool::pushValue(const char *in) {
	lua_pushstring(_L, in);
    }    

    bool LuaTool::exeFunc(int iCount, int oCount) {
	int ret = lua_pcall(_L, iCount, oCount, 0);
	if (ret) {
	    std::string pErrorMsg;
	    getValue(pErrorMsg);
	    if (ret == LUA_ERRRUN) {
		std::cout << "Runtime error.";
	    }else if(ret == LUA_ERRMEM) {
		std::cout << "Memory allocation error.";
	    }else if(ret == LUA_ERRERR) {
		std::cout << "Error while running the error handler function.";
	    }else{
		std::cout << "Unknow error." << std::endl;
	    }
	    //const char *pErrorMsg;// = lua_tostring(ls, -1);
	    std::cout << "[" << pErrorMsg << "]" << std::endl;
	    return false;
	}
	return true;
    }

    // no input, no output
    bool LuaTool::exeFunc(const std::string &funcName) {
	debug();
	lua_getglobal(_L, funcName.c_str());
	return exeFunc(0,0);
    }

    bool LuaTool::debug() {
	int stack_size = lua_gettop(_L);
	std::cout << "Stack size is:" << stack_size << std::endl;
	std::string str;
    }
    
}
