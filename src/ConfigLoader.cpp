#include "ConfigLoader.h"

#include "TemperedArmorManager.h"

void ConfigLoader::LoadConfigs() {
    logger::info("Loading Tempered Armor Variants configs...");

    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    if (!dataHandler) {
        logger::error("Failed to get TESDataHandler");
        return;
    }

    std::uint32_t configCount = 0;
    constexpr auto suffix = ".json"sv;

    for (const auto& entry : std::filesystem::directory_iterator("Data/SKSE/Plugins/TemperedArmorVariants")) {
        if (entry.is_regular_file() && entry.path().extension() == suffix) {
            LoadConfig(entry.path());
            ++configCount;
        }
    }

    logger::info("Loaded {} config file(s)", configCount);
}

void ConfigLoader::LoadConfig(const std::filesystem::path& a_path) {
    logger::info("Loading config: {}", a_path.filename().string());

    try {
        std::ifstream file(a_path);
        if (!file.is_open()) {
            logger::error("Failed to open config file: {}", a_path.string());
            return;
        }

        std::string jsonStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Simple JSON parsing - looking for patterns like "plugin.esm|formID"
        // Format: { "armors": [ { "Skyrim.esm|001": { "fine": "MyMod.esp|001", ... } } ] }

        size_t armorsPos = jsonStr.find("\"armors\"");
        if (armorsPos == std::string::npos) {
            logger::warn("No 'armors' array found in config: {}", a_path.filename().string());
            return;
        }

        // Find the array content
        size_t arrayStart = jsonStr.find('[', armorsPos);
        size_t arrayEnd = jsonStr.rfind(']');

        if (arrayStart == std::string::npos || arrayEnd == std::string::npos) {
            logger::error("Malformed JSON in config: {}", a_path.filename().string());
            return;
        }

        std::string arrayContent = jsonStr.substr(arrayStart + 1, arrayEnd - arrayStart - 1);

        // Parse armor entries
        size_t pos = 0;
        while (pos < arrayContent.length()) {
            // Find base armor FormID (e.g., "Skyrim.esm|00012E49")
            size_t quoteStart = arrayContent.find('"', pos);
            if (quoteStart == std::string::npos) break;

            size_t quoteEnd = arrayContent.find('"', quoteStart + 1);
            if (quoteEnd == std::string::npos) break;

            std::string baseArmorStr = arrayContent.substr(quoteStart + 1, quoteEnd - quoteStart - 1);

            // Check if this looks like a FormID (contains |)
            if (baseArmorStr.find('|') == std::string::npos) {
                pos = quoteEnd + 1;
                continue;
            }

            // Parse the base armor FormID
            size_t pipePos = baseArmorStr.find('|');
            std::string pluginName = baseArmorStr.substr(0, pipePos);
            std::string formIdStr = baseArmorStr.substr(pipePos + 1);

            // Convert hex FormID
            RE::FormID baseFormID = std::stoul(formIdStr, nullptr, 16);
            RE::TESForm* baseForm = RE::TESDataHandler::GetSingleton()->LookupForm(baseFormID, pluginName);

            if (!baseForm || baseForm->GetFormType() != RE::FormType::Armor) {
                logger::warn("Invalid base armor: {}|{:08X}", pluginName, baseFormID);
                pos = quoteEnd + 1;
                continue;
            }

            logger::trace("Found base armor: {}|{:08X}", pluginName, baseFormID);

            // Find the object containing variant definitions
            size_t objStart = arrayContent.find('{', quoteEnd);
            if (objStart == std::string::npos) break;

            size_t objEnd = arrayContent.find('}', objStart);
            if (objEnd == std::string::npos) break;

            std::string variantsObj = arrayContent.substr(objStart + 1, objEnd - objStart - 1);

            // Parse each tempering level
            TemperedArmorManager::ArmorVariants variants;

            auto parseVariant = [&](const std::string& levelName, TemperedArmorManager::TemperLevel level) {
                size_t levelPos = variantsObj.find(std::string("\"") + levelName + "\"");
                if (levelPos != std::string::npos) {
                    size_t colonPos = variantsObj.find(':', levelPos);
                    size_t valueStart = variantsObj.find('"', colonPos);
                    size_t valueEnd = variantsObj.find('"', valueStart + 1);

                    if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                        std::string formIdPair = variantsObj.substr(valueStart + 1, valueEnd - valueStart - 1);
                        size_t pipe = formIdPair.find('|');
                        if (pipe != std::string::npos) {
                            std::string plugin = formIdPair.substr(0, pipe);
                            std::string fid = formIdPair.substr(pipe + 1);
                            RE::FormID variantFormID = std::stoul(fid, nullptr, 16);

                            RE::TESForm* variantForm =
                                RE::TESDataHandler::GetSingleton()->LookupForm(variantFormID, plugin);
                            if (variantForm && variantForm->GetFormType() == RE::FormType::Armature) {
                                variants.levelVariants[level] = variantForm->As<RE::TESObjectARMA>();
                                logger::trace("  {} -> {}|{:08X}", levelName, plugin, variantFormID);
                            }
                        }
                    }
                }
            };

            parseVariant("fine", TemperedArmorManager::TemperLevel::Fine);
            parseVariant("superior", TemperedArmorManager::TemperLevel::Superior);
            parseVariant("exquisite", TemperedArmorManager::TemperLevel::Exquisite);
            parseVariant("flawless", TemperedArmorManager::TemperLevel::Flawless);
            parseVariant("epic", TemperedArmorManager::TemperLevel::Epic);
            parseVariant("legendary", TemperedArmorManager::TemperLevel::Legendary);

            // Parse allowElders setting (defaults to true if not specified)
            size_t eldersPos = variantsObj.find("\"allowElders\"");
            if (eldersPos != std::string::npos) {
                size_t colonPos = variantsObj.find(':', eldersPos);
                size_t truePos = variantsObj.find("true", colonPos);
                size_t falsePos = variantsObj.find("false", colonPos);

                if (falsePos != std::string::npos && (truePos == std::string::npos || falsePos < truePos)) {
                    variants.allowElders = false;
                    logger::trace("  allowElders: false");
                } else {
                    variants.allowElders = true;
                    logger::trace("  allowElders: true");
                }
            }

            // Parse allowChildren setting (defaults to true if not specified)
            size_t childrenPos = variantsObj.find("\"allowChildren\"");
            if (childrenPos != std::string::npos) {
                size_t colonPos = variantsObj.find(':', childrenPos);
                size_t truePos = variantsObj.find("true", colonPos);
                size_t falsePos = variantsObj.find("false", colonPos);

                if (falsePos != std::string::npos && (truePos == std::string::npos || falsePos < truePos)) {
                    variants.allowChildren = false;
                    logger::trace("  allowChildren: false");
                } else {
                    variants.allowChildren = true;
                    logger::trace("  allowChildren: true");
                }
            }

            // Parse allowFemale setting (defaults to true if not specified)
            size_t femalePos = variantsObj.find("\"allowFemale\"");
            if (femalePos != std::string::npos) {
                size_t colonPos = variantsObj.find(':', femalePos);
                size_t truePos = variantsObj.find("true", colonPos);
                size_t falsePos = variantsObj.find("false", colonPos);

                if (falsePos != std::string::npos && (truePos == std::string::npos || falsePos < truePos)) {
                    variants.allowFemale = false;
                    logger::trace("  allowFemale: false");
                } else {
                    variants.allowFemale = true;
                    logger::trace("  allowFemale: true");
                }
            }

            // Parse allowMale setting (defaults to true if not specified)
            size_t malePos = variantsObj.find("\"allowMale\"");
            if (malePos != std::string::npos) {
                size_t colonPos = variantsObj.find(':', malePos);
                size_t truePos = variantsObj.find("true", colonPos);
                size_t falsePos = variantsObj.find("false", colonPos);

                if (falsePos != std::string::npos && (truePos == std::string::npos || falsePos < truePos)) {
                    variants.allowMale = false;
                    logger::trace("  allowMale: false");
                } else {
                    variants.allowMale = true;
                    logger::trace("  allowMale: true");
                }
            }

            if (!variants.levelVariants.empty()) {
                TemperedArmorManager::GetSingleton()->RegisterArmor(baseForm->As<RE::TESObjectARMO>(), variants);
                logger::info("Registered armor with {} variant(s): {}|{:08X}", variants.levelVariants.size(),
                             pluginName, baseFormID);
            }

            pos = objEnd + 1;
        }

    } catch (const std::exception& e) {
        logger::error("Exception while loading config {}: {}", a_path.filename().string(), e.what());
    }
}
