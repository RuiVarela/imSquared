#include "Project.h"

using namespace re;

//
// imsquared
//
imSquared::Figure::Figure()
{
    markedCount = 0;
    width = 0;
    height = 0;
}

imSquared::Level::Level()
{
    figure_spacing = 3;
    speed = 1.0;
    speed_increment_per_second = 0;
    bonus = 100;
    total_figures = 0;
}

imSquared::imSquared()
    : m_screenWidth(0), m_screenHeight(0),
      m_columns(0), m_rows(0),
      m_squareWidth(0), m_squareHeight(0)
{
    m_game_menu_show = false;

    m_hit_score = 10.0f;
    m_miss_score_factor = -0.2f;

    m_audio_started = false;
    m_base_pass.id = -1;
    m_currentLevel = 0;
}

imSquared::~imSquared()
{
    m_matrix.clear();
}

int imSquared::score()
{
    return m_score;
}

void imSquared::shutdown()
{
    if (m_base_pass.id != -1)
    {
        UnloadRenderTexture(m_base_pass);
        m_base_pass.id = -1;
    }

    for (auto &current : m_songs)
    {
        UnloadMusicStream(current);
    }

    if (m_audio_started)
    {
        CloseAudioDevice();
        m_audio_started = false;
    }

    CloseWindow();
}

void imSquared::setup()
{
    SetTraceLogLevel(LOG_DEBUG);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    InitWindow(800, 450, "imSquared");

#ifndef PLATFORM_WEB
    SetTargetFPS(60);
#endif

    m_screenWidth = -1;
    m_screenHeight = -1;
    m_columns = 10;
    m_rows = 13;

    //
    // Set songs
    //
    m_songs_filenames.push_back("resources/__blind_justice__.mod");
    m_songs_filenames.push_back("resources/_escape_.mod");
    m_songs_filenames.push_back("resources/65mix-russian.mod");

    //
    // Load Figure Database
    //
    loadDatabase();

    //
    // Create Squares
    //
    m_matrix.clear();
    for (int currentLine = 0; currentLine != m_rows; ++currentLine)
    {
        std::vector<SquareElement> row;
        for (int currentColumn = 0; currentColumn != m_columns; ++currentColumn)
        {
            SquareElement cell;

            cell.state = Idle;
            cell.figure = -1;
            cell.hasPiece = false;
            row.push_back(cell);
        }
        m_matrix.push_back(row);
    }


    m_game_menu_show = true;
    m_game_menu.addButton("resources/start.png", [this]() { 
        m_game_menu_show = false;
        startLevel(0);
        playNextSong();
    });
}

void imSquared::startLevel(int level)
{
    m_currentLevel = level;

    //
    // initialize level
    //
    m_speed = m_levels[m_currentLevel].speed;
    m_hardness = 0;


    m_translation = 0.0;
    m_lastUpdate = getCurrentSec();

    //
    // Setup Geometry
    //
    m_lineCreated = 0;

    m_score = 0;

    m_figureGeneration = -1;
    m_currentFigure = -1;
    m_currentFigureLine = 0;
    m_currentFigureOffset = 0;

    m_figuresOnBoard.clear();
}

void imSquared::updateResolution()
{
    bool resized = (GetScreenWidth() != m_screenWidth) || (GetScreenHeight() != m_screenHeight);

    if (m_base_pass.id == -1 || resized)
    {
        resized = true;

        //
        // Setup Base Pass
        //
        if (m_base_pass.id != -1)
        {
            UnloadRenderTexture(m_base_pass);
            m_base_pass.id = -1;
        }
        m_base_pass = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        SetTextureFilter(m_base_pass.texture, TEXTURE_FILTER_BILINEAR);
    }

    if (resized)
    {
        m_screenWidth = GetScreenWidth();
        m_screenHeight = GetScreenHeight();

        m_squareWidth = (float)m_screenWidth / (float)m_columns;
        m_squareHeight = (float)m_screenHeight / (float)(m_rows - 1);

        //
        // Compute Square positions
        //
        int margin = re::minimum(m_screenWidth, m_screenHeight) * 0.005;
        if (margin <= 0)
            margin = 1;

        for (int currentLine = 0; currentLine != m_rows; ++currentLine)
        {
            auto& row = m_matrix[currentLine];
            for (int currentColumn = 0; currentColumn != m_columns; ++currentColumn)
            {
                auto& cell = row[currentColumn];

                cell.rectangle.x = currentColumn * m_squareWidth + margin;
                cell.rectangle.y = currentLine * m_squareHeight + margin;
                cell.rectangle.width = m_squareWidth - 2 * margin;
                cell.rectangle.height = m_squareHeight - 2 * margin;
            }
        }
    }
}

