#pragma once

namespace Patches {
    using InitWornArmorFunc = void(RE::TESObjectARMO* a_armor, RE::Actor* a_actor,
                                   RE::BSTSmartPointer<RE::BipedAnim>* a_biped);

    void WriteInitWornPatch(InitWornArmorFunc* a_func);
}
