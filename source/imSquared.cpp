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

bool imSquared::ended()
{
    return m_ended;
}

bool imSquared::completed()
{
    return (m_score > (m_levels[m_currentLevel].clear_factor[m_configuration.hardness] *
                       m_levels[m_currentLevel].max_expected_score[m_configuration.hardness]));
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

    m_configuration.screenWidth = -1;
    m_configuration.screenHeight = -1;
    m_configuration.columns = 10;
    m_configuration.rows = 13;
    m_configuration.menus_enabled = false;

    //
    // Load Figure Database
    //
    loadDatabase();


    //
    // Show menu
    //
    //if ((m_currentLevel == -1) && (m_configuration.menus_enabled))
   // {
   //     ShowMainMenu();
   // }

    startLevel("level 1");
}

void imSquared::startLevel(std::string const &level)
{
    m_ended = false;
    m_currentLevel = indexFromLevelName(level);

    //if (m_currentLevel == -1)
    //{
    //	DPrintf("Invalid level %s", m_configuration.level.c_str());
    //	m_levels.clear();
    //	m_figures.clear();
    //	return;
    //}

    //
    // initialize level
    //
    m_speed = m_levels[m_currentLevel].speed[m_configuration.hardness];

    //
    // Setup Geometry
    //
    m_matrix.clear();

    m_translation = 0.0;
    m_lastUpdate = getCurrentSec();
    m_lineCreated = 0;

    //
    // Compute Square positions
    //
    for (int currentLine = 0; currentLine != m_configuration.rows; ++currentLine)
    {
        SquareElements row;
        for (int currentColumn = 0; currentColumn != m_configuration.columns; ++currentColumn)
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

    m_touches.clear();

    m_figureGeneration = -1;
    m_currentFigure = -1;
    m_currentFigureLine = 0;
    m_currentFigureOffset = 0;

    m_multiplier = 1;
    m_score = 0;


    m_figuresOnBoard.clear();
}

void imSquared::applyColors()
{
    for (SquareMatrix::iterator row = m_matrix.begin(); row != m_matrix.end(); ++row)
    {
        for (SquareElements::iterator cell = row->begin(); cell != row->end(); ++cell)
        {

            if (cell->state == Idle)
            {
            }
            else if (cell->state == Marked)
            {
                cell->color = cell->markedColor;
            }
            else if (cell->state == Hit)
            {
                cell->color.r = 0;
                cell->color.g = 255;
                cell->color.b = 0;
                cell->color.a = 255;
            }
            else if (cell->state == Miss)
            {
                cell->color.r = 255;
                cell->color.g = 0;
                cell->color.b = 0;
                cell->color.a = 255;
            }
        }
    }
}

void imSquared::updateResolution()
{
    bool resized = (GetScreenWidth() != m_configuration.screenWidth) || (GetScreenHeight() != m_configuration.screenHeight);

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
        m_configuration.screenWidth = GetScreenWidth();
        m_configuration.screenHeight = GetScreenHeight();

        m_configuration.squareWidth =
            (float)m_configuration.screenWidth / (float)m_configuration.columns;

        m_configuration.squareHeight =
            (float)m_configuration.screenHeight / (float)(m_configuration.rows - 1);

        //
        // Compute Square positions
        //
        int margin = 1;

        for (int currentLine = 0; currentLine != m_configuration.rows; ++currentLine)
        {
            SquareElements &row = m_matrix[currentLine];
            for (int currentColumn = 0; currentColumn != m_configuration.columns; ++currentColumn)
            {
                SquareElement &cell = row[currentColumn];

                cell.rectangle.x = currentColumn * m_configuration.squareWidth + margin;
                cell.rectangle.y = currentLine * m_configuration.squareHeight + margin;
                cell.rectangle.width = m_configuration.squareWidth - 2 * margin;
                cell.rectangle.height = m_configuration.squareHeight - 2 * margin;
            }
        }
    }
}

