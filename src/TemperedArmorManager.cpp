#include "TemperedArmorManager.h"

void TemperedArmorManager::RegisterArmor(RE::TESObjectARMO* a_armor, const ArmorVariants& a_variants) {
    if (!a_armor) {
        return;
    }

    std::unique_lock lock(m_lock);
    m_armorVariants[a_armor] = a_variants;
}

RE::TESObjectARMA* TemperedArmorManager::GetVariantArmorAddon(RE::TESObjectARMO* a_armor,
                                                              RE::TESObjectARMA* a_originalAddon,
                                                              RE::ExtraDataList* a_extraDataList, RE::Actor* a_actor) {
    if (!a_armor || !a_originalAddon) {
        return a_originalAddon;
    }

    // Check if this armor has any registered variants
    std::shared_lock lock(m_lock);
    auto it = m_armorVariants.find(a_armor);
    if (it == m_armorVariants.end()) {
        return a_originalAddon;
    }

    const auto& variants = it->second;

    if (a_actor) {
        if (!variants.allowChildren && a_actor->IsChild()) {
            return a_originalAddon;
        }

        if (!variants.allowElders) {
            auto race = a_actor->GetRace();
            if (race) {
                std::string raceName = race->GetFormEditorID();
                if (raceName.find("Elder") != std::string::npos || raceName.find("elder") != std::string::npos) {
                    return a_originalAddon;
                }
            }
        }
    }

    // Get the tempering level from ExtraHealth
    float health = 1.0f;
    if (a_extraDataList) {
        auto extraHealth = a_extraDataList->GetByType<RE::ExtraHealth>();
        if (extraHealth) {
            health = extraHealth->health;
        }
    }

    TemperLevel temperLevel = GetTemperingLevel(health);
    if (temperLevel == TemperLevel::None) {
        return a_originalAddon;
    }

    // Look up the variant for this tempering level
    auto variantIt = variants.levelVariants.find(temperLevel);

    if (variantIt != variants.levelVariants.end() && variantIt->second) {
        return variantIt->second;
    }

    return a_originalAddon;
}

TemperedArmorManager::TemperLevel TemperedArmorManager::GetTemperingLevel(float a_health) const {
    if (a_health >= 1.6f) {
        return TemperLevel::Legendary;
    } else if (a_health >= 1.5f) {
        return TemperLevel::Epic;
    } else if (a_health >= 1.4f) {
        return TemperLevel::Flawless;
    } else if (a_health >= 1.3f) {
        return TemperLevel::Exquisite;
    } else if (a_health >= 1.2f) {
        return TemperLevel::Superior;
    } else if (a_health >= 1.1f) {
        return TemperLevel::Fine;
    }

    return TemperLevel::None;
}
