#include "TESObjectARMA.h"

namespace Offsets {
    namespace TESObjectARMA {
        inline std::uintptr_t HasRace() {
            // SE: 1.5.97 = ID 17757, offset 0x1FD060
            // AE: 1.6.353+ = ID 17810
            // VR: offset 0x1FD060
            if (REL::Module::IsVR()) {
                return REL::Relocation<std::uintptr_t>(REL::Offset(0x1FD060)).address();
            } else if (REL::Module::IsAE()) {
                return REL::Relocation<std::uintptr_t>(REL::ID(17810)).address();
            } else {
                return REL::Relocation<std::uintptr_t>(REL::ID(17757)).address();
            }
        }

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

namespace Ext {
    bool TESObjectARMA::HasRace(RE::TESObjectARMA* a_armorAddon, RE::TESRace* a_race) {
        using func_t = decltype(HasRace);
        static REL::Relocation<func_t> func{Offsets::TESObjectARMA::HasRace()};
        return func(a_armorAddon, a_race);
    }

    void TESObjectARMA::InitWornArmorAddon(RE::TESObjectARMA* a_armorAddon, RE::TESObjectARMO* a_armor,
                                           RE::BSTSmartPointer<RE::BipedAnim>* a_biped, RE::SEX a_sex) {
        using func_t = decltype(InitWornArmorAddon);
        static REL::Relocation<func_t> func{Offsets::TESObjectARMA::InitWornArmorAddon()};
        return func(a_armorAddon, a_armor, a_biped, a_sex);
    }
}
