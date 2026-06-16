/*
 * spacer.h
 *
 *      Author: alex@glassoniongames.com
 */

#ifndef SPACER_H
#define SPACER_H

#include "widgets.h"

class Spacer : public Widget
{
public:
    Spacer() = default;
    virtual ~Spacer() override = default;

    virtual void render(SDL_Renderer *renderer) override;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;
};

#endif // SPACER_H