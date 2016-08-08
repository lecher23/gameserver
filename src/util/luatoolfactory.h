#ifndef LUATOOLFACTORY_H
#define LUATOOLFACTORY_H

#include "luatool.h"
#include "threadlock.h"
#include <vector>

namespace cgserver{
    // TODO:consumer and producer lock.
    class LuaToolFactory{
    public:
        ~LuaToolFactory();

	LuaToolPtr borrowTool();
	void returnTool(LuaToolPtr tool);
	bool init(size_t wanted, const std::string &file);
	bool refresh();

	static LuaToolFactory &getInstance();

    private:
        LuaToolFactory();
	LuaToolFactory(const LuaToolFactory &ipt);

	typedef std::vector<LuaToolPtr> LuaTools;
	
	LuaTools _remain;
	ThreadMutex _lock;
	std::string _file;
	size_t _wanted;
    };
}
#endif
