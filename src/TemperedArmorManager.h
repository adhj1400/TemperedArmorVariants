#pragma once

class TemperedArmorManager {
public:
    enum class TemperLevel : std::uint8_t {
        None = 0,
        Fine = 1,
        Superior = 2,
        Exquisite = 3,
        Flawless = 4,
        Epic = 5,
        Legendary = 6
    };

    struct ArmorVariants {
        std::unordered_map<TemperLevel, RE::TESObjectARMA*> levelVariants;
        bool allowElders = true;
        bool allowChildren = false;
    };

    static TemperedArmorManager* GetSingleton() {
        static TemperedArmorManager singleton;
        return &singleton;
    }

    void RegisterArmor(RE::TESObjectARMO* a_armor, const ArmorVariants& a_variants);

    // Get the appropriate armor addon for a specific armor based on its tempering level
    RE::TESObjectARMA* GetVariantArmorAddon(RE::TESObjectARMO* a_armor, RE::TESObjectARMA* a_originalAddon,
                                            RE::ExtraDataList* a_extraDataList, RE::Actor* a_actor);

private:
    TemperedArmorManager() = default;
    TemperedArmorManager(const TemperedArmorManager&) = delete;
    TemperedArmorManager(TemperedArmorManager&&) = delete;

    TemperLevel GetTemperingLevel(float a_health) const;

    std::unordered_map<RE::TESObjectARMO*, ArmorVariants> m_armorVariants;
    mutable std::shared_mutex m_lock;
};
