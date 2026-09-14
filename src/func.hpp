#pragma once
#include "../pch.h"
#include "mem.hpp"
#include "cfg.hpp"
#include <lua.h>
#include <lobject.h>
#include <Luau/BytecodeBuilder.h>
#include <Luau/BytecodeUtils.h>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <android/log.h>
namespace functions {
class cc : public Luau::BytecodeEncoder {
public:
    inline void encode(uint32_t* d, size_t n) override {
        for (auto i = 0u; i < n;) {
            auto& op = *reinterpret_cast<uint8_t*>(d + i);
            i += Luau::getOpLength(LuauOpcode(op));
            op *= 227;
        }
    }
    std::string compile(const std::string& s);
};
inline const auto cmp = std::make_unique<cc>();
enum msg { print = 0, info, warn, error };
namespace types {
using get_ls_inst = void* (*)(int64_t);
using get_ls = void* (*)(int64_t, int*, const void*);
using getstate_t = lua_State* (*)(void* a1);
using resume_t = int64_t(*)(int64_t, void**, int, bool, const std::string&);
using out_t = void* (*)(int, const char*, ...);
using touch_t = void* (*)(int64_t, int64_t, int64_t, bool, int);
using newth_t = lua_State* (*)(lua_State*);
struct load_opt {
    char _p[48];
    const char* name;
    const char* data;
    size_t size;
    int env;
    int status;
};
using load_t = void* (*)(lua_State*, load_opt*);
using loadbuf_t = int (*)(lua_State*, const char*, const char*, size_t, int);
struct rstr {
    uint64_t f0;
    uint64_t size;
    char* data;
};
using loadsrc_t = int (*)(lua_State*, rstr*, int, unsigned int);
using resume2_t = int (*)(lua_State*, lua_State*, int);
using yield_t = int (*)(lua_State*, int);
}
template <typename F>
struct lazy {
    mutable F fn_ = nullptr;
    int64_t addr_ = 0;
    constexpr lazy(int64_t a) : addr_(a) {}
    F get() const { if (!fn_) fn_ = REBASE<F>(addr_); return fn_; }
    operator F() const { return get(); }
    template <typename... A>
    auto operator()(A&&... a) const -> decltype(fn_(std::forward<A>(a)...)) {
        return get()(std::forward<A>(a)...);
    }
};
inline auto get_ls_inst = lazy<types::get_ls_inst>(config::addresses::GetGlobalStateForInstance);
inline auto getstate = lazy<types::getstate_t>(config::addresses::Getluastate);
inline auto resume = lazy<types::resume_t>(config::addresses::ScriptContextResume);
inline auto out = lazy<types::out_t>(config::addresses::Print);
inline auto touch = lazy<types::touch_t>(config::addresses::TriggerTouchTransmitter);
inline auto newth = lazy<types::newth_t>(config::addresses::lua_newthread);
inline auto load = lazy<types::load_t>(config::addresses::rbx_luau_load);
inline auto loadsrc = lazy<types::loadsrc_t>(config::addresses::lua_compile_source);
inline auto loadbuf = lazy<types::loadbuf_t>(config::addresses::LuaLoad);
inline auto vmresume = lazy<types::resume2_t>(config::addresses::LuaResume);
inline auto vmyield = lazy<types::yield_t>(config::addresses::lua_yield_roblox);
namespace types { using spawn_t = int (*)(lua_State*); }
inline auto spawn = lazy<types::spawn_t>(config::addresses::task_spawn);
namespace types {
using mkstr_t = void (*)(void* out, const char* d, size_t n);
using wrapstr_t = void (*)(void* p, void* t);
using dosrc_t = int (*)(lua_State* L, void* p, int env, int a4, int a5);
using freestr_t = void (*)(void* p);
}
inline auto mkstr = lazy<types::mkstr_t>(config::addresses::roblox_build_string);
inline auto wrapstr = lazy<types::wrapstr_t>(config::addresses::roblox_wrap_string);
inline auto dosrc = lazy<types::dosrc_t>(config::addresses::roblox_loadsource);
inline auto freestr = lazy<types::freestr_t>(config::addresses::roblox_string_free);
inline void fix_str() {
    void* b = utilities::memory::rb->base();
    if (!b) return;
    uint8_t* p = reinterpret_cast<uint8_t*>(b) + config::offsets::str_flag;
    if (*p & 1) return;
    long ps = sysconf(_SC_PAGESIZE);
    uintptr_t s = reinterpret_cast<uintptr_t>(p) & ~(uintptr_t)(ps - 1);
    mprotect(reinterpret_cast<void*>(s), ps, PROT_READ | PROT_WRITE);
    *p |= 1;
}
namespace rlua {
using gg_t = void (*)(lua_State*, const char*);
using pls_t = void (*)(lua_State*, const char*, size_t);
using ps_t = void (*)(lua_State*, const char*);
using pc_t = int (*)(lua_State*, int, int, int);
inline void* h() {
    static void* x = dlopen("libroblox.so", RTLD_NOW | RTLD_NOLOAD);
    return x;
}
template <typename F>
inline F find(const char* n) {
    static void* p = dlsym(h(), n);
    return reinterpret_cast<F>(p);
}
inline void gg(lua_State* L, const char* n) { find<gg_t>("lua_getglobal")(L, n); }
inline void pls(lua_State* L, const char* s, size_t n) { find<pls_t>("lua_pushlstring")(L, s, n); }
inline void ps(lua_State* L, const char* s) { find<ps_t>("lua_pushstring")(L, s); }
inline int pc(lua_State* L, int a, int r, int f) { return find<pc_t>("lua_pcall")(L, a, r, f); }
}
namespace off {
using pushcl_t = void (*)(lua_State* L, lua_CFunction fn, const char* n, int u, intptr_t c);
using setf_t = void (*)(lua_State* L, int i, const char* k);
using pushnil_t = void (*)(lua_State* L);
inline pushcl_t pushcl() {
    static pushcl_t f = REBASE<pushcl_t>(config::addresses::lua_pushcclosurek);
    return f;
}
inline setf_t setf() {
    static setf_t f = REBASE<setf_t>(config::addresses::lua_setfield);
    return f;
}
inline pushnil_t pushnil() {
    static pushnil_t f = REBASE<pushnil_t>(config::addresses::lua_pushnil_roblox);
    return f;
}
}
inline void fix_bc() {
    static bool done = false;
    if (done) return;
    void* b = utilities::memory::rb->base();
    if (!b) return;
    uint32_t* f = reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(b) + config::addresses::bytecode_check);
    long ps = sysconf(_SC_PAGESIZE);
    uintptr_t s = reinterpret_cast<uintptr_t>(f) & ~(uintptr_t)(ps - 1);
    mprotect(reinterpret_cast<void*>(s), ps * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
    f[0] = config::offsets::bc_ret0;
    f[1] = config::offsets::bc_ret1;
    done = true;
}
inline int rload(lua_State* L, const char* n, const char* d, size_t s, int e, void** o = nullptr) {
    types::load_opt q{};
    q.name = n;
    q.data = d;
    q.size = s;
    q.env = e;
    void* r = load(L, &q);
    if (q.status == 0 && o) *o = r;
    return q.status;
}
inline int sload(lua_State* L, const char* d, size_t s, int e) {
    alignas(8) unsigned char a[24] = {};
    functions::mkstr(a, d, s);
    alignas(8) unsigned char b[0x20] = {};
    functions::wrapstr(b, a);
    int st = functions::dosrc(L, b, e, 0, 0);
    functions::freestr(b);
    return st;
}
}
