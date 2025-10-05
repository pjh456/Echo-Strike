#ifndef INCLUDE_ATLAS
#define INCLUDE_ATLAS

#include <echo_strike/image/image.hpp>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <vector>

class Atlas
{
private:
    std::vector<Image> textures;
    std::string name;

public:
    Atlas();
    Atlas(const std::vector<SDL_Texture *> &);
    Atlas(std::vector<Image> &&) noexcept;
    ~Atlas();

    Atlas(const Atlas &) = delete;
    Atlas &operator=(const Atlas &) = delete;

    Atlas(Atlas &&) noexcept;
    Atlas &operator=(Atlas &&) noexcept;

public:
    bool push_back(SDL_Texture *);
    bool push_back(SDL_Renderer *, const char *);

    bool insert(size_t, SDL_Texture *);
    bool insert(SDL_Renderer *, size_t, const char *);

    size_t size() const { return textures.size(); }
    bool empty() const { return textures.empty(); }
    void clear();

    std::string get_name() const { return name; }
    void set_name(const std::string &str) { name = str; }

public:
    Image &operator[](size_t);
    const Image &operator[](size_t) const;
};

#endif // INCLUDE_ATLAS