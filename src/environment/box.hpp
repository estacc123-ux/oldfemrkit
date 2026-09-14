#pragma once
#include <memory>
#include <atomic>
#include <lua.h>
namespace environment {
class box {
public:
    bool on = false;
    int64_t sc = 0;
    lua_State* L = nullptr;
    lua_State* s = nullptr;
    std::atomic<bool> wait{false};
    int n = 0;
    void load();
    void join();
    void tick();
    void drop();
};
inline auto env = std::make_unique<box>();
}
