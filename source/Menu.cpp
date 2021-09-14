#include "Project.h"

namespace re
{
    Menu::Menu()
    {
    }

    Menu::~Menu()
    {
        clear();
    }

    void Menu::clear()
    {
        m_items.clear();
    }

    void Menu::addButton(std::string const &resource, std::function<void(void)> callback)
    {
        std::shared_ptr<Button> item = std::make_shared<Button>(resource);
        item->setCallback(callback);
        m_items.push_back(item);
    }

    void Menu::setActive(bool active)
    {
        for (auto &item : m_items)
            item->setActive(active);
    }

    void Menu::update()
    {
        int total_height;
        int max_height;

        float base_scale = 1.0f;
        bool fits;
        do
        {
            fits = true;
            total_height = 0;
            max_height = 0;

            for (auto &item : m_items)
            {
                item->update(Button::Profile::Menu, base_scale);

                int height = item->height();
                if (height > max_height)
                    max_height = height;

                total_height += height;
            }

            float max_h = GetScreenHeight() * 0.7;
            if (total_height > max_h)
            {
                fits = false;
                base_scale -= 0.1;
            }

            if (base_scale < 0.2)
                fits = true;

        } while (!fits);

        int margin = float(max_height) * 0.5f;

        // add spacing
        if (m_items.size() > 1)
            total_height += margin * (m_items.size() - 1);

        Vector2 position;
        position.y = (GetScreenHeight() - total_height) / 2;

        for (auto &item : m_items)
        {
            position.x = (GetScreenWidth() - item->width()) / 2;

            item->setPosition(position);

            position.y += item->height();
            position.y += margin;
        }

        // run callbacks
        for (auto &item : m_items)
            item->checkTrigger();
    }

    void Menu::render()
    {
        for (auto &item : m_items)
            item->render();
    }
}