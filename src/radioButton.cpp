/*
 * radioButton.cpp
 *
 *      Author: alex@glassoniongames.com
 */

#include "radioButton.h"
#include "label.h"

// ==========================================
// RadioButton Implementation
// ==========================================

RadioButton::RadioButton()
    : m_labelWidget(std::make_unique<Label>())
{
}

RadioButton::RadioButton(int value, const std::string &labelText)
    : m_value(value), m_isSelected(false), m_labelWidget(std::make_unique<Label>())
{
    // The label is managed as a child widget
    // The parent app will likely call GetLabel().Load(...) after this
}

RadioButton::~RadioButton()
{
    // unique_ptr handles deletion automatically,
    // but we define the destructor here where Label's size is known.
}

void RadioButton::render(SDL_Renderer *renderer)
{
    if (!visible)
        return;

    if (!m_outlineTex || !m_fillTex)
    {
        generateTextures(renderer);
    }

    float centerX = rect.x + rect.w / 2.0f;
    float centerY = rect.y + rect.h / 2.0f;
    float radius = rect.w / 2.0f;

    // Dimming: If disabled, we reduce both the alpha and the color intensity for a more pronounced effect
    float dimFactor = enabled ? 1.0f : 0.45f;
    float displayAlpha = alpha * dimFactor;
    Uint8 colorIntensity = enabled ? 255 : 120;

    // 1. Draw outer outline circle
    SDL_SetTextureColorMod(m_outlineTex, colorIntensity, colorIntensity, colorIntensity);
    SDL_SetTextureAlphaMod(m_outlineTex, static_cast<Uint8>(255 * displayAlpha));
    SDL_RenderTexture(renderer, m_outlineTex, NULL, &rect);

    // 2. Draw inner filled circle if selected
    if (m_isSelected)
    {
        SDL_SetTextureColorMod(m_fillTex, 0, (Uint8)(150 * dimFactor), (Uint8)(255 * dimFactor));
        SDL_SetTextureAlphaMod(m_fillTex, static_cast<Uint8>(255 * displayAlpha));
        float innerRad = radius * 0.7f;
        SDL_FRect innerRect = {centerX - innerRad, centerY - innerRad, innerRad * 2, innerRad * 2};
        SDL_RenderTexture(renderer, m_fillTex, NULL, &innerRect);
    }

    // 3. Render the associated label
    m_labelWidget->alpha = displayAlpha;
    m_labelWidget->render(renderer);
}

bool RadioButton::handleEvent(const SDL_Event *event, bool &isOver)
{
    isOver = false;
    if (!visible || !enabled)
        return false;

    float mx = -1.0f, my = -1.0f;

    // Use coordinates from the event which have been converted to logical space in main.cpp
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

    if (mx >= 0)
    {
        isOver = isMouseInside(mx, my);
    }

    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT)
    {
        if (isOver)
        {
            return true; // Notify parent group that this button was clicked
        }
    }
    return false;
}

void RadioButton::updateLayout(float x, float y, float w, float h)
{
    // h represents the slot height. Scale diameter to leave natural vertical padding.
    float diameter = h * 0.5f;

    // If size changes, cached textures are no longer valid and must be regenerated
    if (diameter != rect.w || diameter != rect.h)
    {
        cleanup();
    }

    // Center the circle vertically within the slot
    float circleY = y + (h - diameter) / 2.0f;
    rect = {x, circleY, diameter, diameter};

    // Position label to the right of the circle with proportional padding
    float padding = diameter * 0.5f;
    m_labelWidget->updateLayout(x + diameter + padding, y, std::max(0.0f, w - diameter - padding), h);
}

bool RadioButton::isMouseInside(float mouseX, float mouseY) const
{
    // Hit test includes the circle and the actual dynamically scaled label area
    float totalWidth = rect.w + (m_labelWidget->rect.w > 0.0f ? m_labelWidget->rect.w + (rect.w * 0.5f) : 150.0f);
    return (mouseX >= rect.x && mouseX <= rect.x + totalWidth &&
            mouseY >= rect.y - 10.0f && mouseY <= rect.y + rect.h + 10.0f);
}

