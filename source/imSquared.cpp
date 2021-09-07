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
    score = 10;
    hits = 0;
    color.r = 0;
    color.g = 0;
    color.b = 255;
    color.a = 255;
    sound = "default sound";
}

#define FLT_N_INIT -100012.0

imSquared::Level::Level()
{
    for (int i = 0; i != max_hardness; ++i)
    {
        speed[i] = FLT_N_INIT;
        clear_factor[i] = FLT_N_INIT;
        speed_increment_per_second[i] = FLT_N_INIT;
        score_factor[i] = FLT_N_INIT;
        max_expected_score[i] = 0;
        max_expected_score_with_multiplier[i] = 0;
        bonus[i] = 0;
    }

    speed_max = 0.1;
    total_figures = 0;
}

imSquared::imSquared()
    : m_screenWidth(0), m_screenHeight(0),
      m_columns(0), m_rows(0),
      m_squareWidth(0), m_squareHeight(0),
      m_missScoreFactor(-0.2f), m_hardness(0)
{
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

bool imSquared::completed()
{
    return (m_score > (m_levels[m_currentLevel].clear_factor[m_hardness] *
                       m_levels[m_currentLevel].max_expected_score[m_hardness]));
}

void imSquared::shutdown()
{
    if (m_base_pass.id != -1)
    {
        UnloadRenderTexture(m_base_pass);
        m_base_pass.id = -1;
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
    SetConfigFlags(FLAG_WINDOW_RESIZABLE /*| FLAG_MSAA_4X_HINT*/);

    InitWindow(800, 450, "imSquared");

#ifndef PLATFORM_WEB
    SetTargetFPS(30);
#endif

    m_screenWidth = -1;
    m_screenHeight = -1;
    m_columns = 10;
    m_rows = 13;

    //
    // Load Figure Database
    //
    loadDatabase();

    //
    // Start Level
    //
    startLevel(0);
}

void imSquared::startLevel(int level)
{
    m_currentLevel = level;

    //
    // initialize level
    //
    m_currentLevelName = m_levels[m_currentLevel].name;
    m_speed = m_levels[m_currentLevel].speed[m_hardness];


    m_translation = 0.0;
    m_lastUpdate = getCurrentSec();

    //
    // Setup Geometry
    //
    m_matrix.clear();
    m_lineCreated = 0;

    m_score = 0;

    //
    // Compute Square positions
    //
    for (int currentLine = 0; currentLine != m_rows; ++currentLine)
    {
        SquareElements row;
        for (int currentColumn = 0; currentColumn != m_columns; ++currentColumn)
        {
            SquareElement cell;

            {
                cell.color.r = (0.5f + sin(float(currentLine)) * 0.3f) * 255;
                cell.color.g = cell.color.r;
                cell.color.b = cell.color.r;
                cell.color.a = 255;
            }

            cell.state = Idle;
            cell.figure = -1;
            cell.hasPiece = false;
            row.push_back(cell);
        }
        m_matrix.push_back(row);
    }

    m_figureGeneration = -1;
    m_currentFigure = -1;
    m_currentFigureLine = 0;
    m_currentFigureOffset = 0;

    m_multiplier = 1;

    m_figuresOnBoard.clear();
}

void imSquared::applyColors()
{
    for (auto& row : m_matrix)
        for (auto& cell : row)
            if (cell.state == Idle)
            {
            }
            else if (cell.state == Marked)
            {
                cell.color = cell.markedColor;
            }
            else if (cell.state == Hit)
            {
                cell.color.r = 0;
                cell.color.g = 255;
                cell.color.b = 0;
                cell.color.a = 255;
            }
            else if (cell.state == Miss)
            {
                cell.color.r = 255;
                cell.color.g = 0;
                cell.color.b = 0;
                cell.color.a = 255;
            }
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
        int margin = 1;

        for (int currentLine = 0; currentLine != m_rows; ++currentLine)
        {
            SquareElements &row = m_matrix[currentLine];
            for (int currentColumn = 0; currentColumn != m_columns; ++currentColumn)
            {
                SquareElement &cell = row[currentColumn];

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
    updateResolution();

    if (m_levels.empty())
        return;

    double currentTime = getCurrentSec();
    double updateDelta = currentTime - m_lastUpdate;

    m_translation += (updateDelta / m_speed) * m_squareHeight;
    //logDbg("Simon", sfmt("m_translation %0.5f", m_translation));

    if (m_translation > m_squareHeight)
    {
        processLeavingFigures();

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

                    int c = (0.5f + sin(float(currentLine + m_lineCreated)) * 0.3f) * 255;
                    m_matrix[currentLine + 1][currentColumn].color.r = c;
                    m_matrix[currentLine + 1][currentColumn].color.g = c;
                    m_matrix[currentLine + 1][currentColumn].color.b = c;
                }
            }

        ++m_lineCreated;

        while (m_translation > m_squareHeight)
            m_translation -= m_squareHeight;

        processFigures();

        // check if level changed
        if (!m_levels.empty() && (m_figureGeneration > m_levels[m_currentLevel].total_figures) && m_figuresOnBoard.empty())
        {
            m_score += m_levels[m_currentLevel].bonus[m_hardness];

            // change level
            m_currentLevel = (m_currentLevel + 1) % m_levels.size();
            m_currentLevelName = m_levels[m_currentLevel].name;
            m_speed = m_levels[m_currentLevel].speed[m_hardness];
            m_figureGeneration = -1;
            m_currentFigure = -1;
            m_currentFigureLine = 0;
            m_currentFigureOffset = 0;
        }
    }

    m_speed += updateDelta * m_levels[m_currentLevel].speed_increment_per_second[m_hardness];
    if (m_speed < m_levels[m_currentLevel].speed_max)
        m_speed = m_levels[m_currentLevel].speed_max;

    m_lastUpdate = getCurrentSec();
    processTouches();
    applyColors();
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
                if (cell.state == Idle)
                {
                    if (pointHitSquare(touch, cell))
                    {
                        cell.state = Miss;

                        if (cell.figure >= 0)
                        {
                            int computed = (float)m_figures[cell.figure].score *
                                           (float)m_levels[m_currentLevel].score_factor[m_hardness] *
                                           (float)m_missScoreFactor;
                            m_score += computed;
                            if (m_score < 0)
                                m_score = 0;

                            logDbg("Simon", sfmt("Miss Score: %d", computed));
                            m_multiplier = 1;

                            for (FiguresOnBoard::iterator iterator = m_figuresOnBoard.begin();
                                 iterator != m_figuresOnBoard.end(); ++iterator)
                                iterator->multiplied = true;
                        }
                    }
                }
                else if (cell.state == Marked)
                {
                    if (pointHitSquare(touch, cell))
                    {
                        cell.state = Hit;

                        addHitToFigureOnBoard(cell.figureGeneration, 1);
                        if (cell.figure >= 0)
                        {
                            int computed = (float)m_figures[cell.figure].score *
                                           (float)m_levels[m_currentLevel].score_factor[m_hardness] *
                                           (float)m_multiplier;
                            m_score += computed;
                            logDbg("Simon", sfmt("Hit Score: %d %d", m_multiplier, computed));
                        }
                    }
                }
            }
    }

    //
    // check multiplicators
    //
    for (auto &figure : m_figuresOnBoard)
        if ((figure.hitCount == figure.markedCount) && !figure.multiplied)
        {
            m_multiplier++;
            figure.multiplied = true;
        }
    
}

