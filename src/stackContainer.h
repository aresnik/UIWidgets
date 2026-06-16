/*
 * stackContainer.h
 *
 *      Author: alex@glassoniongames.com
 */

#ifndef STACKCONTAINER_H
#define STACKCONTAINER_H

#include "widgets.h"
#include <vector>

class StackContainer : public Widget
{
public:
    StackContainer() = default;
    virtual ~StackContainer() override = default;

    void addChild(Widget *child);

    virtual void render(SDL_Renderer *renderer) override;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;
    virtual void updateLayout(float x, float y, float w, float h) override;

private:
    std::vector<Widget *> m_children;
};

#endif // STACKCONTAINER_H