void imSquared::update()
{    
    if (m_audio_started) { 
        if (GetMusicTimePlayed(m_songs[m_song_playing]) >= (GetMusicTimeLength(m_songs[m_song_playing]) - 0.01)) {
            playNextSong();
        }
        
        UpdateMusicStream(m_songs[m_song_playing]); 
    } 

    updateResolution();

    if (m_game_menu_show) {
        m_game_menu.update();
        return;
    }

    if (m_levels.empty())
        return;

    double currentTime = getCurrentSec();
    double updateDelta = currentTime - m_lastUpdate;

    float const hardnessSpeedChange = 0.05f;

    m_translation += (updateDelta / m_speed) * m_squareHeight;
    //logDbg("Simon", sfmt("m_translation %0.5f", m_translation));

    if (m_translation > m_squareHeight)
    {
        for (int currentColumn = 0; currentColumn != m_columns; ++currentColumn)
            for (int currentLine = 0; currentLine != (m_rows - 1); ++currentLine)
            {
                Rectangle rectangle = m_matrix[currentLine][currentColumn].rectangle;
                m_matrix[currentLine][currentColumn] = m_matrix[currentLine + 1][currentColumn];
                m_matrix[currentLine][currentColumn].rectangle = rectangle;

                if (currentLine == (m_rows - 2))
                {
                    m_matrix[currentLine + 1][currentColumn].figureGeneration = -1;
                    m_matrix[currentLine + 1][currentColumn].state = Idle;
                    m_matrix[currentLine + 1][currentColumn].hasPiece = false;
                }
            }

        ++m_lineCreated;

        while (m_translation > m_squareHeight)
            m_translation -= m_squareHeight;

        processFigures();

        // check if level changed
        if (!m_levels.empty() && (m_figureGeneration > m_levels[m_currentLevel].total_figures) && m_figuresOnBoard.empty())
        {
            m_score += m_levels[m_currentLevel].bonus;

            // change level
            int next = (m_currentLevel + 1) % m_levels.size();
            if (next < m_currentLevel) {
                m_hardness++;
                // completed all levels
                if (m_hardness == 10) {
                    m_hardness = 0;    
                }
            }

            m_currentLevel = next;


            m_speed = m_levels[m_currentLevel].speed - (m_hardness * hardnessSpeedChange);
            m_figureGeneration = -1;
            m_currentFigure = -1;
            m_currentFigureLine = 0;
            m_currentFigureOffset = 0;
        }
    }

    float speed_max = re::clampAbove(0.1f - (m_hardness * hardnessSpeedChange), 0.05f);
    

    m_speed += updateDelta * m_levels[m_currentLevel].speed_increment_per_second;
    if (m_speed < speed_max)
        m_speed = speed_max;

    m_lastUpdate = getCurrentSec();
    processTouches();
}

bool imSquared::pointHitSquare(Vector2 const &point, SquareElement const &square)
{
    Rectangle rectangle = square.rectangle;
    rectangle.y -= m_translation;

    return CheckCollisionPointRec(point, rectangle);
}

void imSquared::processTouches()
{
    if (m_levels.empty())
        return;

    //
    // check hits
    //
    bool pressed = false;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) pressed = true;
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) pressed = true;
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) pressed = true;

    int maxTouches = 3;
    
    for (int index = 0; index != maxTouches; ++index)
    {
        Vector2 touch = GetTouchPosition(index);
        if ((touch.x < 0) || (touch.y < 0) || !pressed)
            continue;

        for (auto &row : m_matrix)
            for (auto &cell : row)
            {
                bool idle = (cell.state == Idle);
                bool marked = (cell.state == Marked);
                if (pointHitSquare(touch, cell) && (idle || marked)) 
                {
                    cell.state = idle ? Miss : Hit;
                    float multiplier = idle ? (float)m_miss_score_factor : 1.0f;
                    m_score += m_hit_score * multiplier;
                    if (m_score < 0)
                        m_score = 0;
                }
            }
    }
  
}

