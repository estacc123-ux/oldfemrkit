#pragma once
#include <cstdint>
#include <cstddef>

namespace config {
namespace offsets {
constexpr int64_t cap_mask = 0x3FFFFFF00LL | (1ull << 48ull);
constexpr int placeid = 88;
constexpr int job_sc = 0x1a0;
constexpr int job_flag = 24;
constexpr int job_ptr = 40;
constexpr int job_inline = 25;
constexpr uintptr_t ls_ex = 0x10;
constexpr uintptr_t ls_top = 0x18;
constexpr uintptr_t gs_cb = 0x4F0;
constexpr uintptr_t ex_id = 0x30;
constexpr uintptr_t ex_cap = 0x48;
constexpr uintptr_t cl_proto = 0x18;
constexpr uintptr_t pr_ud = 0x08;
constexpr uintptr_t pr_p = 0x20;
constexpr uintptr_t pr_np = 0x90;
constexpr uintptr_t pr_line = 0x98;
constexpr size_t tv_size = 0x10;
constexpr int globals = -10002;
constexpr uintptr_t str_flag = 0x66c13f8;
constexpr int ident = 7;
constexpr uintptr_t upenv = 0x20;
constexpr uintptr_t ls_g = 0x60;
constexpr uint32_t arm_ret = 0xD63F03C0;
constexpr uint32_t bc_ret0 = 0xD2800000;
constexpr uint32_t bc_ret1 = 0xD65F03C0;
constexpr int spawn_wait = 5;
constexpr int64_t full_caps = 0x7FFFFFFFFFFFFFFFLL;
constexpr uint64_t heap_lo = 0x7000000000ULL;
constexpr uint64_t heap_hi = 0x8000000000ULL;
constexpr int64_t ident_main = 8;
constexpr int64_t caps_main = 0x1FF3F;
}
}
