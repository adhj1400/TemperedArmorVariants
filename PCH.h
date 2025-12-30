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
