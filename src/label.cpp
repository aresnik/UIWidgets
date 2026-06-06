/*
 * label.cpp
 *
 *      Author: alex@alexanderresnik.com
 */

#include "label.h"

Label::~Label() {
    cleanup();
}

bool Label::load(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Color color) {
    m_Font = font;
    m_Text = text;
    m_Color = color;
    
    if (!m_Font) return false;

    updateTexture(renderer);
    return (m_Texture != nullptr);
}

void Label::cleanup() {
    if (m_Texture) {
        SDL_DestroyTexture(m_Texture);
        m_Texture = nullptr;
    }
}

void Label::updateTexture(SDL_Renderer* renderer) {
    cleanup();

    if (!m_Font || m_Text.empty()) return;

    // Render text to surface
    // SDL3 explicitly benefits from passing the string length
    SDL_Surface* textSurface = TTF_RenderText_Blended(m_Font, m_Text.c_str(), m_Text.length(), m_Color);
    if (!textSurface) {
        return;
    }

    // Convert surface to GPU-accelerated SDL_Texture
    m_Texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    
    // Default the widget rect size to the rendered text dimensions
    rect.w = static_cast<float>(textSurface->w);
    rect.h = static_cast<float>(textSurface->h);

    // Clean up surface
    SDL_DestroySurface(textSurface);
}

void Label::setText(SDL_Renderer* renderer, const std::string& text) {
    if (m_Text != text) {
        m_Text = text;
        updateTexture(renderer);
    }
}

void Label::setColor(SDL_Renderer* renderer, SDL_Color color) {
    // Optimization: SDL_SetTextureColorMod could be used instead of re-rendering, 
    // but for high-quality Blended text, re-rendering with the new color is often cleaner.
    m_Color = color;
    updateTexture(renderer);
}

void Label::render(SDL_Renderer* renderer) {
    if (!visible || !m_Texture) return;

    // Calculate actual drawing position based on alignment within the widget's 'rect'
    SDL_FRect drawRect = rect;
    
    // If the widget rect is wider than the text texture (due to manual layout), adjust x
    float texW = 0, texH = 0;
    SDL_GetTextureSize(m_Texture, &texW, &texH);

    // Calculate scaling to fit within rect while maintaining aspect ratio
    float scale = 1.0f;
    if (texW > rect.w || texH > rect.h) {
        scale = SDL_min(rect.w / texW, rect.h / texH);
    }

    drawRect.w = texW * scale;
    drawRect.h = texH * scale;

    if (m_Alignment == ALIGN_CENTER) {
        drawRect.x += (rect.w - drawRect.w) / 2.0f;
    } else if (m_Alignment == ALIGN_RIGHT) {
        drawRect.x += (rect.w - drawRect.w);
    }

    // Vertically center within the rect.h
    drawRect.y += (rect.h - drawRect.h) / 2.0f;

    // Apply global alpha transparency from base class
    SDL_SetTextureAlphaMod(m_Texture, static_cast<Uint8>(alpha * 255.0f));

    SDL_RenderTexture(renderer, m_Texture, nullptr, &drawRect);
}

bool Label::handleEvent(const SDL_Event* event, bool& isOver) {
    // Standard labels usually don't consume events
    isOver = false;
    return false;
}
