#pragma once

// This file is required.

#include <spdlog/sinks/basic_file_sink.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

namespace fs = std::filesystem;
namespace logger = SKSE::log;

using namespace std::literals;

// VR compatibility - use REL::VariantID for multi-targeting support
// Takes (SE_ID, AE_ID, VR_Offset)
#define MAKE_OFFSET(a_seID, a_aeID, a_vrOffset) REL::VariantID(a_seID, a_aeID, a_vrOffset)

namespace util {
    inline auto MakeHook(REL::VariantID a_id, std::ptrdiff_t a_offset = 0) {
        return REL::Relocation<std::uintptr_t>(a_id, a_offset);
    }
}
