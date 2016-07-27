#include "luatoolfactory.h"
namespace cgserver{
    LuaToolFactory::LuaToolFactory():_file(""),_wanted(0){
    }
    
    LuaToolFactory::~LuaToolFactory(){
    }

    bool LuaToolFactory::init(size_t wanted, const std::string &file){
	_wanted = wanted;
	_file = file;
	return refresh();
    }

    bool LuaToolFactory::refresh() {
	LuaTools tmp;
	for (size_t i = 0; i < _wanted; ++i) {
	    LuaToolPtr luaPtr(new LuaTool);
	    if (!luaPtr->load(_file)) {
		std::cout << "Load lua file:" << _file << " failed." << std::endl;
		return false;
	    }
	    tmp.push_back(luaPtr);
	}
	// use new lua tool replace old lua tool
	_lock.lock();
	_remain.swap(tmp);
	_lock.unlock();
	return true;
    }

    void LuaToolFactory::returnTool(LuaToolPtr tool) {
	_lock.lock();
	_remain.push_back(tool);
	_lock.unlock();
    }

    LuaToolPtr LuaToolFactory::borrowTool(){
	LuaToolPtr ret;
	_lock.lock();
	if (!_remain.empty()) {
	    ret = _remain.back();
	    _remain.pop_back();
	}
	_lock.unlock();
	return ret;
    }

    LuaToolFactory &LuaToolFactory::getInstance() {
	static LuaToolFactory factory;
	return factory;
    }

}
