/*
 * toggleSwitch.h
 *
 *      Author: alex@glassoniongames.com
 */

#ifndef TOGGLE_SWITCH_H
#define TOGGLE_SWITCH_H

#include "widgets.h"

class ToggleSwitch : public Widget
{
public:
    ToggleSwitch() = default;
    // Constructor requiring dimensions and position
    ToggleSwitch(SDL_FRect bounds);
    virtual ~ToggleSwitch() override;

    // Processes mouse clicks to update the switch state
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;

    // Updates position and size
    virtual void updateLayout(float x, float y, float w, float h) override;

    // Sets the state immediately without animation
    void setToggled(bool toggled);

    // Draws the switch onto the window
    virtual void render(SDL_Renderer *renderer) override;

    // Returns the current ON/OFF state
    bool isSwitchToggled() const;

private:
    void generateTextures(SDL_Renderer *renderer);
    void cleanup();

    float m_handlePos{0.0f}; // 0.0 (OFF) to 1.0 (ON) for animation
    bool isToggled{false};

    SDL_Texture *m_pillTex = nullptr;
    SDL_Texture *m_handleTex = nullptr;
};

#endif // TOGGLE_SWITCH_H
