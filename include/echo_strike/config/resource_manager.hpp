#ifndef INCLUDE_RESOURCE_MANAGER
#define INCLUDE_RESOURCE_MANAGER

#include <string>
#include <memory>
#include <unordered_map>
#include <filesystem>
#include <typeindex>
#include <tuple>
#include <vector>

struct SDL_Texture;
struct SDL_Renderer;

class Image;
class Atlas;
class Renderer;

class ResourceManager
{
public:
    using KeyType = std::string;
    using ResourcePool = std::unordered_map<KeyType, std::shared_ptr<void>>;
    using AtlasPool = std::unordered_map<KeyType, std::shared_ptr<Atlas>>;
    using TypeTable = std::unordered_map<KeyType, std::type_index>;

public:
    static ResourceManager &instance();

    std::shared_ptr<std::string> load_resource_str(const std::filesystem::path &);

    void destroy_resource(const std::filesystem::path &);
    void destroy_resource(void *);

public:
    std::shared_ptr<Image>
    load_image(SDL_Renderer *, const std::filesystem::path &);
    std::tuple<size_t, std::shared_ptr<Atlas>>
    load_atlas(SDL_Renderer *, const char *, size_t = 0);
    std::vector<std::shared_ptr<Atlas>>
    load_atlases(SDL_Renderer *, const std::filesystem::path &, const char *);

private:
    std::filesystem::path m_folder;
    ResourcePool m_cache;
    AtlasPool m_atlases;
    TypeTable m_types;

public:
    ResourceManager()
    {
        set_default_folder();
    }
    ~ResourceManager() = default;

    void set_default_folder();

    bool set_resource_folder(const std::filesystem::path &);

public:
    bool is_exist(const std::filesystem::path &) const;
    bool is_file(const std::filesystem::path &) const;
    bool is_directory(const std::filesystem::path &) const;

public:
    std::filesystem::path absolute_path(const std::filesystem::path &) const;
    std::string absolute_path_str(const std::filesystem::path &) const;

    std::filesystem::path
    relative_path(
        const std::filesystem::path &,
        const std::filesystem::path & = std::filesystem::current_path()) const;
    std::string
    relative_path_str(
        const std::filesystem::path &,
        const std::filesystem::path & = std::filesystem::current_path()) const;

public:
    ResourcePool &get_cache() { return m_cache; }
    const ResourcePool &get_cache() const { return m_cache; }

    AtlasPool &get_atlases() { return m_atlases; }
    const AtlasPool &get_atlases() const { return m_atlases; }
    std::shared_ptr<Atlas> get_atlas(const KeyType &) const;

public:
    template <typename T>
    void store(const KeyType &str, std::shared_ptr<T> resource)
    {
        auto key = normalize_key(str);
        m_cache[key] = resource;
        auto it = m_types.find(key);
        if (it == m_types.end() || it->second != std::type_index(typeid(T)))
            m_types.insert({key, std::type_index(typeid(T))});
    }

    template <typename T>
    std::shared_ptr<T> get(const KeyType &key)
    {
        auto cache_it = m_cache.find(key);
        if (cache_it == m_cache.end())
            return nullptr;

        auto type_it = m_types.find(key);
        if (type_it == m_types.end() || type_it->second != std::type_index(typeid(T)))
            throw std::bad_cast();

        return std::static_pointer_cast<T>(cache_it->second);
    }

private:
    inline static std::string normalize_key(const std::filesystem::path &path);

    // 这个重载函数也保持不变，它会调用上面的版本
    inline static std::string normalize_key(const std::string &str)
    {
        // 使用u8path从UTF-8字符串构造path对象，避免编码问题
        return normalize_key(std::filesystem::u8path(str));
    }
};

#endif // INCLUDE_RESOURCE_MANAGER