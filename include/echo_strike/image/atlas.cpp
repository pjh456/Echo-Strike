#include <echo_strike/image/atlas.hpp>

#include <utility>
#include <format>

Atlas::Atlas() = default;

Atlas::Atlas(const std::vector<SDL_Texture *> &texs)
{
    if (texs.empty())
        return;

    textures.reserve(texs.size());
    for (const auto &tex : texs)
    {
        textures.emplace_back(tex);
    }
}

Atlas::Atlas(std::vector<Image> &&texs) noexcept
    : textures(std::move(texs))
{
}

Atlas::~Atlas() { clear(); }

Atlas::Atlas(Atlas &&other) noexcept
    : textures(std::move(other.textures))
{
}

Atlas &Atlas::operator=(Atlas &&other) noexcept
{
    if (this == &other)
        return *this;
    textures = std::move(other.textures);
    return *this;
}

bool Atlas::push_back(SDL_Texture *tex)
{
    if (tex == nullptr)
        return false;
    textures.emplace_back(tex);
    return true;
}

bool Atlas::push_back(SDL_Renderer *renderer, const char *path)
{
    return this->push_back(IMG_LoadTexture(renderer, path));
}

bool Atlas::insert(size_t index, SDL_Texture *tex)
{
    if (tex == nullptr)
        return false;

    Image temp(tex);
    textures.insert(textures.begin() + index, std::move(temp));
    return true;
}

bool Atlas::insert(SDL_Renderer *renderer, size_t index, const char *path)
{
    return this->insert(index, IMG_LoadTexture(renderer, path));
}

void Atlas::clear()
{
    textures.clear();
}

Image &Atlas::operator[](size_t index) { return textures[index]; }

const Image &Atlas::operator[](size_t index) const { return textures[index]; }