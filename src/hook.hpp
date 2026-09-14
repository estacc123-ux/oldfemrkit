#pragma once
#include <dobby.h>
#include <thread>
#include <chrono>
#include <functional>
#include <type_traits>
#include <cstdint>
namespace hooks {
template<typename F>
class hook {
private:
    F org = nullptr;
public:
    void set(void* src, void* dst) {
        if (!src) return;
        for (int i = 0; i < 3; i++) {
            org = nullptr;
            if (DobbyHook(src, dst, reinterpret_cast<void**>(&org)) == 0 && org) return;
            std::this_thread::sleep_for(std::chrono::milliseconds(500 * (i + 1)));
        }
    }
    template <typename T> void set(void* src, T f) { set(src, *reinterpret_cast<void**>(&f)); }
    template <typename T> void attach(void* src, T f) { set(src, f); }
    template<typename... A>
    auto operator()(A... a) -> std::invoke_result_t<F, A...> { return org(a...); }
    F get() const { return org; }
    F original() const { return org; }
};
struct all {
    hook<void(*)(int64_t, int64_t)> leave;
    hook<void(*)(int64_t, int64_t)> join;
    hook<void(*)(int64_t, int64_t, int64_t)> sc;
    hook<void(*)(int64_t)> job;
    hook<uint64_t(*)(void*, uint64_t)> step;
};
class box {
public:
    all h;
    all &hooks = h;
    bool live = false;
    bool gameBegan = false;
    void init();
};
inline box mgr;
}
