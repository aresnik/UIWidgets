/*
 * label.h
 *
 *      Author: alex@glassoniongames.com
 */

#ifndef LABEL_H
#define LABEL_H

#include "widgets.h"

class Label : public Widget
{
public:
    enum Alignment
    {
        ALIGN_LEFT,
        ALIGN_CENTER,
        ALIGN_RIGHT
    };

    Label() = default;
    virtual ~Label() override;

    // Prevent copying (since we manage an SDL_Texture)
    Label(const Label &) = delete;
    Label &operator=(const Label &) = delete;

    // Modular loading: Accept an existing font pointer instead of a path
    bool load(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Color color);

    void setText(SDL_Renderer *renderer, const std::string &text);
    void setColor(SDL_Renderer *renderer, SDL_Color color);
    void setAlignment(Alignment align) { m_Alignment = align; }

    virtual void render(SDL_Renderer *renderer) override;
    virtual void updateLayout(float x, float y, float w, float h) override;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;

private:
    void cleanup();
    void updateTexture(SDL_Renderer *renderer);

    TTF_Font *m_Font = nullptr;
    std::string m_Text;
    SDL_Color m_Color{255, 255, 255, 255};
    SDL_Texture *m_Texture = nullptr;
    Alignment m_Alignment = ALIGN_LEFT;
};

#endif
