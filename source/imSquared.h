#pragma once

#include "Project.h"

#define IMS_MAIN_MENU_TITLE "Main Menu"

class imSquared
{
public:
    static int const max_hardness = 3;

    enum State
    {
        Idle,
        Marked,

        Hit, // marked and ok
        Miss // marked but not ok
    };

    struct SquareElement
    {
        Rectangle rectangle;
        State state;
        bool hasPiece;
        int figureGeneration;
        int figure;
    };

    struct Figure
    {
        Figure();

        std::vector<std::string> lines;
        int markedCount;
        int width;
        int height;
        std::string name;
    };

    struct FigureOnBoard
    {
        int figure;
        int markedCount;
        int figureGeneration;
    };

    struct Level
    {
        Level();

        std::string type;
        std::string message;
        int figure_spacing;
        float speed; // time to move one square in seconds
        float speed_increment_per_second;
        int total_figures;
        float bonus;
        std::vector<std::string> figure_names;
        std::vector<int> figures;
    };


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

    std::vector<std::vector<SquareElement>> m_matrix;

    std::vector<Figure> m_figures;
    int m_lastFigure;
    int m_currentFigure;
    int m_currentFigureLine;
    int m_currentFigureOffset;
    int m_figureGeneration;

    float m_speed;
    std::vector<Level> m_levels;
    int m_currentLevel;
    int m_lineCreated;

    std::vector<FigureOnBoard> m_figuresOnBoard;

    int m_score;

    RenderTexture2D m_base_pass;
    std::vector<std::string> m_songs_filenames;
    std::vector<Music> m_songs;
    int m_song_playing;
    bool m_audio_started;
    std::string m_center_message;

    re::Menu m_game_menu;
    bool m_game_menu_show;

    void playNextSong();

    void updateResolution();
    void processTouches();
    bool pointHitSquare(Vector2 const &point, SquareElement const &square);
    void processFigures();

    void loadDatabase();
    void appendSyntheticDatabase();

    int indexFromFigureName(std::string const &name);

    bool isFigureOnMatrix(int figureGeneration);

    void renderBackground();
    void renderBlocks();

};
