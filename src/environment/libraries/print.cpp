#include "../../../pch.h"
#include "../reg.hpp"
#include "../../func.hpp"
#include <lualib.h>

static int out(lua_State* L) {
    int n = lua_gettop(L);
    std::string r;
    for (int i = 1; i <= n; i++) {
        if (i > 1) r += "\t";
        const char* s = lua_tostring(L, i);
        r += s ? s : "<nil>";
    }
    functions::out(functions::print, "%s", r.c_str());
    return 0;
}

static int warn(lua_State* L) {
    int n = lua_gettop(L);
    std::string r;
    for (int i = 1; i <= n; i++) {
        if (i > 1) r += "\t";
        const char* s = lua_tostring(L, i);
        r += s ? s : "<nil>";
    }
    functions::out(functions::warn, "%s", r.c_str());
    return 0;
}

void lib_basic(lua_State* L) {
    environment::rg->open(L, [&](lua_State* l) {
        environment::rg->add(l, out, "print");
        environment::rg->add(l, warn, "warn");
    }, nullptr, false);
}
