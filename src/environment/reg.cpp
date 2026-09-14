#include "reg.hpp"
#include "../exploit/closure.hpp"
void lib_basic(lua_State* L);
void environment::reg::all(lua_State* L) {
    lib_basic(L);
}
void environment::reg::open(lua_State* L, const std::function<void(lua_State*)>& cb, const char* n, bool g) {
    if (n) {
        lua_newtable(L);
        lua_pushvalue(L, -1);
        if (g) lua_setglobal(L, n);
        else lua_setfield(L, -2, n);
    } else {
        lua_pushvalue(L, LUA_GLOBALSINDEX);
    }
    cb(L);
    lua_pop(L, 1);
}
void environment::reg::add(lua_State* L, lua_CFunction f, const char* n) {
    exploit::closures::push(L, f, n, 0, nullptr);
    lua_setfield(L, -2, n);
}
