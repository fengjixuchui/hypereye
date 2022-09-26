#include "shared/std/mutex.hpp"
#include "shared/cpu.hpp"
#include "shared/nt.hpp"

namespace cpu
{
uint64_t count()
{
    return KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
}

uint64_t current()
{
    return KeGetCurrentProcessorNumberEx(nullptr);
}

void for_each(core_cb fn)
{
    KeIpiGenericCall([](uint64_t arg) -> uint64_t
    {
        auto fn = reinterpret_cast<core_cb*>(arg);
        (*fn)(current());
        return true;
    }, reinterpret_cast<uint64_t>(&fn));
}
};
