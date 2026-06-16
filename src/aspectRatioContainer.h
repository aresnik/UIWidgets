/*
 * aspectRatioContainer.h
 *
 *      Author: alex@glassoniongames.com
 */

#ifndef ASPECT_RATIO_CONTAINER_H
#define ASPECT_RATIO_CONTAINER_H

#include "widgets.h"
#include <vector>

class AspectRatioContainer : public Widget
{
public:
    // Default to a 1:1 ratio (perfect square)
    AspectRatioContainer(float ratio = 1.0f);
    virtual ~AspectRatioContainer() override = default;

    void setRatio(float ratio) { m_ratio = ratio; }
    float getRatio() const { return m_ratio; }

    void addChild(Widget* child);

    virtual void render(SDL_Renderer* renderer) override;
    virtual bool handleEvent(const SDL_Event* event, bool& isOver) override;
    virtual void updateLayout(float x, float y, float w, float h) override;

private:
    std::vector<Widget*> m_children;
    float m_ratio = 1.0f; // Width / Height
};

#endif // ASPECT_RATIO_CONTAINER_H