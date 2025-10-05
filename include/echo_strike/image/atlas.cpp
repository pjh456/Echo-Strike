#include <echo_strike/image/atlas.hpp>

#include <utility>
#include <format>

Atlas::Atlas() = default;

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

bool Atlas::push_back(SDL_Renderer *renderer, SDL_Texture *tex)
{
    if (tex == nullptr)
        return false;
    textures.emplace_back(tex);
    return true;
}

bool Atlas::push_back(SDL_Renderer *renderer, const char *path)
{
    return this->push_back(renderer, IMG_LoadTexture(renderer, path));
}

bool Atlas::insert(SDL_Renderer *renderer, size_t index, SDL_Texture *tex)
{
    if (tex == nullptr)
        return false;

    Image temp(tex);
    textures.insert(textures.begin() + index, std::move(temp));
    return true;
}

bool Atlas::insert(SDL_Renderer *renderer, size_t index, const char *path)
{
    return this->insert(renderer, index, IMG_LoadTexture(renderer, path));
}

size_t Atlas::load(SDL_Renderer *renderer, const char *template_str, size_t counts)
{
    size_t success_count = 0;
    textures.reserve(counts);

    for (size_t idx = 0; idx < counts; ++idx)
    {
        auto fmt_path = std::vformat(template_str, std::make_format_args(idx));
        if (this->push_back(renderer, fmt_path.c_str()))
            success_count++;
    }
    return success_count;
}

void Atlas::clear()
{
    textures.clear();
}

Image &Atlas::operator[](size_t index) { return textures[index]; }

const Image &Atlas::operator[](size_t index) const { return textures[index]; }