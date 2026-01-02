#include "TESObjectARMA.h"

namespace Offsets {
    namespace TESObjectARMA {
        // HasRace - SE: 17757, AE: 17810, VR: 0x2380A0
        inline constexpr auto HasRace = MAKE_OFFSET(17757, 17810, 0x2380A0);
        // InitWornArmorAddon - SE: 17759, AE: 17812, VR: 0x2383A0
        inline constexpr auto InitWornArmorAddon = MAKE_OFFSET(17759, 17812, 0x2383A0);
    }
}

namespace Ext {
    bool TESObjectARMA::HasRace(RE::TESObjectARMA* a_armorAddon, RE::TESRace* a_race) {
        using func_t = decltype(HasRace);
        static REL::Relocation<func_t> func{Offsets::TESObjectARMA::HasRace};
        return func(a_armorAddon, a_race);
    }

    void TESObjectARMA::InitWornArmorAddon(RE::TESObjectARMA* a_armorAddon, RE::TESObjectARMO* a_armor,
                                           RE::BSTSmartPointer<RE::BipedAnim>* a_biped, RE::SEX a_sex) {
        using func_t = decltype(InitWornArmorAddon);
        static REL::Relocation<func_t> func{Offsets::TESObjectARMA::InitWornArmorAddon};
        func(a_armorAddon, a_armor, a_biped, a_sex);
    }
}
