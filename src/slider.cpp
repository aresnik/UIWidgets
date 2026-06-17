/*
 * slider.cpp
 *
 *      Author: alex@glassoniongames.com
 */

#include "slider.h"
#include <algorithm>
#include <cmath>

Slider::Slider(int numPositions)
    : m_numPositions(std::max(2, numPositions)),
      m_currentValue(0),
      m_isDragging(false),
      m_trackX(0), m_trackY(0), m_trackW(0), m_trackH(0), m_handleRadius(15.0f)
{
    setNumPositions(m_numPositions);
}

Slider::~Slider()
{
    cleanup();
}

void Slider::setNumPositions(int numPositions)
{
    m_numPositions = std::max(2, numPositions);
    m_labels.clear();
    for (int i = 0; i < m_numPositions; ++i)
    {
        auto lbl = std::make_unique<Label>();
        lbl->setAlignment(Label::ALIGN_CENTER);
        m_labels.push_back(std::move(lbl));
    }
    
    // Clamp the value just in case the position count shrunk
    m_currentValue = std::clamp(m_currentValue, 0, m_numPositions - 1);
}

void Slider::setValue(int value)
{
    int oldVal = m_currentValue;
    m_currentValue = std::clamp(value, 0, m_numPositions - 1);
    
    if (m_currentValue != oldVal && m_onValueChangedCallback)
    {
        m_onValueChangedCallback(m_currentValue);
    }
}

void Slider::setLabel(int index, SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Color color)
{
    if (index >= 0 && index < m_numPositions && m_labels[index])
    {
        m_labels[index]->load(renderer, font, text, color);
    }
}

void Slider::setOnValueChangedCallback(std::function<void(int)> callback)
{
    m_onValueChangedCallback = callback;
}

void Slider::updateLayout(float x, float y, float w, float h)
{
    rect = {x, y, w, h};

    // Proportional sizing relative to flex box
    m_handleRadius = std::clamp(h * 0.15f, 10.0f, 25.0f);
    m_trackW = std::max(10.0f, w - (m_handleRadius * 3.0f)); 
    m_trackH = m_handleRadius * 0.4f;
    
    m_trackX = x + (w - m_trackW) / 2.0f;
    m_trackY = y + (h * 0.35f) - (m_trackH / 2.0f); // Placed slightly above center

    // Generate textures if the size has significantly changed
    if (m_handleTex == nullptr) 
    {
        // We will defer texture generation until render is called to ensure we have a renderer
    }

    // Update layout for all labels
    float labelH = h * 0.35f;
    float labelW = m_trackW / (float)m_numPositions; // Approximate width allowed per label
    float labelY = m_trackY + m_handleRadius + (h * 0.05f); // Below the track

    for (int i = 0; i < m_numPositions; ++i)
    {
        float tickX = getTickX(i);
        m_labels[i]->updateLayout(tickX - (labelW / 2.0f), labelY, labelW, labelH);
    }
}

float Slider::getTickX(int index) const
{
    if (m_numPositions <= 1) return m_trackX;
    float spacing = m_trackW / (float)(m_numPositions - 1);
    return m_trackX + (index * spacing);
}

void Slider::updateValueFromPos(float mx)
{
    if (m_numPositions <= 1) return;
    float relativeX = mx - m_trackX;
    float ratio = std::clamp(relativeX / m_trackW, 0.0f, 1.0f);
    
    int newValue = static_cast<int>(std::round(ratio * (m_numPositions - 1)));
    setValue(newValue);
}

bool Slider::handleEvent(const SDL_Event *event, bool &isOver)
{
    isOver = false;
    if (!visible || !enabled) return false;

    float mx = -1.0f, my = -1.0f;
    if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        mx = event->motion.x; my = event->motion.y;
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        mx = event->button.x; my = event->button.y;
    }

    // Check bounds
    if (mx >= 0)
    {
        if (mx >= rect.x && mx <= rect.x + rect.w && my >= rect.y && my <= rect.y + rect.h)
            isOver = true;
    }

    bool handled = false;
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT)
    {
        if (isOver)
        {
            m_isDragging = true;
            updateValueFromPos(mx);
            handled = true;
        }
    }
    else if (event->type == SDL_EVENT_MOUSE_MOTION && m_isDragging)
    {
        updateValueFromPos(mx);
        handled = true;
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP && event->button.button == SDL_BUTTON_LEFT)
    {
        if (m_isDragging)
        {
            m_isDragging = false;
            handled = true;
        }
    }

    return handled;
}

