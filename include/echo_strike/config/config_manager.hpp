#ifndef INCLUDE_CONFIG_MANAGER
#define INCLUDE_CONFIG_MANAGER

#include <pjh_json/helpers/json_ref.hpp>
#include <pjh_json/parsers/json_parser.hpp>

#include <filesystem>

class ConfigManager
{
public:
    static ConfigManager &instance();

private:
    pjh_std::json::Ref config;

    // Avoid the string_view's lifetime problem.
    pjh_std::json::Parser *parser = nullptr;

public:
    ConfigManager() = default;
    ~ConfigManager()
    {
        if (parser)
            delete parser;
        parser = nullptr;

        if (config.get())
            delete config.get();
    }

    ConfigManager(const ConfigManager &) = delete;
    ConfigManager &operator=(const ConfigManager &) = delete;

    ConfigManager(ConfigManager &&) noexcept = delete;
    ConfigManager &operator=(ConfigManager &&) = delete;

public:
    bool load_config(const std::filesystem::path &);
    pjh_std::json::Ref &get_config() { return config; }
    const pjh_std::json::Ref &get_config() const { return config; }
};

#endif