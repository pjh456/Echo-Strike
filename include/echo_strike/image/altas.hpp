#ifndef INCLUDE_ATLAS
#define INCLUDE_ATLAS

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <vector>

class Atlas
{
private:
    std::vector<SDL_Texture *> textures;

public:
    Atlas();
    ~Atlas();

    Atlas(const Atlas &) = delete;
    Atlas &operator=(const Atlas &) = delete;

    Atlas(Atlas &&) noexcept;
    Atlas &operator=(Atlas &&) noexcept;

public:
    bool push_back(SDL_Renderer *, const char *);
    bool insert(SDL_Renderer *, size_t, const char *);
    size_t load(SDL_Renderer *, const char *, size_t);

    size_t size() { return textures.size(); }
    bool empty() { return textures.empty(); }
    void clear();

public:
    SDL_Texture &operator[](size_t);
    const SDL_Texture &operator[](size_t) const;
};

#endif // INCLUDE_ATLAS