void RadioButton::generateTextures(SDL_Renderer *renderer)
{
    cleanup();

    int size = (int)rect.w;
    if (size <= 0)
        return;

    float radius = size / 2.0f;
    float center = size / 2.0f;

    // 1. Generate Outline Ring Texture (White/Tintable)
    SDL_Surface *outlineSurf = SDL_CreateSurface(size, size, SDL_PIXELFORMAT_RGBA32);
    float thickness = std::max(1.2f, radius * 0.15f);

    float ringCenterRadius = radius - thickness - 1.0f;

    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            float dx = (float)x + 0.5f - center;
            float dy = (float)y + 0.5f - center;
            float dist = SDL_sqrtf(dx * dx + dy * dy);

            // Create a smooth ring coverage factor
            float coverage = SDL_clamp(thickness - SDL_fabsf(dist - ringCenterRadius), 0.0f, 1.0f);

            Uint8 *pixel = (Uint8 *)outlineSurf->pixels + y * outlineSurf->pitch + x * 4;
            pixel[0] = 255;
            pixel[1] = 255;
            pixel[2] = 255;
            pixel[3] = (Uint8)(255 * coverage);
        }
    }
    m_outlineTex = SDL_CreateTextureFromSurface(renderer, outlineSurf);
    SDL_DestroySurface(outlineSurf);

    // 2. Generate Inner Fill Texture
    float fillRadius = radius * 0.7f;
    int fillSize = (int)(fillRadius * 2.0f) + 2;
    SDL_Surface *fillSurf = SDL_CreateSurface(fillSize, fillSize, SDL_PIXELFORMAT_RGBA32);
    float fillCenter = fillSize / 2.0f;

    for (int y = 0; y < fillSize; y++)
    {
        for (int x = 0; x < fillSize; x++)
        {
            float dx = (float)x + 0.5f - fillCenter;
            float dy = (float)y + 0.5f - fillCenter;
            float dist = SDL_sqrtf(dx * dx + dy * dy);

            float coverage = SDL_clamp(fillRadius - dist + 0.5f, 0.0f, 1.0f);

            Uint8 *pixel = (Uint8 *)fillSurf->pixels + y * fillSurf->pitch + x * 4;
            pixel[0] = 255;
            pixel[1] = 255;
            pixel[2] = 255;
            pixel[3] = (Uint8)(255 * coverage);
        }
    }
    m_fillTex = SDL_CreateTextureFromSurface(renderer, fillSurf);
    SDL_DestroySurface(fillSurf);

    if (m_outlineTex)
        SDL_SetTextureBlendMode(m_outlineTex, SDL_BLENDMODE_BLEND);
    if (m_fillTex)
        SDL_SetTextureBlendMode(m_fillTex, SDL_BLENDMODE_BLEND);
}

void RadioButton::cleanup()
{
    if (m_outlineTex)
    {
        SDL_DestroyTexture(m_outlineTex);
        m_outlineTex = nullptr;
    }
    if (m_fillTex)
    {
        SDL_DestroyTexture(m_fillTex);
        m_fillTex = nullptr;
    }
}

// ==========================================
// RadioButtonGroup Implementation
// ==========================================

void RadioButtonGroup::addButton(RadioButton *button)
{
    if (button)
    {
        m_buttons.push_back(button);
        if (m_selectedValue == -1)
        {
            setSelectedValue(button->getValue());
        }
    }
}

void RadioButtonGroup::setSelectedValue(int value)
{
    m_selectedValue = value;
    for (auto *btn : m_buttons)
    {
        btn->setSelected(btn->getValue() == m_selectedValue);
    }
}

bool RadioButtonGroup::handleEvent(const SDL_Event *event, bool &isOver)
{
    isOver = false;
    if (!visible || !enabled)
        return false;

    bool selectionChanged = false;
    for (auto *btn : m_buttons)
    {
        bool overBtn = false;
        if (btn->handleEvent(event, overBtn))
        {
            setSelectedValue(btn->getValue());
            selectionChanged = true;
        }
        if (overBtn)
            isOver = true;
    }
    return selectionChanged;
}

void RadioButtonGroup::render(SDL_Renderer *renderer)
{
    if (!visible)
        return;
    for (auto *btn : m_buttons)
    {
        btn->alpha = this->alpha;
        btn->enabled = this->enabled; // Propagate group enabled state to children
        btn->render(renderer);
    }
}

void RadioButtonGroup::updateLayout(float x, float y, float w, float h)
{
    rect = {x, y, w, h};
    if (m_buttons.empty())
        return;

    // Distribute the total available height evenly into slots for each button
    float slotHeight = h / m_buttons.size();

    for (size_t i = 0; i < m_buttons.size(); ++i)
    {
        m_buttons[i]->updateLayout(x, y + (i * slotHeight), w, slotHeight);
    }
}
