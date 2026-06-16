/*
 * boxContainer.h
 *
 *      Author: alex@glassoniongames.com
 */

#ifndef BOXCONTAINER_H
#define BOXCONTAINER_H

#include "widgets.h"
#include <vector>

struct BoxChild
{
    Widget *widget;
    float flexWeight;
};

class VBoxContainer : public Widget
{
public:
    VBoxContainer() = default;
    virtual ~VBoxContainer() override = default;

    void addChild(Widget *child, float flexWeight = 1.0f);
    void setSpacing(float spacing) { m_spacing = spacing; }

    // Standardized Widget Interface
    virtual void render(SDL_Renderer *renderer) override;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;
    virtual void updateLayout(float x, float y, float w, float h) override;

private:
    std::vector<BoxChild> m_children;
    float m_spacing = 10.0f;
};

class HBoxContainer : public Widget
{
public:
    HBoxContainer() = default;
    virtual ~HBoxContainer() override = default;

    void addChild(Widget *child, float flexWeight = 1.0f);
    void setSpacing(float spacing) { m_spacing = spacing; }

    // Standardized Widget Interface
    virtual void render(SDL_Renderer *renderer) override;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;
    virtual void updateLayout(float x, float y, float w, float h) override;

private:
    std::vector<BoxChild> m_children;
    float m_spacing = 10.0f;
};

#endif // BOXCONTAINER_H