/*
 * workingIndicator.cpp
 *
 *      Author: alex@alexanderresnik.com
 */

#include "workingIndicator.h"

void WorkingIndicator::updateLayout(float x, float y, float w, float h)
{
    rect = {x, y, w, h};
}

bool WorkingIndicator::handleEvent(const SDL_Event *event, bool &isOver)
{
    isOver = false;
    return false;
}

void WorkingIndicator::render(SDL_Renderer *renderer)
{
    if (!active || !visible)
        return;

    // Enable blending for the translucent box
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Background box with alpha transparency
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(alpha * 180));
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, static_cast<Uint8>(alpha * 255));
    SDL_RenderRect(renderer, &rect);

    Uint32 ticks = SDL_GetTicks();
    int dotCount = (ticks / 300) % 7;

    for (int i = 0; i < dotCount; i++)
    {
        SDL_FRect dot;
        dot.x = rect.x + 10 + i * 12;
        dot.y = rect.y + (rect.h / 2) - 2;
        dot.w = 4;
        dot.h = 4;
        SDL_RenderFillRect(renderer, &dot);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}