#include "vmexit.hpp"
#include "vmcall.hpp"
#include "vmcs.hpp"
#include "vmx.hpp"

#include "shared/trace.hpp"
#include "shared/cpu.hpp"
#include "arch/arch.hpp"

static void handle_cpuid(cpu::context_t* context)
{
    if (context->rax == 1)
    {
        auto features = read<cpuid::processor_features>();
        // Mark hypervisor as present.
        //
        features.hypervisor = true;
        context->rax = features.eax;
        context->rbx = features.ebx;
        context->rcx = features.ecx;
        context->rdx = features.edx;
    }
    else
    {
        int info[4];
        __cpuidex(info, context->eax, context->ecx);
        context->rax = info[0];
        context->rbx = info[1];
        context->rcx = info[2];
        context->rdx = info[3];
    }
}

static void handle_msr_read(cpu::context_t* context)
{
    auto msr = __readmsr(context->ecx);
    context->rax = msr & 0xffffffff;
    context->rdx = msr >> 32;
}

static void handle_msr_write(cpu::context_t* context)
{
    __writemsr(context->ecx, context->rax | context->rdx << 32);
}

extern "C" bool vmexit_handler(cpu::context_t* context)
{
    bool terminate    = false;
    const auto reason = static_cast<vmx::exit_reason>(vmx::read(vmx::vmcs::vm_exit_reason) & 0xffff);
    const auto rip    = vmx::read(vmx::vmcs::guest_rip);
    const auto len    = vmx::read(vmx::vmcs::vm_exit_instruction_len);
    const auto cr3    = read<cr3_t>();

    context->rsp      = vmx::read(vmx::vmcs::guest_rsp);

    write<cr3_t>(cr3_t{ vmx::read(vmx::vmcs::guest_cr3) });

    logger::info("vmexit code: %d rip: 0x%016llx", reason, rip);

    switch (reason)
    {
    case vmx::exit_reason::vmclear:  [[fallthrough]];
    case vmx::exit_reason::vmptrld:  [[fallthrough]];
    case vmx::exit_reason::vmptrst:  [[fallthrough]];
    case vmx::exit_reason::vmread:   [[fallthrough]];
    case vmx::exit_reason::vmresume: [[fallthrough]];
    case vmx::exit_reason::vmwrite:  [[fallthrough]];
    case vmx::exit_reason::vmxoff:   [[fallthrough]];
    case vmx::exit_reason::vmlaunch:
    {
        // Signal that vmx instructions are not supported.
        //
        vmx::write(vmx::vmcs::guest_rflags, vmx::read(vmx::vmcs::guest_rflags) | 1);
        break;
    }
    case vmx::exit_reason::ept_violation: [[fallthrough]];
    case vmx::exit_reason::ept_misconfig:
    {
        __debugbreak();
        break;
    }
    case vmx::exit_reason::cpuid:
    {
        handle_cpuid(context);
        break;
    }
    case vmx::exit_reason::msr_read:
    {
        handle_msr_read(context);
        break;
    }
    case vmx::exit_reason::msr_write:
    {
        handle_msr_write(context);
        break;
    }
    case vmx::exit_reason::vmcall:
    {
        terminate = handle_vmcall(context);
        break;
    }
    default:
        __debugbreak();
        break;
    }

    write<cr3_t>(cr3);
    vmx::write(vmx::vmcs::guest_rip, rip + len);

    return terminate;
}
