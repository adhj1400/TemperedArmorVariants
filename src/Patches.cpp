#include "Patches.h"

#include <xbyak/xbyak.h>

namespace Offsets {
    namespace TESNPC {
        inline std::uintptr_t InitWornForm() {
            // SE: 1.5.97 = ID 24736, offset 0x5D0F80
            // AE: 1.6.353+ = ID 24583
            // VR: offset 0x5EB880
            if (REL::Module::IsVR()) {
                return REL::Relocation<std::uintptr_t>(REL::Offset(0x5EB880)).address();
            } else if (REL::Module::IsAE()) {
                return REL::Relocation<std::uintptr_t>(REL::ID(24583)).address();
            } else {
                return REL::Relocation<std::uintptr_t>(REL::ID(24736)).address();
            }
        }
    }

    namespace TESObjectARMA {
        inline std::uintptr_t InitWornArmorAddon() {
            // SE: 1.5.97 = ID 17759, offset 0x1FD160
            // AE: 1.6.353+ = ID 17812
            // VR: offset 0x1FD160
            if (REL::Module::IsVR()) {
                return REL::Relocation<std::uintptr_t>(REL::Offset(0x1FD160)).address();
            } else if (REL::Module::IsAE()) {
                return REL::Relocation<std::uintptr_t>(REL::ID(17812)).address();
            } else {
                return REL::Relocation<std::uintptr_t>(REL::ID(17759)).address();
            }
        }
    }
}

void Patches::WriteInitWornPatch(InitWornArmorFunc* a_func) {
    // Hook location: TESNPC::InitWornForm at offset 0x2F0
    auto base = Offsets::TESNPC::InitWornForm();
    auto hook = REL::Relocation<std::uintptr_t>(base + 0x2F0);

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
