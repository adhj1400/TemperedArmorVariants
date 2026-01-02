#include "Patches.h"

#include <xbyak/xbyak.h>

namespace Offsets {
    namespace TESNPC {
        // InitWornForm - SE: 24736, AE: 24583, VR: 0x373CB0
        inline constexpr auto InitWornForm = MAKE_OFFSET(24736, 24583, 0x373CB0);
    }

    namespace TESObjectARMA {
        // InitWornArmorAddon - SE: 17759, AE: 17812, VR: 0x2383A0
        inline constexpr auto InitWornArmorAddon = MAKE_OFFSET(17759, 17812, 0x2383A0);
    }
}

void Patches::WriteInitWornPatch(InitWornArmorFunc* a_func) {
    // Hook location: TESNPC::InitWornForm at offset 0x2F0
    auto hook = util::MakeHook(Offsets::TESNPC::InitWornForm, 0x2F0);

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

    logger::info("Installing InitWornArmor patch at {:X}, size: {} bytes", hook.address(), patchSize);

    // Fill with NOPs then write our patch
    REL::safe_fill(hook.address(), REL::NOP, 0x17);
    REL::safe_write(hook.address(), patch.getCode(), patchSize);
}
