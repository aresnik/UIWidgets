/*
 * stackContainer.cpp
 *
 *      Author: alex@glassoniongames.com
 */

#include "stackContainer.h"

void StackContainer::addChild(Widget *child)
{
    if (child)
    {
        m_children.push_back(child);
    }
}

void StackContainer::render(SDL_Renderer *renderer)
{
    if (!visible)
        return;

    // Render back-to-front
    for (auto *child : m_children)
    {
        child->alpha = this->alpha;
        child->render(renderer);
    }
}

bool StackContainer::handleEvent(const SDL_Event *event, bool &isOver)
{
    if (!visible || !enabled)
    {
        isOver = false;
        return false;
    }

    bool handled = false;
    bool anyChildOver = false;

    // Handle events front-to-back
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
    {
        bool childOver = false;
        if ((*it)->handleEvent(event, childOver))
            handled = true;

        if (childOver)
            anyChildOver = true;
    }
    isOver = anyChildOver;
    return handled;
}

void StackContainer::updateLayout(float x, float y, float w, float h)
{
    rect = {x, y, w, h};
    for (auto *child : m_children)
        child->updateLayout(x, y, w, h);
}