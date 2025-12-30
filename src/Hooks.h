#pragma once

class Hooks {
public:
    Hooks() = delete;

    static void Install();

private:
    static void InitWornArmor(RE::TESObjectARMO* a_armor, RE::Actor* a_actor,
                              RE::BSTSmartPointer<RE::BipedAnim>* a_biped);

    static RE::ExtraDataList* GetArmorExtraData(RE::Actor* a_actor, RE::TESObjectARMO* a_armor);
};
