#include "Project.h"

namespace re
{
    Button::Button(std::string resource)
    {
        m_texture = LoadTexture(resource.c_str());
        m_scale = 1.0f;
        m_width = m_texture.width;
        m_height = m_texture.height;
        m_pressed = false;
        m_position.x = 0;
        m_position.y = 0;
        m_active = true;
    }

    Button::~Button()
    {
        UnloadTexture(m_texture);
    }

    void Button::setCallback(std::function<void(void)> callback)
    {
        m_callback = callback;
    }

    void Button::setActive(bool active) {
        m_active = active;
    }

    void Button::update(Profile profile, float base_scale)
    {
        //
        // update sizes
        //
        m_scale = 1.0f;
        m_width = float(m_texture.width) * base_scale;
        m_height = float(m_texture.height) * base_scale;

        if (profile == Profile::Menu)
        {
            float max_w = GetScreenWidth() * 0.7;

            if (m_width > max_w)
                m_width = max_w;
        }
        else if (profile == Profile::ScreenRightCorner)
        {
            float max_w = GetScreenWidth() * 0.2;
            if (m_width > max_w)
                m_width = max_w;

            float max_h = GetScreenHeight() * 0.2;
            if (m_height > max_h)
                m_height = max_h;

            checkTrigger();
        }

        m_scale = re::minimum(float(m_width) / float(m_texture.width),
                              float(m_height) / float(m_texture.height));

        m_width = float(m_texture.width) * m_scale;
        m_height = float(m_texture.height) * m_scale;

        if (profile == Profile::ScreenRightCorner)
        {
            float margin = m_width * 0.3;

            Vector2 position; 
            position.x = GetScreenWidth() - m_width - margin;
            position.y = GetScreenHeight() - m_height - margin;
            setPosition(position);

            checkTrigger();
        }
    }

    void Button::setPosition(Vector2 position)
    {
        m_position = position;
    }

    void Button::checkTrigger()
    {
        Vector2 m = GetMousePosition();

        bool inside = true;
        inside &= m.x >= m_position.x && m.x < (m_position.x + m_width);
        inside &= m.y >= m_position.y && m.y < (m_position.y + m_height);

        bool was_pressed = m_pressed;
        m_pressed = inside && IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        m_triggered = (was_pressed && !m_pressed);

        if (m_triggered && m_callback && m_active)
            m_callback();
    }

    bool Button::triggered()
    {
        return m_triggered;
    }

    int Button::width()
    {
        return m_width;
    }

    int Button::height()
    {
        return m_height;
    }

    void Button::render()
    {
        float rotation = 0.0f;

        Color tint;
        tint.r = 255;
        tint.g = 255;
        tint.b = 255;
        tint.a = m_pressed ? 150 : 255;

        DrawTextureEx(m_texture, m_position, rotation, m_scale, tint);
    }
}