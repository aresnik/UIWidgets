/*
 * textureButton.cpp
 *
 *      Author: alex@glassoniongames.com
 */

#include "textureButton.h"

TextureButton::TextureButton(float x, float y, float width, float height,
                             SDL_Texture *normalTex,
                             SDL_Texture *hoverTex,
                             SDL_Texture *pressedTex)
    : m_currentState(STATE_NORMAL), m_hoverScale(1.05f), m_onClickCallback(nullptr)
{
    rect = {x, y, width, height};
    setTextures(normalTex, hoverTex, pressedTex);
}

bool TextureButton::isMouseInside(float mouseX, float mouseY) const
{
    return (mouseX >= rect.x &&
            mouseX <= rect.x + rect.w &&
            mouseY >= rect.y &&
            mouseY <= rect.y + rect.h);
}

bool TextureButton::handleEvent(const SDL_Event *event, bool &isOver)
{
    isOver = false;
    if (!visible || !enabled)
        return false;

    float mx, my;
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
    else
    {
        return false;
    }

    isOver = isMouseInside(mx, my);

    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        if (isOver)
        {
            if (m_currentState != STATE_PRESSED)
                m_currentState = STATE_HOVER;
        }
        else
        {
            m_currentState = STATE_NORMAL;
        }
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (isOver && event->button.button == SDL_BUTTON_LEFT)
        {
            m_currentState = STATE_PRESSED;
        }
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            bool wasPressed = (m_currentState == STATE_PRESSED);
            if (isOver)
            {
                m_currentState = STATE_HOVER;
                if (wasPressed)
                {
                    if (m_onClickCallback)
                        m_onClickCallback();
                    return true;
                }
            }
            else
            {
                m_currentState = STATE_NORMAL;
            }
        }
    }
    return false;
}

void TextureButton::render(SDL_Renderer *renderer)
{
    if (!visible)
        return;

    SDL_Texture *textureToDraw = m_textures[m_currentState];
    if (textureToDraw)
    {
        // Apply global alpha and dim if disabled
        Uint8 mod = enabled ? 255 : 160;
        SDL_SetTextureColorMod(textureToDraw, mod, mod, mod);
        SDL_SetTextureAlphaMod(textureToDraw, static_cast<Uint8>(alpha * 255.0f));

        SDL_FRect drawRect = rect;
        // Apply hover scaling (Juice!)
        if (m_currentState == STATE_HOVER && m_hoverScale != 1.0f)
        {
            float newW = rect.w * m_hoverScale;
            float newH = rect.h * m_hoverScale;
            drawRect.x -= (newW - rect.w) / 2.0f;
            drawRect.y -= (newH - rect.h) / 2.0f;
            drawRect.w = newW;
            drawRect.h = newH;
        }

        SDL_RenderTexture(renderer, textureToDraw, nullptr, &drawRect);
    }
}

void TextureButton::updateLayout(float x, float y, float w, float h)
{
    rect = {x, y, w, h};
}

void TextureButton::setTextures(SDL_Texture *normal, SDL_Texture *hover, SDL_Texture *pressed)
{
    m_textures[STATE_NORMAL] = normal;
    m_textures[STATE_HOVER] = hover ? hover : normal;
    m_textures[STATE_PRESSED] = pressed ? pressed : normal;
}

void TextureButton::setOnClickCallback(std::function<void()> callback)
{
    m_onClickCallback = callback;
}
