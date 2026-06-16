/*
 * aspectRatioContainer.cpp
 *
 *      Author: alex@glassoniongames.com
 */

#include "aspectRatioContainer.h"
#include <algorithm>

AspectRatioContainer::AspectRatioContainer(float ratio) : m_ratio(ratio) {}

void AspectRatioContainer::addChild(Widget* child)
{
    if (child)
    {
        m_children.push_back(child);
    }
}

void AspectRatioContainer::render(SDL_Renderer* renderer)
{
    if (!visible) return;
    for (auto* child : m_children)
    {
        child->alpha = this->alpha;
        child->render(renderer);
    }
}

bool AspectRatioContainer::handleEvent(const SDL_Event* event, bool& isOver)
{
    if (!visible || !enabled)
    {
        isOver = false;
        return false;
    }

    bool handled = false;
    bool anyChildOver = false;

    // Pass events to children in reverse order (top-most gets priority)
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
    {
        bool childOver = false;
        if ((*it)->handleEvent(event, childOver))
        {
            handled = true;
        }
        if (childOver)
            anyChildOver = true;
    }

    isOver = anyChildOver;
    return handled;
}

void AspectRatioContainer::updateLayout(float x, float y, float w, float h)
{
    rect = {x, y, w, h}; // The full flex space assigned to the container
    if (m_children.empty() || w <= 0.0f || h <= 0.0f) return;

    // Calculate the largest bounding box that fits within (w, h) while maintaining m_ratio
    float targetW = w;
    float targetH = w / m_ratio;

    // If the calculated height is too tall, constrain by height instead
    if (targetH > h)
    {
        targetH = h;
        targetW = h * m_ratio;
    }

    // Center the perfectly proportioned rectangle within the available flex space
    float finalX = x + (w - targetW) / 2.0f;
    float finalY = y + (h - targetH) / 2.0f;

    // Pass the perfectly constrained box down to all children
    for (auto* child : m_children)
    {
        child->updateLayout(finalX, finalY, targetW, targetH);
    }
}