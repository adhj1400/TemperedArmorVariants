#include "TESObjectARMA.h"

namespace Offsets {
    namespace TESObjectARMA {
        inline constexpr REL::ID HasRace(17757);
        inline constexpr REL::ID InitWornArmorAddon(17759);
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
        return func(a_armorAddon, a_armor, a_biped, a_sex);
    }
}
