#include <echo_strike/config/resource_manager.hpp>

#include <echo_strike/image/image.hpp>
#include <echo_strike/image/atlas.hpp>

#include <fstream>
#include <algorithm>
#include <format>

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

    auto it = m_cache.find(abs_path.u8string());
    if (it != m_cache.end())
        return get<std::string>(abs_path.u8string());

    std::ifstream ifs(abs_path, std::ios::binary);
    if (!ifs)
        return nullptr;

    auto str_ptr = std::make_shared<std::string>(
        std::istreambuf_iterator<char>(ifs),
        std::istreambuf_iterator<char>());

    store<std::string>(abs_path.u8string(), str_ptr);

    return str_ptr;
}

std::shared_ptr<std::string> ResourceManager::load_resource_str(const std::u8string &path)
{
    namespace fs = std::filesystem;
    return load_resource_str(fs::path(path));
}

void ResourceManager::destroy_resource(const std::filesystem::path &path)
{
    auto it = m_cache.find(path.u8string());
    if (it != m_cache.end())
        m_cache.erase(it);
}

void ResourceManager::destroy_resource(const std::u8string &path)
{
    namespace fs = std::filesystem;
    return destroy_resource(fs::path(path));
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

std::shared_ptr<Image> ResourceManager::load_texture(
    SDL_Renderer *renderer,
    const std::u8string &path)
{
    namespace fs = std::filesystem;
    return load_texture(renderer, fs::path(path));
}

std::tuple<size_t, Atlas> ResourceManager::load_textures(
    SDL_Renderer *renderer,
    const char *template_str,
    size_t counts)
{
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

    return {success_count, Atlas(std::move(textures))};
}

std::vector<Atlas> ResourceManager::load_texture_folder(
    SDL_Renderer *renderer,
    const std::filesystem::path &folder_path,
    const char *template_str)
{
    namespace fs = std::filesystem;
    std::vector<Atlas> atlases;

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

        auto subfolder_str = subfolder.u8string();
        auto full_subpath = subfolder / template_str;
        auto full_template = full_subpath.u8string();
        auto full_template_str = std::string(full_template.begin(), full_template.end());

        // 调用 load_textures 自动加载 Atlas
        auto [count, atlas] = load_textures(renderer, full_template_str.c_str(), 0);

        if (count > 0)
            atlases.push_back(std::move(atlas));
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

bool ResourceManager::set_resource_folder(const std::u8string &path)
{
    namespace fs = std::filesystem;
    return set_resource_folder(fs::path(path));
}

bool ResourceManager::is_exist(const std::filesystem::path &path) const
{
    namespace fs = std::filesystem;
    return fs::exists(path);
}

bool ResourceManager::is_exist(const std::u8string &path) const
{
    namespace fs = std::filesystem;
    return is_exist(fs::path(path));
}

bool ResourceManager::is_file(const std::filesystem::path &path) const
{
    namespace fs = std::filesystem;
    return fs::is_regular_file(path);
}

bool ResourceManager::is_file(const std::u8string &path) const
{
    namespace fs = std::filesystem;
    return is_file(fs::path(path));
}

bool ResourceManager::is_directory(const std::filesystem::path &path) const
{
    namespace fs = std::filesystem;
    return fs::is_directory(path);
}

bool ResourceManager::is_directory(const std::u8string &path) const
{
    namespace fs = std::filesystem;
    return is_directory(fs::path(path));
}

std::filesystem::path ResourceManager::absolute_path(const std::filesystem::path &path) const
{
    namespace fs = std::filesystem;
    return fs::absolute(path);
}

std::filesystem::path ResourceManager::absolute_path(const std::u8string &path) const
{
    namespace fs = std::filesystem;
    return absolute_path(fs::path(path));
}

std::filesystem::path ResourceManager::relative_path(
    const std::filesystem::path &path,
    const std::filesystem::path &base) const
{
    namespace fs = std::filesystem;
    return fs::relative(path, base);
}

std::filesystem::path ResourceManager::relative_path(
    const std::u8string &path,
    const std::u8string &base) const
{
    namespace fs = std::filesystem;
    return relative_path(fs::path(path), fs::path(base));
}

std::u8string ResourceManager::absolute_path_str(const std::filesystem::path &path) const
{
    return absolute_path(path).u8string();
}

std::u8string ResourceManager::absolute_path_str(const std::u8string &path) const
{
    return absolute_path(path).u8string();
}

std::u8string ResourceManager::relative_path_str(
    const std::filesystem::path &path,
    const std::filesystem::path &base) const
{
    return relative_path(path, base).u8string();
}

std::u8string ResourceManager::relative_path_str(const std::u8string &path, const std::u8string &base) const
{
    return relative_path(path, base).u8string();
}
