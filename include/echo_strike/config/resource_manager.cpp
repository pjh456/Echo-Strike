#include <echo_strike/config/resource_manager.hpp>

#include <echo_strike/image/image.hpp>
#include <echo_strike/image/atlas.hpp>
#include <echo_strike/config/config_manager.hpp>

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

    auto key = normalize_key(remove_prefix(abs_path));
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

std::shared_ptr<Image> ResourceManager::load_image(
    SDL_Renderer *renderer,
    const std::filesystem::path &path)
{
    if (!is_file(path))
        return nullptr;

    auto [key, cache] = load_cache<Image>(path);
    if (cache)
        return cache;

    auto raw = IMG_LoadTexture(renderer, path.string().c_str());
    if (!raw)
        return nullptr;

    auto img = std::make_shared<Image>(raw);
    store<Image>(key, img);

    return img;
}

std::tuple<size_t, std::shared_ptr<Atlas>> ResourceManager::load_atlas(
    SDL_Renderer *renderer,
    const char *template_str,
    size_t counts)
{
    auto temp_path = std::filesystem::u8path(template_str).parent_path();
    auto [key, cache] = load_atlas_cache(temp_path);
    if (cache)
        return {cache->size(), cache};

    size_t success_count = 0;
    std::vector<Image> images;

    for (size_t idx = 0;
         counts == 0 ? true : idx < counts;
         ++idx)
    {
        auto fmt_path = std::vformat(template_str, std::make_format_args(idx));
        auto img_ptr = load_image(renderer, fmt_path);

        if (img_ptr.get() != nullptr)
        {
            success_count++;
            images.push_back(std::move(*img_ptr.get()));
        }
        else if (idx != 0)
            break;
    }

    auto ptr = std::make_shared<Atlas>(std::move(images));
    if (ptr->size() > 0)
        m_atlases.insert({key, ptr});

    // auto u8_atlas_name = key;
    // std::string atlas_name(reinterpret_cast<const char *>(u8_atlas_name.c_str()), u8_atlas_name.size());
    ptr->set_name(key);
    return {success_count, ptr};
}

std::tuple<size_t, std::shared_ptr<Atlas>>
ResourceManager::
    load_atlas(
        SDL_Renderer *renderer,
        const std::filesystem::path &path,
        int rows,
        int cols)
{
    auto [key, cache] = load_atlas_cache(path);
    if (cache)
        return {cache->size(), cache};

    auto image = load_image(renderer, path);
    if (!image)
        return {0, nullptr};

    int frame_w = image->get_width() / cols;
    int frame_h = image->get_height() / rows;
    auto tex = image->get_texture();

    std::vector<Image> frames;
    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            SDL_FRect frect{(float)c * frame_w, (float)r * frame_h, (float)frame_w, (float)frame_h};
            SDL_Texture *sub_tex =
                SDL_CreateTexture(
                    renderer,
                    SDL_PIXELFORMAT_RGBA32,
                    SDL_TEXTUREACCESS_TARGET,
                    frame_w, frame_h);

            SDL_SetRenderTarget(renderer, sub_tex);
            SDL_RenderTexture(renderer, tex, &frect, nullptr);
            SDL_SetRenderTarget(renderer, nullptr);
            frames.emplace_back(sub_tex);
        }
    }

    auto ptr = std::make_shared<Atlas>(std::move(frames));
    if (ptr->size() > 0)
        m_atlases.insert({key, ptr});

    ptr->set_name(key);
    return {ptr->size(), ptr};
}

std::vector<std::shared_ptr<Atlas>> ResourceManager::load_atlases(
    SDL_Renderer *renderer,
    const std::filesystem::path &folder_path,
    const char *template_str)
{
    namespace fs = std::filesystem;
    std::vector<std::shared_ptr<Atlas>> atlases;

    m_folder = folder_path;
    auto full_path = absolute_path(folder_path);

    if (!is_directory(full_path))
        return {};

    std::vector<fs::directory_entry> dirs;
    for (auto &entry : fs::directory_iterator(full_path))
        dirs.push_back(entry);

    while (!dirs.empty())
    {
        auto entry = dirs.back();
        dirs.pop_back();

        const auto &subpath = entry.path();

        auto [key, cache] = load_atlas_cache(subpath);
        if (cache)
        {
            atlases.push_back(cache);
            continue;
        }

        if (fs::is_regular_file(subpath))
        {
            try
            {
                auto &config_file = ConfigManager::instance().get_config();
                auto obj = config_file.get()->as_object();
                auto path_config = config_file["sprite_cut"][key];
                auto [count, atlas] =
                    load_atlas(
                        renderer,
                        subpath,
                        path_config["rows"].as_int(),
                        path_config["cols"].as_int());

                if (count > 0)
                    atlases.push_back(atlas);
            }
            catch (...)
            {
                continue;
            }
        }
        else if (fs::is_directory(subpath))
        {
            for (auto &subentry : fs::directory_iterator(entry))
                dirs.push_back(subentry);

            auto subfolder_str = subpath.string();
            auto full_subpath = subpath / template_str;
            auto full_template_str = full_subpath.u8string();
            std::string full_template_std_str(reinterpret_cast<const char *>(full_template_str.c_str()), full_template_str.size());
            auto [count, atlas] = load_atlas(renderer, full_template_std_str.c_str(), 0);
            if (count > 0)
                atlases.push_back(atlas);
        }
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

std::filesystem::path
ResourceManager::
    remove_prefix(const std::filesystem::path &path)
{
    namespace fs = std::filesystem;

    auto abs_path = absolute_path(path).lexically_normal();
    auto folder = absolute_path(m_folder).lexically_normal();

    fs::path rel_path;
    auto it_abs = abs_path.begin();
    auto it_folder = folder.begin();
    while (it_abs != abs_path.end() && it_folder != folder.end() && *it_abs == *it_folder)
    {
        ++it_abs;
        ++it_folder;
    }
    for (; it_abs != abs_path.end(); ++it_abs)
    {
        rel_path /= *it_abs;
    }

    return rel_path;
}

std::string ResourceManager::normalize_key(const std::filesystem::path &path)
{
    namespace fs = std::filesystem;
    auto normal_path = path.lexically_normal();

    std::u8string u8str = normal_path.u8string();

    std::string result(reinterpret_cast<const char *>(u8str.c_str()), u8str.size());

    std::replace(result.begin(), result.end(), '\\', '/');

    return result;
}