void imSquared::update()
{
    updateResolution();

    if (!UpdateLevelChaining())
    {
        return;
    }

    if (m_menu.activated())
    {
        m_menu.update(this);
        return;
    }

    if (m_levels.empty())
        return;

    double currentTime = getCurrentSec();
    double updateDelta = currentTime - m_lastUpdate;

    m_translation += (updateDelta / m_speed) * m_configuration.squareHeight;
    //logDbg("Simon", sfmt("m_translation %0.5f", m_translation));

    if (m_translation > m_configuration.squareHeight)
    {
        processLeavingFigures();

        for (int currentColumn = 0; currentColumn != m_configuration.columns; ++currentColumn)
            for (int currentLine = 0; currentLine != (m_configuration.rows - 1); ++currentLine)
            {
                Rectangle rectangle = m_matrix[currentLine][currentColumn].rectangle;
                m_matrix[currentLine][currentColumn] = m_matrix[currentLine + 1][currentColumn];
                m_matrix[currentLine][currentColumn].rectangle = rectangle;

                if (currentLine == (m_configuration.rows - 2))
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

        while (m_translation > m_configuration.squareHeight)
            m_translation -= m_configuration.squareHeight;

        processFigures();
    }

    m_speed += updateDelta * m_levels[m_currentLevel].speed_increment_per_second[m_configuration.hardness];
    if (m_speed < m_levels[m_currentLevel].speed_max)
        m_speed = m_levels[m_currentLevel].speed_max;

    m_lastUpdate = getCurrentSec();
    processTouches();
    applyColors();

    if ((!m_levels.empty() && (m_figureGeneration > m_levels[m_currentLevel].total_figures) && m_figuresOnBoard.empty()))
    {
        m_ended = true;
        m_score += m_levels[m_currentLevel].bonus[m_configuration.hardness];
    }
}

bool imSquared::pointHitSquare(Vector2 const &point, SquareElement const &square)
{
    // Test point againt corner 1 and corner 4
    return CheckCollisionPointRec(point, square.rectangle);
}

void imSquared::processTouches()
{
    if (m_levels.empty())
        return;

    //
    // check hits
    //
    for (Points::const_iterator i = m_touches.begin(); i != m_touches.end(); ++i)
        for (SquareMatrix::iterator row = m_matrix.begin(); row != m_matrix.end(); ++row)
            for (SquareElements::iterator cell = row->begin(); cell != row->end(); ++cell)
            {
                if (cell->state == Idle)
                {
                    if (pointHitSquare(*i, *cell))
                    {
                        cell->state = Miss;

                        if (cell->figure >= 0)
                        {
                            int computed = (float)m_figures[cell->figure].score *
                                           (float)m_levels[m_currentLevel].score_factor[m_configuration.hardness] *
                                           (float)m_configuration.missScoreFactor;
                            m_score += computed;
                            logDbg("Simon", sfmt("Miss Score: %d", computed));
                            m_multiplier = 1;

                            for (FiguresOnBoard::iterator iterator = m_figuresOnBoard.begin();
                                 iterator != m_figuresOnBoard.end(); ++iterator)
                                iterator->multiplied = true;
                        }
                    }
                }
                else if (cell->state == Marked)
                {
                    if (pointHitSquare(*i, *cell))
                    {
                        cell->state = Hit;

                        addHitToFigureOnBoard(cell->figureGeneration, 1);
                        if (cell->figure >= 0)
                        {
                            int computed = (float)m_figures[cell->figure].score *
                                           (float)m_levels[m_currentLevel].score_factor[m_configuration.hardness] *
                                           (float)m_multiplier;
                            m_score += computed;
                            logDbg("Simon", sfmt("Hit Score: %d %d", m_multiplier, computed));
                        }
                    }
                }
            }

    //
    // check multiplicators
    //
    for (auto& figure : m_figuresOnBoard)
    {
        if ((figure.hitCount == figure.markedCount) && !figure.multiplied)
        {
            m_multiplier++;
            figure.multiplied = true;
        }
    }
}

void imSquared::setTouches(Points const &touches)
{
    m_touches = touches;

    if (m_menu.activated())
    {
        m_menu.click(this);
        return;
    }

    processTouches();
}

Menu &imSquared::menu()
{
    return m_menu;
}

void imSquared::processLeavingFigures()
{
    //
    // check for squares not pressed on last line
    //
    for (SquareElements::iterator i = m_matrix.rbegin()->begin(); i != m_matrix.rbegin()->end(); ++i)
    {
        if (i->state == Marked)
        {
            m_multiplier = 1;
        }
    }
}

void imSquared::addHitToFigureOnBoard(int figureGeneration, int hit)
{
    for (FiguresOnBoard::iterator iterator = m_figuresOnBoard.begin();
         iterator != m_figuresOnBoard.end(); ++iterator)
    {
        if (iterator->figureGeneration == figureGeneration)
        {
            iterator->hitCount += hit;
        }
    }
}

bool imSquared::isFigureOnMatrix(int figureGeneration)
{
    for (auto& row : m_matrix)
        for (auto& cell : row)
        {
            if ((cell.figureGeneration == figureGeneration) && (cell.hasPiece))
                return true;
        }

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

            m_currentFigureOffset = rand() % (m_configuration.columns - m_figures[m_currentFigure].width + 1);

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

void imSquared::ShowMainMenu()
{
    Menu::Elements elements;

    for (Levels::iterator level = m_levels.begin(); level != m_levels.end(); ++level)
    {
        elements.push_back(level->name);
    }
    m_menu.SetElements(elements);
    m_menu.SetTitle(IMS_MAIN_MENU_TITLE);
    m_menu.Show();
    logDbg("Simon", sfmt("ShowMainMenu() finished"));
}

bool imSquared::UpdateLevelChaining()
{
    if (!m_configuration.menus_enabled)
        return true;

    if (m_menu.activated())
    {
        if (m_menu.Selected() != -1)
        {
            std::string level = m_levels[m_menu.Selected()].name;
            //Configuration configuration = m_configuration;
            //configuration.level = level;
            m_menu.Hide();

            // initialize(configuration);

            return false;
        }
    }
    else
    {
        if (ended())
        {
            ShowMainMenu();
            return false;
        }
    }

    return true;
}
