/*
 * dialogBox.h
 *
 *      Author: alex@glassoniongames.com
 */

#ifndef DIALOGBOX_H
#define DIALOGBOX_H

#include "widgets.h"
#include <vector>

class DialogBox : public Widget
{
public:
    DialogBox() = default;
    DialogBox(float x, float y, float width, float height,
              SDL_Color bgColor = {40, 40, 40, 255},
              SDL_Color borderColor = {255, 255, 255, 255},
              float borderWidth = 2.0f);

    virtual ~DialogBox() override = default;

    // Composition: Manage child widgets contained within this dialog
    void addChild(Widget *child);

    // Standardized Widget Interface
    virtual void render(SDL_Renderer *renderer) override;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;
    virtual void updateLayout(float x, float y, float w, float h) override;

    void setColors(SDL_Color bgColor, SDL_Color borderColor);
    void setBorderWidth(float width) { m_borderWidth = width; }

    // Set padding as a percentage of the dialog's size (0.0 to 1.0)
    void setPadding(float p) { m_padLeft = m_padRight = m_padTop = m_padBottom = p; }
    void setPadding(float x, float y)
    {
        m_padLeft = m_padRight = x;
        m_padTop = m_padBottom = y;
    }
    void setPadding(float top, float right, float bottom, float left)
    {
        m_padTop = top;
        m_padRight = right;
        m_padBottom = bottom;
        m_padLeft = left;
    }

private:
    std::vector<Widget *> m_children;
    SDL_Color m_bgColor;
    SDL_Color m_borderColor;
    float m_borderWidth;

    float m_padTop = 0.05f;
    float m_padBottom = 0.05f;
    float m_padLeft = 0.1f;
    float m_padRight = 0.1f;
};

#endif
