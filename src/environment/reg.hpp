#pragma once
#include <lua.h>
#include <functional>
#include <memory>
namespace environment {
class reg {
public:
    void all(lua_State* L);
    void open(lua_State* L, const std::function<void(lua_State*)>& cb, const char* n, bool g = true);
    void add(lua_State* L, lua_CFunction f, const char* n);
};
inline const auto rg = std::make_unique<reg>();
}
