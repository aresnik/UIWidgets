/*
 * boxContainer.cpp
 *
 *      Author: alex@glassoniongames.com
 */

#include "boxContainer.h"
#include <algorithm>

// ==========================================
// VBoxContainer Implementation
// ==========================================

void VBoxContainer::addChild(Widget *child, float flexWeight)
{
    if (child)
    {
        m_children.push_back({child, flexWeight});
    }
}

void VBoxContainer::render(SDL_Renderer *renderer)
{
    if (!visible)
        return;
    for (auto &childNode : m_children)
    {
        childNode.widget->alpha = this->alpha;
        childNode.widget->render(renderer);
    }
}

bool VBoxContainer::handleEvent(const SDL_Event *event, bool &isOver)
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
        if (it->widget->handleEvent(event, childOver))
        {
            handled = true;
        }
        if (childOver)
            anyChildOver = true;
    }

    isOver = anyChildOver;
    return handled;
}

void VBoxContainer::updateLayout(float x, float y, float w, float h)
{
    rect = {x, y, w, h};
    if (m_children.empty())
        return;

    float totalSpacing = m_spacing * (m_children.size() - 1);
    float availableHeight = std::max(0.0f, h - totalSpacing);

    float totalWeight = 0.0f;
    for (const auto &childNode : m_children)
        totalWeight += childNode.flexWeight;

    if (totalWeight <= 0.0f)
        totalWeight = 1.0f; // Prevent division by zero

    float currentY = y;
    for (auto &childNode : m_children)
    {
        float childHeight = (childNode.flexWeight / totalWeight) * availableHeight;
        childNode.widget->updateLayout(x, currentY, w, childHeight);
        currentY += childHeight + m_spacing;
    }
}

// ==========================================
// HBoxContainer Implementation
// ==========================================

void HBoxContainer::addChild(Widget *child, float flexWeight)
{
    if (child)
    {
        m_children.push_back({child, flexWeight});
    }
}

void HBoxContainer::render(SDL_Renderer *renderer)
{
    if (!visible)
        return;
    for (auto &childNode : m_children)
    {
        childNode.widget->alpha = this->alpha;
        childNode.widget->render(renderer);
    }
}

bool HBoxContainer::handleEvent(const SDL_Event *event, bool &isOver)
{
    if (!visible || !enabled)
    {
        isOver = false;
        return false;
    }

    bool handled = false;
    bool anyChildOver = false;

    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it)
    {
        bool childOver = false;
        if (it->widget->handleEvent(event, childOver))
        {
            handled = true;
        }
        if (childOver)
            anyChildOver = true;
    }

    isOver = anyChildOver;
    return handled;
}

void HBoxContainer::updateLayout(float x, float y, float w, float h)
{
    rect = {x, y, w, h};
    if (m_children.empty())
        return;

    float totalSpacing = m_spacing * (m_children.size() - 1);
    float availableWidth = std::max(0.0f, w - totalSpacing);

    float totalWeight = 0.0f;
    for (const auto &childNode : m_children)
        totalWeight += childNode.flexWeight;

    if (totalWeight <= 0.0f)
        totalWeight = 1.0f;

    float currentX = x;
    for (auto &childNode : m_children)
    {
        float childWidth = (childNode.flexWeight / totalWeight) * availableWidth;
        childNode.widget->updateLayout(currentX, y, childWidth, h);
        currentX += childWidth + m_spacing;
    }
}