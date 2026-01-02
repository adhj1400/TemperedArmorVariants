#include "Patches.h"

#include <xbyak/xbyak.h>

namespace Offsets {
    namespace TESNPC {
        // InitWornForm offset - SE: 1.5.97 = 24736, AE: 1.6.353 = 24583
        inline constexpr REL::ID InitWornForm(24736);
    }

    namespace TESObjectARMA {
        // InitWornArmorAddon offset - SE: 1.5.97 = 17759, AE: 1.6.353 = 17812
        inline constexpr REL::ID InitWornArmorAddon(17759);
    }
}

void Patches::WriteInitWornPatch(InitWornArmorFunc* a_func) {
    // Hook location: TESNPC::InitWornForm at offset 0x2F0
    auto hook = REL::Relocation<std::uintptr_t>(Offsets::TESNPC::InitWornForm, 0x2F0);

    // Create assembly patch using Xbyak
    struct Patch : public Xbyak::CodeGenerator {
        Patch(std::uintptr_t a_funcAddr) {
            // Move arguments into proper registers
            mov(rdx, r13);  // actor
            mov(rcx, rbp);  // armor
            mov(rax, a_funcAddr);
            call(rax);
        }
    };

    Patch patch{reinterpret_cast<std::uintptr_t>(a_func)};
    patch.ready();

    auto patchSize = patch.getSize();
    if (patchSize > 0x17) {
        logger::critical("Patch was too large ({} bytes), failed to install", patchSize);
        return;
    }

    logger::debug("Installing InitWornArmor patch at {:X}, size: {} bytes", hook.address(), patchSize);

    // Fill with NOPs then write our patch
    REL::safe_fill(hook.address(), REL::NOP, 0x17);
    REL::safe_write(hook.address(), patch.getCode(), patchSize);
}
