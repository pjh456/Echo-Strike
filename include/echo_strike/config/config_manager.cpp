#include <echo_strike/config/config_manager.hpp>

#include <pjh_json/parsers/json_parser.hpp>
#include <echo_strike/config/resource_manager.hpp>

#include <iostream>

ConfigManager &ConfigManager::instance()
{
    static ConfigManager manager;
    return manager;
}

bool ConfigManager::load_config(const std::filesystem::path &path)
{
    namespace fs = std::filesystem;
    if (!fs::is_regular_file(path))
        return false;

    auto config_str = ResourceManager::instance().load_resource_str(path);
    parser = new pjh_std::json::Parser(*config_str);

    try
    {
        config = parser->parse();
        return true;
    }
    catch (pjh_std::json::Exception e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}