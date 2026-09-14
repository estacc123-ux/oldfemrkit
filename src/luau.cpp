#include "lua.h"
#include "lobject.h"
#include "lgc.h"
#include "ldo.h"
#include "lvm.h"
#include "ltable.h"
#include "mem.hpp"
#include "cfg.hpp"
#include "func.hpp"
int luau_load(lua_State* L, const char* chunkname, const char* data, size_t size, int env)
{
    return functions::rload(L, chunkname, data, size, env);
}
void luaC_barriertable(lua_State* L, LuaTable* t, GCObject* v)
{
    using tmp = void(*)(lua_State*, LuaTable*, GCObject*);
    static tmp fn = REBASE<tmp>(config::addresses::luaC_barriertable_roblox);
    fn(L, t, v);
}
