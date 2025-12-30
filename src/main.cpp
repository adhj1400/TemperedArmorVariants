#include "ConfigLoader.h"
#include "Hooks.h"

namespace {
    void InitializeLog() {
        auto path = logger::log_directory();
        if (!path) {
            return;
        }

        *path /= "TemperedArmorVariants.log"sv;
        auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

        auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
        log->set_level(spdlog::level::info);
        log->flush_on(spdlog::level::info);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern("%g(%#): [%l] %v"s);
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    InitializeLog();
    logger::info("TemperedArmorVariants v1.0.0");

    SKSE::Init(skse);

    // Install hooks
    Hooks::Install();

    // Register for game events
    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
        if (message->type == SKSE::MessagingInterface::kDataLoaded) {
            logger::info("Data loaded, loading configurations...");
            ConfigLoader::LoadConfigs();
            logger::info("TemperedArmorVariants initialized successfully");
        }
    });

    return true;
}