void Slider::render(SDL_Renderer *renderer)
{
    if (!visible) return;
    if (!m_handleTex) generateTextures(renderer);

    float displayAlpha = enabled ? alpha : alpha * 0.4f;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // 1. Draw Track Base (Gray)
    SDL_FRect trackRect = {m_trackX, m_trackY, m_trackW, m_trackH};
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, static_cast<Uint8>(255 * displayAlpha));
    SDL_RenderFillRect(renderer, &trackRect);

    // 2. Draw Active Track (Blue fill up to current handle)
    float handleX = getTickX(m_currentValue);
    SDL_FRect activeTrackRect = {m_trackX, m_trackY, handleX - m_trackX, m_trackH};
    SDL_SetRenderDrawColor(renderer, 0, 150, 255, static_cast<Uint8>(255 * displayAlpha));
    SDL_RenderFillRect(renderer, &activeTrackRect);

    // 3. Draw Tick Marks
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, static_cast<Uint8>(150 * displayAlpha));
    float tickExt = m_trackH * 0.8f; // Ticks extend slightly past track
    for (int i = 0; i < m_numPositions; ++i)
    {
        float tx = getTickX(i);
        SDL_RenderLine(renderer, tx, m_trackY - tickExt, tx, m_trackY + m_trackH + tickExt);
    }

    // 4. Render Labels
    for (auto &lbl : m_labels)
    {
        lbl->alpha = displayAlpha;
        lbl->render(renderer);
    }

    // 5. Draw Handle (Knob)
    SDL_SetTextureAlphaMod(m_handleTex, static_cast<Uint8>(255 * displayAlpha));
    Uint8 colorIntensity = enabled ? 255 : 150;
    SDL_SetTextureColorMod(m_handleTex, colorIntensity, colorIntensity, colorIntensity);

    SDL_FRect handleRect = {handleX - m_handleRadius, (m_trackY + m_trackH / 2.0f) - m_handleRadius, 
                            m_handleRadius * 2.0f, m_handleRadius * 2.0f};
    SDL_RenderTexture(renderer, m_handleTex, NULL, &handleRect);
}

void Slider::generateTextures(SDL_Renderer *renderer)
{
    cleanup();
    float radius = std::max(10.0f, m_handleRadius);
    int size = (int)(radius * 2.0f) + 2;
    SDL_Surface *handleSurf = SDL_CreateSurface(size, size, SDL_PIXELFORMAT_RGBA32);
    float center = size / 2.0f;

    // Generate anti-aliased white circle
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            float dx = (float)x + 0.5f - center;
            float dy = (float)y + 0.5f - center;
            float dist = SDL_sqrtf(dx * dx + dy * dy);
            
            float coverage = SDL_clamp(radius - dist + 0.5f, 0.0f, 1.0f);
            Uint8 *pixel = (Uint8 *)handleSurf->pixels + y * handleSurf->pitch + x * 4;
            pixel[0] = 255; 
            pixel[1] = 255; 
            pixel[2] = 255; 
            pixel[3] = (Uint8)(255 * coverage);
        }
    }
    m_handleTex = SDL_CreateTextureFromSurface(renderer, handleSurf);
    SDL_DestroySurface(handleSurf);
    
    if (m_handleTex) SDL_SetTextureBlendMode(m_handleTex, SDL_BLENDMODE_BLEND);
}

void Slider::cleanup()
{
    if (m_handleTex)
    {
        SDL_DestroyTexture(m_handleTex);
        m_handleTex = nullptr;
    }
}