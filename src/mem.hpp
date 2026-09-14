#pragma once
#include <memory>
#include <atomic>
namespace utilities::memory {
class lib {
private:
    mutable std::atomic<int64_t> base_;
    const char* name_;
public:
    lib(const char* n);
    void sync() const;
    template <typename T = void*>
    T at(int64_t off) const {
        if (!ok()) sync();
        return reinterpret_cast<T>(base_.load() + off);
    }
    bool ok() const { return base_.load() != 0; }
    void* base() const { return reinterpret_cast<void*>(base_.load()); }
};
inline const auto rb = std::make_unique<lib>("libroblox.so");
}
#define REBASE utilities::memory::rb->at
