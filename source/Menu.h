#pragma once

#include "Project.h"

namespace re
{
    class Menu
    {
    public:
        Menu();
        ~Menu();

        void clear();

        void setActive(bool active);
        void addButton(std::string const& resource, std::function<void(void)> callback);
        void update();
        void render();

    private:
        std::vector<std::shared_ptr<Button>> m_items;
    };

}