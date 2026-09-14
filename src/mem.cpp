#include "mem.hpp"
#include <dlfcn.h>
utilities::memory::lib::lib(const char* n) : base_(0), name_(n) {}
void utilities::memory::lib::sync() const {
    void* handle = dlopen(name_, RTLD_NOLOAD | RTLD_LAZY);
    if (handle) {
        Dl_info info;
        void* sym = dlsym(handle, "JNI_OnLoad");
        if (sym) {
            dladdr(sym, &info);
            base_.store(reinterpret_cast<int64_t>(info.dli_fbase));
        }
        dlclose(handle);
    }
}
