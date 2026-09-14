#include "../pch.h"
#include "box.hpp"
#include "../cfg.hpp"
#include "../hook.hpp"
#include "../func.hpp"
#include "reg.hpp"

static void onleave(int64_t a1, int64_t pid) {
    hooks::mgr.h.leave(a1, pid);
    environment::env->drop();
}

static void onjoin(int64_t a1, int64_t pid) {
    hooks::mgr.h.join(a1, pid);
    if (pid <= 0)
        return;
    auto* e = environment::env.get();
    e->on = false;
    e->sc = 0;
    e->n = 0;
    e->wait.store(true, std::memory_order_release);
}

static void onjob(int64_t job) {
    hooks::mgr.h.job.get()(job);
    const char* name;
    if (*(uint8_t*)(job + config::offsets::job_flag) & 1)
        name = *(const char**)(job + config::offsets::job_ptr);
    else
        name = (const char*)(job + config::offsets::job_inline);
    if (name && memcmp(name, "WaitingHybridScriptsJob", 23) == 0) {
        if (!environment::env->sc) {
            int64_t sc = *(int64_t*)(job + config::offsets::job_sc);
            if (sc < 0x1000000000LL || sc > 0x00007FFF00000000LL)
                return;
            environment::env->sc = sc;
            return;
        }
    }
}

static uint64_t onstep(void* L, uint64_t op) {
    environment::env->L = (lua_State*)L;
    uint64_t r = hooks::mgr.h.step.get()(L, op);
    if (environment::env->wait.load(std::memory_order_acquire))
        environment::env->tick();
    return r;
}

void environment::box::load() {
    hooks::mgr.init();
    std::thread([]() {
        auto t0 = std::chrono::steady_clock::now();
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (environment::env->on)
                return;
            auto s = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - t0).count();
            if (s >= 45 && environment::env->L && environment::env->sc) {
                environment::env->wait.store(true, std::memory_order_release);
                return;
            }
        }
    }).detach();
}

void hooks::box::init() {
    h.leave.attach(REBASE<void*>(config::addresses::OnGameLeave), onleave);
    h.join.attach(REBASE<void*>(config::addresses::OnGameBegin), onjoin);
    h.job.attach(REBASE<void*>(config::addresses::JobStart), onjob);
    h.step.attach(REBASE<void*>(config::addresses::LuaC_Step), onstep);
}

void environment::box::tick() {
    if (!wait.load(std::memory_order_acquire))
        return;
    if (n < config::offsets::spawn_wait) {
        n++;
        return;
    }
    wait.store(false, std::memory_order_release);
    join();
}

void environment::box::join() {
    if (on)
        return;
    on = true;
    lua_State* m = L;
    if (!m) {
        on = false;
        return;
    }
    s = functions::newth(m);
    if (!s) {
        on = false;
        return;
    }
    environment::reg::all(s);
}

void environment::box::drop() {
    wait.store(false, std::memory_order_release);
    n = 0;
    s = nullptr;
    sc = 0;
    L = nullptr;
    on = false;
}
