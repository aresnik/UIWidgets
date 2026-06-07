/*
 * toggleSwitch.h
 *
 *      Author: alex@glassoniongames.com
 */

#include "toggleSwitch.h"
#include <iostream>

ToggleSwitch::ToggleSwitch(SDL_FRect bounds)
    : bounds(bounds), m_handlePos(0.0f), isToggled(false) {}

ToggleSwitch::~ToggleSwitch()
{
    cleanup();
}

void ToggleSwitch::handleEvent(const SDL_Event &event)
{
    if (event.type != SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        return;
    }

    if (event.button.button != SDL_BUTTON_LEFT)
    {
        return;
    }

    float mouseX = event.button.x;
    float mouseY = event.button.y;

    // Check if the click coordinates fall inside the rectangle bounds
    if (mouseX >= bounds.x && mouseX <= bounds.x + bounds.w &&
        mouseY >= bounds.y && mouseY <= bounds.y + bounds.h)
    {

        isToggled = !isToggled;
        std::cout << "Toggle state changed to: " << (isToggled ? "ON" : "OFF") << "\n";
    }
}

void ToggleSwitch::updateLayout(float x, float y, float w, float h)
{
    // If size changes, cached textures are no longer valid
    if (w != bounds.w || h != bounds.h)
    {
        cleanup();
    }
    bounds = {x, y, w, h};
}

void ToggleSwitch::setToggled(bool toggled)
{
    isToggled = toggled;
    m_handlePos = toggled ? 1.0f : 0.0f; // Jump straight to the end of the animation
}

void ToggleSwitch::render(SDL_Renderer *renderer)
{
    if (!m_pillTex || !m_handleTex)
    {
        generateTextures(renderer);
    }

    // Animate the handle position: 0.0 is Left/Gray, 1.0 is Right/Blue
    float targetPos = isToggled ? 1.0f : 0.0f;
    m_handlePos += (targetPos - m_handlePos) * 0.2f; // Smooth easing factor

    // Interpolate background color: Gray (150, 150, 150) to Blue (0, 150, 255)
    Uint8 r = static_cast<Uint8>(150.0f + (0.0f - 150.0f) * m_handlePos);
    Uint8 g = 150;
    Uint8 b = static_cast<Uint8>(150.0f + (255.0f - 150.0f) * m_handlePos);

    // Draw background pill with dynamic color modulation
    SDL_SetTextureColorMod(m_pillTex, r, g, b);
    SDL_SetTextureAlphaMod(m_pillTex, static_cast<Uint8>(alpha * 255.0f));
    SDL_RenderTexture(renderer, m_pillTex, NULL, &bounds);

    float radius = (bounds.h / 2.0f) - 2.0f; // Padding of 2 pixels
    float centerY = bounds.y + (bounds.h / 2.0f);
    float leftX = bounds.x + (bounds.h / 2.0f);
    float rightX = bounds.x + bounds.w - (bounds.h / 2.0f);
    float centerX = leftX + (rightX - leftX) * m_handlePos;

    // Draw the circular handle
    SDL_FRect handleRect = {centerX - radius, centerY - radius, radius * 2.0f, radius * 2.0f};
    SDL_SetTextureAlphaMod(m_handleTex, static_cast<Uint8>(alpha * 255.0f));
    SDL_RenderTexture(renderer, m_handleTex, NULL, &handleRect);
}

void ToggleSwitch::generateTextures(SDL_Renderer *renderer)
{
    cleanup();
    float radius = bounds.h / 2.0f;

    // 1. Generate White Pill Surface (Tintable)
    SDL_Surface *pillSurf = SDL_CreateSurface((int)bounds.w, (int)bounds.h, SDL_PIXELFORMAT_RGBA32);
    for (int y = 0; y < (int)bounds.h; y++)
    {
        for (int x = 0; x < (int)bounds.w; x++)
        {
            float fx = (float)x + 0.5f, fy = (float)y + 0.5f, dist = 0;
            if (fx < radius)
                dist = SDL_sqrtf(SDL_powf(fx - radius, 2) + SDL_powf(fy - radius, 2));
            else if (fx > bounds.w - radius)
                dist = SDL_sqrtf(SDL_powf(fx - (bounds.w - radius), 2) + SDL_powf(fy - radius, 2));
            else
                dist = SDL_fabsf(fy - radius);

            float coverage = SDL_clamp(radius - dist + 0.5f, 0.0f, 1.0f);
            Uint8 *pixel = (Uint8 *)pillSurf->pixels + y * pillSurf->pitch + x * 4;
            pixel[0] = 255;
            pixel[1] = 255;
            pixel[2] = 255;
            pixel[3] = (Uint8)(255 * coverage);
        }
    }
    m_pillTex = SDL_CreateTextureFromSurface(renderer, pillSurf);
    SDL_DestroySurface(pillSurf);

    // 2. Generate White Handle Surface
    float hRadius = (bounds.h / 2.0f) - 2.0f;
    int hSize = (int)(hRadius * 2.0f) + 2;
    SDL_Surface *handleSurf = SDL_CreateSurface(hSize, hSize, SDL_PIXELFORMAT_RGBA32);
    float hCenter = hSize / 2.0f;

    for (int y = 0; y < hSize; y++)
    {
        for (int x = 0; x < hSize; x++)
        {
            float dx = (float)x + 0.5f - hCenter, dy = (float)y + 0.5f - hCenter;
            float dist = SDL_sqrtf(dx * dx + dy * dy);
            float coverage = SDL_clamp(hRadius - dist + 0.5f, 0.0f, 1.0f);

            Uint8 *pixel = (Uint8 *)handleSurf->pixels + y * handleSurf->pitch + x * 4;
            pixel[0] = 255;
            pixel[1] = 255;
            pixel[2] = 255;
            pixel[3] = (Uint8)(255 * coverage);
        }
    }
    m_handleTex = SDL_CreateTextureFromSurface(renderer, handleSurf);
    SDL_DestroySurface(handleSurf);

    if (m_pillTex)
        SDL_SetTextureBlendMode(m_pillTex, SDL_BLENDMODE_BLEND);
    if (m_handleTex)
        SDL_SetTextureBlendMode(m_handleTex, SDL_BLENDMODE_BLEND);
}

void ToggleSwitch::cleanup()
{
    if (m_pillTex)
    {
        SDL_DestroyTexture(m_pillTex);
        m_pillTex = nullptr;
    }
    if (m_handleTex)
    {
        SDL_DestroyTexture(m_handleTex);
        m_handleTex = nullptr;
    }
}

bool ToggleSwitch::isSwitchToggled() const
{
    return isToggled;
}