bool imSquared::isFigureOnMatrix(int figureGeneration)
{
    for (auto& row : m_matrix)
        for (auto& cell : row)
            if ((cell.figureGeneration == figureGeneration) && (cell.hasPiece))
                return true;
        

    return false;
}

void imSquared::processFigures()
{
    if (m_figures.empty())
        return;

    size_t entry = m_matrix.size() - 1;

    //
    // Check leaving figure
    //
    auto iterator = m_figuresOnBoard.begin();
    while (iterator != m_figuresOnBoard.end())
    {
        if (!isFigureOnMatrix(iterator->figureGeneration))
        {
            logDbg("Simon", sfmt("Removed figureOnBoard %d", iterator->figureGeneration));
            iterator = m_figuresOnBoard.erase(iterator);
        }
        else
        {
            ++iterator;
        }
    }

    //
    // Create new Figures
    //
    if (m_currentFigure == -1)
    {
        m_figureGeneration++;

        if (m_figureGeneration < m_levels[m_currentLevel].total_figures)
        {
            m_lastFigure = -1;
            m_currentFigureLine = 0;

            if (m_levels[m_currentLevel].type == "random puzzle")
            {
                do {
                    m_currentFigure = rand() % int(m_figures.size());
                } while (m_figures[m_currentFigure].lines.size() > 5);

            }
            else
            {
                m_currentFigure = m_levels[m_currentLevel].figures[m_figureGeneration];
            }

            m_currentFigureOffset = rand() % (m_columns - m_figures[m_currentFigure].width + 1);

            FigureOnBoard figureOnBoard;
            figureOnBoard.figure = m_currentFigure;
            figureOnBoard.figureGeneration = m_figureGeneration;
            figureOnBoard.markedCount = m_figures[m_currentFigure].markedCount;
            m_figuresOnBoard.push_back(figureOnBoard);
            logDbg("Simon", sfmt("Generated figureOnBoard %d", figureOnBoard.figureGeneration));
        }
        else
            m_currentFigure--;
    }

    //
    // Add Spacing
    //
    if (m_currentFigure < 0)
    {
        for (auto& cell : m_matrix[entry])
            cell.figure = m_lastFigure;

        ++m_currentFigure;
        return;
    }

    //
    // Feed Figure Lines to board
    //
    if (m_currentFigureLine < m_figures[m_currentFigure].height)
    {
        for (int x = 0; x < m_figures[m_currentFigure].width; x++)
        {
            if (m_figures[m_currentFigure].lines[m_currentFigureLine][x] == 'X')
            {
                m_matrix[entry][m_currentFigureOffset + x].state = Marked;
                m_matrix[entry][m_currentFigureOffset + x].figure = m_currentFigure;
                m_matrix[entry][m_currentFigureOffset + x].hasPiece = true;
                m_matrix[entry][m_currentFigureOffset + x].figureGeneration = m_figureGeneration;
            }
        }

        for (auto& cell : m_matrix[entry])
            if (!cell.hasPiece)
                cell.figure = m_currentFigure;

        m_lastFigure = m_currentFigure;
        ++m_currentFigureLine;
    }

    if (m_currentFigureLine >= m_figures[m_currentFigure].height)
        m_currentFigure = -1 - 1 * m_levels[m_currentLevel].figure_spacing;
    
}

void imSquared::playNextSong() 
{
    if (!m_audio_started)
    {
        InitAudioDevice();
        m_audio_started = true;

        m_song_playing = -1;
        m_songs.resize(m_songs_filenames.size());
        for (size_t i = 0; i != m_songs_filenames.size(); ++i)
            m_songs[i] = LoadMusicStream(m_songs_filenames[i].c_str());
    }

    if (m_song_playing >= 0) {
        StopMusicStream(m_songs[m_song_playing]);
    }

    m_song_playing = (m_song_playing + 1) % m_songs.size();
    PlayMusicStream(m_songs[m_song_playing]);
}

void imSquared::step()
{
    update();
    render();
}

void imSquared::run()
{
    while (!WindowShouldClose())
    {
        step();
    }
}
