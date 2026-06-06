/*
 * textureButton.h
 *
 *      Author: alex@alexanderresnik.com
 */

#ifndef TEXTUREBUTTON_H
#define TEXTUREBUTTON_H

#include "widgets.h"
#include <functional>

class TextureButton : public Widget {
public:
    enum ButtonState {
        STATE_NORMAL = 0,
        STATE_HOVER,
        STATE_PRESSED,
        STATE_TOTAL
    };

    TextureButton() = default;
    TextureButton(float x, float y, float width, float height,
                  SDL_Texture* normalTex, 
                  SDL_Texture* hoverTex, 
                  SDL_Texture* pressedTex);

    virtual ~TextureButton() override = default;

    // Standardized Widget Interface
    virtual void render(SDL_Renderer* renderer) override;
    virtual bool handleEvent(const SDL_Event* event, bool& isOver) override;
    virtual void updateLayout(float x, float y, float w, float h) override;

    // Texture Management
    void setTextures(SDL_Texture* normal, SDL_Texture* hover = nullptr, SDL_Texture* pressed = nullptr);
    void setHoverScale(float scale) { m_hoverScale = scale; }

    // Assigns a callback function when the button is fully clicked (released inside)
    void setOnClickCallback(std::function<void()> callback);

private:
    // Helper to check if mouse coordinates lie within the button bounds
    bool isMouseInside(float mouseX, float mouseY) const;

    SDL_Texture* m_textures[STATE_TOTAL];
    ButtonState m_currentState;
    float m_hoverScale = 1.05f;
    std::function<void()> m_onClickCallback;
};


#endif
