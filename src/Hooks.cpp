#include "Hooks.h"

#include "Ext/TESObjectARMA.h"
#include "Patches.h"
#include "TemperedArmorManager.h"

void Hooks::Install() {
    logger::info("Installing Tempered Armor Variants hooks...");

    // Install assembly patch to intercept armor initialization
    Patches::WriteInitWornPatch(&InitWornArmor);

    logger::info("Hooks installed successfully");
}

void Hooks::InitWornArmor(RE::TESObjectARMO* a_armor, RE::Actor* a_actor, RE::BSTSmartPointer<RE::BipedAnim>* a_biped) {
    logger::info("InitWornArmor called - armor: {:08X}, actor: {}", a_armor ? a_armor->GetFormID() : 0,
                 a_actor ? a_actor->GetName() : "null");

    if (!a_armor || !a_actor || !a_biped) {
        return;
    }

    auto race = a_actor->GetRace();
    auto sex = a_actor->GetActorBase()->GetSex();

    // Get extra data to check tempering level
    auto extraData = GetArmorExtraData(a_actor, a_armor);
    auto manager = TemperedArmorManager::GetSingleton();

    // Process each armor addon
    for (auto& armorAddon : a_armor->armorAddons) {
        if (!armorAddon) {
            continue;
        }

        // Check if this addon is valid for this race
        if (!Ext::TESObjectARMA::HasRace(armorAddon, race)) {
            continue;
        }

        // Check if we have a tempered variant for this addon
        auto temperedAddon = manager->GetVariantArmorAddon(a_armor, armorAddon, extraData, a_actor);

        if (temperedAddon && temperedAddon != armorAddon) {
            logger::trace("Swapping armor addon {:08X} -> {:08X} for {}", armorAddon->GetFormID(),
                          temperedAddon->GetFormID(), a_actor->GetName());

            // Initialize the tempered variant instead
            Ext::TESObjectARMA::InitWornArmorAddon(temperedAddon, a_armor, a_biped, sex);
        } else {
            // Use the original addon
            Ext::TESObjectARMA::InitWornArmorAddon(armorAddon, a_armor, a_biped, sex);
        }
    }
}

RE::ExtraDataList* Hooks::GetArmorExtraData(RE::Actor* a_actor, RE::TESObjectARMO* a_armor) {
    if (!a_actor || !a_armor) {
        return nullptr;
    }

    auto inventoryChanges = a_actor->GetInventoryChanges();
    if (!inventoryChanges || !inventoryChanges->entryList) {
        return nullptr;
    }

    for (auto& entry : *inventoryChanges->entryList) {
        if (!entry || !entry->object) {
            continue;
        }

        if (entry->object == a_armor) {
            // Found the armor entry, now find the worn extra data
            if (entry->extraLists) {
                for (auto& extraList : *entry->extraLists) {
                    if (extraList && (extraList->HasType(RE::ExtraDataType::kWorn) ||
                                      extraList->HasType(RE::ExtraDataType::kWornLeft))) {
                        return extraList;
                    }
                }
            }
        }
    }

    return nullptr;
}