void imSquared::processLeavingFigures()
{
    //
    // check for squares not pressed on last line
    //
    for (SquareElements::iterator i = m_matrix.rbegin()->begin(); i != m_matrix.rbegin()->end(); ++i)
        if (i->state == Marked)
            m_multiplier = 1;
}

void imSquared::addHitToFigureOnBoard(int figureGeneration, int hit)
{
    for (auto& figure : m_figuresOnBoard)
        if (figure.figureGeneration == figureGeneration)
            figure.hitCount += hit;  
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
    FiguresOnBoard::iterator iterator = m_figuresOnBoard.begin();
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
                m_currentFigure = rand() % int(m_figures.size());
            }
            else
            {
                m_currentFigure = m_levels[m_currentLevel].figures[m_figureGeneration];
            }

            m_currentFigureOffset = rand() % (m_columns - m_figures[m_currentFigure].width + 1);

            FigureOnBoard figureOnBoard;
            figureOnBoard.figure = m_currentFigure;
            figureOnBoard.figureGeneration = m_figureGeneration;
            figureOnBoard.hitCount = 0;
            figureOnBoard.markedCount = m_figures[m_currentFigure].markedCount;
            figureOnBoard.multiplied = false;
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
                m_matrix[entry][m_currentFigureOffset + x].markedColor = m_figures[m_currentFigure].color;
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
