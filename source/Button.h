#pragma once

#include "Project.h"

namespace re
{
    class Button
    {
    public:
        Button(std::string resource);

        Button(Button const& noncopyable) = delete;
        Button& operator=(Button const& noncopyable) =delete;

        ~Button();

        void setActive(bool active);
        void setCallback(std::function<void(void)> callback);

        enum Profile {
            Menu,
            ScreenRightCorner
        };

        void update(Profile profile, float base_scale = 1.0f);

        void setPosition(Vector2 position);

        void checkTrigger();
        bool triggered();

        int width();
        int height();

        void render();

    private:
        Texture2D m_texture;       
        float m_scale;
        int m_width;
        int m_height;
        bool m_pressed;
        bool m_triggered;
        Vector2 m_position;
        std::function<void(void)> m_callback;
        bool m_active;
    };

}