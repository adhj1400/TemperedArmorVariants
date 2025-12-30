#pragma once

class ConfigLoader {
public:
    ConfigLoader() = delete;

    static void LoadConfigs();

private:
    static void LoadConfig(const std::filesystem::path& a_path);
};
