/*
 * label.cpp
 *
 *      Author: alex@glassoniongames.com
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
    if (m_Texture) {
        SDL_SetTextureScaleMode(m_Texture, SDL_SCALEMODE_LINEAR);
    }

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

void Label::updateLayout(float x, float y, float w, float h) {
    if (!m_Texture) {
        rect = {x, y, w, h};
        return;
    }

    float texW = 0.0f, texH = 0.0f;
    SDL_GetTextureSize(m_Texture, &texW, &texH);

    if (texW > 0.0f && texH > 0.0f) {
        // Calculate scaling to perfectly fit the flex box space while preserving aspect ratio
        float scale = SDL_min(w / texW, h / texH);

        float finalW = texW * scale;
        float finalH = texH * scale;

        // Handle Horizontal Alignment
        float finalX = x; // ALIGN_LEFT
        if (m_Alignment == ALIGN_CENTER) {
            finalX = x + (w - finalW) / 2.0f;
        } else if (m_Alignment == ALIGN_RIGHT) {
            finalX = x + (w - finalW);
        }

        // Vertically center within the slot
        float finalY = y + (h - finalH) / 2.0f;

        rect = {finalX, finalY, finalW, finalH};
    } else {
        rect = {x, y, w, h};
    }
}

void Label::render(SDL_Renderer* renderer) {
    if (!visible || !m_Texture) return;

    // Apply global alpha transparency from base class
    SDL_SetTextureAlphaMod(m_Texture, static_cast<Uint8>(alpha * 255.0f));

    SDL_RenderTexture(renderer, m_Texture, nullptr, &rect);
}

bool Label::handleEvent(const SDL_Event* event, bool& isOver) {
    // Standard labels usually don't consume events
    isOver = false;
    return false;
}
