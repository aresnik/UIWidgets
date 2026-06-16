/*
 * workingIndicator.cpp
 *
 *      Author: alex@glassoniongames.com
 */

#include "workingIndicator.h"
#include <algorithm>

void WorkingIndicator::updateLayout(float x, float y, float w, float h)
{
    // Maintain an aspect ratio of roughly 4.16:1 (like the original 100x24)
    float aspect = 4.16f;

    // Let it scale up proportionally, but cap the height so it doesn't become gigantic
    float maxH = std::min(h, 36.0f);

    float finalH = maxH;
    float finalW = finalH * aspect;

    // If it somehow exceeds the available width, constrain it by width instead
    if (finalW > w)
    {
        finalW = w;
        finalH = finalW / aspect;
    }

    // Center it horizontally and vertically within the provided flex space
    float finalX = x + (w - finalW) / 2.0f;
    float finalY = y + (h - finalH) / 2.0f;

    rect = {finalX, finalY, finalW, finalH};
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

    // Solid black background to ensure high contrast for the indicator dots
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(alpha * 255));
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, static_cast<Uint8>(alpha * 255));
    SDL_RenderRect(renderer, &rect);

    Uint32 ticks = SDL_GetTicks();
    int dotCount = (ticks / 300) % 7;

    // Calculate dynamic dot size and spacing based on the final scaled rectangle
    float dotSize = rect.h * 0.25f;                     // Dots are 25% of the indicator's height
    float spacing = rect.w / 7.0f;                      // Evenly divide the width to perfectly fit up to 7 dots
    float startX = rect.x + (spacing - dotSize) / 2.0f; // Center the first dot in its slot

    for (int i = 0; i < dotCount; i++)
    {
        SDL_FRect dot;
        dot.x = startX + i * spacing;
        dot.y = rect.y + (rect.h - dotSize) / 2.0f;
        dot.w = dotSize;
        dot.h = dotSize;
        SDL_RenderFillRect(renderer, &dot);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}