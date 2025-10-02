#include <echo_strike/image/animation.hpp>

#include <utility>

Animation::Animation(float interval)
    : m_interval(interval),
      frame_idx(0),
      timer(
          [this]()
          { frame_idx = frame_idx == frames.size() - 1 ? 0 : frame_idx + 1; }),
      m_position(),
      m_anchor_mode(AnchorMode::Centered)
{
    timer.set_timeout_time(m_interval);
}

Animation::~Animation()
{
}

Animation::Animation(Animation &&other) noexcept
    : m_interval(other.m_interval),
      timer(std::move(other.timer)),
      frames(std::move(frames)),
      frame_idx(other.frame_idx),
      m_position(other.m_position),
      m_anchor_mode(other.m_anchor_mode)
{
}

Animation &Animation::operator=(Animation &&other) noexcept
{
    if (this == &other)
        return *this;

    timer = std::move(other.timer);
    frames = std::move(other.frames);
    frame_idx = other.frame_idx;
    m_anchor_mode = other.m_anchor_mode;
    return *this;
}

void Animation::insert_frame(size_t idx, const Image *img, const SDL_FRect *rect)
{
    frames.insert(frames.begin() + idx, {img, *rect});
}

void Animation::add_frame(const Image *img, const SDL_FRect *rect)
{
    frames.push_back({img, *rect});
}

void Animation::insert_frames(size_t idx, const Image *img, size_t wcount, size_t hcount)
{
    int frame_width = img->get_width() / wcount;
    int frame_height = img->get_height() / hcount;
    int insert_count = 0;
    frames.reserve(frames.size() + wcount * hcount);

    SDL_FRect rect;
    rect.w = frame_width, rect.h = frame_height;

    for (int ridx = 0; ridx < hcount; ++ridx)
    {
        for (int cidx = 0; cidx < hcount; ++cidx)
        {
            rect.x = cidx * frame_width;
            rect.y = ridx * frame_height;

            frames.emplace(frames.begin() + idx + insert_count, img, rect);
            insert_count++;
        }
    }
}

void Animation::add_frames(const Image *img, size_t wcount, size_t hcount)
{
    int frame_width = img->get_width() / wcount;
    int frame_height = img->get_height() / hcount;
    frames.reserve(frames.size() + wcount * hcount);

    SDL_FRect rect;
    rect.w = frame_width, rect.h = frame_height;

    for (int ridx = 0; ridx < hcount; ++ridx)
    {
        for (int cidx = 0; cidx < hcount; ++cidx)
        {
            rect.x = cidx * frame_width;
            rect.y = ridx * frame_height;
            frames.emplace_back(img, rect);
        }
    }
}

void Animation::insert_frames(size_t idx, const Atlas &atlas)
{
    SDL_FRect rect;
    rect.x = rect.y = 0;
    frames.reserve(frames.size() + atlas.size());

    for (size_t i = 0; i < atlas.size(); ++i)
    {
        rect.w = atlas[i].get_width();
        rect.h = atlas[i].get_height();
        frames.emplace(frames.begin() + idx + i, &(atlas[i]), rect);
    }
}

void Animation::add_frames(const Atlas &atlas)
{
    SDL_FRect rect;
    rect.x = rect.y = 0;
    frames.reserve(frames.size() + atlas.size());

    for (size_t i = 0; i < atlas.size(); ++i)
    {
        rect.w = atlas[i].get_width();
        rect.h = atlas[i].get_height();
        frames.emplace_back(&(atlas[i]), rect);
    }
}

void Animation::render(SDL_Renderer *renderer) const
{
    auto frame = get_current_frame();
    SDL_FRect dst;
    dst.w = frame.img->get_width(), dst.h = frame.img->get_height();
    dst.x = m_position.get_x() + (frame.img->get_width() / 2);
    dst.y = m_position.get_y();
    if (m_anchor_mode == AnchorMode::BottomCentered)
        dst.y += (frame.img->get_height());
    else
        dst.y += (frame.img->get_height() / 2);

    SDL_RenderTexture(renderer, frame.img->get_texture(), &frame.src, &dst);
}