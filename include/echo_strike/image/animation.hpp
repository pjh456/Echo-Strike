#ifndef INCLUDE_ANIMATION
#define INCLUDE_ANIMATION

#include <echo_strike/utils/class_marcos.hpp>

#include <echo_strike/transform/vec2.hpp>
#include <echo_strike/transform/rect.hpp>
#include <echo_strike/core/timer.hpp>
#include <echo_strike/image/image.hpp>
#include <echo_strike/image/atlas.hpp>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <vector>

class Animation
{
public:
    enum class AnchorMode
    {
        Centered,
        BottomCentered
    };

private:
    struct Frame
    {
        const Image *img = nullptr;
        Rect src;
    };

private:
    Timer timer;
    std::vector<Frame> frames;
    size_t frame_idx;
    CLASS_PROPERTY(float, interval);
    CLASS_PROPERTY(Vec2, position);
    CLASS_PROPERTY(AnchorMode, anchor_mode);

public:
    Animation(float);
    ~Animation();

    Animation(const Animation &) = delete;
    Animation &operator=(const Animation &) = delete;

    Animation(Animation &&) noexcept;
    Animation &operator=(Animation &&) noexcept;

public:
    void set_one_shot(bool flag) { timer.set_one_shot(flag); }
    void pause() { timer.pause(); }
    void resume() { timer.resume(); }
    void restart() { frame_idx = 0, timer.restart(); }
    void update(float delta) { timer.update(delta); }

    size_t size() const { return frames.size(); }

public:
    void insert_frame(size_t, const Image *, const SDL_FRect *);
    void add_frame(const Image *, const SDL_FRect *);

    void insert_frames(size_t, const Image *, size_t = 1, size_t = 1);
    void add_frames(const Image *, size_t = 1, size_t = 1);

    void insert_frames(size_t, const Atlas &);
    void add_frames(const Atlas &);

public:
    Frame &get_current_frame() { return frames[frame_idx]; }
    const Frame &get_current_frame() const { return frames[frame_idx]; }

    Frame &operator[](size_t idx) { return frames[idx]; }
    const Frame &operator[](size_t idx) const { return frames[idx]; }

    void render(SDL_Renderer *) const;
};

#endif // INCLUDE_ANIMATION