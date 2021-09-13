#include "Project.h"

using namespace re;

#define MINT_CREAM  CLITERAL(Color) { 240, 247, 238, 255 }
#define BEAU_BLUE CLITERAL(Color) { 196, 215, 242, 255 }
#define OLD_LAVANDER CLITERAL(Color) { 119, 104, 113, 255 }

void imSquared::renderBackground()
{
    ClearBackground(RAYWHITE);

    Rectangle rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = GetScreenWidth();
    rect.height = GetScreenHeight();

    int base_alpha = 55;

    Color c0 = ORANGE;
    Color c1 = BEIGE;
    c0.a = base_alpha + sin(re::getCurrentSec() * 0.5) * 25;
    c1.a = base_alpha + sin(re::getCurrentSec()) * 25;

    Color tl_color = c0;
    Color bl_color = c1;
    Color br_color = c0;
    Color tr_color = c1;

    DrawRectangleGradientEx(rect, tl_color, bl_color, br_color, tr_color); 

}

void imSquared::renderBlocks()
{
    //
    // draw squares
    //
    for (auto &row : m_matrix)
        for (auto &cell : row)
        {
            Color color;
            color.r = 170;
            color.g = 210;
            color.b = 236;
            color.a = 50;


            if (cell.state == Marked)
            {
                color.r = 153;
                color.g = 195;
                color.b = 236;
                color.a = 255;
            }
            else if (cell.state == Hit)
            {
                color.r = 151;
                color.g = 227;
                color.b = 194;
                color.a = 255;
            }
            else if (cell.state == Miss)
            {
                color.r = 246;
                color.g = 156;
                color.b = 158;
                color.a = 255;
            }
 

            Rectangle rectangle = cell.rectangle;
            rectangle.y -= m_translation;
            
            DrawRectangleRec(rectangle, color);
        }
}

void imSquared::render()
{
    //
    // Render Base Pass
    //
    BeginTextureMode(m_base_pass);
    {
        renderBackground();

        renderBlocks();

        if (!m_center_message.empty())
        {
            float font_size = 34.0f;

            int size = MeasureText(m_center_message.c_str(), font_size);
            int x = (GetScreenWidth() - size) / 2;
            int y = (GetScreenHeight() - font_size) / 2;

            int margin = 10;

            Rectangle region;
            region.x = x - margin;
            region.y = y - margin;
            region.width = size + 2 * margin;
            region.height = font_size + 2 * margin;

            Color c_start = RAYWHITE;
            Color c_mid = MINT_CREAM;
            Color c_end = BEAU_BLUE;

            DrawRectangleGradientEx(region, c_mid, c_start, c_mid, c_end);
            DrawRectangleLinesEx(region, font_size * 0.1f, OLD_LAVANDER);

            DrawText(m_center_message.c_str(), x, y, font_size, OLD_LAVANDER);
        }

        int level = m_currentLevel + 1;
        level += m_hardness * m_levels.size();

        //DrawText(re::sfmt("%08d Level %d - %.4f", m_score, level, m_speed).c_str(), 20, 20, 20, GRAY);
        DrawText(re::sfmt("%08d Level %d", m_score, level).c_str(), 20, 20, 20, GRAY);
    }
    EndTextureMode();

    //
    // Render the texture to screen
    //
    BeginDrawing();
    {
        Rectangle src{0.0f, 0.0f, (float)m_base_pass.texture.width, -(float)m_base_pass.texture.height};
        Rectangle dst{0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight()};
        Vector2 origin{0, 0};

        DrawTexturePro(m_base_pass.texture, src, dst, origin, 0.0f, WHITE);
    }
    EndDrawing();

    
    //if (m_menu.activated())
    //{
    //    m_menu.render(this);
    //    return;
    //}
}
