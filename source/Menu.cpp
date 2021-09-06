#include "Project.h"

Menu::Menu()
{
    m_title = "none";
    m_letter_height = -1;
    m_top = -1;
    m_selected = -1;
    m_show = false;
}

bool Menu::activated()
{
    return m_show;
}

void Menu::Show()
{
    m_selected = -1;
    m_show = true;
}

void Menu::Hide()
{
    m_show = false;
}

void Menu::click(imSquared *game)
{
    if (m_letter_height <= 0)
        return;

    for (imSquared::Points::const_iterator p = game->m_touches.begin();
         p != game->m_touches.end(); ++p)
    {
        float rescaled = ((p->y + 1.0f) * 0.5f) * game->m_configuration.screenHeight;
        for (int i = 0; i != m_elements.size(); ++i)
        {
            int low = GetYPos(i);
            int high = low + m_letter_height;

            if ((rescaled >= low) && (rescaled <= high))
            {
                m_selected = i;

                logDbg("Menu", re::sfmt("click %s\n", m_elements[i].c_str()));
                return;
            }
        }
    }
}

void Menu::update(imSquared *game)
{
    if (m_letter_height <= 0)
        return;
}


int Menu::GetYPos(int element)
{
    return m_top - element * m_letter_height - m_letter_height;
}

void Menu::render(imSquared *game)
{
    /*
    glClearColor(0.0, 0.0, 0.0, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    game->m_font.CurrentScaler = 1.5f;

    m_letter_height = game->m_font.GetHeight();
    m_top = game->m_configuration.screenHeight - 5 - m_letter_height;
    int x = m_letter_height;

    //int maxElements = m_top / m_letter_height;

    //
    // force projection
    //
    game->m_font.Bind(game->m_configuration.screenWidth, game->m_configuration.screenHeight);
    game->m_font.Unbind();

    //
    // draw the lines
    //
    glLineWidth(1.0f);

    x = 30;
    int box_border = 5;
    int box_width = game->m_configuration.screenWidth - box_border - box_border;

    //
    // draw boxes
    //
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.3, 0.0, 0.0, 1.0);
    for (int i = 0; i != m_elements.size(); ++i)
    {
        int currentY = GetYPos(i);
        DrawRectLines(box_border, currentY, box_width, m_letter_height);
    }

    //
    // highlight selected box
    //
    if ((m_selected >= 0) && (m_elements.size()))
    {
        glColor4f(0.9, 0.9, 0.9, 1.0);
        int currentY = GetYPos(m_selected);
        DrawRectLines(box_border, currentY, box_width, m_letter_height);
    }

    game->m_font.Bind(game->m_configuration.screenWidth, game->m_configuration.screenHeight);
    for (int i = 0; i != m_elements.size(); ++i)
    {
        int currentY = GetYPos(i);
        game->m_font.Print(m_elements[i], x, currentY);
    }
    game->m_font.Unbind();

    //
    // Go for the header
    //
    glColor4f(0.4, 0.4, 0.4, 0.4);
    DrawRect(box_border, m_top, box_width, m_letter_height);

    x = 15;
    {
        game->m_font.Bind(game->m_configuration.screenWidth, game->m_configuration.screenHeight);
        game->m_font.Print(m_title.c_str(), x, m_top);
        game->m_font.Unbind();
    }

    */
}
