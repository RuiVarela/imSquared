#include "Project.h"

using namespace re;

#define MINT_CREAM \
    CLITERAL(Color) { 240, 247, 238, 255 }
#define BEAU_BLUE \
    CLITERAL(Color) { 196, 215, 242, 255 }
#define POWDER_BLUE \
    CLITERAL(Color) { 175, 222, 220, 255 }
#define MORNING_BLUE \
    CLITERAL(Color) { 145, 168, 164, 255 }
#define OLD_LAVANDER \
    CLITERAL(Color) { 119, 104, 113, 255 }

void imSquared::renderBackground()
{
    ClearBackground(RAYWHITE);
}

void imSquared::renderBlocks()
{
    //
    // draw squares
    //
    for (auto &row : m_matrix)
        for (auto &cell : row)
        {
            Rectangle rectangle = cell.rectangle;
            rectangle.y -= m_translation;
            DrawRectangleRec(rectangle, cell.color);
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

        DrawText(re::sfmt("%08d %03d", m_score, m_multiplier).c_str(), 20, 20, 20, LIGHTGRAY);
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
