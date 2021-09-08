#pragma once

#include "Project.h"

#define IMS_MAIN_MENU_TITLE "Main Menu"

class imSquared
{
public:
    const float FLT_N_INIT = -100012.0f;

    typedef std::vector<Vector2> Points;

    enum State
    {
        Idle,
        Marked,

        Hit, // marked and ok
        Miss // marked but not ok
    };

    // watch out.. this is beeing copied with memcpy
    struct SquareElement
    {
        Color color;
        Color markedColor;
        Rectangle rectangle;
        State state;
        bool hasPiece;
        int figureGeneration;
        int figure;
    };
    typedef std::vector<SquareElement> SquareElements;
    typedef std::vector<SquareElements> SquareMatrix;

    typedef std::vector<std::string> FigureLines;

    struct Figure
    {
        Figure();

        FigureLines lines;
        int markedCount;
        int width;
        int height;
        std::string name;
    };

    typedef std::vector<Figure> Figures;

    struct FigureOnBoard
    {
        int figure;
        int markedCount;
        int figureGeneration;
    };
    typedef std::vector<FigureOnBoard> FiguresOnBoard;

    typedef std::vector<int> FigureIndex;
    static int const max_hardness = 3;

    struct Level
    {
        Level();

        std::string name;
        std::string type;
        int figure_spacing;
        float speed; // time to move one square in seconds
        float speed_increment_per_second;
        float speed_max;
        int total_figures;
        float bonus;
        FigureIndex figures;
    };

    typedef std::vector<Level> Levels;


    imSquared();
    ~imSquared();

    void setup();
    void shutdown();
    void startLevel(int level);

    int score();

    void update();
    void render();
    void step();
    void run();

private:
    int m_screenWidth;
    int m_screenHeight;
    int m_columns;
    int m_rows;

    float m_squareWidth;
    float m_squareHeight;

    float m_hit_score;
    float m_miss_score_factor;

    int m_hardness;

    float m_translation;
    double m_lastUpdate;

    SquareMatrix m_matrix;

    Figures m_figures;
    int m_lastFigure;
    int m_currentFigure;
    int m_currentFigureLine;
    int m_currentFigureOffset;
    int m_figureGeneration;

    float m_speed;
    Levels m_levels;
    int m_currentLevel;
    std::string m_currentLevelName;
    int m_lineCreated;

    FiguresOnBoard m_figuresOnBoard;

    int m_score;

    RenderTexture2D m_base_pass;
    bool m_audio_started;
    std::string m_center_message;

    void updateResolution();
    void applyColors();
    void processTouches();
    bool pointHitSquare(Vector2 const &point, SquareElement const &square);
    void processFigures();

    void loadDatabase();
    void appendSyntheticDatabase();

    int indexFromFigureName(std::string const &name);
    int indexFromLevelName(std::string const &name);

    bool isFigureOnMatrix(int figureGeneration);

    void renderBackground();
    void renderBlocks();
};
