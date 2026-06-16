/*
 * spacer.cpp
 *
 *      Author: alex@glassoniongames.com
 */

#include "spacer.h"

void Spacer::render(SDL_Renderer *renderer)
{
    // A spacer has no visual representation.
}

bool Spacer::handleEvent(const SDL_Event *event, bool &isOver)
{
    isOver = false;
    return false; // A spacer never handles events.
}