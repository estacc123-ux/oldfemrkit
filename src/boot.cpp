#include "pch.h"
#include "mem.hpp"
#include "environment/box.hpp"
#include <thread>
#include <chrono>
#include <Luau/Compiler.h>
__attribute__((constructor))
void boot() {
    std::thread([]() {
        auto tmp = utilities::memory::rb.get();
        while (!tmp->ok()) {
            tmp->sync();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        environment::env->load();
        Luau::assertHandler() = ([](const char*, const char*, int, const char*) -> int {
            return 1;
        });
    }).detach();
}

extern "C" {
    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, [[maybe_unused]] void* reserved) {
        (void)vm;
        (void)reserved;
        return JNI_VERSION_1_6;
    }

    JNIEXPORT void JNI_OnUnload(JavaVM*, void*) {}
}
