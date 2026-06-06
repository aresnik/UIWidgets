
//=================================================================================
// Name        : UIWidgets
// Author      : Copyright 2026 Alexander Resnik
// Email       : alex@alexanderresnik.com
// Version     : 1.00
// License     : MIT License
// Description : UIWidgets is a library written in C++ and SDL3.
//=================================================================================

#ifndef WIDGETS_H
#define WIDGETS_H

// Master Aggregator Header
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <stdexcept>

class Widget
{
public:
    SDL_FRect rect = {0.0f, 0.0f, 0.0f, 0.0f};
    bool enabled = true;
    bool visible = true;
    float alpha = 1.0f; // Global transparency support

    virtual ~Widget();

    virtual void updateLayout(float x, float y, float w, float h) { rect = {x, y, w, h}; }
    virtual void render(SDL_Renderer *renderer) = 0;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) = 0;
};

// Derived classes must be included AFTER the Widget base class definition
#include "dialogBox.h"
#include "label.h"
#include "radioButton.h"
#include "textureButton.h"
#include "workingIndicator.h"
#include "toggleSwitch.h"

#endif
