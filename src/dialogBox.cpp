/*
 * dialogBox.cpp
 *
 *      Author: alex@glassoniongames.com
 */

#include "dialogBox.h"

DialogBox::DialogBox(float x, float y, float width, float height,
                     SDL_Color bgColor, SDL_Color borderColor, float borderWidth)
    : m_bgColor(bgColor),
      m_borderColor(borderColor),
      m_borderWidth(borderWidth)
{
    rect = {x, y, width, height};
}

void DialogBox::addChild(Widget *child)
{
    if (child)
    {
        m_children.push_back(child);
    }
}

void DialogBox::updateLayout(float x, float y, float w, float h)
{
    rect = {x, y, w, h};

    // Update children with padding calculated from percentages
    float padLeftPixels = w * m_padLeft;
    float padRightPixels = w * m_padRight;
    float padTopPixels = h * m_padTop;
    float padBottomPixels = h * m_padBottom;

    float childX = x + padLeftPixels;
    float childY = y + padTopPixels;
    float childW = w - (padLeftPixels + padRightPixels);
    float childH = h - (padTopPixels + padBottomPixels);

    for (auto* child : m_children)
        child->updateLayout(childX, childY, childW, childH);
}

void DialogBox::render(SDL_Renderer *renderer)
{
    if (!visible)
        return;

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // 1. Draw the background
    SDL_SetRenderDrawColor(renderer, m_bgColor.r, m_bgColor.g, m_bgColor.b,
                           static_cast<Uint8>(m_bgColor.a * alpha));
    SDL_RenderFillRect(renderer, &rect);

    // 2. Draw the border
    if (m_borderWidth > 0.0f)
    {
        SDL_SetRenderDrawColor(renderer, m_borderColor.r, m_borderColor.g, m_borderColor.b,
                               static_cast<Uint8>(m_borderColor.a * alpha));

        SDL_FRect borderRect = rect;
        for (int i = 0; i < static_cast<int>(m_borderWidth); ++i)
        {
            SDL_RenderRect(renderer, &borderRect);
            borderRect.x -= 1.0f;
            borderRect.y -= 1.0f;
            borderRect.w += 2.0f;
            borderRect.h += 2.0f;
        }
    }

    // 3. Render children
    for (auto *child : m_children)
    {
        child->alpha = this->alpha; // Propagate alpha to children
        child->render(renderer);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

bool DialogBox::handleEvent(const SDL_Event *event, bool &isOver)
{
    if (!visible || !enabled)
    {
        isOver = false;
        return false;
    }

    float mx = -1.0f, my = -1.0f;

    // Pull logical coordinates from the event, which were converted in main.cpp
    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        mx = event->motion.x;
        my = event->motion.y;
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        mx = event->button.x;
        my = event->button.y;
    }

    // Input Sinking: Check if mouse is within the dialog area
    if (mx >= 0)
    {
        isOver = (mx >= rect.x && mx <= rect.x + rect.w &&
                  my >= rect.y && my <= rect.y + rect.h);
    }

    bool handled = false;
    // Pass events to children in reverse (top-to-bottom)
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
    {
        bool childOver = false;
        if ((*it)->handleEvent(event, childOver))
        {
            handled = true;
        }
        if (childOver)
            isOver = true;
    }

    return handled;
}

void DialogBox::setColors(SDL_Color bgColor, SDL_Color borderColor)
{
    m_bgColor = bgColor;
    m_borderColor = borderColor;
}
