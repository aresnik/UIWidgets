#include "toggleSwitch.h"
#include <iostream>

ToggleSwitch::ToggleSwitch(SDL_FRect bounds)
    : bounds(bounds), m_handlePos(0.0f), isToggled(false) {}

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
    bounds = {x, y, w, h};
}

void ToggleSwitch::setToggled(bool toggled)
{
    isToggled = toggled;
    m_handlePos = toggled ? 1.0f : 0.0f; // Jump straight to the end of the animation
}

void ToggleSwitch::render(SDL_Renderer *renderer)
{
    // Animate the handle position: 0.0 is Left/Gray, 1.0 is Right/Blue
    float targetPos = isToggled ? 1.0f : 0.0f;
    m_handlePos += (targetPos - m_handlePos) * 0.2f; // Smooth easing factor

    // Interpolate background color: Gray (150, 150, 150) to Blue (0, 150, 255)
    Uint8 r = static_cast<Uint8>(150.0f + (0.0f - 150.0f) * m_handlePos);
    Uint8 g = 150;
    Uint8 b = static_cast<Uint8>(150.0f + (255.0f - 150.0f) * m_handlePos);
    SDL_Color bgColor = {r, g, b, 255};

    // Draw the rounded background (pill shape)
    drawPill(renderer, bounds, bgColor);

    float radius = (bounds.h / 2.0f) - 2.0f; // Padding of 2 pixels
    float centerY = bounds.y + (bounds.h / 2.0f);
    float leftX = bounds.x + (bounds.h / 2.0f);
    float rightX = bounds.x + bounds.w - (bounds.h / 2.0f);
    float centerX = leftX + (rightX - leftX) * m_handlePos;

    drawCircle(renderer, centerX, centerY, radius, {255, 255, 255, 255});
}

void ToggleSwitch::drawCircle(SDL_Renderer *renderer, float centerX, float centerY, float radius, SDL_Color color)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Procedural anti-aliased circle drawing
    for (float dy = -radius - 1.0f; dy <= radius + 1.0f; dy += 1.0f)
    {
        for (float dx = -radius - 1.0f; dx <= radius + 1.0f; dx += 1.0f)
        {
            float dist = SDL_sqrtf(dx * dx + dy * dy);

            // Smoothly fade alpha based on distance from the edge for anti-aliasing
            float coverage = SDL_clamp(radius - dist + 0.5f, 0.0f, 1.0f);

            if (coverage > 0.0f)
            {
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                       static_cast<Uint8>(color.a * coverage));
                SDL_RenderPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

void ToggleSwitch::drawPill(SDL_Renderer *renderer, SDL_FRect rect, SDL_Color color)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    float radius = rect.h / 2.0f;

    // Iterate over the rectangle area plus a small margin for anti-aliasing
    for (float dy = -0.5f; dy <= rect.h + 0.5f; dy += 1.0f)
    {
        for (float dx = -0.5f; dx <= rect.w + 0.5f; dx += 1.0f)
        {
            float dist = 0;
            if (dx < radius)
            {
                // Left circle cap
                float ddx = dx - radius;
                float ddy = dy - radius;
                dist = SDL_sqrtf(ddx * ddx + ddy * ddy);
            }
            else if (dx > rect.w - radius)
            {
                // Right circle cap
                float ddx = dx - (rect.w - radius);
                float ddy = dy - radius;
                dist = SDL_sqrtf(ddx * ddx + ddy * ddy);
            }
            else
            {
                // Middle rectangular section: distance is just vertical offset from center line
                dist = SDL_fabsf(dy - radius);
            }

            float coverage = SDL_clamp(radius - dist + 0.5f, 0.0f, 1.0f);
            if (coverage > 0.0f)
            {
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, static_cast<Uint8>(color.a * coverage));
                SDL_RenderPoint(renderer, rect.x + dx, rect.y + dy);
            }
        }
    }
}

bool ToggleSwitch::isSwitchToggled() const
{
    return isToggled;
}
