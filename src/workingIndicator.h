/*
 * workingIndicator.h
 *
 *      Author: alex@glassoniongames.com
 */

#ifndef WORKINGINDICATOR_H
#define WORKINGINDICATOR_H

#include "widgets.h"

class WorkingIndicator : public Widget
{
public:
    bool active = false;

    WorkingIndicator() = default;
    virtual ~WorkingIndicator() override = default;

    // Standardized Widget Interface
    virtual void render(SDL_Renderer *renderer) override;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;
    virtual void updateLayout(float x, float y, float w, float h) override;
};

#endif