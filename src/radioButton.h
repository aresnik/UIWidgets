/*
 * radioButton.h
 *
 *      Author: alex@alexanderresnik.com
 */

#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include "widgets.h"
#include <vector>
#include <string>
#include <memory>

// Forward declaration to break circular dependency
class Label;

class RadioButton : public Widget
{
public:
    RadioButton();
    RadioButton(int value, const std::string &labelText);
    virtual ~RadioButton() override;

    // Standardized Widget Interface
    virtual void render(SDL_Renderer *renderer) override;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;
    virtual void updateLayout(float x, float y, float w, float h) override;

    // Accessors
    void setSelected(bool selected) { m_isSelected = selected; }
    bool isSelected() const { return m_isSelected; }

    // This sets what value this specific button represents
    int getValue() const { return m_value; }
    // Ensure this exists to avoid the deleted assignment operator error
    void setValue(int value) { m_value = value; }

    // Expose the internal label for customization (Font, Color, etc.)
    Label &getLabel() { return *m_labelWidget; }

private:
    void generateTextures(SDL_Renderer *renderer);
    void cleanup();

    int m_value = 0;
    bool m_isSelected = false;
    std::unique_ptr<Label> m_labelWidget;
    SDL_Texture *m_outlineTex = nullptr;
    SDL_Texture *m_fillTex = nullptr;

    // Internal drawing helpers for procedural circles
    bool isMouseInside(float mouseX, float mouseY) const;
};

class RadioButtonGroup : public Widget
{
public:
    RadioButtonGroup() = default;
    virtual ~RadioButtonGroup() override = default;

    // Management
    void addButton(RadioButton *button);
    int getSelectedValue() const { return m_selectedValue; }
    void setSelectedValue(int value);

    // Standardized Widget Interface
    virtual void render(SDL_Renderer *renderer) override;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;
    virtual void updateLayout(float x, float y, float w, float h) override;

    // Layout properties
    void setSpacing(float spacing) { m_spacing = spacing; }

private:
    std::vector<RadioButton *> m_buttons;
    int m_selectedValue = -1;
    float m_spacing = 40.0f;
};

#endif
