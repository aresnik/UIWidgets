/*
 * slider.h
 *
 *      Author: alex@glassoniongames.com
 */

#ifndef SLIDER_H
#define SLIDER_H

#include "widgets.h"
#include "label.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>

class Slider : public Widget
{
public:
    Slider(int numPositions = 5);
    virtual ~Slider() override;

    virtual void render(SDL_Renderer *renderer) override;
    virtual bool handleEvent(const SDL_Event *event, bool &isOver) override;
    virtual void updateLayout(float x, float y, float w, float h) override;

    int getValue() const { return m_currentValue; }
    void setValue(int value);
    void setNumPositions(int numPositions);

    // Sets the text for a specific position tick
    void setLabel(int index, SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Color color);
    void setOnValueChangedCallback(std::function<void(int)> callback);

private:
    void generateTextures(SDL_Renderer *renderer);
    void cleanup();
    float getTickX(int index) const;
    void updateValueFromPos(float mx);

    int m_numPositions;
    int m_currentValue;
    bool m_isDragging;

    std::vector<std::unique_ptr<Label>> m_labels;
    std::function<void(int)> m_onValueChangedCallback;

    SDL_Texture *m_handleTex = nullptr;
    float m_trackX, m_trackY, m_trackW, m_trackH;
    float m_handleRadius;
};

#endif