#include <echo_strike/config/resource_manager.hpp>

#include <echo_strike/image/image.hpp>
#include <echo_strike/image/atlas.hpp>

#include <fstream>
#include <algorithm>
#include <format>
#include <iostream>

#include <SDL3/SDL_Render.h>
#include <SDL3_image/SDL_image.h>

ResourceManager &ResourceManager::instance()
{
    static ResourceManager manager;
    return manager;
}

std::shared_ptr<std::string> ResourceManager::load_resource_str(const std::filesystem::path &path)
{
    namespace fs = std::filesystem;
    auto abs_path = absolute_path(path);
    if (!is_exist(abs_path))
        return nullptr;

    if (!is_file(abs_path))
        return nullptr;

    auto key = normalize_key(abs_path);
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return get<std::string>(key);

    std::ifstream ifs(abs_path, std::ios::binary);
    if (!ifs)
        return nullptr;

    auto str_ptr = std::make_shared<std::string>(
        std::istreambuf_iterator<char>(ifs),
        std::istreambuf_iterator<char>());

    store<std::string>(key, str_ptr);

    return str_ptr;
}

void ResourceManager::destroy_resource(const std::filesystem::path &path)
{
    auto it = m_cache.find(normalize_key(path));
    if (it != m_cache.end())
        m_cache.erase(it);
}

void ResourceManager::destroy_resource(void *ptr)
{
    auto it = std::find_if(
        m_cache.begin(),
        m_cache.end(),
        [&](auto &pair)
        { return pair.second.get() == ptr; });

    if (it != m_cache.end())
        m_cache.erase(it);
}

std::shared_ptr<Image> ResourceManager::load_texture(
    SDL_Renderer *renderer,
    const std::filesystem::path &path)
{
    auto key = absolute_path_str(path);

    if (!is_file(path))
        return nullptr;

    if (auto cached = get<Image>(key))
        return cached;

    auto raw = IMG_LoadTexture(renderer, path.string().c_str());
    if (!raw)
        return nullptr;

    auto img = std::make_shared<Image>(raw);
    store<Image>(key, img);

    return img;
}

std::tuple<size_t, std::shared_ptr<Atlas>> ResourceManager::load_textures(
    SDL_Renderer *renderer,
    const char *template_str,
    size_t counts)
{
    auto abs_path = absolute_path(std::filesystem::u8path(template_str)).parent_path();
    auto key = normalize_key(abs_path);
    auto it = m_atlases.find(key);
    if (it != m_atlases.end())
        return {it->second->size(), it->second};

    size_t success_count = 0;
    std::vector<Image> textures;

    for (size_t idx = 0;
         counts == 0 ? true : idx < counts;
         ++idx)
    {
        auto fmt_path = std::vformat(template_str, std::make_format_args(idx));
        auto tex_ptr = load_texture(renderer, fmt_path);

        if (tex_ptr.get() != nullptr)
        {
            success_count++;
            textures.push_back(std::move(*tex_ptr.get()));
        }
        else if (idx != 0)
            break;
    }

    auto ptr = std::make_shared<Atlas>(std::move(textures));
    m_atlases.insert({key, ptr});

    auto u8_atlas_name = relative_path(abs_path, abs_path.parent_path()).u8string();
    std::string atlas_name(reinterpret_cast<const char *>(u8_atlas_name.c_str()), u8_atlas_name.size());
    ptr->set_name(atlas_name);
    return {success_count, ptr};
}

std::vector<std::shared_ptr<Atlas>> ResourceManager::load_texture_folder(
    SDL_Renderer *renderer,
    const std::filesystem::path &folder_path,
    const char *template_str)
{
    namespace fs = std::filesystem;
    std::vector<std::shared_ptr<Atlas>> atlases;

    auto full_path = absolute_path(folder_path);

    if (!is_directory(full_path))
        return {};

    std::vector<fs::directory_entry> dirs;
    for (auto &entry : fs::directory_iterator(full_path))
        if (entry.is_directory())
            dirs.push_back(entry);

    while (!dirs.empty())
    {
        auto entry = dirs.back();
        dirs.pop_back();

        for (auto &subentry : fs::directory_iterator(entry))
            if (subentry.is_directory())
                dirs.push_back(subentry);

        const auto &subfolder = entry.path();

        auto subfolder_str = subfolder.string();
        auto full_subpath = subfolder / template_str;
        auto full_template_str = full_subpath.u8string();
        std::string full_template_std_str(reinterpret_cast<const char *>(full_template_str.c_str()), full_template_str.size());

        // 调用 load_textures 自动加载 Atlas
        auto [count, atlas] = load_textures(renderer, full_template_std_str.c_str(), 0);

        if (count > 0)
            atlases.push_back(atlas);
    }

    return atlases;
}

void ResourceManager::set_default_folder()
{
    namespace fs = std::filesystem;
    m_folder = fs::current_path();
}

bool ResourceManager::set_resource_folder(const std::filesystem::path &path)
{
    if (!is_directory(path))
        return false;
    m_folder = path;
    return true;
}

bool ResourceManager::is_exist(const std::filesystem::path &path) const
{
    namespace fs = std::filesystem;
    return fs::exists(path);
}

bool ResourceManager::is_file(const std::filesystem::path &path) const
{
    namespace fs = std::filesystem;
    return fs::is_regular_file(path);
}

bool ResourceManager::is_directory(const std::filesystem::path &path) const
{
    namespace fs = std::filesystem;
    return fs::is_directory(path);
}

std::filesystem::path ResourceManager::absolute_path(const std::filesystem::path &path) const
{
    namespace fs = std::filesystem;
    return fs::absolute(path);
}

std::filesystem::path ResourceManager::relative_path(
    const std::filesystem::path &path,
    const std::filesystem::path &base) const
{
    namespace fs = std::filesystem;
    return fs::relative(path, base);
}

std::string ResourceManager::absolute_path_str(const std::filesystem::path &path) const
{
    return normalize_key(absolute_path(path));
}

std::string ResourceManager::relative_path_str(
    const std::filesystem::path &path,
    const std::filesystem::path &base) const
{
    return normalize_key(relative_path(path, base));
}

std::shared_ptr<Atlas> ResourceManager::get_atlas(const KeyType &name) const
{
    auto it = std::find_if(
        m_atlases.begin(),
        m_atlases.end(),
        [&](const auto &pair)
        {
            return pair.second->get_name() == name;
        });

    if (it == m_atlases.end())
        return nullptr;
    return it->second;
}

inline std::string ResourceManager::normalize_key(const std::filesystem::path &path)
{
    namespace fs = std::filesystem;
    // 先获取词法规范的绝对路径
    auto abs_path = fs::absolute(path).lexically_normal();

    // 将路径转换为UTF-8编码的字符串
    std::u8string u8str = abs_path.u8string();

    // 将std::u8string转换为std::string以便用作key
    // C++20可以直接转换
    std::string result(reinterpret_cast<const char *>(u8str.c_str()), u8str.size());

    // 将所有反斜杠'\'替换为斜杠'/'，确保格式统一
    std::replace(result.begin(), result.end(), '\\', '/');

    return result;
}