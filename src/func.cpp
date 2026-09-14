#include "func.hpp"
#include <Luau/Compiler.h>

std::string functions::cc::compile(const std::string& s) {
    Luau::CompileOptions o;
    o.optimizationLevel = 1;
    o.debugLevel = 0;
    return Luau::compile(s, o, {}, this);
}
