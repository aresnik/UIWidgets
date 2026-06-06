#ifndef TOGGLE_SWITCH_H
#define TOGGLE_SWITCH_H

#include <SDL3/SDL.h>

class ToggleSwitch
{
public:
    // Constructor requiring dimensions and position
    ToggleSwitch(SDL_FRect bounds);

    // Processes mouse clicks to update the switch state
    void handleEvent(const SDL_Event &event);

    // Updates position and size
    void updateLayout(float x, float y, float w, float h);

    // Sets the state immediately without animation
    void setToggled(bool toggled);

    // Draws the switch onto the window
    void render(SDL_Renderer *renderer);

    // Returns the current ON/OFF state
    bool isSwitchToggled() const;

private:
    void drawCircle(SDL_Renderer *renderer, float centerX, float centerY, float radius, SDL_Color color);
    void drawPill(SDL_Renderer *renderer, SDL_FRect rect, SDL_Color color);
    SDL_FRect bounds;
    float m_handlePos; // 0.0 (OFF) to 1.0 (ON) for animation
    bool isToggled;
};

#endif // TOGGLE_SWITCH_H
