/*
 * radioButton.cpp
 *
 *      Author: alex@alexanderresnik.com
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

    float centerX = rect.x + rect.w / 2.0f;
    float centerY = rect.y + rect.h / 2.0f;
    float radius = rect.w / 2.0f;

    // Dimming: If disabled, we reduce both the alpha and the color intensity for a more pronounced effect
    float dimFactor = enabled ? 1.0f : 0.45f;
    float displayAlpha = alpha * dimFactor;
    Uint8 colorIntensity = enabled ? 255 : 120;

    // 1. Draw outer outline circle
    SDL_Color outlineColor = {colorIntensity, colorIntensity, colorIntensity, static_cast<Uint8>(255 * displayAlpha)};
    drawCircle(renderer, centerX, centerY, radius, outlineColor, false);

    // 2. Draw inner filled circle if selected
    if (m_isSelected)
    {
        // Selection Blue: (0, 150, 255). We dim the RGB components as well.
        SDL_Color blue = {
            0,
            static_cast<Uint8>(150 * dimFactor),
            static_cast<Uint8>(255 * dimFactor),
            static_cast<Uint8>(255 * displayAlpha)};
        drawCircle(renderer, centerX, centerY, radius - 4.0f, blue, true);
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
    rect = {x, y, w, h};
    // Position label 10 pixels to the right of the radio circle
    m_labelWidget->updateLayout(x + w + 10.0f, y, w * 10.0f, h);
}

bool RadioButton::isMouseInside(float mouseX, float mouseY) const
{
    // Hit test includes the circle and the likely area of the label (approx 150px width)
    return (mouseX >= rect.x && mouseX <= rect.x + rect.w + 150.0f &&
            mouseY >= rect.y && mouseY <= rect.y + rect.h);
}

void RadioButton::drawCircle(SDL_Renderer *renderer, float centerX, float centerY, float radius, SDL_Color color, bool filled)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Iterate slightly outside the radius to capture the anti-aliased fade
    for (float dy = -radius - 1.0f; dy <= radius + 1.0f; dy += 1.0f)
    {
        for (float dx = -radius - 1.0f; dx <= radius + 1.0f; dx += 1.0f)
        {

            float distSq = dx * dx + dy * dy;
            float dist = SDL_sqrtf(distSq);
            float coverage = 0.0f;

            if (filled)
            {
                // Smoothly fade alpha based on distance from the edge
                coverage = SDL_clamp(radius - dist + 0.5f, 0.0f, 1.0f);
            }
            else
            {
                // Create a smooth ring (approx 1.5px thickness)
                float thickness = 1.2f;
                coverage = SDL_clamp(thickness - SDL_fabsf(dist - (radius - thickness / 2.0f)), 0.0f, 1.0f);
            }

            if (coverage > 0.0f)
            {
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                       static_cast<Uint8>(color.a * coverage));
                SDL_RenderPoint(renderer, centerX + dx, centerY + dy);
            }
        }
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
    for (size_t i = 0; i < m_buttons.size(); ++i)
    {
        m_buttons[i]->updateLayout(x, y + (i * m_spacing), w, h);
    }
}
